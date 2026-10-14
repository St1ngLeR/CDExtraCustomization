bool nitrocolorbtn_state = true;

const char* NitroColor_Key = "NITROCOLOR";

static char gButtonName[] = "btnOpenNitroColor";
static uintptr_t gButton = 0;

static const uint32_t gPresets[12] =
{
    0xFFD23Fu, 0x070CAAu, 0xA585C0, 0x298716,
    0xD76993u, 0x4F24C1, 0xBF00FFu, 0x97DBF1,
    0x416143u, 0xCFCF59, 0x878787u, 0xFA0505
};

static uint8_t gColorsOpen = 0;
static uint32_t gColor = 0xFFD23Fu;
static uint32_t gFlameColor = 0;
static uintptr_t gPreviewCar = 0;
static uintptr_t gPreviewFlame = 0;

static uintptr_t GetRenderer()
{
    return *(uintptr_t*)0x7CF700;
}

static double HueToRgb(double p, double q, double t)
{
    if (t < 0.0) t += 1.0;
    if (t > 1.0) t -= 1.0;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}

static uint8_t ToByte(double value)
{
    return value <= 0.0 ? 0 : (value >= 1.0 ? 255 : (uint8_t)(value * 255.0));
}

static uint32_t FlameColor(uint32_t rgb, double lightness)
{
    const double r0 = (double)((rgb >> 16) & 0xFFu) / 255.0;
    const double g0 = (double)((rgb >> 8) & 0xFFu) / 255.0;
    const double b0 = (double)(rgb & 0xFFu) / 255.0;
    const double lo = r0 < g0 ? (r0 < b0 ? r0 : b0) : (g0 < b0 ? g0 : b0);
    const double hi = r0 > g0 ? (r0 > b0 ? r0 : b0) : (g0 > b0 ? g0 : b0);
    const double delta = hi - lo;
    const double light = (hi + lo) * 0.5;

    double h = 0.0;
    double s = 0.0;
    if (delta != 0.0)
    {
        s = light > 0.5 ? delta / (2.0 - hi - lo) : delta / (hi + lo);
        if (hi == r0) h = (g0 - b0) / delta + (g0 < b0 ? 6.0 : 0.0);
        else if (hi == g0) h = (b0 - r0) / delta + 2.0;
        else h = (r0 - g0) / delta + 4.0;
        h /= 6.0;
    }

    double r = lightness, g = lightness, b = lightness;
    if (s != 0.0)
    {
        const double q = lightness * (1.0 + s);
        const double p = 2.0 * lightness - q;
        r = HueToRgb(p, q, h + 1.0 / 3.0);
        g = HueToRgb(p, q, h);
        b = HueToRgb(p, q, h - 1.0 / 3.0);
    }

    return ((uint32_t)ToByte(r) << 16) | ((uint32_t)ToByte(g) << 8) | (uint32_t)ToByte(b);
}

static const int gColorSlots = 8;

struct ColorSlot
{
    uintptr_t key;
    uint32_t color;
    uint32_t drawColor;
    uint32_t forced;
    bool random;
};

static ColorSlot gParamColors[gColorSlots] = {};
static ColorSlot gCarColors[gColorSlots] = {};
static uint32_t gNextParam = 0;
static uint32_t gNextCar = 0;
enum { gCarDwords = sizeof(gCarColors) / 4 };
static uintptr_t gParseStack = 0;
static uint8_t gColorRead = 0;
static uintptr_t gGame = 0;

struct GameText
{
    uintptr_t storage;
    uint32_t offset;
    uint32_t length;
    uint32_t reserved;
};

static const char* TextData(const GameText* value)
{
    if (!value || value->storage == 0 || value->length == 0) return 0;
    return (const char*)(value->storage + value->offset + 0x0C);
}

static bool TextEquals(const GameText* value, const char* text)
{
    if (!value || value->length != strlen(text)) return false;
    if (!value->length) return true;
    const char* data = TextData(value);
    return data && memcmp(data, text, value->length) == 0;
}

static bool TextContains(const GameText* value, const char* text)
{
    if (!value) return false;
    const size_t length = strlen(text);
    if (!length) return true;
    const char* data = TextData(value);
    if (!data || value->length < length) return false;
    for (size_t i = 0; i <= value->length - length; ++i)
    {
        if (memcmp(data + i, text, length) == 0)
            return true;
    }
    return false;
}

static uint32_t Tint(uint32_t selected)
{
    selected &= 0xFFFFFF;
    return selected == gPresets[0] ? 0x5A3B1Cu : FlameColor(selected, 0.22);
}

static bool CarFolder(uintptr_t car, char* output, uint32_t size)
{
    if (!car || !output || size < 2) return false;

    const GameText* folder = (const GameText*)(car + 0x48B8);
    const char* text = TextData(folder);
    if (!text || !folder->length || folder->length >= size || folder->length > 127) return false;

    memcpy(output, text, folder->length);
    output[folder->length] = 0;
    return true;
}

static bool ColorPath(uintptr_t car, char path[260])
{
    char folder[128] = { 0 };
    if (!CarFolder(car, folder, sizeof(folder))) return false;
    return _snprintf_s(path, 260, _TRUNCATE, "../trkdata/cars/%s/nitrocol.cfg", folder) >= 0;
}

