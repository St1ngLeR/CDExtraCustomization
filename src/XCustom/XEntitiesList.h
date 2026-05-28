int entry_offset;

std::vector<uint16_t> parts_count(12);

int part_index;

int available_entries;

int cursel;

int level_page;

bool has_pages;	// we need this variable to handle button hovering resetting when refreshing the page

bool has_stock;

std::vector<BYTE> buttons_state(13);

int last_available_btn;

void __declspec(naked) a_EntriesList()
{
	__asm
	{
		movsx ecx, di
		mov eax, esi
		mov edx, ecx

		add edx, entry_offset

		jmp loc_53046A

	loc_53046A:
		push 0x53046A
		retn
	}
}

void __declspec(naked) a_EntriesList2()
{
	__asm
	{
		call sub_695A15

		mov edx, ecx
		mov eax, esi

		add edx, entry_offset

		jmp loc_5304A5

	loc_5304A5:
		push 0x5304A5
		retn

	sub_695A15:
		push 0x695A15
		retn
	}
}

void __declspec(naked) a_EntriesList3()
{
	__asm
	{
		je loc_530769

		add ecx, entry_offset

		jmp loc_5304D1

	loc_530769:
		push 0x530769
		retn

	loc_5304D1:
		push 0x5304D1
		retn
	}
}

void __declspec(naked) a_EntriesList4()
{
	__asm
	{
		movsx ebp, di
		mov eax, esi
		mov edx, ebp

		add edx, entry_offset

		jmp loc_53052E

	loc_53052E:
		push 0x53052E
		retn
	}
}

void __declspec(naked) a_GetPartsCount()
{
	__asm
	{
		call sub_52F740

		mov ecx, dword ptr ds: [0x795DBC]
		mov ebx, dword ptr ds: [parts_count]
		mov edx, ecx
		shl edx, 1
		add ebx, edx
		mov word ptr [ebx], ax

		mov [ebp + 0x6E], ax

		jmp loc_530A4C

	sub_52F740:
		push 0x52F740
		retn

	loc_530A4C:
		push 0x530A4C
		retn
	}
}

const char* btnListUp_Name = "btnListUp";
uintptr_t btnListUp_Ptr;
const char* btnListDown_Name = "btnListDown";
uintptr_t btnListDown_Ptr;

std::string page_name;

void __declspec(naked) a_BtnUpDownElemsDeclr()
{
	__asm
	{
		call sub_6959C9
		
		// declaring the UI elements
	
		mov ebx, -1
		mov edx, dword ptr [btnListUp_Name]
		lea eax, [esp + 0x8B0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnListUp_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x8B0]
		call sub_6959C9
			
		mov ebx, -1
		mov edx, dword ptr [btnListDown_Name]
		lea eax, [esp + 0x8D0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnListDown_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x8D0]
		call sub_6959C9
			
		// assigning the functions to the UI elements

		mov ebx, -1
		mov edx, dword ptr [btnListUp_Name]
		lea eax, [esp + 0x8F0]
		call sub_69586C
		mov ebx, btnListUp_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnListUp_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x8F0]
		call sub_6959C9
		
		mov ebx, -1
		mov edx, dword ptr [btnListDown_Name]
		lea eax, [esp + 0x920]
		call sub_69586C
		mov ebx, btnListDown_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnListDown_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x920]
		call sub_6959C9

		jmp loc_537681

	btnListDown_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		mov eax, cursel
		cmp eax, last_available_btn // 13 entries (counting from 0)
		je inc_entry_offset

		jmp skip

	btnListUp_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		cmp cursel, 0
		je dec_entry_offset

		jmp skip

	dec_entry_offset:
		mov byte ptr[has_pages], 1

		dec entry_offset
		
		mov ebx, -1
		lea edx, page_name
		mov eax, esp
		mov ecx, 0x69586C
		call ecx

		xor edx, edx
		mov ecx, 0x530880
		call ecx

		mov eax, esp
		xor edx, edx
		mov ecx, 0x6959C9
		call ecx

		mov byte ptr [has_pages], 0

		jmp skip

	inc_entry_offset:
		mov byte ptr[has_pages], 1

		inc entry_offset
		
		mov ebx, -1
		lea edx, page_name
		mov eax, esp
		mov ecx, 0x69586C
		call ecx

		xor edx, edx
		mov ecx, 0x530880
		call ecx

		mov eax, esp
		xor edx, edx
		mov ecx, 0x6959C9
		call ecx

		mov byte ptr [has_pages], 0

		jmp skip

	skip:
		add esp, 0x40
		pop edi
		pop esi
		pop ecx
		pop ebx
		retn

	sub_695AA9:
		push 0x695AA9
		retn

	sub_695DB9:
		push 0x695DB9
		retn

	sub_69586C:
		push 0x69586C
		retn

	sub_6959C9:
		push 0x6959C9
		retn

	sub_49B170:
		push 0x49B170
		retn

	loc_537681:
		push 0x537681
		retn

	sub_4A0410:
		push 0x4A0410
		retn

	sub_52CE50:
		push 0x52CE50
		retn

	sub_539A0F:
		push 0x539A0F
		retn

	sub_4AF990:
		push 0x4AF990
		retn

	sub_672700:
		push 0x672700
		retn

	sub_5E4D20:
		push 0x5E4D20
		retn
	}
}

