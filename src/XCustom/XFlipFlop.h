uintptr_t gOriginalCarSetup = 0x44CDD0;
uintptr_t gOriginalTextureLookup = 0x5E7860;
uintptr_t gOriginalRenderMesh = 0x5E8B30;
uintptr_t gOriginalMaterialSubmit = 0x6134E0;

static SRWLOCK gColorEffectLock = SRWLOCK_INIT;
struct ColorEffectEntry
{
    bool used = false;
    bool attempted = false;
    uint32_t color1 = 0;
    uint32_t color2 = 0;
    uintptr_t effect = 0;
};
static ColorEffectEntry gColorEffects[32]{};
static uintptr_t gSubmitReturnStack[64]{};
static unsigned gSubmitReturnDepth = 0;
static uintptr_t gSubmitReturnAddressForStub = 0;
static bool gLoggedAlphaSkip = false;


struct ObservedBatch
{
    uintptr_t mesh;
    uint16_t materialId;
    uint8_t target;
    uint8_t reserved;
};

struct OwnerRecord
{
    uintptr_t owner;
    uint32_t color1;
    uint32_t color2;
    uint16_t targets[64];
    uint16_t targetCount;
    uint16_t observedCount;
    uint8_t targetOverflow;
    uint8_t observedOverflow;
    ObservedBatch observed[256];
};

struct RenderContext
{
    uintptr_t previousOwner;
    uintptr_t previousMesh;
};

OwnerRecord gOwners[64]{};
SRWLOCK gOwnerLock = SRWLOCK_INIT;
thread_local RenderContext gContextStack[64]{};
thread_local unsigned gContextDepth = 0;
thread_local unsigned gContextOverflow = 0;
thread_local uintptr_t gCurrentOwner = 0;
static volatile LONG gRaceContextLogCount = 0;
static volatile LONG gRaceSubmitLogCount = 0;
static volatile LONG gRaceSkipLogCount = 0;
thread_local uintptr_t gCurrentMesh = 0;

struct FlipEffectSwapState
{
    bool active = false;
    uintptr_t renderer = 0;
    uint32_t originalRendererEnvmapEffect = 0;
    uint32_t originalRendererBaseEffect = 0;
    uintptr_t baseEffectOffset = 0;
    uint8_t originalCacheFlags = 0;
    uint8_t cacheBytes[0x15C]{};
};
thread_local FlipEffectSwapState gSwapState{};

bool IsFlipflopCar(uintptr_t owner)
{
    uint8_t enabled = injector::ReadMemory<uint8_t>(owner + 0x68A4, true);
    if (!owner || !enabled)
        return false;
    return enabled != 0;
}

OwnerRecord* FindOwnerLocked(uintptr_t owner, bool create);

bool IsTrackedOwner(uintptr_t owner)
{
    if (!owner) return false;
    AcquireSRWLockShared(&gOwnerLock);
    const bool tracked = FindOwnerLocked(owner, false) != nullptr;
    ReleaseSRWLockShared(&gOwnerLock);
    return tracked;
}

OwnerRecord* FindOwnerLocked(uintptr_t owner, bool create)
{
    OwnerRecord* freeSlot = nullptr;
    for (auto& record : gOwners)
    {
        if (record.owner == owner)
            return &record;
        if (!record.owner && !freeSlot)
            freeSlot = &record;
    }
    if (!create || !freeSlot)
        return nullptr;
    memset(freeSlot, 0, sizeof(*freeSlot));
    freeSlot->owner = owner;
    freeSlot->color1 = injector::ReadMemory<uint32_t>(owner + 0x68A8, true);
    freeSlot->color2 = injector::ReadMemory<uint32_t>(owner + 0x68AC, true);
    freeSlot->color1 &= 0x00FFFFFFu;
    freeSlot->color2 &= 0x00FFFFFFu;
    return freeSlot;
}

bool ContainsTarget(const OwnerRecord& record, uint16_t id)
{
    for (uint16_t i = 0; i < record.targetCount; ++i)
        if (record.targets[i] == id)
            return true;
    return false;
}

bool ContainsObserved(const OwnerRecord& record, uintptr_t mesh, uint16_t id)
{
    for (uint16_t i = 0; i < record.observedCount; ++i)
        if (record.observed[i].mesh == mesh && record.observed[i].materialId == id)
            return true;
    return false;
}

