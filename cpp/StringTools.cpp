#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable : 4061)
#include <boost/date_time/time.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#pragma warning(pop)

#include "MathTools.h"
#include "StringTools.h"

std::wstring StringTools::CreateStdWString(const QString &str)
{
#ifdef _MSC_VER
	return std::wstring( reinterpret_cast<const wchar_t *>(str.utf16()) );
#else
	return str.toStdWString();
#endif
}

std::string StringTools::CreateStdString(const QString &str, bool utf8)
{
	if (utf8) {
		const std::string result = str.toUtf8().constData();
		return result;
	}
	else {
		return str.toStdString();
	}
}

QString StringTools::CreateQString(const std::wstring& wstr)
{
#ifdef _MSC_VER
	return QString::fromUtf16( reinterpret_cast<const ushort *>(wstr.c_str()) );
#else
	return QString::fromStdWString(str);
#endif
}

QString StringTools::CreateQString(const std::string& str, const bool utf8)
{
	if (utf8) {
		return QString::fromUtf8(str.c_str());
	}
	else {
		return QString::fromStdString(str);
	}
}

QString StringTools::CreateQString(const wchar_t* array, int size /*= -1*/)
{
	CHECK_PTR(array);

#ifdef _MSC_VER
	return QString::fromUtf16( reinterpret_cast<const ushort *>(array), size);
#else
	return QString::fromUcs4((uint *)string, size);
#endif
}

QString StringTools::CreateQString(const char* array, const int size)
{
	CHECK_PTR(array);

	return QString::fromAscii(array, size);
}

int StringTools::CopyQStringToWArray(const QString &str, wchar_t* array)
{
	CHECK_PTR(array);

#ifdef _MSC_VER
	memcpy(array, str.utf16(), sizeof(wchar_t) * str.length());
	return str.length();
#else
	wchar_t *a = array;
	const unsigned short *uc = utf16();
	for (int i = 0; i < length(); ++i) {
		uint u = uc[i];
		if (QChar::isHighSurrogate(u) && i + 1 < length()) {
			ushort low = uc[i+1];
			if (QChar::isLowSurrogate(low)) {
				u = QChar::surrogateToUcs4(u, low);
				++i;
			}
		}
		*a = wchar_t(u);
		++a;
	}
	return a - array;
#endif
}

QString StringTools::BytesToHumanString(const uint64_t bytesAmount, unsigned int digitsAfterDecimal)
{
	uint64_t tempBytesAmount = bytesAmount;
	int powAmount = 0;
	while ( (tempBytesAmount /= 1024) > 0) {
		++powAmount;
	}

	QString prefix;
	switch (powAmount) {
	case 0:
		prefix = QCoreApplication::translate("StringTools", "b");
		digitsAfterDecimal = 0;
		break;
	case 1:
		prefix = QCoreApplication::translate("StringTools","Kb");
		break;
	case 2:
		prefix = QCoreApplication::translate("StringTools","Mb");
		break;
	case 3:
		prefix = QCoreApplication::translate("StringTools","Gb");
		break;
	case 4:
		prefix = QCoreApplication::translate("StringTools","Tb");
		break;
	case 5:
		prefix = QCoreApplication::translate("StringTools","Pb");
		break;
	case 6:
		prefix = QCoreApplication::translate("StringTools","Eb");
		break;
	case 7:
		prefix = QCoreApplication::translate("StringTools","Zb");
		break;
	case 8:
		prefix = QCoreApplication::translate("StringTools","Yb");
		break;
	default:
		prefix = STR("after Yb");
		G_ASSERT_MSG(false, "UNKNOW VALUE. WHAT AFTER YB?");
	}
	
	G_ASSERT(digitsAfterDecimal < 10);
	char strformat[10];
	sprintf(strformat, "%%.%df", digitsAfterDecimal);

	long double tmpValue = (long double)bytesAmount / pow(1024.0l, powAmount);
	char doubleValue[256];
	sprintf(doubleValue, strformat, tmpValue);

	return QString(STR("%1 %2")).arg(CreateQString(doubleValue), prefix);
}

QString StringTools::SecondsToHumanString(const unsigned long seconds)
{
	return TimeToHumanString(boost::posix_time::seconds(seconds)); 
}

QString StringTools::TimeToHumanString(const boost::posix_time::time_duration& time)
{
	QString dayPart;
	const int days = time.hours()/24;
	if (days != 0) {
		dayPart = QCoreApplication::translate("StringTools", "%1 day ").arg(days);
	}
	
	QString hoursPart;
	const int hours = time.hours() - days*24;
	if (hours != 0) {
		hoursPart = QCoreApplication::translate("StringTools", "%1 hr. ").arg(hours);
	}

	QString minutesPart;
	const int minutes = time.minutes();
	if (minutes != 0) {
		minutesPart = QCoreApplication::translate("StringTools", "%1 min. ").arg(minutes);
	}

	QString result = dayPart + hoursPart + minutesPart;
	if (result.isEmpty()) {
		const int seconds = time.seconds();
		if (seconds != 0) {
			result += QCoreApplication::translate("StringTools", "%1 sec. ").arg(seconds);
		}
	}

	return result;
}

QString StringTools::ExtractFirstWord(const QString& str)
{
	QSTRING_NOT_EMPTY(str);

	QStringList words = str.split(STR(" "), QString::SkipEmptyParts, Qt::CaseInsensitive);
	G_ASSERT(!words.empty());

	return words.first();
}

bool StringTools::IsValidEmail(const QString& email)
{
	QSTRING_NOT_EMPTY(email);

	const QString pattern = STR("^(\\S+)@([a-z0-9-]+)(\\.)([a-z]{2,4})(\\.?)([a-z]{0,4})+$");
	const QRegExp reg(pattern);
	return (email.contains(reg).operator const void *() != nullptr);
}

wchar_t* StringTools::DeleteEnclosingQuote(wchar_t* stringInQuote)
{
	G_ASSERT(stringInQuote != nullptr);

	wchar_t* temp = stringInQuote;
	int valueLent = wcslen(stringInQuote);
	if(valueLent > 2) {
		if(stringInQuote[valueLent-1] == '\"') {
			stringInQuote[valueLent-1] = '\0';
		}
		if(stringInQuote[0] == '\"') {
			temp = stringInQuote + 1;
		}
	}
	return temp;
}