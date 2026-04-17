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
		int NumMeshes = GetPlayerParam<BYTE>(CDPlayer::NumMeshes, player - 1);
		float curwhlangle = GetPlayerParam<float>(CDPlayer::CurWheelsAngle, player - 1);

		Matrix4x4 whlrot = rotationToMatrix4x4(0.f, curwhlangle * -1, 0.f);

		for (int i = 0; i < NumMeshes; i++)
		{
			DWORD cursubmesh = CarSubmesh(player - 1, i);
			std::string cursubmesh_name = GetString(injector::ReadMemory<void*>(cursubmesh));

			if (cursubmesh_name.contains("mudfl") || cursubmesh_name.contains("mudfr"))
			{
				WriteMemoryMatrix4x4(cursubmesh + 0x40, whlrot, true);
			}

			if (cursubmesh_name.contains("mudfr"))
			{
				injector::WriteMemory(cursubmesh + 0x38, GetPlayerParam<float>(CDPlayer::SpringLoadFR, player - 1) * -1);
			}
			else if (cursubmesh_name.contains("mudfl"))
			{
				injector::WriteMemory(cursubmesh + 0x38, GetPlayerParam<float>(CDPlayer::SpringLoadFL, player - 1) * -1);
			}
			else if (cursubmesh_name.contains("mudrr"))
			{
				injector::WriteMemory(cursubmesh + 0x38, GetPlayerParam<float>(CDPlayer::SpringLoadRR, player - 1) * -1);
			}
			else if (cursubmesh_name.contains("mudrl"))
			{
				injector::WriteMemory(cursubmesh + 0x38, GetPlayerParam<float>(CDPlayer::SpringLoadRL, player - 1) * -1);
			}

			if (cursubmesh_name.contains("axlefr"))
			{
				float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
				float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
				float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFR, player - 1));
				float whl_y = GetPlayerParam<float>(CDPlayer::WheelFRPosY, player - 1);
				injector::WriteMemory(cursubmesh + 0x44, ((springload * -1) / bb_x) + (whl_y * -1));
			}
			else if (cursubmesh_name.contains("axlefl"))
			{
				float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
				float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
				float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadFL, player - 1));
				float whl_y = GetPlayerParam<float>(CDPlayer::WheelFLPosY, player - 1);
				injector::WriteMemory(cursubmesh + 0x44, (((springload * -1) / bb_x) + (whl_y * -1)) * -1);
			}
			else if (cursubmesh_name.contains("axlerr"))
			{
				float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
				float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
				float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRR, player - 1));
				float whl_y = GetPlayerParam<float>(CDPlayer::WheelRRPosY, player - 1);
				injector::WriteMemory(cursubmesh + 0x44, ((springload * -1) / bb_x) + (whl_y * -1));
			}
			else if (cursubmesh_name.contains("axlerl"))
			{
				float bb_x = injector::ReadMemory<float>(cursubmesh + 0x28);
				float pos_y = injector::ReadMemory<float>(cursubmesh + 0x38);
				float springload = (GetPlayerParam<float>(CDPlayer::SpringLoadRL, player - 1));
				float whl_y = GetPlayerParam<float>(CDPlayer::WheelRLPosY, player - 1);
				injector::WriteMemory(cursubmesh + 0x44, (((springload * -1) / bb_x) + (whl_y * -1)) * -1);
			}

			if (cursubmesh_name.contains("axlefl") || cursubmesh_name.contains("axlerl"))
			{
				injector::WriteMemory<BYTE>(GetPlayerParam<DWORD>(CDPlayer::AxleFPtr, player - 1) + 0x108, 0);
			}
			if (cursubmesh_name.contains("axlefr") || cursubmesh_name.contains("axlerr"))
			{
				injector::WriteMemory<BYTE>(GetPlayerParam<DWORD>(CDPlayer::AxleRPtr, player - 1) + 0x108, 0);
			}
		}
	}
}