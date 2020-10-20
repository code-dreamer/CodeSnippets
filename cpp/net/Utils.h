#pragma once

#include "Net/Reply.h"

namespace Net {
namespace Utils {
;
Reply MakeRequest(const wxString& urlStr);
void MakeARequest(const wxString& urlStr);
wxString GetHttpContent(const wxString& urlStr);
wxString GetRedirectUrl(const wxString& urlStr, wxString* name = nullptr);
} // namespace Utils
} // namespace Net


