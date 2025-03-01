#include "RegisterBit.h"
#include <string>

RegisterBit::RegisterBit()
	:description(""), isChecked(false)
{
}

void RegisterBit::SetDescription(std::string value) { description = value; }
std::string RegisterBit::GetDescription() { return description; }
LPCWSTR RegisterBit::GetDescription_LPCWSTR()
{
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, NULL, 0);
	wchar_t* wideString = new wchar_t[sizeNeeded];
	MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, wideString, sizeNeeded);
	return wideString;
}

void RegisterBit::SetIsChecked(bool isChecked) { this->isChecked = isChecked; }
bool RegisterBit::GetIsChecked() { return isChecked; }
