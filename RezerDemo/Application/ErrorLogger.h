#pragma once
#include <string>
#include <Windows.h>

class ErrorLogger
{
public:
	static bool hrError(HRESULT hr, const std::string& message);
	static void errorMessage(const std::string& message);
};

