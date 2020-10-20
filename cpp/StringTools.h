#pragma once

//
// QString translatedString = tr("File")
// QString untranslatedString = QLatin1String("window")
// QString untranslatedChar = QLatin1Char("w")

namespace boost
{
namespace posix_time
{
	class time_duration;
}
}

namespace StringTools
{
	KERNEL_API std::wstring CreateStdWString(const QString& str);
	KERNEL_API std::string CreateStdString(const QString& str, bool utf8 = false);
	KERNEL_API QString CreateQString(const std::wstring& wstr);
	KERNEL_API QString CreateQString(const std::string& str, bool utf8 = false);
	KERNEL_API QString CreateQString(const wchar_t* array, int size = -1);
	KERNEL_API QString CreateQString(const char* array, int size = -1);
	KERNEL_API int CopyQStringToWArray(const QString& str, wchar_t* array);

	KERNEL_API QString BytesToHumanString(uint64_t bytesAmount, unsigned int digitsAfterDecimal = 2);
	KERNEL_API QString SecondsToHumanString(unsigned long);
	KERNEL_API QString TimeToHumanString(const boost::posix_time::time_duration& time);

	KERNEL_API QString ExtractFirstWord(const QString& str);

	KERNEL_API bool IsValidEmail(const QString& email);
	KERNEL_API wchar_t* DeleteEnclosingQuote(wchar_t* stringInQuote);
}