extern "C" void __cdecl FFTraceResetOwner(uintptr_t owner)
{
    if (!owner)
        return;
    uint8_t enabled = injector::ReadMemory<uint8_t>(owner + 0x68A4, true);
    if (!enabled)
        return;

    uint32_t rawColor1 = 0, rawColor2 = 0;
    rawColor1 = injector::ReadMemory<uint32_t>(owner + 0x68A8, true);
    rawColor2 = injector::ReadMemory<uint32_t>(owner + 0x68AC, true);
    const uint32_t color1 = rawColor1 & 0x00FFFFFFu;
    const uint32_t color2 = rawColor2 & 0x00FFFFFFu;

    AcquireSRWLockExclusive(&gOwnerLock);
    OwnerRecord* record = FindOwnerLocked(owner, true);
    if (record)
    {
        record->color1 = color1;
        record->color2 = color2;
        record->targetCount = 0;
        record->observedCount = 0;
        record->targetOverflow = 0;
        record->observedOverflow = 0;
    }
    ReleaseSRWLockExclusive(&gOwnerLock);

    InterlockedExchange(&gRaceContextLogCount, 0);
    InterlockedExchange(&gRaceSubmitLogCount, 0);
    InterlockedExchange(&gRaceSkipLogCount, 0);
}

extern "C" void __cdecl FFTraceRecordMaterialLookup(uintptr_t owner, uintptr_t modePointer)
{
    if (!owner)
        return;
    uint8_t enabled = injector::ReadMemory<uint8_t>(owner + 0x68A4, true);
    if (!enabled)
        return;

    uint32_t renderer = CDRender();
    if (!renderer || modePointer < renderer + 0xD3BC)
        return;
    const uintptr_t delta = modePointer - (renderer + 0xD3BC);
    if ((delta % 0x1CC) != 0)
        return;
    const uintptr_t materialIndexWide = delta / 0x1CC;
    if (materialIndexWide >= 0x400)
        return;
    const uint16_t materialId = static_cast<uint16_t>(materialIndexWide);

    bool inserted = false;
    uint32_t color1 = 0, color2 = 0;
    AcquireSRWLockExclusive(&gOwnerLock);
    OwnerRecord* record = FindOwnerLocked(owner, true);
    if (record)
    {
        color1 = record->color1;
        color2 = record->color2;
        if (!ContainsTarget(*record, materialId))
        {
            if (record->targetCount < 64)
            {
                record->targets[record->targetCount++] = materialId;
                inserted = true;
            }
            else
            {
                record->targetOverflow = 1;
            }
        }
    }
    ReleaseSRWLockExclusive(&gOwnerLock);
}

extern "C" void __cdecl FFTraceBeginRender(uintptr_t mesh, uintptr_t owner)
{
    uint32_t vtable = injector::ReadMemory<uint32_t>(owner + 0xD4, true);
    uint32_t enabled = injector::ReadMemory<uint32_t>(owner + 0x68A4, true);
    const bool trackedOwner = IsTrackedOwner(owner);
    if (trackedOwner || (vtable == 0x705CC4 && enabled != 0))
    {
        LONG sample = InterlockedIncrement(&gRaceContextLogCount);
        if (sample <= 80)
        {
            uint32_t color1 = injector::ReadMemory<uint32_t>(owner + 0x68A8, true);
            uint32_t color2 = injector::ReadMemory<uint32_t>(owner + 0x68AC, true);
        }
    }
    if (gContextOverflow || gContextDepth >= 64)
    {
        ++gContextOverflow;
        return;
    }
    gContextStack[gContextDepth++] = { gCurrentOwner, gCurrentMesh };
    gCurrentOwner = IsFlipflopCar(owner) ? owner : 0;
    gCurrentMesh = mesh;
}

extern "C" void __cdecl FFTraceEndRender()
{
    if (gContextOverflow)
    {
        --gContextOverflow;
        return;
    }
    if (!gContextDepth)
    {
        gCurrentOwner = 0;
        gCurrentMesh = 0;
        return;
    }
    const RenderContext previous = gContextStack[--gContextDepth];
    gCurrentOwner = previous.previousOwner;
    gCurrentMesh = previous.previousMesh;
}

