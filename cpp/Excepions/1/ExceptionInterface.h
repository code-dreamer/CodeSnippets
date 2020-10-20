#pragma once

enum ErrorSeverity
{
	InformationErrorSeverity = 0,	// not critical error, user can continues his work
	WarningErrorSeverity,			// dangerous error, application can be unstable
	CriticalErrorSeverity			// fatal error, very unstable state
};

class NOINIT_VTABLE ExceptionInterface 
{
public:
	virtual QString What() const = 0;	// override for error custom message for user
	virtual QString GetErrorCorection() const = 0;	// override to show to user how to fix this error

	virtual QString GetUserDescription() const = 0;	// constructs string for user
	virtual QString GetDump() const = 0;			// constructs information about this and all inner exceptions.

	virtual void SetSeverity(ErrorSeverity severity) = 0;
	virtual ErrorSeverity GetSeverity() const = 0;

	virtual ~ExceptionInterface() = 0 {};
};