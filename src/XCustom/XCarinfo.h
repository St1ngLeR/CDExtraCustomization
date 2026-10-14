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

const char* var_torquecrv = "torquecrv";
const char* var_rpm_max = "rpm_max";
const char* var_rpm_idle = "rpm_idle";
const char* var_rpm_shift_down = "rpm_shift_down";
const char* var_rpm_shift_up = "rpm_shift_up";
const char* var_steerspeed = "steerspeed";
const char* var_gear_ratios = "gear_ratios";
const char* var_scale_shadowz = "scale_shadowz";
const char* var_add_wheelposzfront = "add_wheelposzfront";
const char* var_add_wheelposzrear = "add_wheelposzrear";
const char* var_drive = "drive";
const char* var_aftbur_accel = "aftbur_accel";

float mul_steerspeed = 0.5f;

void* ptr_curcrvfile;
std::string str_curcrvfile;

void* ptr_curcarfolder;
std::string str_curcarfolder;

void* ptr_gear_ratios;
std::string str_gear_ratios;
std::vector<float> arr_gear_ratios;

void* ptr_drive;
std::string str_drive;

int drive_bm;
BYTE drive_whls;

int ptr_curcar;

void GetCRVFile()
{
	str_curcrvfile = GetString(ptr_curcrvfile);
}

void GetFullCRVFilePath()
{
	if (str_curcrvfile != "default")
	{
		str_curcarfolder = GetStringSimple(ptr_curcarfolder);
		std::string str_fullpath = "trkdata/cars/" + str_curcarfolder + "/" + str_curcrvfile;
		AllocString(ptr_curcar + 0x4910, str_fullpath);
	}
	else
	{
		AllocString(ptr_curcar + 0x4910, str_curcrvfile);
	}
}

void GetGearRatios()
{
	arr_gear_ratios.clear();

	str_gear_ratios = GetString(ptr_gear_ratios);

	std::stringstream ss(str_gear_ratios);
	std::string token;

	while (std::getline(ss, token, '|'))
	{
		if (!token.empty())
		{
			arr_gear_ratios.push_back(std::stof(token));
		}
	}
}

void GetDrive()
{
	str_drive = GetString(ptr_drive);

	if (str_drive == "front")
	{
		drive_bm = 0x00000101;
		drive_whls = 2;
	}
	else if (str_drive == "rear")
	{
		drive_bm = 0x01010000;
		drive_whls = 2;
	}
	else if (str_drive == "4wd")
	{
		drive_bm = 0x01010101;
		drive_whls = 4;
	}
	else
	{
		drive_bm = -1;
		drive_whls = -1;
	}
}

uint32_t gameStr[4] = { 0 };	// спиздил у фиксика