void __declspec(naked) a_PageCheck()
{
	__asm
	{
		cmp has_pages, 0
		je truecond
		jmp end

	truecond:
		cmp entry_offset, 0
		je skip
		mov edi, last_available_btn
		jmp end

	skip:
		mov ds: [0x795DC0], edi
		jmp end

	end:
		push 0x530900
		retn
	}
}

void __declspec(naked) a_PageCheck2()
{
	__asm
	{
		cmp has_pages, 0
		je truecond
		jmp end

	truecond:
		cmp entry_offset, 0
		je skip
		mov ecx, last_available_btn
		jmp end

	skip:
		mov ds: [0x795DC0], ecx
		jmp end

	end:
		push 0x52FE2D
		retn
	}
}

//std::string GetCarTuningPageName(int index)
//{
//	switch (index)
//	{
//	case 0: return "performance";
//	case 1: return "crashpower";
//	case 2: return "armor";
//	case 3: return "wheels";
//	case 4: return "suspension";
//	case 5: return "fbumper";
//	case 6: return "rbumper";
//	case 7: return "sidekit";
//	case 8: return "hood";
//	case 9: return "rearwing";
//	case 10: return "paint";
//	default: return "";
//	}
//}

int GetLastAvailableButton()
{
	int lastAvailableIndex;

	buttons_state[0] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBB4) + 0x54);
	buttons_state[1] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBB8) + 0x54);
	buttons_state[2] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBBC) + 0x54);
	buttons_state[3] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBC0) + 0x54);
	buttons_state[4] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBC4) + 0x54);
	buttons_state[5] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBC8) + 0x54);
	buttons_state[6] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBCC) + 0x54);
	buttons_state[7] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBD0) + 0x54);
	buttons_state[8] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBD4) + 0x54);
	buttons_state[9] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBD8) + 0x54);
	buttons_state[10] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBDC) + 0x54);
	buttons_state[11] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBE0) + 0x54);
	buttons_state[12] = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(0x78EBE4) + 0x54);

	for (int i = buttons_state.size() - 1; i >= 0; --i) {
		if (buttons_state[i] == 1) {
			lastAvailableIndex = i;
			break;
		}
	}

	return lastAvailableIndex;
}

void EntriesList_MainFunc()
{
	if (injector::ReadMemory<int>(0x78ED10) == 0)
	{
		has_stock = true;
	}
	else
	{
		has_stock = false;
	}

	level_page = injector::ReadMemory<int>(0x795D38);

	if (level_page < 3)
	{
		page_name = GetString(injector::ReadMemory<void*>(0x795D28));
	}

	if (level_page == 0)
	{
		part_index = injector::ReadMemory<int>(0x795DC0);
		injector::WriteMemory<BYTE>(btnListUp_Ptr + 0x55, false);
		injector::WriteMemory<BYTE>(btnListDown_Ptr + 0x55, false);
		entry_offset = 0;
	}
	else
	{
		available_entries = injector::ReadMemory<int>(0x795DBC);

		if ((available_entries < parts_count[part_index] + has_stock)/* || (level_page == 3)*/)	// + 1 because of "(Stock)" button
		{
			// Make arrow buttons visible
			injector::WriteMemory<BYTE>(btnListUp_Ptr + 0x55, true);
			injector::WriteMemory<BYTE>(btnListDown_Ptr + 0x55, true);

			cursel = injector::ReadMemory<int>(0x795DC0);

			// make "up" button unavailable when selecting the first element
			if (cursel == 0 && entry_offset > 0 && last_available_btn > 0)
			{
				injector::WriteMemory<BYTE>(btnListUp_Ptr + 0x54, true);
			}
			else
			{
				injector::WriteMemory<BYTE>(btnListUp_Ptr + 0x54, false);
			}

			last_available_btn = GetLastAvailableButton();

			// make "down" button unavailable when selecting the last element
			if ((entry_offset == parts_count[part_index] - available_entries + has_stock) || (cursel < last_available_btn)) // + 1 because of "(Stock)" button
			{
				injector::WriteMemory<BYTE>(btnListDown_Ptr + 0x54, false);
			}
			else
			{
				injector::WriteMemory<BYTE>(btnListDown_Ptr + 0x54, true);
			}
		}
		else
		{
			injector::WriteMemory<BYTE>(btnListUp_Ptr + 0x55, false);
			injector::WriteMemory<BYTE>(btnListDown_Ptr + 0x55, false);
		}
	}
}

void __declspec(naked) a_EntriesList_MainFunc()
{
	__asm
	{
		call EntriesList_MainFunc

		cmp dword ptr ds: [0x7CD0E0], 0

		jmp loc_537D8C

	loc_537D8C:
		push 0x537D8C
		retn
	}
}

void XEntriesList()
{
	injector::MakeJMP(0x530463, a_EntriesList);
	injector::MakeJMP(0x53049C, a_EntriesList2);
	injector::MakeJMP(0x5304CB, a_EntriesList3);
	injector::MakeJMP(0x530527, a_EntriesList4);

	injector::MakeJMP(0x530A43, a_GetPartsCount);
	injector::MakeJMP(0x53767C, a_BtnUpDownElemsDeclr);

	injector::MakeJMP(0x5308FA, a_PageCheck);
	injector::MakeJMP(0x52FE27, a_PageCheck2);

	injector::MakeJMP(0x537D85, a_EntriesList_MainFunc);	// we do a little trolling
}