static bool ReadColor(const char* text, uint32_t size, uint32_t* color)
{
    uint32_t pos = 0;
    while (pos < size && text[pos] && strchr(" \t\r\n", text[pos])) ++pos;
    if (pos < size && text[pos] == '#') ++pos;
    if (size - pos < 6) return false;
    char digits[7] = { 0 };
    memcpy(digits, text + pos, 6);
    if (strspn(digits, "0123456789abcdefABCDEF") != 6) return false;
    pos += 6;
    while (pos < size)
    {
        if (text[pos] && !strchr(" \t\r\n", text[pos])) return false;
        ++pos;
    }
    *color = (uint32_t)strtoul(digits, NULL, 16);
    return true;
}

static bool SaveColor(uintptr_t car, uint32_t color)
{
    char path[260] = { 0 };
    if (!ColorPath(car, path)) return false;
    FILE* file = fopen(path, "wb");
    if (!file) return false;
    char text[7] = { 0 };
    _snprintf_s(text, sizeof(text), _TRUNCATE, "%06X", color & 0xFFFFFFu);
    const bool ok = fwrite(text, 1, 6, file) == 6;
    return fclose(file) == 0 && ok;
}

static bool LoadColor(uintptr_t car, uint32_t* selected, uint32_t* render)
{
    if (!car || !selected || !render) return false;
    char path[260] = { 0 };
    if (!ColorPath(car, path)) return false;
    uint32_t color = gPresets[0];
    bool valid = false;
    FILE* file = fopen(path, "rb");
    if (file)
    {
        char text[32] = { 0 };
        const size_t size = fread(text, 1, sizeof(text) - 1, file);
        if (!ferror(file)) valid = ReadColor(text, (uint32_t)size, &color);
        fclose(file);
    }
    if (!valid)
    {
        color = gPresets[0];
        SaveColor(car, color);
    }
    *selected = color;
    *render = Tint(color);
    return true;
}

static uint32_t GarageCar()
{
    const uintptr_t controller = *(const uint32_t*)0x7BC998;
    return controller ? *(const uint32_t*)(controller + 0x1C) : 0;
}

static bool IsPreviewCar(uintptr_t car)
{
    if (!car || car != gPreviewCar || !gPreviewFlame) return false;
    return *(const uint32_t*)0x7BC998 != 0;
}

static uint32_t PlayerCar()
{
    const uintptr_t playerTable = *(const uint32_t*)0x78CEE0;
    if (!playerTable) return 0;

    const uintptr_t player = *(const uint32_t*)playerTable;
    if (!player) return 0;

    uintptr_t car = 0;
    __asm
    {
        pushad
        mov eax, player
        mov edi, 05DA0E0h
        call edi
        mov car, eax
        popad
    }
    return car;
}

static __declspec(naked) uint32_t __cdecl ReadEventColor(
    const GameText* details, const GameText* token)
{
    __asm
    {
        push ebx
        push esi
        sub esp, 40

        mov ecx, 0FFFFFFFFh
        mov edx, dword ptr[esp + 56]
        lea eax, dword ptr[esp]
        xor ebx, ebx
        mov esi, 0696097h
        call esi

        mov ebx, eax
        mov edx, dword ptr[esp + 52]
        lea eax, dword ptr[esp + 16]
        mov esi, 0677C00h
        call esi

        mov esi, 06781C0h
        call esi
        mov dword ptr[esp + 32], eax

        xor edx, edx
        lea eax, dword ptr[esp + 16]
        mov esi, 06959C9h
        call esi
        xor edx, edx
        lea eax, dword ptr[esp]
        mov esi, 06959C9h
        call esi

        mov eax, dword ptr[esp + 32]
        add esp, 40
        pop esi
        pop ebx
        ret
    }
}

static void ClearColor(ColorSlot table[gColorSlots], uintptr_t key)
{
    if (!key) return;
    for (uint32_t i = 0; i < gColorSlots; ++i)
    {
        if (table[i].key == key)
            table[i] = {};
    }
}

static void SetColor(ColorSlot table[gColorSlots], uint32_t& next, uintptr_t key,
    uint32_t color, uint32_t drawColor, uint32_t forced)
{
    if (!key) return;
    for (uint32_t i = 0; i < gColorSlots; ++i)
    {
        if (table[i].key == key)
        {
            table[i] = { key, color, drawColor, forced, false };
            return;
        }
    }
    for (uint32_t i = 0; i < gColorSlots; ++i)
    {
        if (!table[i].key)
        {
            table[i] = { key, color, drawColor, forced, false };
            return;
        }
    }
    const uint32_t slot = next % gColorSlots;
    table[slot] = { key, color, drawColor, forced, false };
    next = (slot + 1) % gColorSlots;
}

static ColorSlot* FindColor(ColorSlot* colors, uintptr_t key)
{
    if (!key) return NULL;
    for (int i = 0; i < gColorSlots; ++i)
    {
        if (colors[i].key == key)
            return &colors[i];
    }
    return NULL;
}

static void __cdecl ClearParamColor(uintptr_t params)
{
    ClearColor(gParamColors, params);
}