void __declspec(naked) a_TorqueCrv()
{
	__asm
	{
		mov ptr_curcar, esi

		// ============ torquecrv ============

		mov ebx, -1
		mov edx, var_torquecrv
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je rpm_max

		mov ebx, -1
		mov edx, var_torquecrv
		lea eax, [esp + 0x640]
		call sub_69586C

		lea edx, [esp + 0x20]
		mov ebx, eax
		lea eax, [esp + 0x470]
		call sub_677C00

		mov eax, [eax]
		mov ptr_curcrvfile, eax
		call GetCRVFile

		mov eax, esi	// car
		lea edx, gameStr
		call sub_4512C0

		lea  eax, gameStr
		call sub_695B53

		mov ptr_curcarfolder, eax

		call GetFullCRVFilePath

		lea eax, [esp + 0x470]
		xor edx, edx
		call sub_6959C9

		lea eax, [esp + 0x640]
		xor edx, edx
		call sub_6959C9

		// ============ rpm_max ============

	rpm_max:
		mov ebx, -1
		mov edx, var_rpm_max
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je rpm_idle

		mov ebx, -1
		mov edx, var_rpm_max
		lea eax,[esp+0x50]
		call sub_69586C

		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x50]
		xor edx,edx
		fstp dword ptr [esi+0x494C]
		call sub_6959C9

		// ============ rpm_idle ============

	rpm_idle:
		mov ebx, -1
		mov edx, var_rpm_idle
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je rpm_shift_down

		mov ebx, -1
		mov edx, var_rpm_idle
		lea eax,[esp+0x50]
		call sub_69586C

		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x50]
		xor edx,edx
		fstp dword ptr [esi+0x4950]
		call sub_6959C9

		// ============ rpm_shift_down ============

	rpm_shift_down:
		mov ebx, -1
		mov edx, var_rpm_shift_down
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je rpm_shift_up

		mov ebx, -1
		mov edx, var_rpm_shift_down
		lea eax,[esp+0x50]
		call sub_69586C

		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x50]
		xor edx,edx
		fstp dword ptr [esi+0x4954]
		call sub_6959C9

		// ============ rpm_shift_up ============

	rpm_shift_up:
		mov ebx, -1
		mov edx, var_rpm_shift_up
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je steerspeed

		mov ebx, -1
		mov edx, var_rpm_shift_up
		lea eax,[esp+0x50]
		call sub_69586C

		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x50]
		xor edx,edx
		fstp dword ptr [esi+0x4958]
		call sub_6959C9

		// ============ steerspeed ============

	steerspeed:
		mov ebx, -1
		mov edx, var_steerspeed
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je gear_ratios

		mov ebx, -1
		mov edx, var_steerspeed
		lea eax,[esp+0x50]
		call sub_69586C

		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x50]
		xor edx,edx

		fmul dword ptr [mul_steerspeed]

		fstp dword ptr [esi+0x6634]
		call sub_6959C9

		// ============ gear_ratios ============

	gear_ratios:
		mov ebx, -1
		mov edx, var_gear_ratios
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je scale_shadowz

		mov ebx, -1
		mov edx, var_gear_ratios
		lea eax, [esp + 0x640]
		call sub_69586C

		lea edx, [esp + 0x20]
		mov ebx, eax
		lea eax, [esp + 0x470]
		call sub_677C00

		mov eax, [eax]
		mov ptr_gear_ratios, eax
		call GetGearRatios

		mov ecx, [arr_gear_ratios]
		mov edx, [arr_gear_ratios + 4]
		sub edx, ecx  
		shr edx, 2  

		cmp edx, 255
		jbe size_ok
		mov edx, 255  

	size_ok:
		dec dl
		mov byte ptr [esi + 0x490C], dl
		inc dl

		test edx, edx
		jz copy_done

		lea edi, [esi + 0x496C] 
		mov ecx, [arr_gear_ratios]

	copy_loop:
		mov eax, [ecx]  
		mov [edi], eax 
		add ecx, 4   
		add edi, 4    
		dec edx  
		jnz copy_loop        

	copy_done:
		lea eax, [esp + 0x470]
		xor edx, edx
		call sub_6959C9

		lea eax, [esp + 0x640]
		xor edx, edx
		call sub_6959C9

		// ============ scale_shadowz ============

	scale_shadowz:
		mov ebx, -1
		mov edx, var_scale_shadowz
		lea eax,[esp+0x580]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_676250
		test al,al
		setne cl
		xor edx,edx
		lea eax,[esp+0x580]
		call sub_6959C9
		test cl,cl
		je add_wheelposzfront

		mov ebx, -1
		mov edx, var_scale_shadowz
		lea eax,[esp+0x4F0]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		fmul dword ptr [esi+0x4A78]
		lea eax,[esp+0x4F0]
		xor edx,edx
		fstp dword ptr [esi+0x4A78]
		call sub_6959C9

		// ============ add_wheelposzfront ============

	add_wheelposzfront:
		mov ebx,-1
		mov edx,var_add_wheelposzfront
		lea eax,[esp+0x580]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_676250
		test al,al
		setne cl
		xor edx,edx
		lea eax,[esp+0x580]
		call sub_6959C9
		test cl,cl
		je add_wheelposzrear

		mov ebx,-1
		mov edx, var_add_wheelposzfront
		lea eax,[esp+0x310]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		mov ebx,-1
		call sub_677960
		lea eax,[esp+0x310]
		fadd dword ptr [esi+0x6674]
		xor edx,edx
		fstp dword ptr [esi+0x6674]
		call sub_6959C9

		mov edx, var_add_wheelposzfront
		lea eax,[esp+0x320]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x320]
		fadd dword ptr [esi+0x6680]
		xor edx,edx
		fstp dword ptr [esi+0x6680]
		call sub_6959C9

		// ============ add_wheelposzrear ============

	add_wheelposzrear:
		mov ebx,-1
		mov edx,var_add_wheelposzrear
		lea eax,[esp+0x580]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_676250
		test al,al
		setne cl
		xor edx,edx
		lea eax,[esp+0x580]
		call sub_6959C9
		test cl,cl
		je drive

		mov ebx,-1
		mov edx, var_add_wheelposzrear
		lea eax,[esp+0x310]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		mov ebx,-1
		call sub_677960
		lea eax,[esp+0x310]
		fadd dword ptr [esi+0x668C]
		xor edx,edx
		fstp dword ptr [esi+0x668C]
		call sub_6959C9

		mov edx, var_add_wheelposzrear
		lea eax,[esp+0x320]
		call sub_69586C
		mov edx,eax
		lea eax,[esp+0x20]
		call sub_677960
		lea eax,[esp+0x320]
		fadd dword ptr [esi+0x6698]
		xor edx,edx
		fstp dword ptr [esi+0x6698]
		call sub_6959C9

		// ============ drive ============

	drive:
		mov ebx, -1
		mov edx, var_drive
		lea eax, [esp + 0x210]
		call sub_69586C
		mov edx, eax
		lea eax, [esp + 0x20]
		call sub_676250
		test al, al
		setne cl
		xor edx, edx
		lea eax, [esp + 0x210]
		call sub_6959C9
		test cl, cl
		je aftbur_accel

		mov ebx, -1
		mov edx, var_drive
		lea eax, [esp + 0x640]
		call sub_69586C

		lea edx, [esp + 0x20]
		mov ebx, eax
		lea eax, [esp + 0x470]
		call sub_677C00

		mov eax, [eax]
		mov ptr_drive, eax
		call GetDrive

		cmp dword ptr [drive_bm], -1
		je drive_end
		cmp dword ptr [drive_whls], -1
		je drive_end

		mov eax, drive_bm
		mov dword ptr [esi + 0x6621], eax
		mov al, drive_whls
		mov byte ptr [esi + 0x6620], al

	drive_end:
		lea eax, [esp + 0x470]
		xor edx, edx
		call sub_6959C9

		lea eax, [esp + 0x640]
		xor edx, edx
		call sub_6959C9
					
		// ============ aftbur_accel ============

	aftbur_accel:
		//mov ebx, -1
		//mov edx, var_aftbur_accel
		//lea eax, [esp + 0x210]
		//call sub_69586C
		//mov edx, eax
		//lea eax, [esp + 0x20]
		//call sub_676250
		//test al, al
		//setne cl
		//xor edx, edx
		//lea eax, [esp + 0x210]
		//call sub_6959C9
		//test cl, cl
		//je end

		//mov ebx, -1
		//mov edx, var_aftbur_accel
		//lea eax,[esp+0x50]
		//call sub_69586C

		//mov edx,eax
		//lea eax,[esp+0x20]
		//call sub_677960
		//lea eax,[esp+0x50]
		//xor edx,edx

		//fstp dword ptr [eax+0x3E4]
		//call sub_6959C9

	end:
		mov ebx, -1
		jmp loc_464770

	sub_677960:
		push 0x677960
		retn

	sub_4512C0:
		push 0x4512C0
		retn

	sub_46D540:
		push 0x46D540
		retn

	sub_69586C:
		push 0x69586C
		retn

	sub_676250:
		push 0x676250
		retn

	sub_6959C9:
		push 0x6959C9
		retn

	sub_677C00:
		push 0x677C00
		retn

	sub_695B53:
		push 0x695B53
		retn

	loc_464770:
		push 0x464770
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

	injector::MakeJMP(0x46D73D, 0x46D585, true);	// use default torque curve values as a fallback if torque curve file doesn't exists

	injector::MakeJMP(0x46476B, a_TorqueCrv, true);
}