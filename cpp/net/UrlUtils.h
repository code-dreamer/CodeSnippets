#pragma once

namespace Net {
namespace UrlUtils {
;
wxURL MakeUrl(wxString str);

bool IsUrlStrValid(const wxString& urlStr);
bool IsUrlValid(const wxURL& url);

wxString GetPath(const wxString& urlStr);
wxString GetPath(const wxURL& url);

} // namespace UrlUtils
} // namespace Net
