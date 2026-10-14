#include "XCustom/XNitroColor.h"	// by Fixik

#include "filesystem"

const char* NumPlate_Key = "NUMPLATE";
const char* group_numplate = "numplate";

const char* btnOpenNumPlate_Name = "btnOpenNumPlate";
uintptr_t btnOpenNumPlate_Ptr;
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

const char* btnOpenNitroColor_Name = "btnOpenNitroColor";
uintptr_t btnOpenNitroColor_Ptr;
const char* lblNitroColorSelSubCap_Name = "lblNitroColorSelSubCap";
uintptr_t lblNitroColorSelSubCap_Ptr;
const char* btnQuitNitroColorChange_Name = "btnQuitNitroColorChange";
uintptr_t btnQuitNitroColorChange_Ptr;

bool numplatebtn;
bool numplatebtn_state = true;

bool numplate_window;
bool nitrocolor_window;

std::string numplate_filename;

int numplate_addr;

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

int find_index(const std::vector<std::string>& vec, const std::string& value)
{
	auto it = std::find(vec.begin(), vec.end(), value);
	if (it != vec.end())
	{
		return std::distance(vec.begin(), it);
	}
	return 0;
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

	std::ranges::replace(numplate_tex, '\\', '/');

	if ((numplate_tex != "l_def.tga") && (numplate_tex != "s_def.tga") && (numplate_tex != "u_def.tga"))
	{
		numplate_tex2 = "numplate/" + numplate_tex;
	}
	else
	{
		numplate_tex2 = numplate_tex;
	}

	numplate_select_text = std::to_string(cur_numplate + 1) + "/" + std::to_string(total_numplates);

	AllocString(lblNumPlatePage_Ptr + 0x10C, numplate_select_text);

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
	AllocString(numplate_addr, numplate_tex);
	AllocString(0x78ED18 + (0x70 * curcar) + 0x38, numplate_tex);
}

void __declspec(naked) NumPlateElemsDeclr()
{
	__asm
	{
		call sub_6959C9
		
		// declaring the UI elements

		mov ebx, -1
		mov edx, dword ptr [btnOpenNumPlate_Name]
		lea eax, [esp + 0x80]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnOpenNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x80]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnQuitNumPlate_Name]
		lea eax, [esp + 0x700]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnQuitNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x700]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [lblNumPlatePage_Name]
		lea eax, [esp + 0x720]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [lblNumPlatePage_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x720]
		call sub_6959C9
			
		mov ebx, -1
		mov edx, dword ptr [picNumPlate_Name]
		lea eax, [esp + 0x740]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [picNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x740]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnLastNumPlate_Name]
		lea eax, [esp + 0x760]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnLastNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x760]
		call sub_6959C9
			
		mov ebx, -1
		mov edx, dword ptr [btnNextNumPlate_Name]
		lea eax, [esp + 0x780]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnNextNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x780]
		call sub_6959C9
			
		mov ebx, -1
		mov edx, dword ptr [btnSelectNumPlate_Name]
		lea eax, [esp + 0x7A0]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnSelectNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x7A0]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnOpenNitroColor_Name]
		lea eax, [esp + 0x80]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnOpenNitroColor_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x80]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnQuitNitroColorChange_Name]
		lea eax, [esp + 0x80]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [btnQuitNitroColorChange_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x80]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [lblNitroColorSelSubCap_Name]
		lea eax, [esp + 0x80]
		call sub_69586C
		mov edx, eax
		mov eax, ecx
		call sub_49B170
		mov ds: [lblNitroColorSelSubCap_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x80]
		call sub_6959C9

		// assigning the functions to the UI elements
		
		mov ebx, -1
		mov edx, dword ptr [btnOpenNumPlate_Name]
		lea eax, [esp + 0xC0]
		call sub_69586C
		mov ebx, btnOpenNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnOpenNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0xC0]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnQuitNumPlate_Name]
		lea eax, [esp + 0x7C0]
		call sub_69586C
		mov ebx, btnQuitNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnQuitNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x7C0]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnLastNumPlate_Name]
		lea eax, [esp + 0x7F0]
		call sub_69586C
		mov ebx, NumPlateDecrAsm
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnLastNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x7F0]
		call sub_6959C9
		
		mov ebx, -1
		mov edx, dword ptr [btnNextNumPlate_Name]
		lea eax, [esp + 0x820]
		call sub_69586C
		mov ebx, NumPlateIncrAsm
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnNextNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x820]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [picNumPlate_Name]
		lea eax, [esp + 0x850]
		call sub_69586C
		mov ebx, picNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [picNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x850]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnSelectNumPlate_Name]
		lea eax, [esp + 0x880]
		call sub_69586C
		mov ebx, btnSelectNumPlate_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnSelectNumPlate_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0x880]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnOpenNitroColor_Name]
		lea eax, [esp + 0xC0]
		call sub_69586C
		mov ebx, btnOpenNitroColor_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnOpenNitroColor_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0xC0]
		call sub_6959C9

		mov ebx, -1
		mov edx, dword ptr [btnQuitNitroColorChange_Name]
		lea eax, [esp + 0xC0]
		call sub_69586C
		mov ebx, btnQuitNitroColorChange_Func
		mov edx, eax
		mov eax, ecx
		call sub_4A0410
		mov ds: [btnQuitNitroColorChange_Ptr], eax
		xor edx, edx
		lea eax, [esp + 0xC0]
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
		mov byte ptr [nitrocolor_window], 0

		mov ah, byte ptr ds: [0x78ED10]
		cmp ah, 0
		jne skip2

		call sub_52CE50
		jmp loc_539A0F

	skip2:
		call sub_539B70

		jmp skip

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

	btnOpenNumPlate_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

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

		mov byte ptr [numplate_window], 1

		jmp skip

	btnQuitNitroColorChange_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		mov byte ptr [numplate_window], 0
		mov byte ptr [nitrocolor_window], 0

		mov dword ptr [gColorsOpen], 0

		mov ah, byte ptr ds: [0x78ED10]
		cmp ah, 0
		jne skip2

		call sub_52CE50
		jmp loc_539A0F

	btnOpenNitroColor_Func:
		push ebx
		push ecx
		push esi
		push edi
		sub esp, 0x40
		cmp dl, 03
		jne skip

		call OpenNitroMenu

		mov byte ptr [nitrocolor_window], 1

		jmp skip

	skip:
		add esp, 0x40
		pop edi
		pop esi
		pop ecx
		pop ebx
		retn

	sub_539B70:
		push 0x539B70
		retn

	sub_695A15:
		push 0x695A15
		retn

	sub_538EE0:
		push 0x538EE0
		retn

	sub_4970E0:
		push 0x4970E0
		retn

	sub_4A4690:
		push 0x4A4690
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

	loc_539A0F:
		push 0x539A0F
		retn

	loc_56768D:
		push 0x56768D
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

