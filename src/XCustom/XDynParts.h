float calculateDistortion(float heightA, float heightB, float lengthA) {
	// Коэффициент жесткости подвески
	const float stiffness = 0.5f;

	// Разница в высоте между объектами
	float heightDifference = heightB - heightA;

	// Простая модель искажения, основанная на разнице высот
	float distortion = (heightDifference / lengthA) * stiffness;

	// Поскольку искажение не должно выходить за пределы определенных значений, можно добавить ограничение
	if (distortion > 1.0f) {
		distortion = 1.0f; // Максимальное искажение
	}
	else if (distortion < -1.0f) {
		distortion = -1.0f; // Минимальное искажение
	}

	return distortion;
}

void XDynParts()
{
	LOOP_PLAYERS
	{
		if (CarCurrentlyLoaded() || CDRace())
		{
			float curwhlangle = GetPlayerParam<float>(CDPlayer::CurWheelsAngle, player - 1);

			Matrix4x4 whlrot = rotationToMatrix4x4(0.f, curwhlangle * -1, 0.f);

			int NumMeshes = GetPlayerParam<short>(CDPlayer::NumMeshes, player - 1);
			
			for (int i = 0; i < NumMeshes; ++i)
			{
				DWORD cursubmesh = CarSubmesh(player - 1, i);
				if (cursubmesh)
				{
					std::string cursubmesh_name = GetString(injector::ReadMemory<void*>(cursubmesh));

					if (cursubmesh_name.contains("mudfl") || cursubmesh_name.contains("mudfr"))
					{
						WriteMemoryMatrix4x4(cursubmesh + 0x40, whlrot, true);
					}

					if (cursubmesh_name.contains("mudfr"))
					{
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFR, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelFRPosY, player - 1);
						float whl_x = GetPlayerParam<float>(CDPlayer::WheelFRPosX, player - 1);
						float width = GetWheelParam<float>(CDWheel::WidthF, player - 1, 1);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 1) / 2.f;
						injector::WriteMemory(cursubmesh + 0x38, (springload * -1) - (whl_y * -1) + whl_rad);
						injector::WriteMemory(cursubmesh + 0x34, whl_x - (width / 2.f));
						injector::WriteMemory(cursubmesh + 0x3C, GetPlayerParam<float>(CDPlayer::WheelFRPosZ, player - 1));
					}
					else if (cursubmesh_name.contains("mudfl"))
					{
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFL, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelFLPosY, player - 1);
						float whl_x = GetPlayerParam<float>(CDPlayer::WheelFLPosX, player - 1);
						float width = GetWheelParam<float>(CDWheel::WidthF, player - 1, 0);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 0) / 2.f;
						injector::WriteMemory(cursubmesh + 0x38, (springload * -1) - (whl_y * -1) + whl_rad);
						injector::WriteMemory(cursubmesh + 0x34, whl_x + (width / 2.f));
						injector::WriteMemory(cursubmesh + 0x3C, GetPlayerParam<float>(CDPlayer::WheelFLPosZ, player - 1));
					}
					else if (cursubmesh_name.contains("mudrr"))
					{
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRR, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelRRPosY, player - 1);
						float whl_x = GetPlayerParam<float>(CDPlayer::WheelRRPosX, player - 1);
						float width = GetWheelParam<float>(CDWheel::WidthR, player - 1, 3);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 3) / 2.f;
						injector::WriteMemory(cursubmesh + 0x38, (springload * -1) - (whl_y * -1) + whl_rad);
						injector::WriteMemory(cursubmesh + 0x34, whl_x - (width / 2.f));
						injector::WriteMemory(cursubmesh + 0x3C, GetPlayerParam<float>(CDPlayer::WheelRRPosZ, player - 1));
					}
					else if (cursubmesh_name.contains("mudrl"))
					{
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRL, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelRLPosY, player - 1);
						float whl_x = GetPlayerParam<float>(CDPlayer::WheelRLPosX, player - 1);
						float width = GetWheelParam<float>(CDWheel::WidthR, player - 1, 2);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 2) / 2.f;
						injector::WriteMemory(cursubmesh + 0x38, (springload * -1) - (whl_y * -1) + whl_rad);
						injector::WriteMemory(cursubmesh + 0x34, whl_x + (width / 2.f));
						injector::WriteMemory(cursubmesh + 0x3C, GetPlayerParam<float>(CDPlayer::WheelRLPosZ, player - 1));
					}

					if (cursubmesh_name.contains("axlefr"))
					{
						float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
						float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFR, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelFRPosY, player - 1);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 1) / 2.f;
						injector::WriteMemory(cursubmesh + 0x44, (whl_rad / bb_x) + (whl_y / bb_x) + ((springload * -1) / bb_x));
					}
					else if (cursubmesh_name.contains("axlefl"))
					{
						float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
						float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFL, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelFLPosY, player - 1);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 0) / 2.f;
						injector::WriteMemory(cursubmesh + 0x44, ((whl_rad / bb_x) + (whl_y / bb_x) + ((springload * -1) / bb_x)) * -1);
					}
					else if (cursubmesh_name.contains("axlerr"))
					{
						float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
						float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRR, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelRRPosY, player - 1);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 3) / 2.f;
						injector::WriteMemory(cursubmesh + 0x44, (whl_rad / bb_x) + (whl_y / bb_x) + ((springload * -1) / bb_x));
					}
					else if (cursubmesh_name.contains("axlerl"))
					{
						float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
						float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
						float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRL, player - 1));
						float whl_y = GetPlayerParam<float>(CDPlayer::WheelRLPosY, player - 1);
						float whl_rad = GetWheelParam<float>(CDWheel::Diameter, player - 1, 2) / 2.f;
						injector::WriteMemory(cursubmesh + 0x44, ((whl_rad / bb_x) + (whl_y / bb_x) + ((springload * -1) / bb_x)) * -1);
					}

					if (cursubmesh_name.contains("axlefl") || cursubmesh_name.contains("axlefr"))
					{
						injector::WriteMemory<BYTE>(GetPlayerParam<DWORD>(CDPlayer::AxleFPtr, player - 1) + 0x108, 0);
					}
					if (cursubmesh_name.contains("axlerl") || cursubmesh_name.contains("axlerr"))
					{
						injector::WriteMemory<BYTE>(GetPlayerParam<DWORD>(CDPlayer::AxleRPtr, player - 1) + 0x108, 0);
					}
				}
			}
		}
	}
}