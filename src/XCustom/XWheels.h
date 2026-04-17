
void __declspec(naked) WheelsSize()
{
	__asm
	{
		cmp byte ptr [esi+0x4B24], 1
		je end_false
		jmp truecond

	truecond:
		mov[edx + 0x3C], al

		call ReadTextLine
		call sub_696137
		mov eax, [esp+0x36C]
		fstp dword ptr [eax+0x40] 
		cmp dword ptr [eax+0x40], 0  
		je use_alternative      
		fld dword ptr [eax+0x40]   
		jmp store_value
		
	use_alternative:
		fld dword ptr [eax+0x8]    
	
	store_value:
		fst dword ptr [eax+0x8]  
		jmp end_true

	end_true:
		jmp loc_47EBA8

	end_false:
		mov[edx + 0x3C], al
		jmp loc_47EBA8

	loc_47EFBA:
		push 0x47EFBA
		retn

	sub_696137:
		push 0x696137
		retn

	ReadTextLine:
		push 0x661200
		retn

	loc_47EBA8:
		push 0x47EBA8
		retn
	}
}

void __declspec(naked) WheelsSizeValDupl()
{
	__asm
	{
		fstp st(1)
		fst dword ptr[eax + 0x08]	// front wheels
		fst dword ptr[eax + 0x40]	// rear wheels (duplicating for using it as a fallback if rear wheels size = 0)
		jmp loc_47EA9D

	loc_47EA9D:
		push 0x47EA9D
		retn
	}
}

void __declspec(naked) WheelsUniqueModels()
{
	__asm
	{
		cmp byte ptr[esi + 0x4B24], 1
		je end
		jmp truecond

	truecond:
		call ReadTextLine
		//call sub_6960F0
		cmp byte ptr [eax], 0
		je end
		mov edx, eax
		lea eax, [esp + 0xC0]
		mov ebx, 0x695FE0
		call sub_695A15
		lea edx, [esp + 0x2D0]
		lea eax, [esp + 0xC0]
		call sub_695FEE
		lea edx, [esp + 0x2D0]
		lea eax, [esp + 0xC0]
		call sub_695F1D
		lea eax, [esp + 0x2D0]
		xor edx, edx
		call sub_6959C9
		jmp end

	end:
		call sub_660970
		lea edx, [esp + 0xC0]
		jmp loc_47ECEF

	sub_6960F0:
		push 0x6960F0
		retn

	ReadTextLine:
		push 0x661200
		retn

	sub_6959C9:
		push 0x6959C9
		retn

	sub_695F1D:
		push 0x695F1D
		retn

	sub_695FEE:
		push 0x695FEE
		retn

	sub_695A15:
		push 0x695A15
		retn

	loc_47ECEF:
		push 0x47ECEF
		retn

	sub_660970:
		push 0x660970
		retn
	}
}

void XWheels()
{
	injector::MakeNOP(0x47EBF9, 5);

	injector::MakeJMP(0x47EB9D, WheelsSize, true);
	injector::MakeJMP(0x47EA98, WheelsSizeValDupl, true);
	injector::MakeJMP(0x47ECE8, WheelsUniqueModels, true);
}