static void __cdecl ReadForceColor(uintptr_t frame, uintptr_t params, uintptr_t details)
{
    gColorRead = 0;
    const GameText* key = (const GameText*)(frame + 0x520);
    if (!TextEquals(key, "forcenitrocolor")) return;

    const GameText* token = (const GameText*)(frame + 0x510);
    const uint32_t selected = ReadEventColor(
        (const GameText*)details, token) & 0xFFFFFF;
    const uint32_t render = Tint(selected);
    SetColor(gParamColors, gNextParam, params, selected, render, 0);
    gColorRead = 1;
}

static void __cdecl SetCarColor(uintptr_t car, uintptr_t params)
{
    ClearColor(gCarColors, car);
    const ColorSlot* color = FindColor(gParamColors, params);
    if (color)
        SetColor(gCarColors, gNextCar, car, color->color, color->drawColor, 1);
}

static void __cdecl ReadCarColor(uintptr_t car)
{
    if (!car) return;
    const ColorSlot* color = FindColor(gCarColors, car);
    uint32_t selected = 0;
    uint32_t render = 0;
    if (!LoadColor(car, &selected, &render)) return;
    if (!color || !color->forced)
        SetColor(gCarColors, gNextCar, car, selected, render, 0);
}

static bool IsBot(uintptr_t car)
{
    if (!car || *(uintptr_t*)0x7CD0E0) return false;
    const uintptr_t players = *(uintptr_t*)0x78CEE0;
    const int count = *(uint8_t*)0x78CEDC;
    if (!players || count < 1 || count > 8) return false;
    if (!gGame || *(const uint8_t*)gGame != 0) return false;
    for (int i = 0; i < count; ++i)
    {
        const uintptr_t player = *(uintptr_t*)(players + i * 8);
        if (player && *(uintptr_t*)(player + 0xF6EC) == car)
            return *(uintptr_t*)(player + 0xF6F4) == 0x708D44;
    }
    return false;
}

static uint32_t __cdecl CarColor(uintptr_t car)
{
    ColorSlot* color = FindColor(gCarColors, car);
    if (color && color->forced) return color->drawColor;
    if (IsBot(car))
    {
        if (!color || !color->random)
        {
            const uint32_t selected = gPresets[rand() % (sizeof(gPresets) / sizeof(gPresets[0]))];
            SetColor(gCarColors, gNextCar, car, selected, Tint(selected), 0);
            color = FindColor(gCarColors, car);
            if (color) color->random = true;
        }
        return color ? color->drawColor : 0x5A3B1Cu;
    }

    if (!color || color->random) return 0x5A3B1Cu;
    return car == PlayerCar() || IsPreviewCar(car) ? color->drawColor : 0x5A3B1Cu;
}

static uint8_t gTextureActive = 0;
static uint16_t gOldTexture = 0xFFFFu;
static uint16_t gColorTexture = 0xFFFFu;
static uint16_t gTailTexture = 0xFFFFu;
static uint32_t gTextureColor = 0xFFFFFFFFu;
static char gColorPath[256] = { 0 };
static char gTailPath[256] = { 0 };
static uintptr_t gDrawModel = 0x6015A0;

static bool FlamePath(const char* source, const char* name, char out[256])
{
    const size_t length = strnlen(source, 256);
    if (!length || length >= 255) return false;
    const char* slash = strrchr(source, '/');
    const char* backslash = strrchr(source, '\\');
    if (!slash || (backslash && backslash > slash)) slash = backslash;
    const char* start = slash ? slash + 1 : source;
    const char* dot = strrchr(start, '.');
    const char* end = dot ? dot : source + length;
    if (end - start != 5 || _strnicmp(start, "flame", 5) != 0) return false;
    return _snprintf_s(out, 256, _TRUNCATE, "%.*s%s", (int)(start - source), source, name) >= 0;
}

static uint16_t LoadTexture(const char* path)
{
    const uintptr_t renderer = GetRenderer();
    if (!renderer || !path || !path[0]) return 0xFFFFu;

    uint32_t id = 0xFFFFu;
    __asm
    {
        pushad
        push 0
        mov edx, path
        mov eax, renderer
        mov ecx, 05E4D20h
        call ecx
        movzx eax, ax
        mov id, eax
        popad
    }
    return (uint16_t)id;
}

static uintptr_t TextureImage(const char* path)
{
    const uintptr_t renderer = GetRenderer();
    if (!renderer || !path || !path[0]) return 0;

    uintptr_t image = 0;
    __asm
    {
        pushad
        mov edx, path
        mov eax, renderer
        mov ecx, 05E7240h
        call ecx
        mov image, eax
        popad
    }
    return image;
}

static void FreeTexture(const char* path)
{
    const uintptr_t renderer = GetRenderer();
    if (!renderer || !path || !path[0]) return;

    __asm
    {
        pushad
        mov edx, path
        mov eax, renderer
        mov ecx, 05E6CE0h
        call ecx
        popad
    }
}

static void PaintTexture(uintptr_t image, uint32_t color)
{
    if (!image) return;
    __asm
    {
        pushad
        mov eax, image
        mov edx, color
        mov ecx, 05E3CB0h
        call ecx
        popad
    }
}

