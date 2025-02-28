#pragma once

#include <string>
#include <windows.h>

class DeviceRegister
{
private:
	std::string name;
	int numberOfBits;

public:
	DeviceRegister(std::string name, int numberOfBits);

	std::string GetName();
	LPCWSTR GetName_LPCWSTR();
};

