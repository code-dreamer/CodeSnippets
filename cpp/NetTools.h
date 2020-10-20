#pragma once

namespace NetTools {
;
WX_DECLARE_STRING_HASH_MAP(wxString, HeadersMap);

wxCharTypeBuffer<char> Download(wxString urlStr, wxInt32 maxCount = -1, const HeadersMap& headers = HeadersMap());
wxString DownloadToString(const wxString& urlStr, wxInt32 maxCount = -1, const HeadersMap& headers = HeadersMap(), const wxMBConv& converter = wxConvAuto());
std::string DownloadToStdString(const wxString& urlStr, wxInt32 maxCount = -1, const HeadersMap& headers = HeadersMap());
bool DownloadToFile(const wxString& url, const wxString& outFilePath);

void Request(wxString urlStr, const HeadersMap& headers = HeadersMap());
void ARequest(const wxString& urlStr, const HeadersMap& headers = HeadersMap());

wxString GetRedirectUrl(wxString urlStr);
wxCharTypeBuffer<char> CombineChunkedResponse(const wxCharTypeBuffer<char>& buffer);
} // namespace NetTools