static void MixTextures(uintptr_t target, uintptr_t source)
{
    if (!target || !source) return;
    __asm
    {
        pushad
        mov eax, target
        mov edx, source
        mov ecx, 05E2FF0h
        call ecx
        popad
    }
}


static uint32_t TextureColor(uintptr_t flame)
{
    if (!flame)
        return gPresets[0];

    // В меню разрешаем перекраску только пламени выбранной машины.
    if (gColorsOpen)
    {
        const uintptr_t car = *(const uintptr_t*)(flame + 0x930);
        if (car == gPreviewCar && IsPreviewCar(car))
            return gColor & 0xFFFFFFu;

        return gPresets[0];
    }

    // Во время гонки используем перекраску только для машины игрока.
    const uintptr_t car = *(const uintptr_t*)(flame + 0x930);
    if (!car || car != PlayerCar())
        return gPresets[0];

    const ColorSlot* color = FindColor(gCarColors, car);
    if (!color || color->random)
        return gPresets[0];

    return color->color & 0xFFFFFFu;
}


//static uint32_t TextureColor(uintptr_t flame)
//{
//    if (gColorsOpen) return gColor & 0xFFFFFFu;
//    if (!flame) return gPresets[0];
//    const uintptr_t car = *(uintptr_t*)(flame + 0x930);
//    const ColorSlot* color = FindColor(gCarColors, car);
//    if (!color || color->random || (car != PlayerCar() && !IsPreviewCar(car)))
//        return gPresets[0];
//    return color->color & 0xFFFFFFu;
//}

static bool LoadTailTexture(const char* path, uintptr_t* image)
{
    if (strcmp(gTailPath, path) != 0)
    {
        if (gTailTexture != 0xFFFFu && gTailPath[0])
            FreeTexture(gTailPath);
        gTailTexture = 0xFFFFu;
        strncpy_s(gTailPath, sizeof(gTailPath), path, _TRUNCATE);
    }

    if (gTailTexture == 0xFFFFu)
        gTailTexture = LoadTexture(gTailPath);
    if (gTailTexture == 0xFFFFu) return false;

    *image = TextureImage(gTailPath);
    return *image && !*(const uint8_t*)(*image + 0x348);
}

static bool LoadColorTexture(const char* path, const char* tail, uint32_t color)
{
    if (strcmp(gColorPath, path) != 0 ||
        (gColorTexture != 0xFFFFu && gTextureColor != color))
    {
        if (gColorTexture != 0xFFFFu && gColorPath[0])
            FreeTexture(gColorPath);
        gColorTexture = 0xFFFFu;
        gTextureColor = 0xFFFFFFFFu;
        strncpy_s(gColorPath, sizeof(gColorPath), path, _TRUNCATE);
    }
    if (gColorTexture == 0xFFFFu)
    {
        uintptr_t tailImage = 0;
        if (!LoadTailTexture(tail, &tailImage)) return false;
        gColorTexture = LoadTexture(gColorPath);
        if (gColorTexture == 0xFFFFu) return false;
        const uintptr_t image = TextureImage(gColorPath);
        if (!image || *(const uint8_t*)(image + 0x348))
        {
            FreeTexture(gColorPath);
            gColorTexture = 0xFFFFu;
            return false;
        }
        PaintTexture(image, FlameColor(color, 0.55));
        MixTextures(image, tailImage);
        gTextureColor = color;
    }
    return true;
}

static void __cdecl BeginFlame(uintptr_t flame)
{
    gTextureActive = 0;
    gOldTexture = 0xFFFFu;
    if (!flame) return;

    if (gColorsOpen && *(const uint32_t*)(flame + 0x930) == gPreviewCar)
        gPreviewFlame = flame;

    const uint32_t color = TextureColor(flame);
    if (color == gPresets[0]) return;

    const uint16_t count = *(const uint16_t*)(flame + 0x100);
    const uint32_t textures = *(const uint32_t*)(flame + 0x104);
    if (!count || count > 64 || !textures) return;

    for (uint16_t i = 0; i < count; ++i)
    {
        const uint32_t record = textures + (uint32_t)i * 0x102;
        char colorPath[256] = { 0 };
        char tail[256] = { 0 };
        if (!FlamePath((const char*)record, "flamecolor.tga", colorPath)) continue;
        if (!FlamePath((const char*)record, "flame2.tga", tail)) return;
        if (!LoadColorTexture(colorPath, tail, color)) return;
        gOldTexture = *(const uint16_t*)(record + 0x100);
        gTextureActive = 1;
        return;
    }
}

static void __cdecl EndFlame()
{
    gTextureActive = 0;
}

struct Camera
{
    uint32_t angleX;
    uint32_t angleY;
    uint32_t angleZ;
    uint32_t scale;
    uint32_t fov;
    uint32_t offsetX;
    uint32_t offsetY;
    uint32_t offsetZ;
};

