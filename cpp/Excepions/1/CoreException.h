#pragma once

//////////////////////////////////////////////////
// don't write something like that:
// try {
// ...
// }
// catch (Exception& e) {
//		throw e; // wrong
//		throw; // ok
// }
//////////////////////////////////////////////////

#include "ExceptionInterface.h"
#include "CppTools.h"
#include "StringTools.h"

class KERNEL_API CoreException : public ExceptionInterface
{
public:
	CoreException(const QString& whatMessage);
	virtual ~CoreException();

	//Exception(const Exception&){};
private:
	CoreException& operator=(const CoreException&);
	
protected: // deny creating on the heap
	static void* operator new(size_t size);
	static void* operator new[](size_t size);
	static void operator delete(void* ptr);
	static void operator delete[](void* ptr);

public:
	virtual QString What() const override;	// override for error custom message for user
	virtual QString GetErrorCorection() const override;	// override to show the user how to fix this error

	virtual QString GetUserDescription() const override;
	virtual QString GetDump() const override;

	virtual void SetSeverity(ErrorSeverity severity) override;
	virtual ErrorSeverity GetSeverity() const override;

	void AddCustomUserMessage(const QString& message);
	void AddCustomString(const QString& str);
	
	virtual void AddDumpFromException(const ExceptionInterface& exception);

protected:
	virtual QString MembersToString() const;	// override, to put members values to dump
	const QString& GetInnerExceptionDump() const;

private:
	QString whatMessage_;
	ErrorSeverity severity_;
	QString customData_;
	QString customUserMessage_;
	QString innerExceptionsDump_;
};

namespace CoreExceptionOperators
{

template<typename ExceptionType>
QTextStream& operator<<(QTextStream& dest, const ExceptionType& src)
{
	dest << src.GetDump();
	return dest;
}

template<typename ExceptionType>
QString& operator<<(QString& dest, const ExceptionType& src)
{
	//QTextStream cout(&dest, QIODevice::ReadWrite | QIODevice::Text);
	QTextStream cout(&dest);
	cout << src.GetDump();

	return dest;
}

template<typename ExceptionType>
ExceptionType& operator<<(ExceptionType& dest, const QString& src)
{
	dest.AddCustomString(src);
	return dest;
}

template<typename ExceptionType>
ExceptionType& operator<<(ExceptionType& dest, const char* src)
{
	const QString srcStr = StringTools::CreateQString(src);
	dest.AddCustomString(srcStr);

	return dest;
}

template<typename ExceptionType>
ExceptionType& operator<<(ExceptionType& dest, const ExceptionInterface& src)
{
	dest.AddDumpFromException(src);
	return dest;
}

template<typename ExceptionType>
ExceptionType& operator<<(ExceptionType& dest, QTextStream &(*fun)(QTextStream &))
{
	//QTextStream cout(&tempStr, QIODevice::ReadWrite | QIODevice::Text);

	QString tempStr;
	QTextStream cout(&tempStr);
	fun(cout);
	dest.AddCustomString(tempStr);

	return dest;
}

}