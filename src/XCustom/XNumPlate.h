
const char* NumPlate_Key = "NUMPLATE";
const char* group_numplate = "numplate";
const char* lstbtnlvl_numplate = "numplate";

const char* btnQuitNumPlate_Name = "btnQuitNumPlate";
uintptr_t btnQuitNumPlate_Ptr;
const char* lblNumPlatePage_Name = "lblNumPlatePage";
uintptr_t lblNumPlatePage_Ptr;
const char* picNumPlate_Name = "picNumPlate";
uintptr_t picNumPlate_Ptr;
const char* btnLastNumPlate_Name = "btnLastNumPlate";
uintptr_t btnLastNumPlate_Ptr;
const char* btnNextNumPlate_Name = "btnNextNumPlate";
uintptr_t btnNextNumPlate_Ptr;
const char* btnSelectNumPlate_Name = "btnSelectNumPlate";
uintptr_t btnSelectNumPlate_Ptr;

bool numplatebtn;
bool numplatebtn_state = true;

bool numplate_window;

std::string numplate_filename;

int numplate_addr1;
int numplate_addr2;
int numplate_addr3;

int numplate_offset;

int total_numplates;

int cur_numplate;

std::string numplate_select_text;

bool is_numplate_found;

std::string sel_numplates_dir = "\\textures\\numplate\\";

std::vector<std::string> sel_numplates_l = { "l_def.tga" };
std::vector<std::string> sel_numplates_s = { "s_def.tga" };
std::vector<std::string> sel_numplates_u = { "u_def.tga" };

bool getnumplates;

std::string numplate_page_str;

int numplate_type;

int curcar;

bool is_texture_loaded;

std::string numplate_tex;
std::string numplate_tex2;

int find_index(const std::vector<std::string>& vec, const std::string& value) {
	auto it = std::find(vec.begin(), vec.end(), value);
	if (it != vec.end()) {
		return std::distance(vec.begin(), it);
	}
	return -1;
}

void CDWriteString(int addr, std::string str)
{
	if (addr)
	{
		injector::WriteMemory<int>(addr + 0x8, str.length(), true);
		injector::WriteMemory<int>(injector::ReadMemory<DWORD>(addr) + 0x4, str.length(), true);
		injector::WriteMemory<int>(injector::ReadMemory<DWORD>(addr) + 0x8, str.length(), true);
		WriteString<uint32_t>(injector::ReadMemory<DWORD>(addr) + 0xC, str.c_str(), true);
	}
}