static const Camera gCamWrecker =
{
    0xC0A00000, 0xC1000000, 0xC1200000, 0x3FB851ECu,
    0x41E00000, 0xC0126E98u, 0x3E87AE14, 0x000000
};
static const Camera gCamSpecial =
{
    0x428A0000, 0x000000, 0xC1000000, 0x3FC872B0,
    0x41E80000, 0x3EFDF3B6, 0xBE9EB852u, 0x000000
};
static const Camera gCamLong =
{
    0x43160000, 0x000000, 0xC1000000, 0x3FF47AE1,
    0x42240000, 0x3F09BA5Eu, 0xBF266666, 0x000000
};
static const Camera gCamHunter =
{
    0x431A0000, 0x000000, 0xC1000000, 0x3FF47AE1,
    0x42240000, 0x3E970A3Du, 0xBEDFBE77u, 0x000000
};
static const Camera gCamDefault =
{
    0x43160000, 0x000000, 0xC1000000, 0x3FF47AE1,
    0x42240000, 0x3F09BA5Eu, 0xBEF851ECu, 0x000000
};
static const Camera gCamJudge =
{
    0x43170000, 0x000000, 0xC1000000, 0x3FE7AE14,
    0x42240000, 0x3F07AE14, 0xBE800000, 0x000000
};

static void SetCamera(const Camera& preset)
{
    *(volatile uint32_t*)0x78E6C0 = preset.angleX;
    *(volatile uint32_t*)0x78E6C4 = preset.angleY;
    *(volatile uint32_t*)0x78E6C8u = preset.angleZ;
    *(volatile uint32_t*)0x70DAB4 = preset.scale;
    *(volatile uint32_t*)0x70DAB8u = preset.fov;
    *(volatile uint32_t*)0x78E6CCu = preset.offsetX;
    *(volatile uint32_t*)0x78E6D0 = preset.offsetY;
    *(volatile uint32_t*)0x78E6D4 = preset.offsetZ;
}

static void __cdecl ChooseCamera(uintptr_t controller)
{
    if (controller == 0) return;
    const uintptr_t car = *(const uint32_t*)(controller + 0x1C);
    if (car == 0) return;

    const GameText* model = (const GameText*)(car + 0x6810);
    const GameText* bumper = (const GameText*)(car + 0x6824);

    const Camera* preset = &gCamDefault;
    if (TextEquals(model, "wrecker"))
        preset = &gCamWrecker;
    else if (TextEquals(model, "judge") || TextEquals(model, "ironhorze"))
        preset = &gCamJudge;
    else if ((TextEquals(model, "firespitter") && TextContains(bumper, "rb1")) ||
        (TextEquals(model, "pickup") && TextContains(bumper, "rb2")))
        preset = &gCamSpecial;
    else if (TextEquals(model, "wolf") ||
        (TextEquals(model, "hunter") &&
            (TextContains(bumper, "rb4") || TextContains(bumper, "rb5"))) ||
        (TextEquals(model, "incubator") && TextContains(bumper, "rb2")) ||
        TextEquals(model, "pickup"))
        preset = &gCamLong;
    else if (TextEquals(model, "hunter") &&
        (TextContains(bumper, "rb1") || TextContains(bumper, "rb2")))
        preset = &gCamHunter;
    SetCamera(*preset);
}

static void ShowColors()
{
    __asm
    {
        pushad
        mov edi, 52D600h
        call edi
        popad
    }
}

static void HideColors()
{
    __asm
    {
        pushad
        mov edi, 52CE50h
        call edi
        popad
    }
}

static void SetButtonColor(void* object, uint32_t color)
{
    __asm
    {
        pushad
        mov eax, object
        mov edx, color
        mov edi, 4AFC20h
        call edi
        popad
    }
}

// Synchronize the game's native RGB -> HSV state. 0x79620C is the HSV
// triple consumed by the stock picker callbacks (picColBar/Saturation/etc.).
// Updating picColorCustom alone only changes the swatch, not this state.
static void SyncNativeColorPicker(uint32_t rgb)
{
    rgb &= 0xFFFFFFu;

    // Hue хранится в первом DWORD HSV-блока.
    const uint32_t oldHue = *(uint32_t*)0x79620C;

    __asm
    {
        pushad
        mov eax, rgb
        mov edx, 079620Ch
        mov edi, 0483E50h
        call edi
        popad
    }

    const uint8_t r = (rgb >> 16) & 0xFF;
    const uint8_t g = (rgb >> 8) & 0xFF;
    const uint8_t b = rgb & 0xFF;

    // У ахроматических цветов Hue не определён.
    if (r == g && g == b)
        *(uint32_t*)0x79620C = oldHue;

    // These two flags are mouse-drag state, not generic redraw requests.
    // Setting either to 1 makes the stock picker behave as if the mouse is
    // still dragging inside a slider/saturation area, trapping interaction
    // there. Reset them when seeding the picker from a preset/programmatic RGB.
    //*(uint8_t*)0x796209 = 0;
    //*(uint8_t*)0x79620A = 0;

    // The stock picker initialization does more than set the custom swatch:
    // it derives the hue-bar marker color from H and rebuilds both markers.
    // Mirror that sequence so the saturation/value field is based on the same
    // HSV hue as the selected nitro RGB.
    typedef int(__stdcall* ColorFromHsvFn)(int, int, float);
    const int hueBits = *(int*)0x79620C;
    const int barColor = ((ColorFromHsvFn)0x4844B0)(hueBits, 0x3F800000, 0.5f);

    void* custom = *(void**)0x78EC34;
    if (custom) SetButtonColor(custom, rgb);

    void* hueBar = *(void**)0x78EC40;
    if (hueBar) SetButtonColor(hueBar, (uint32_t)barColor);

    // Native routine positions the hue and saturation/value markers from
    // dword_79620C / flt_796210 / flt_796214.
    __asm
    {
        pushad
        mov edi, 052AF60h
        call edi
        popad
    }
}

