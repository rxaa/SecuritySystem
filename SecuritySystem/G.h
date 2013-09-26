#pragma once
#include "MainModel.h"

class G
{
private:
	static df::Config mainIni_;

public:
	static MainModel main;

	static bool ReadMain()
	{
		return mainIni_.ReadToObject(main);
	}

	static bool WriteMain()
	{
		return mainIni_.WriteFromObject(main);
	}
};

