BYTE max_meshes = 32;	// 16 - default

void __declspec(naked) a_StartEngSoundFix()
{
	__asm
	{
		mov ebx, -1
		mov edx, eax
		mov eax, esp
		call sub_69586C
		mov ecx, ds: [0x7DF0AC]

		jmp loc_46D37C

	loc_46D37C:
		push 0x46D37C
		retn

	sub_69586C:
		push 0x69586C
		retn
	}
}

void XCarinfo()
{
	injector::WriteMemory(0x44C501, max_meshes, true);
	injector::WriteMemory(0x44C65E, max_meshes, true);

	injector::WriteMemory(0x44CD52, 0, true);	// hide fallback front flare
	injector::WriteMemory<BYTE>(0x44C9E2, 0x74, true);	// hide fallback rear flare

	//injector::MakeJMP(0x46D376, a_StartEngSoundFix, true);
	injector::WriteMemory(0x65610F, 0x78AC70, true);
}