static uintptr_t gShowButton = 0x4970E0;

static void ShowButton(void* object, uint32_t visible)
{
    __asm
    {
        pushad
        mov eax, object
        mov edx, visible
        mov edi, 4970E0h
        call edi
        popad
    }
}

static void SetFlameCamera()
{
    const uintptr_t controller = *(uintptr_t*)0x7BC998;
    if (!controller) return;
    __asm
    {
        pushad
        mov eax, controller
        mov edx, 0Bh
        mov edi, 510220h
        call edi
        popad
    }
}

static void SetColorButtons()
{
    void** presets = (void**)0x78EC04;
    for (uint32_t i = 0; i < 12; ++i)
    {
        if (presets[i])
        {
            ShowButton(presets[i], 1);
            SetButtonColor(presets[i], gPresets[i]);
        }
    }
    void* custom = *(void**)0x78EC34;
    if (custom)
    {
        ShowButton(custom, 1);
        SetButtonColor(custom, gColor & 0xFFFFFF);
    }
}

static void OpenNitroMenu()
{
    const uintptr_t car = GarageCar();
    gPreviewCar = car;
    gPreviewFlame = 0;
    if (car)
    {
        const ColorSlot* color = FindColor(gCarColors, car);
        if (!color)
        {
            ReadCarColor(car);
            color = FindColor(gCarColors, car);
        }
        if (color && color->drawColor)
        {
            gColor = color->color & 0xFFFFFFu;
            gFlameColor = color->drawColor & 0xFFFFFFu;
        }
    }

    // Switch the stock picker's HSV state before showing the shared dialog.
    gColorsOpen = 1;
    SyncNativeColorPicker(gColor);
    ShowColors();
    SetColorButtons();
    SetFlameCamera();
}

static void OpenPaint()
{
    // Переключаемся обратно на штатный цвет машины.
    gColorsOpen = 0;

    // Сбрасываем сохранённый HSV нитро, чтобы он не влиял
    // на штатную инициализацию колорпикера.
    ShowColors();

    // Не вызываем SyncNativeColorPicker(gColor):
    // gColor относится к цвету нитро, а не машины.
}

static void CloseColors()
{
    HideColors();
    gColorsOpen = 0;
}

static void ApplyColor()
{
    gColor &= 0xFFFFFFu;
    gFlameColor = Tint(gColor);

    const uintptr_t car = GarageCar();
    if (car)
    {
        SetColor(gCarColors, gNextCar, car, gColor, gFlameColor, 0);
        SaveColor(car, gColor);
    }

    // Keep the native HSV state and all picker callbacks in sync with RGB.
    //SyncNativeColorPicker(gFlameColor);

    if (car)
    {
        const ColorSlot* color = FindColor(gCarColors, car);
        if (!color)
        {
            ReadCarColor(car);
            color = FindColor(gCarColors, car);
        }
        if (color && color->drawColor)
        {
            gColor = color->color & 0xFFFFFFu;
            gFlameColor = color->drawColor & 0xFFFFFFu;
        }
    }

    // Switch the stock picker's HSV state before showing the shared dialog.
    SyncNativeColorPicker(gColor);
}

extern "C" __declspec(naked) void OnPaintOpen()
{
    __asm
    {
        cmp dl, 3
        jne done
        pushad
        call OpenPaint
        popad
        done :
        ret
    }
}

extern "C" __declspec(naked) void OnNitroMenuOpen()
{
    __asm
    {
        cmp dl, 3
        jne done
        pushad
        call  OpenNitroMenu
        popad
        done :
        ret
    }
}

extern "C" __declspec(naked) void NitroButtonHook()
{
    __asm
    {
        pushfd
        pushad
        sub esp, 010h

        lea eax, [esp]
        mov edx, offset gButtonName
        mov ebx, 0FFFFFFFFh
        mov ecx, 069586Ch
        call ecx

        lea edx, [esp]
        mov eax, dword ptr[esp + 028h]
        mov ecx, 049B170h
        call ecx
        mov dword ptr[gButton], eax
        test eax, eax
        je set_button
        mov edx, dword ptr ds : [078EBE8h]
        test edx, edx
        je set_button
        movzx edx, byte ptr[edx + 055h]
        mov ecx, 04970E0h
        call ecx

        set_button :
        lea edx, [esp]
            mov eax, dword ptr[esp + 028h]
            lea ebx, OnNitroMenuOpen
            mov ecx, 04A0410h
            call ecx

            lea eax, [esp]
            xor edx, edx
            mov ecx, 06959C9h
            call ecx

            add esp, 010h
            popad
            popfd

            mov edx, 006CEE9Bh
            push 05370E7h
            ret
    }
}

