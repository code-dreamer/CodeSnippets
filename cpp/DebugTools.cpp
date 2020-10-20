#include "stdafx.h"

#include <csignal>
#include "DebugTools.h"

int DebugTools::ShowDebugMessageBox(const wchar_t* expression, const wchar_t* functionName, 
	const wchar_t* filename, unsigned lineNumber, const wchar_t* errorMessage)
{
	wchar_t exeName[MAX_PATH];
	GetModuleFileNameW(nullptr, exeName, MAX_PATH);

	std::wstringstream customMessage;
	if (errorMessage != nullptr) {
		customMessage << L"Error message: '" << errorMessage << L"'" << std::endl;
	}

	std::wstringstream message;
	message << L"Assertion failed!" << std::endl
		<< std::endl
		<< L"Program: " << exeName << std::endl
		<< std::endl
		<< L"Expression: '" << expression << L"'"
		<< std::endl
		<< customMessage.str()
		<< std::endl
		<< L"Function: " << functionName << std::endl
		<< L"File: " << filename << std::endl
		<< L"Line: " << lineNumber << std::endl;

	return MessageBoxW(0, message.str().c_str(), L"Assertion", MB_ICONERROR | MB_ABORTRETRYIGNORE);
}

void DebugTools::AbortApplication()
{
	raise(SIGABRT);																														\
	_exit(3);
}
