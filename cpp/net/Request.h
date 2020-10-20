#pragma once

namespace Net {
;
enum class KnownHeaders
{
	UserAgent,
	Referer,
	Host,
	Get,
	ContentType,
	Connection,
	ContentDisposition
};

wxString GetHeaderName(KnownHeaders header);

class Request
{
public:
	WX_DECLARE_STRING_HASH_MAP(wxString, HeadersMap);

public:
	Request(const wxURL& url);
	~Request();

	void SetHeader(KnownHeaders header, const wxString& value);
	void SetHeader(const wxString& name, const wxString& value);
	bool IsHeaderPresent(const wxString& name) const;
	bool IsHeaderPresent(KnownHeaders header) const;

	const HeadersMap& GetHeaders() const;
	const wxURL& GetUrl() const;
	void SetUrl(const wxURL& url);

private:
	HeadersMap mHeaders;
	wxURL mUrl;
};

} // namespace Net
