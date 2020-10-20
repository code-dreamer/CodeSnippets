#include "stdafx.h"
#include "UrlUtils.h"

namespace Net {
namespace UrlUtils {

wxURL MakeUrl(wxString str)
{
	CHECK_WXSTRING(str);

	wxString prefix = _S("http://");
	if (!str.StartsWith(prefix))
		str.Prepend(prefix);

	wxURL url(str);
	wxURLError error = url.GetError();
	DCHECK(error == wxURL_NOERR);
	
	return url;
}

bool IsUrlStrValid(const wxString& urlStr)
{
	CHECK_WXSTRING(urlStr);
	return (wxURL(urlStr).GetError() == wxURL_NOERR);
}

bool IsUrlValid(const wxURL& url)
{
	return (url.GetError() == wxURL_NOERR);
}

wxString GetPath(const wxURL& url)
{
	if (!url.IsOk()) {
		FAIL;
		return wxEmptyString;
	}

	wxString urlStr = url.BuildURI();
	wxString host = url.GetServer();

	size_t ind;
	if ((ind = urlStr.find(host)) == wxString::npos) {
		wxFAIL;
		return wxEmptyString;
	}

	ind += host.Len();
	wxString path = urlStr.Mid(ind, urlStr.Len() - ind);
	return path;
}

} // namespace UrlUtils
} // namespace Net