extern "C" void __cdecl FFTraceMaterialSubmit(uint32_t materialIdRaw)
{
    const uintptr_t owner = gCurrentOwner;
    const uintptr_t mesh = gCurrentMesh;
    if (!owner || !mesh || !IsFlipflopCar(owner))
    {
        return;
    }
    const uint16_t materialId = static_cast<uint16_t>(materialIdRaw & 0xFFFFu);

    bool inserted = false;
    bool isTarget = false;
    uint16_t targetCount = 0;
    uint32_t color1 = 0, color2 = 0;
    uint32_t currentMode = 0xFFFFFFFFu;
    uint32_t renderer = CDRender();
    if (materialId < 0x400 && renderer)
    {
        currentMode = injector::ReadMemory<uint32_t>(renderer + 0xD3BC + static_cast<uintptr_t>(materialId) * 0x1CC, true);
    }
    AcquireSRWLockExclusive(&gOwnerLock);
    OwnerRecord* record = FindOwnerLocked(owner, false);
    if (record)
    {
        color1 = record->color1;
        color2 = record->color2;
        targetCount = record->targetCount;
        isTarget = ContainsTarget(*record, materialId);
        if (!ContainsObserved(*record, mesh, materialId))
        {
            if (record->observedCount < 256)
            {
                ObservedBatch& observed = record->observed[record->observedCount++];
                observed.mesh = mesh;
                observed.materialId = materialId;
                observed.target = isTarget ? 1 : 0;
                inserted = true;
            }
            else
            {
                record->observedOverflow = 1;
            }
        }
    }
    ReleaseSRWLockExclusive(&gOwnerLock);
}

extern "C" __declspec(naked) void FFHookCarSetupCall()
{
    __asm
    {
        pushad
        mov eax, dword ptr [esp+0x1C]
        push eax
        call FFTraceResetOwner
        add esp, 0x4
        popad
        call dword ptr [gOriginalCarSetup]
        ret
    }
}

extern "C" __declspec(naked) void FFHookTextureMaterialLookup()
{
    __asm
    {
        push ebx
        push esi
        mov esi, edx
        call dword ptr [gOriginalTextureLookup]
        mov ebx, eax
        mov ecx, dword ptr [ebp+0x46]
        push ebx  
        push ecx 
        call FFTraceRecordMaterialLookup
        add esp, 0x8
        mov eax, ebx
        pop esi
        pop ebx
        ret
    }
}

extern "C" __declspec(naked) void FFHookRenderMesh()
{
    __asm
    {
        pushad
        mov eax, dword ptr [esp+0x14] 
        mov ecx, dword ptr [esp+0x1C]
        push eax
        push ecx
        call FFTraceBeginRender 
        add esp, 0x8
        popad

        call dword ptr [gOriginalRenderMesh]

        pushad
        call FFTraceEndRender
        popad
        ret
    }
}

static uintptr_t TryCreateEffect(uint32_t factory, const char* path)
{
    uint32_t vtable = injector::ReadMemory<uint32_t>(factory, true);
    uint32_t createAddress = injector::ReadMemory<uint32_t>(static_cast<uintptr_t>(vtable) + 0x30, true);
    if (!vtable || !createAddress)
        return 0;

    using CreateEffectFn = int (__stdcall*)(int, const char*, void*);
    uintptr_t effect = 0;
    __try
    {
        effect = reinterpret_cast<CreateEffectFn>(createAddress)(static_cast<int>(factory), path, reinterpret_cast<void*>(0x6EC3FE));
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        effect = 0;
    }
    return effect;
}

static bool GetGameShaderPaths(char* gameDir, size_t gameDirSize, char* sourceFx, size_t sourceFxSize,
                               uint32_t color1, uint32_t color2, char* variantFx, size_t variantFxSize)
{
    char gameExe[MAX_PATH]{};
    const DWORD length = GetModuleFileNameA(nullptr, gameExe, MAX_PATH);
    if (!length || length >= MAX_PATH)
        return false;
    char* slash = strrchr(gameExe, '\\');
    char* forwardSlash = strrchr(gameExe, '/');
    if (!slash || (forwardSlash && forwardSlash > slash)) slash = forwardSlash;
    if (!slash) return false;
    *slash = '\0';
    if (strncpy_s(gameDir, gameDirSize, gameExe, _TRUNCATE) != 0) return false;
    if (_snprintf_s(sourceFx, sourceFxSize, _TRUNCATE, "%s\\shader\\flipflop.fx", gameDir) < 0) return false;
    if (_snprintf_s(variantFx, variantFxSize, _TRUNCATE, "%s\\shader\\flipflop_dyn_%06lX_%06lX.fx", gameDir,
                    (unsigned long)(color1 & 0xFFFFFFu), (unsigned long)(color2 & 0xFFFFFFu)) < 0) return false;
    return true;
}

