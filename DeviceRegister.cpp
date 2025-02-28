#include "DeviceRegister.h"
#include <string.h>
#include <windows.h>

DeviceRegister::DeviceRegister(std::string name, int numberOfBits)
	: name(name), numberOfBits(numberOfBits)
{
}

std::string DeviceRegister::GetName() { return name; }

LPCWSTR DeviceRegister::GetName_LPCWSTR()
{
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, NULL, 0);
	wchar_t* wideString = new wchar_t[sizeNeeded];
	MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wideString, sizeNeeded);
	return wideString;
}

