#pragma once

#include "Net/Reply.h"
#include "Net/Request.h"

namespace Net {

class Request;

class HttpClient
{
public:
	HttpClient();
	~HttpClient();

	Reply Get(const Request& request);

	NO_COPY_CLASS(HttpClient);
};

Request CreateDefaultRequest(const wxString& urlStr);

} // namespace Net
