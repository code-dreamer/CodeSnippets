#include "stdafx.h"
#include "Request.h"
#include "UrlUtils.h"

namespace Net {
;
using namespace UrlUtils;

wxString GetHeaderName(KnownHeaders header)
{
	switch (header) {
		case KnownHeaders::UserAgent:
			return wxS("User-Agent");
			break;
		case KnownHeaders::Referer:
			return wxS("Referer");
			break;
		case KnownHeaders::Host:
			return wxS("Host");
			break;
		case KnownHeaders::Get:
			return wxS("GET");
			break;
		case KnownHeaders::ContentType:
			return wxS("Content-type");
			break;
		case KnownHeaders::Connection:
			return wxS("Connection");
			break;
		case KnownHeaders::ContentDisposition:
			return wxS("Content-Disposition");
			break;
	}

	DFAIL;
	return wxEmptyString;
}

Request::Request(const wxURL& url)
{
	SetUrl(url);
}

Request::~Request()
{}

void Request::SetHeader(KnownHeaders header, const wxString& value)
{
	wxString headerName = GetHeaderName(header);
	SetHeader(headerName, value);
}

void Request::SetHeader(const wxString& name, const wxString& value)
{
	mHeaders[name] = value;
}

const Request::HeadersMap& Request::GetHeaders() const
{
	return mHeaders;
}

const wxURL& Request::GetUrl() const
{
	DCHECK(IsUrlValid(mUrl));
	return mUrl;
}

void Request::SetUrl(const wxURL& url)
{
	DCHECK(IsUrlValid(url));
	mUrl = url;
}

bool Request::IsHeaderPresent(const wxString& name) const
{
	return (mHeaders.find(name) != mHeaders.end());
}

bool Request::IsHeaderPresent(KnownHeaders header) const
{
	return IsHeaderPresent(GetHeaderName(header));
}

} // namespace Net