void __declspec(naked) LicensePlateBtn()
{
	__asm
	{
		cmp byte ptr ds: [0x78ED10], 0x02
		jnb skip

		mov edx, 0x6CE390	// "toplevel" - main list
		mov eax, esi
		call string_equal_operator
		test eax, eax
		jz skip

		mov byte ptr [numplatebtn], 1

		mov eax, ds: [0x78ED14]
		sar eax, 0x10
		shl eax, 0x04
		lea ecx, ds: [eax * 0x08]
		mov edx, ds: [0x795DBC]
		sub ecx, eax

		lea eax, ds: [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		mov bl, ds: [ecx + 0x78ED85]
		shl eax, 0x02

		mov dl, byte ptr [numplatebtn_state]
		mov byte ptr ds: [eax + 0x795DE4], dl	// disabled / enabled
		mov edx, ds: [0x795DBC]
		lea eax, ds: [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		shl eax, 0x02
		add eax, 0x795DE4
		mov edx, 0x6CE399
		add eax, 0x28
		mov ebx, dword ptr [NumPlate_Key]
		call sub_695A15
		mov edx, dword ptr [XCustom_Section_Main]
		mov eax, ds: [0x7E3110]
		call sub_66F410
		mov ebx, -1
		mov edx, eax
		lea eax, [ebp - 0x156]
		call sub_69586C
		mov edx, ds: [0x795DBC]
		mov ecx, eax
		lea eax, [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		shl eax, 0x02
		add eax, 0x795DE4
		mov edx, ecx
		add eax, 0x14
		call sub_695F1D
		lea eax, [ebp - 0x156]
		xor edx, edx
		call sub_6959C9
		mov edx, ds: [0x795DBC]
		lea eax, [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		xor bh, bh
		mov ds: [eax * 0x04 + 0x795E2E], bh
		mov eax, ds: [0x78ED14]
		sar eax, 0x10
		shl eax, 0x04
		mov edx, eax
		shl eax, 0x03
		sub eax, edx
		add eax, 0x78ED18
		mov edx, ds: [0x795DBC]
		lea ecx, ds: [eax + 0x04]
		lea eax, ds: [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		shl eax, 0x02
		add eax, 0x795DE4
		mov edx, ecx
		add eax, 0x38
		call sub_695F1D
		mov edx, ds: [0x795DBC]
		lea eax, ds: [edx * 0x4]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		mov ds: [eax * 0x04 + 0x795E2C], bh
		mov edx, ds: [0x795DBC]
		lea eax, ds: [edx * 4]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		mov byte ptr [eax * 0x04 + 0x795E2F], 0x01
		mov edx, ds: [0x795DBC]
		lea eax, ds: [edx * 0x04]
		add eax, edx
		shl eax, 0x02
		sub eax, edx
		mov[eax * 0x04 + 0x795E2D], bh

		jmp loc_530E38

	skip:
		mov byte ptr [numplatebtn], 0

		cmp byte ptr ds: [0x78ED10], 0x02
		jmp loc_530CAB

	string_equal_operator:
		push 0x696058
		retn

	loc_530CAB:
		push 0x530CAB
		retn

	loc_530E38:
		push 0x530E38
		retn

	sub_695A15:
		push 0x695A15
		retn

	sub_66F410:
		push 0x66F410
		retn

	sub_69586C:
		push 0x69586C
		retn

	sub_695F1D:
		push 0x695F1D
		retn

	sub_6959C9:
		push 0x6959C9
		retn
	}
}

void __declspec(naked) LicensePlateBtn_Handler()
{
	__asm
	{
		cmp byte ptr [numplatebtn], 1
		jne defaultbtn

		call go_to_numplate_page
		jmp loc_53998B

	defaultbtn:
		mov eax, 0x0B
		call sub_533B60
		jmp loc_53998B

	go_to_numplate_page:
		push ebx
		push ecx
		push edx
		push esi

		mov ebx, -1
		mov edx, dword ptr [group_numplate]
		mov eax, esp
		call sub_69586C

		mov ecx, ds: [0x78D65C]
		mov edx, eax
		mov eax, ecx
		call sub_4A4690

		mov eax, esp
		xor edx, edx
		call sub_6959C9
			
		mov edx, dword ptr [lstbtnlvl_numplate]
		mov eax, 0x795D28
		call sub_695A15

		/*mov ebx, ds: 0x795D38
		inc ebx
		mov ds: 0x795D38, ebx*/

		mov byte ptr [numplate_window], 1

		pop esi
		pop edx
		pop ecx
		pop ebx
		retn

	sub_533B60:
		push 0x533B60
		retn

	sub_52D600:
		push 0x52D600
		retn

	loc_53998B:
		push 0x53998B
		retn

	sub_69586C:
		push 0x69586C
		retn

	sub_4A4690:
		push 0x4A4690
		retn

	sub_6959C9:
		push 0x6959C9
		retn

	sub_695A15:
		push 0x695A15
		retn

	sub_52CE50:
		push 0x52CE50
		retn
	}
}

void NumPlateSelect(int index)
{
	if (index == -1)
	{
		index = total_numplates - 1;
	}
	else if (index == total_numplates)
	{
		index = 0;
	}

	cur_numplate = index;

	if (numplate_type == 0)
	{
		numplate_tex = sel_numplates_l[index].data();
	}
	else if (numplate_type == 1)
	{
		numplate_tex = sel_numplates_s[index].data();
	}
	else if (numplate_type == 2)
	{
		numplate_tex = sel_numplates_u[index].data();
	}

	if ((numplate_tex != "l_def.tga") && (numplate_tex != "s_def.tga") && (numplate_tex != "u_def.tga"))
	{
		numplate_tex2 = "numplate/" + numplate_tex;
	}
	else
	{
		numplate_tex2 = numplate_tex;
	}

	numplate_select_text = std::to_string(cur_numplate + 1) + "/" + std::to_string(total_numplates);

	CDWriteString(lblNumPlatePage_Ptr + 0x10C, numplate_select_text);

	injector::WriteMemory<float>(picNumPlate_Ptr + 0x13C, 0.f, true);

	numplate_filename = numplate_tex2;
}

void NumPlateDecr()
{
	NumPlateSelect(cur_numplate - 1);
}

void NumPlateIncr()
{
	NumPlateSelect(cur_numplate + 1);
}

void ChangeNumPlate()
{
	CDWriteString(injector::ReadMemory<DWORD>(numplate_addr1) + (numplate_offset + (0x5C * curcar)), numplate_tex);
	CDWriteString(0x78ED50 + (0x70 * curcar), numplate_tex);
}

void __declspec(naked) LicensePlateBtn_Disable()
{
	__asm
	{
		call sub_491010
		mov byte ptr[numplatebtn], 0
		jmp loc_538F14

	sub_491010:
		push 0x491010
		retn

	loc_538F14 :
		push 0x538F14
		retn
	}
}

void __declspec(naked) NumPlateElemsDeclr()
{
	__asm
	{
		call sub_6959C9
		
		// declaring the UI elements

		mov ebx, -1
		mov edx, dword ptr [btnQuitNumPlate_Name]
		lea eax, [esp + 0x460]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnQuitNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x460]

		mov ebx, -1
		mov edx, dword ptr [lblNumPlatePage_Name]
		lea eax, [esp + 0x480]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [lblNumPlatePage_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x480]
			
		mov ebx, -1
		mov edx, dword ptr [picNumPlate_Name]
		lea eax, [esp + 0x490]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [picNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x490]
						
		mov ebx, -1
		mov edx, dword ptr [btnLastNumPlate_Name]
		lea eax, [esp + 0x4B0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnLastNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4B0]
			
		mov ebx, -1
		mov edx, dword ptr [btnNextNumPlate_Name]
		lea eax, [esp + 0x4C0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnNextNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4C0]
			
		mov ebx, -1
		mov edx, dword ptr [btnSelectNumPlate_Name]
		lea eax, [esp + 0x4F0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnSelectNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4F0]

		// assigning the functions to the UI elements

		mov ebx, -1
		mov edx, dword ptr [btnQuitNumPlate_Name]
		lea eax, [esp + 0x470]
		call sub_69586C
		mov ebx, btnQuitNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnQuitNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x470]

		mov ebx, -1
		mov edx, dword ptr [btnLastNumPlate_Name]
		lea eax, [esp + 0x4A0]
		call sub_69586C
		mov ebx, NumPlateDecrAsm
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnLastNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4A0]
		
		mov ebx, -1
		mov edx, dword ptr [btnNextNumPlate_Name]
		lea eax, [esp + 0x4D0]
		call sub_69586C
		mov ebx, NumPlateIncrAsm
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnNextNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4D0]
	
		mov ebx, -1
		mov edx, dword ptr [picNumPlate_Name]
		lea eax, [esp + 0x4E0]
		call sub_69586C
		mov ebx, picNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [picNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x4E0]
				
		mov ebx, -1
		mov edx, dword ptr [btnSelectNumPlate_Name]
		lea eax, [esp + 0x500]
		call sub_69586C
		mov ebx, btnSelectNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnSelectNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x500]

		call sub_6959C9
		jmp loc_536D1D
	
	NumPlateDecrAsm:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		call NumPlateDecr

		jmp skip

	NumPlateIncrAsm:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		call NumPlateIncr

		jmp skip


	btnQuitNumPlate_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		mov byte ptr [numplate_window], 0

		call sub_52CE50

		jmp sub_539A0F

	picNumPlate_Func:
		push ebx
		push ecx
		push edx
		sub esp, 0x160

		mov ebx, -1
		mov edx, dword ptr [numplate_filename]
		lea eax, [esp + 0x40]
		call sub_69586C

		mov ecx, ds: [picNumPlate_Ptr]
		mov edx, eax
		mov eax, ecx
		call sub_4AF990

		lea eax, [esp + 0x40]
		xor edx, edx
		call sub_6959C9

		add esp, 0x160
		pop edx
		pop ecx
		pop ebx
		retn
		
	btnSelectNumPlate_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		call ChangeNumPlate

		mov dword ptr ds: [0x795D24], 0	// CurrCarTunedSpecs
		mov dword ptr ds: [0x7961F8], 0	// Curr3DModel
		mov byte ptr ds: [0x796200], 1	// LoadingNewModelRequired

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

	loc_536D1D:
		push 0x536D1D
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

void XNumPlate()
{
	injector::MakeJMP(0x530CA4, LicensePlateBtn, true);
	injector::MakeJMP(0x538F0F, LicensePlateBtn_Disable, true);
	injector::MakeJMP(0x539981, LicensePlateBtn_Handler, true);
	injector::MakeJMP(0x536D18, NumPlateElemsDeclr, true);

	WriteString<uint32_t>(0x6C94D6, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6C94E2, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6D6956, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6D6A7E, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DDD1F, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DDD34, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DE653, gui_garage2.c_str(), true);
	WriteString<uint32_t>(0x6DE739, gui_garage2.c_str(), true);

	numplate_filename.reserve(128);

	if (GetGarageColorType())
	{
		garagecar_ptr = 0x5C3C;	// shop
	}
	else
	{
		garagecar_ptr = 0xD7A0;	// garage
	}

	if (injector::ReadMemory<BYTE>(0x796330, true) == 4) /*if (*(BYTE*)0x7CF704 == 1)*/
	{
		if (!CDDir().empty())
		{
			if (getnumplates == false)
			{
				for (const auto& entry : std::filesystem::recursive_directory_iterator(CDDir() + sel_numplates_dir))
				{
					if (entry.is_regular_file())
					{
						std::filesystem::path relative = std::filesystem::relative(entry.path(), CDDir() + sel_numplates_dir);
						std::string filename = relative.filename().stem().string();
						std::string extension = relative.extension().string();
						std::string result = relative.replace_extension(".tga").string();

						if ((extension == ".tga") || (extension == ".dds"))
						{
							if (filename.rfind("l_", 0) == 0)
							{
								sel_numplates_l.push_back(result);
							}
							else if (filename.rfind("s_", 0) == 0)
							{
								sel_numplates_s.push_back(result);
							}
							else if (filename.rfind("u_", 0) == 0)
							{
								sel_numplates_u.push_back(result);
							}
						}
					}
				}
				getnumplates = true;
			}
			if (getnumplates == true)
			{
				if (numplatebtn)
				{
					curcar = injector::ReadMemory<short>(0x78ED16, true);

					if (GetGarageColorType())
					{
						numplate_offset = 0x70;
					}
					else
					{
						numplate_offset = 0x7BD4;
					}

					numplate_addr1 = injector::ReadMemory<DWORD>(0x7EA2CC);
					numplate_addr2 = injector::ReadMemory<DWORD>(injector::ReadMemory<DWORD>(numplate_addr1) + (numplate_offset + (0x5C * curcar)));
					std::string cur_numplate2 = GetString((void*)numplate_addr2);

					numplate_type = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(injector::ReadMemory<DWORD>(0x7EA2CC)) + garagecar_ptr + 0x1AF8, true);

					/*if (total_numplates == 1)
					{
						numplatebtn_state = false;
					}
					else
					{
						numplatebtn_state = true;
					}*/

					if (numplate_type == 0)
					{
						total_numplates = sel_numplates_l.size();
					}
					else if (numplate_type == 1)
					{
						total_numplates = sel_numplates_s.size();
					}
					else if (numplate_type == 2)
					{
						total_numplates = sel_numplates_u.size();
					}

					if (numplate_window)
					{
						if (is_numplate_found == false)
						{
							numplate_filename = cur_numplate2;

							if (numplate_type == 0)
							{
								NumPlateSelect(find_index(sel_numplates_l, numplate_filename));
							}
							else if (numplate_type == 1)
							{
								NumPlateSelect(find_index(sel_numplates_s, numplate_filename));
							}
							else if (numplate_type == 2)
							{
								NumPlateSelect(find_index(sel_numplates_u, numplate_filename));
							}

							is_numplate_found = true;
						}
					}
					else
					{
						is_numplate_found = false;
					}
				}
			}
		}
	}
}