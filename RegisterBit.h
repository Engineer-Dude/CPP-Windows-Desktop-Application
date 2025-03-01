#pragma once

#include <string>
#include <windows.h>

class RegisterBit
{
private:
	std::string description;
	bool isChecked;
public:
	RegisterBit();

	void SetDescription(std::string value);
	std::string GetDescription();
	LPCWSTR GetDescription_LPCWSTR();

	void SetIsChecked(bool isChecked);
	bool GetIsChecked();

};

