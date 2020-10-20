#include "stdafx.h"

#include "CoreException.h"
#include "StringTools.h"

const ErrorSeverity g_defaultSeverity = CriticalErrorSeverity;

static void* NewImpl(size_t size)
{
	G_ASSERT(size > 0);

	return malloc(size);
}

static void DeleteImpl(void* ptr)
{
	G_ASSERT(ptr != nullptr);

	free(ptr);
}

CoreException::CoreException(const QString& whatMessage)
	: whatMessage_(whatMessage)
	, severity_(g_defaultSeverity)
{
	QSTRING_NOT_EMPTY(whatMessage_);
}

CoreException::~CoreException()
{
}

void* CoreException::operator new(size_t size)
{
	return NewImpl(size);
}
void* CoreException::operator new[](size_t size) 
{  
	return NewImpl(size);
}

void CoreException::operator delete(void* ptr) {  
	DeleteImpl(ptr);
}

void CoreException::operator delete[](void* ptr) {
	DeleteImpl(ptr);
}

QString CoreException::GetErrorCorection() const
{
	return EMPTY_STR;
}

QString CoreException::GetDump() const
{
	QString dump = QString( STR("Exception '%1'") ).arg(CppTools::GetClassName(*this));
	dump += QString( STR("\n\nUser description:\n'%1'") ).arg( What() );

	const QString& errorCorrection = GetErrorCorection();
	if ( !errorCorrection.isEmpty() ) {
		dump += QString( STR("\n\nError correction:\n'%1'") ).arg( errorCorrection );
	}

	const QString& members = MembersToString();
	if ( !members.isEmpty() ) {
		dump += QString( STR("\n\nClass members:\n'%1'") ).arg(members);
	}

	if ( !customData_.isEmpty() ) {
		dump += QString( STR("\n\nCustom data:\n'%1'") ).arg( customData_ );
	}
	
	const QString& innerExceptionDump = GetInnerExceptionDump();
	if ( !innerExceptionDump.isEmpty() ) {
		dump += STR("\n\nDump from inner exception:\n") + innerExceptionDump;
	}

	return dump;
}

QString CoreException::MembersToString() const
{
	return EMPTY_STR;
}

QString CoreException::GetUserDescription() const
{
	QString description = What();
	if ( !customUserMessage_.isEmpty() ) {
		description += CHAR('\n') + customUserMessage_;
	}
	const QString errorCorrection = GetErrorCorection();
	if ( !errorCorrection.isEmpty() ) {
		description += QCoreApplication::translate("CoreException", "\n\nThis can help you:\n%1").arg(errorCorrection);
	}

	return description;
}

ErrorSeverity CoreException::GetSeverity() const
{
	return severity_;
}

void CoreException::SetSeverity(ErrorSeverity severity)
{
	severity_ = severity;
}

void CoreException::AddDumpFromException(const ExceptionInterface& exception)
{
	if (&exception != this) {
		innerExceptionsDump_ += exception.GetDump();
	}
}

void CoreException::AddCustomString(const QString& str)
{
	customData_ += str;
}

const QString& CoreException::GetInnerExceptionDump() const
{
	return innerExceptionsDump_;
}

void CoreException::AddCustomUserMessage(const QString& message)
{
	G_ASSERT( !message.isEmpty() );

	customUserMessage_ = message;
}

QString CoreException::What() const
{
	return whatMessage_;
}