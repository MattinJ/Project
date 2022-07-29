#include "ErrorLogger.h"

bool ErrorLogger::hrError(HRESULT hr, const std::string& message)
{
	if (FAILED(hr))
	{
		MessageBoxA(NULL, message.c_str(), "HR error", MB_OK);
		return false;
	}

	return true;
}

void ErrorLogger::errorMessage(const std::string& message)
{
	MessageBoxA(NULL, message.c_str(), "Error message", MB_OK);
}
	
