#pragma once

#include <string>
#include <windows.h>
#include "RegisterBit.h"

class DeviceRegister
{
private:
	std::string name;
	int numberOfBits;
	RegisterBit* bits;

public:
	DeviceRegister(std::string name, int numberOfBits);

	std::string GetName();
	LPCWSTR GetName_LPCWSTR();

	int GetNumberOfBits();

	void SetBit(int position, RegisterBit bit);
	RegisterBit GetBit(int position);

};