extern "C" __declspec(naked) void NitroVisibleHook()
{
    __asm
    {
        push edx
        call dword ptr ds : [gShowButton]
        pop edx
        pushad
        mov eax, dword ptr[gButton]
        test eax, eax
        je done
        call dword ptr ds : [gShowButton]
        done :
        popad
        push 052D081h
        ret
    }
}

extern "C" __declspec(naked) void OnColorsClose()
{
    __asm
    {
        cmp dl, 3
        jne done
        pushad
        call CloseColors
        popad
        done :
        ret
    }
}

extern "C" __declspec(naked) void SetColorHook()
{
    __asm
    {
        cmp byte ptr[gColorsOpen], 0
        je normal_color
        and eax, 00FFFFFFh
        mov dword ptr[gColor], eax

        pushad
        call ApplyColor
        popad
        ret

        normal_color :
        push ebx
            push ecx
            push edx
            push esi
            push edi
            push 052E265h
            ret
    }
}

extern "C" __declspec(naked) void FlameDrawHook()
{
    __asm
    {
        pushad
        push ecx
        call BeginFlame
        add esp, 4
        popad

        mov eax, ecx
        call dword ptr[gDrawModel]

        pushad
        call EndFlame
        popad

        push 05BEDE9h
        ret
    }
}

extern "C" __declspec(naked) void FlameIdHook()
{
    __asm
    {
        cmp byte ptr[gTextureActive], 0
        je original_texture
        cmp bx, word ptr[gOldTexture]
        jne original_texture
        mov bx, word ptr[gColorTexture]
        original_texture:
        mov eax, dword ptr ds : [007CF700h]
            push 05E8F37h
            ret
    }
}

extern "C" __declspec(naked) void FlameColorHook()
{
    __asm
    {
        push eax
        push edx
        pushfd

        mov edx, 005A3B1Ch
        cmp byte ptr[gColorsOpen], 0
        jne use_selected

        mov eax, dword ptr[ecx + 0930h]
        test eax, eax
        je color_ready
        pushad
        push eax
        call CarColor
        add esp, 4
        mov dword ptr[esp + 014h], eax
        popad
        and edx, 00FFFFFFh
        jmp color_ready

        use_selected :
        mov edx, dword ptr[gFlameColor]
            and edx, 00FFFFFFh

            color_ready :

        xchg edx, dword ptr[esp + 4]
            popfd

            pop eax
            xchg eax, dword ptr[esp]

            push 05BED77h
            ret
    }
}

extern "C" __declspec(naked) void ExhaustHook()
{
    __asm
    {
        push ecx
        cmp byte ptr[gColorsOpen], 0
        je check_fade
        mov ecx, dword ptr[eax + 0930h]
        cmp ecx, dword ptr[gPreviewCar]
        jne game_code
        mov dword ptr[gPreviewFlame], eax
        mov byte ptr[eax + 0934h], 1
        mov dword ptr[eax + 0938h], 03F800000h
        mov dword ptr[eax + 093Ch], 03F800000h
        jmp game_code
        check_fade :
        cmp eax, dword ptr[gPreviewFlame]
            jne game_code
            cmp dword ptr[eax + 0938h], 0
            jne game_code
            mov dword ptr[gPreviewFlame], 0
            mov dword ptr[gPreviewCar], 0
            game_code:
        pop ecx
            push ebx
            push ecx
            push edx
            push esi
            push edi
            push 05BE965h
            ret
    }
}

extern "C" __declspec(naked) void ReadParamsHook()
{
    __asm
    {
        pushfd
        pushad
        push edx
        call ClearParamColor
        add esp, 4
        popad
        popfd

        push esi
        push edi
        push ebp
        sub esp, 0844h
        push 05DAAB9h
        ret
    }
}

extern "C" __declspec(naked) void ReadColorHook()
{
    __asm
    {
        mov dword ptr[gParseStack], esp
        pushfd
        pushad
        push ebp
        push edi
        push dword ptr[gParseStack]
        call ReadForceColor
        add esp, 12
        popad

        cmp byte ptr[gColorRead], 0
        jne force_param_handled
        popfd
        push 069Ah
        push 05DC7EEh
        ret

        force_param_handled :
        popfd
            push 05DB19Fh
            ret
    }
}

extern "C" __declspec(naked) void CarParamsHook()
{
    __asm
    {
        pushfd
        pushad
        push ebx
        push eax
        call SetCarColor
        add esp, 8
        popad
        popfd

        push ecx
        push esi
        push edi
        push ebp
        mov ebp, esp
        push 04494A6h
        ret
    }
}

extern "C" __declspec(naked) void CarReadyHook()
{
    __asm
    {
        pushfd
        pushad
        push dword ptr[ebp + 06Eh]
        call ReadCarColor
        add esp, 4
        popad
        popfd

        lea esp, [ebp + 082h]
        push 044A633h
        ret
    }
}

extern "C" __declspec(naked) void GarageCameraHook()
{
    __asm
    {
        cmp ah, 0Bh
        jne original_mode_dispatch

        pushfd
        pushad
        push ecx
        call ChooseCamera
        add esp, 4
        popad
        popfd
        push 0510CCBh
        ret

        original_mode_dispatch :
        cmp ah, 2
            jne original_mode_not2
            push 0510E33h
            ret

            original_mode_not2 :
        push 0510E87h
            ret
    }
}