void NumPlate_MainFunc()
{
	if (GetGarageColorType())
	{
		garagecar_ptr = 0x5C3C;	// shop
	}
	else
	{
		garagecar_ptr = 0xD7A0;	// garage
	}

	curcar = injector::ReadMemory<short>(0x78ED16, true);

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
					std::ranges::replace(result, '\\', '/');
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
			if (GetGarageColorType())
			{
				numplate_offset = 0x70;
			}
			else
			{
				numplate_offset = 0x7BD4;
			}

			if (!CDNetwork())
			{
				numplate_addr = injector::ReadMemory<DWORD>(injector::ReadMemory<DWORD>(0x7EA2CC)) + (numplate_offset + (0x5C * curcar));
				numplate_type = injector::ReadMemory<BYTE>(injector::ReadMemory<DWORD>(injector::ReadMemory<DWORD>(0x7EA2CC)) + garagecar_ptr + 0x1AF8, true);
			}
			else
			{
				numplate_addr = injector::ReadMemory<DWORD>(GetPlayerParam<DWORD>((CDPlayer)0x6854, 0));
				numplate_type = GetPlayerParam<BYTE>((CDPlayer)0x63C0, 0);
			}

			std::string cur_numplate2 = GetString(injector::ReadMemory<void*>(numplate_addr));

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

					std::ranges::replace(numplate_filename, '\\', '/');

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

void __declspec(naked) a_NumPlate_MainFunc()
{
	__asm
	{
		call NumPlate_MainFunc

		jne loc_538246
		jmp loc_537D92

	loc_538246:
		push 0x538246
		retn

	loc_537D92:
		push 0x537D92
		retn
	}
}

static void ExtraCustomBtns_Handler()
{
	std::string cur_list_level = GetString(injector::ReadMemory<void*>(0x795D28));
	std::string cur_group = GetString(injector::ReadMemory<void*>(injector::ReadMemory<DWORD>(injector::ReadMemory<DWORD>(0x78D65C) + 0x248)));

	bool numplate_visibility = cur_list_level == "toplevel" == !numplate_window == (*(uint8_t*)0x795D1C || *(uint8_t*)0x7CD0E0) && cur_group != "colorsel";
	bool nitrocolor_visibility = nitrocolor_window;
	bool colorsel_visibility = cur_group == "colorsel" == !nitrocolor_window;

	bool enabled_state = (*(DWORD*)p_Curr3DModel != NULL && *(DWORD*)p_CDNetwork != NULL) || (*(DWORD*)p_CDNetwork == NULL);

	__asm
	{
		pushad

		mov eax, btnOpenNumPlate_Ptr
		mov dl, numplate_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, btnOpenNitroColor_Ptr
		mov dl, numplate_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, lblNitroColorSelSubCap_Ptr
		mov dl, nitrocolor_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, btnQuitNitroColorChange_Ptr
		mov dl, nitrocolor_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, ds: [0x78EBF4]	// lblColorSelSubCap
		mov dl, colorsel_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, ds: [0x78EBF8]	// btnQuitColorChange
		mov dl, colorsel_visibility
		mov ebx, 0x4970E0
		call ebx

		mov eax, btnOpenNumPlate_Ptr
		mov dl, enabled_state
		mov ebx, 0x497070
		call ebx

		popad
	}
}

void XNumPlate()
{
	injector::MakeJMP(0x536D18, NumPlateElemsDeclr, true);
	injector::MakeJMP(0x537D8C, a_NumPlate_MainFunc, true);
	injector::MakeJMP(0x538820, ExtraCustomBtns_Handler, true);

	//injector::MakeNOP(0x5DAC04, 5, true);
}