static bool ReplaceColorDeclaration(std::string& source, const char* variable, uint32_t rgb)
{
    const std::string prefix = std::string("float4 ") + variable + " = float4(";
    const size_t begin = source.find(prefix);
    if (begin == std::string::npos) return false;
    const size_t end = source.find(';', begin);
    if (end == std::string::npos) return false;
    char declaration[192]{};
    const float r = static_cast<float>((rgb >> 16) & 0xFFu) / 255.0f;
    const float g = static_cast<float>((rgb >> 8) & 0xFFu) / 255.0f;
    const float b = static_cast<float>(rgb & 0xFFu) / 255.0f;
    if (_snprintf_s(declaration, sizeof(declaration), _TRUNCATE, "float4 %s = float4(%.9g, %.9g, %.9g, 1.0)", variable, r, g, b) < 0)
        return false;
    source.replace(begin, end - begin, declaration);
    return true;
}

static bool WriteDynamicShader(uint32_t color1, uint32_t color2, char* variantFx, size_t variantFxSize)
{
    char gameDir[MAX_PATH]{}, sourceFx[MAX_PATH]{};
    if (!GetGameShaderPaths(gameDir, sizeof(gameDir), sourceFx, sizeof(sourceFx),
                            color1, color2, variantFx, variantFxSize))
    {
        return false;
    }

    HANDLE input = CreateFileA(sourceFx, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (input == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(input, &size) || size.QuadPart <= 0 || size.QuadPart > 1024 * 1024)
    {
        CloseHandle(input);
        return false;
    }
    std::string source(static_cast<size_t>(size.QuadPart), '\0');
    DWORD bytesRead = 0;
    const BOOL readOk = ReadFile(input, &source[0], static_cast<DWORD>(source.size()), &bytesRead, nullptr);
    CloseHandle(input);
    if (!readOk || bytesRead != source.size())
    {
        return false;
    }
    if (!ReplaceColorDeclaration(source, "FlipFlopColor1", color1) ||
        !ReplaceColorDeclaration(source, "FlipFlopColor2", color2))
    {
        return false;
    }

    HANDLE output = CreateFileA(variantFx, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (output == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    DWORD bytesWritten = 0;
    const BOOL writeOk = WriteFile(output, source.data(), static_cast<DWORD>(source.size()), &bytesWritten, nullptr);
    FlushFileBuffers(output);
    CloseHandle(output);
    if (!writeOk || bytesWritten != source.size())
    {
        return false;
    }

    return true;
}

static void ProbeMemoryEffectPath(uintptr_t effect)
{
    static volatile LONG probed = 0;
    if (InterlockedCompareExchange(const_cast<LONG*>(&probed), 1, 0) != 0) return;

    uint32_t vtable = injector::ReadMemory<uint32_t>(effect, true);
    uint32_t getDeviceAddress = injector::ReadMemory<uint32_t>(static_cast<uintptr_t>(vtable) + 0x0C, true);
    uintptr_t device = 0;
    LONG hr = static_cast<LONG>(0x80004005u);
    if (effect && vtable && getDeviceAddress)
    {
        using GetDeviceFn = LONG (__stdcall*)(void*, void**);
        __try
        {
            hr = reinterpret_cast<GetDeviceFn>(getDeviceAddress)(reinterpret_cast<void*>(effect), reinterpret_cast<void**>(&device));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            hr = static_cast<LONG>(0x8000FFFFu);
            device = 0;
        }
    }

    if (device)
    {
        uint32_t deviceVtable = injector::ReadMemory<uint32_t>(device, true);
        uint32_t releaseAddress = injector::ReadMemory<uint32_t>(static_cast<uintptr_t>(deviceVtable) + 0x8, true);
        if (deviceVtable && releaseAddress)
        {
            using ReleaseFn = uint32_t (__stdcall*)(void*);
            __try
            {
                reinterpret_cast<ReleaseFn>(releaseAddress)(reinterpret_cast<void*>(device));
            }
            __except (EXCEPTION_EXECUTE_HANDLER) { }
        }
    }
}

static uintptr_t CreateColorEffect(uint32_t factory, uint32_t color1, uint32_t color2)
{
    char gameDir[MAX_PATH]{}, sourceFx[MAX_PATH]{}, variantFx[MAX_PATH]{};
    if (!WriteDynamicShader(color1, color2, variantFx, sizeof(variantFx)))
    {
        return 0;
    }

    uintptr_t effect = TryCreateEffect(factory, variantFx);
    const BOOL deleted = DeleteFileA(variantFx);
    const DWORD deleteError = deleted ? ERROR_SUCCESS : GetLastError();
    const bool sourceClean = deleted || deleteError == ERROR_FILE_NOT_FOUND;

    if (!effect)
    {
        return 0;
    }

    ProbeMemoryEffectPath(effect);

    uint32_t vtable = injector::ReadMemory<uint32_t>(effect, true);
    uint32_t getParameterAddress = injector::ReadMemory<uint32_t>(static_cast<uintptr_t>(vtable) + 0x14, true);
    if (!vtable || !getParameterAddress)
    {
        return 0;
    }

    using GetParameterFn = uint32_t (__stdcall*)(int, const char*);
    uint32_t c1 = 0, c2 = 0;
    __try
    {
        c1 = reinterpret_cast<GetParameterFn>(getParameterAddress)(static_cast<int>(effect), "FlipFlopColor1");
        c2 = reinterpret_cast<GetParameterFn>(getParameterAddress)(static_cast<int>(effect), "FlipFlopColor2");
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        c1 = c2 = 0;
    }

    if (!c1 || !c2)
    {
        return 0;
    }

    return effect;
}

static uintptr_t EnsureFlipEffectForColors(uint32_t color1, uint32_t color2)
{
    color1 &= 0x00FFFFFFu;
    color2 &= 0x00FFFFFFu;
    AcquireSRWLockExclusive(&gColorEffectLock);
    ColorEffectEntry* entry = nullptr;
    ColorEffectEntry* freeEntry = nullptr;
    for (auto& candidate : gColorEffects)
    {
        if (candidate.used && candidate.color1 == color1 && candidate.color2 == color2)
        {
            entry = &candidate;
            break;
        }
        if (!candidate.used && !freeEntry) freeEntry = &candidate;
    }
    if (!entry)
    {
        if (!freeEntry)
        {
            ReleaseSRWLockExclusive(&gColorEffectLock);
            return 0;
        }
        entry = freeEntry;
        entry->used = true;
        entry->color1 = color1;
        entry->color2 = color2;
    }
    if (entry->attempted)
    {
        const uintptr_t result = entry->effect;
        ReleaseSRWLockExclusive(&gColorEffectLock);
        return result;
    }
    entry->attempted = true;
    uint32_t factory = injector::ReadMemory<uint32_t>(0x7CF6FC, true);
    if (factory)
        entry->effect = CreateColorEffect(factory, color1, color2);
    const uintptr_t result = entry->effect;
    ReleaseSRWLockExclusive(&gColorEffectLock);
    return result;
}

extern "C" void __cdecl FFBeginFlipEffect(uint32_t materialIdRaw, uint32_t unk_bits)
{
    gSwapState.active = false;
    const uintptr_t owner = gCurrentOwner;
    const uint16_t materialId = static_cast<uint16_t>(materialIdRaw & 0xFFFFu);
    if (!owner || !gCurrentMesh || !IsFlipflopCar(owner))
    {
        return;
    }

    uint32_t color1 = 0, color2 = 0, renderer = 0, mode = 0xFFFFFFFFu;
    bool target = false;
    AcquireSRWLockShared(&gOwnerLock);
    OwnerRecord* record = FindOwnerLocked(owner, false);
    if (record)
    {
        target = ContainsTarget(*record, materialId);
        color1 = record->color1;
        color2 = record->color2;
    }
    ReleaseSRWLockShared(&gOwnerLock);
    if (!target)
    {
        return;
    }

    renderer = CDRender();
    if (!renderer || materialId >= 0x400)
    {
        return;
    }

	mode = injector::ReadMemory<uint32_t>(static_cast<uintptr_t>(renderer) + 0xD3BC + static_cast<uintptr_t>(materialId) * 0x1CC, true);
    if (!mode || mode != 4)
    {
        return;
    }

    const uintptr_t colorEffect = EnsureFlipEffectForColors(color1, color2);
    if (!colorEffect)
    {
        return;
    }

    float unk = 0.f;
    memcpy(&unk, &unk_bits, sizeof(unk));

    if (unk < 0.f)
    {
        if (!gLoggedAlphaSkip)
        {
            gLoggedAlphaSkip = true;
        }
        return;
    }

    const uintptr_t envmapSlot = static_cast<uintptr_t>(renderer) + 0xD36C;
    const uintptr_t baseSlot = static_cast<uintptr_t>(renderer) + 0xD368;

    if (!injector::ReadMemoryRaw(envmapSlot, &gSwapState.originalRendererEnvmapEffect, sizeof(uint32_t), true) ||
        !injector::ReadMemoryRaw(baseSlot, &gSwapState.originalRendererBaseEffect, sizeof(uint32_t), true) ||
        !injector::ReadMemoryRaw(0x7DDEB0, gSwapState.cacheBytes, 0x15C, true) ||
        !injector::ReadMemoryRaw(0x7DE120, &gSwapState.originalCacheFlags, sizeof(gSwapState.originalCacheFlags), true))
    {
        return;
    }
    gSwapState.renderer = renderer;

    if (!injector::WriteMemory<uint32_t>(envmapSlot, colorEffect, true) ||
        !injector::WriteMemory<uint32_t>(baseSlot, colorEffect, true))
    {
        injector::WriteMemory<uint32_t>(envmapSlot, gSwapState.originalRendererEnvmapEffect, true);
        injector::WriteMemory<uint32_t>(baseSlot, gSwapState.originalRendererBaseEffect, true);
        return;
    }
    const uint8_t resetFlags = static_cast<uint8_t>(gSwapState.originalCacheFlags & ~(0x10u | 0x01u));
    if (!injector::WriteMemory<uint8_t>(0x7DE120, resetFlags, true))
    {
        injector::WriteMemory<uint32_t>(envmapSlot, gSwapState.originalRendererEnvmapEffect, true);
        injector::WriteMemory<uint32_t>(baseSlot, gSwapState.originalRendererBaseEffect, true);
        return;
    }
    gSwapState.active = true;
}

extern "C" void __cdecl FFEndFlipEffect()
{
    if (!gSwapState.active) return;
    const uintptr_t envmapSlot = gSwapState.renderer + 0xD36C;
    const uintptr_t baseSlot = gSwapState.renderer + 0xD368;
	injector::WriteMemory<uint32_t>(envmapSlot, gSwapState.originalRendererEnvmapEffect, true);
	injector::WriteMemory<uint32_t>(baseSlot, gSwapState.originalRendererBaseEffect, true);
	injector::WriteMemory<uint8_t>(0x7DE120, static_cast<uint8_t>(gSwapState.originalCacheFlags), true);
    injector::WriteMemoryRaw(0x7DDEB0, gSwapState.cacheBytes, 0x15C, true);
    gSwapState.active = false;
}

extern "C" void __cdecl FFPushSubmitReturnAddress(uintptr_t address)
{
    if (gSubmitReturnDepth < sizeof(gSubmitReturnStack) / sizeof(gSubmitReturnStack[0]))
    {
        gSubmitReturnStack[gSubmitReturnDepth++] = address;
    }
}

extern "C" void __cdecl FFPopSubmitReturnAddress()
{
    if (gSubmitReturnDepth)
    {
        gSubmitReturnAddressForStub = gSubmitReturnStack[--gSubmitReturnDepth];
    }
    else
    {
        gSubmitReturnAddressForStub = 0;
    }
}

extern "C" void FFMaterialSubmitReturnStub();

extern "C" __declspec(naked) void FFHookMaterialSubmit()
{
    __asm
    {
        pushfd
        pushad
        mov eax, dword ptr [esp+0x24]
        push eax
        call FFPushSubmitReturnAddress
        add esp, 0x4
        mov eax, dword ptr [esp+0x10]
        and eax, 0xFFFF
        push eax
        call FFTraceMaterialSubmit
        add esp, 0x4
        mov edx, dword ptr [esp+0x28]
        mov eax, dword ptr [esp+0x10]
        and eax, 0xFFFF
        push edx
        push eax
        call FFBeginFlipEffect
        add esp, 0x8
        popad
        popfd

        mov dword ptr [esp], offset FFMaterialSubmitReturnStub
        jmp dword ptr [gOriginalMaterialSubmit]
    }
}

extern "C" __declspec(naked) void FFMaterialSubmitReturnStub()
{
    __asm
    {
        pushfd
        pushad
        call FFEndFlipEffect
        call FFPopSubmitReturnAddress
        popad
        popfd
        push dword ptr [gSubmitReturnAddressForStub]
        ret
    }
}

void XFlipFlop()
{
    injector::MakeCALL(0x44A2B9, FFHookCarSetupCall, true);
    injector::MakeCALL(0x44B3A5, FFHookCarSetupCall, true);
    injector::MakeCALL(0x44D2F2, FFHookTextureMaterialLookup, true);
    injector::MakeCALL(0x601707, FFHookRenderMesh, true);
    injector::MakeCALL(0x601798, FFHookRenderMesh, true);
    injector::MakeCALL(0x5E8F37, FFHookMaterialSubmit, true);
}