extern "C" __declspec(naked) void RaceResetHook()
{
    __asm
    {
        pushfd
        pushad
        mov dword ptr[gGame], eax
        cld
        xor eax, eax
        mov edi, offset gCarColors
        mov ecx, gCarDwords
        rep stosd
        mov dword ptr[gNextCar], eax
        popad
        popfd

        push ebx
        push ecx
        push edx
        push esi
        sub esp, 20h
        push 06454E7h
        ret
    }
}

static uintptr_t CallTarget(uintptr_t site, uint8_t opcode)
{
    if (!site || *(const uint8_t*)site != opcode)
        return 0;
    int32_t rel = 0;
    memcpy(&rel, (const void*)(site + 1), sizeof(rel));
    return site + 5 + (intptr_t)rel;
}

static bool WriteBytes(uintptr_t site, const void* bytes, size_t size)
{
    if (!site)
        return false;

    DWORD oldProtect = 0;
    if (!VirtualProtect((void*)site, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    memcpy((void*)site, bytes, size);
    FlushInstructionCache(GetCurrentProcess(), (void*)site, size);
    VirtualProtect((void*)site, size, oldProtect, &oldProtect);
    return true;
}

static bool WriteJump(uintptr_t site, void* target, unsigned char oldBytes[9], uint32_t size, uint8_t opcode)
{
    unsigned char patch[9] = { 0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90, 0x90 };
    if (!site || size < 5 || size > 9 || *(const uint8_t*)site != opcode)
        return false;

    const int32_t rel = (int32_t)((uintptr_t)target - (site + 5));
    memcpy(oldBytes, (const void*)site, size);
    memcpy(&patch[1], &rel, sizeof(rel));
    return WriteBytes(site, patch, size);
}

static bool WriteDword(uintptr_t site, uint32_t value, uint32_t* oldValue)
{
    if (!site || !oldValue)
        return false;
    *oldValue = *(const uint32_t*)site;
    return WriteBytes(site, &value, 4);
}

struct Hook
{
    uintptr_t site;
    void* hook;
    uint32_t size;
    uint8_t opcode;
};

static const Hook gHooks[] =
{
    {0x5DAAB0, (void*)&ReadParamsHook, 9, 0x56},
    {0x5DC7E9, (void*)&ReadColorHook, 5, 0x68},
    {0x4494A0, (void*)&CarParamsHook, 6, 0x51},
    {0x44A62D, (void*)&CarReadyHook, 6, 0x8D},
    {0x510E2E, (void*)&GarageCameraHook, 5, 0x80},
    //{0x5370E2, (void*)&NitroButtonHook, 5, 0xBA},
    {0x52D07C, (void*)&NitroVisibleHook, 5, 0xE8},
    {0x52E260, (void*)&SetColorHook, 5, 0x53},
    {0x5BE960, (void*)&ExhaustHook, 5, 0x53},
    {0x5BED72, (void*)&FlameColorHook, 5, 0x68},
    {0x5BEDE2, (void*)&FlameDrawHook, 7, 0x89},
    {0x5E8F32, (void*)&FlameIdHook, 5, 0xA1},
    {0x6454E0, (void*)&RaceResetHook, 7, 0x53},
};
static const int gHookCount = sizeof(gHooks) / sizeof(gHooks[0]);

static unsigned char gOldHooks[gHookCount][9] = {};
static uint32_t gOldPaintOpen = 0;
static uint32_t gOldColorsClose = 0;

static void NitroColor_RemoveHooks()
{
    if (*(const uint32_t*)0x537184 == (uint32_t)(uintptr_t)&OnColorsClose)
        WriteBytes(0x537184, &gOldColorsClose, 4);
    if (*(const uint32_t*)0x5370F1 == (uint32_t)(uintptr_t)&OnPaintOpen)
        WriteBytes(0x5370F1, &gOldPaintOpen, 4);

    for (int i = gHookCount - 1; i >= 0; --i)
    {
        if (CallTarget(gHooks[i].site, 0xE9) == (uintptr_t)gHooks[i].hook)
            WriteBytes(gHooks[i].site, gOldHooks[i], gHooks[i].size);
    }
    gButton = 0;
}

static bool NitroColor_InstallHooks()
{
    //injector::MakeJMP(0x530E38, a_NitroColorBtn, true);

    for (int i = 0; i < gHookCount; ++i)
    {
        if (!WriteJump(gHooks[i].site, gHooks[i].hook, gOldHooks[i], gHooks[i].size, gHooks[i].opcode))
        {
            NitroColor_RemoveHooks();
            return false;
        }
    }

    if (!WriteDword(0x5370F1, (uint32_t)(uintptr_t)&OnPaintOpen, &gOldPaintOpen) ||
        !WriteDword(0x537184, (uint32_t)(uintptr_t)&OnColorsClose, &gOldColorsClose))
    {
        NitroColor_RemoveHooks();
        return false;
    }

    gColor &= 0xFFFFFF;
    gFlameColor = Tint(gColor);
    return true;
}