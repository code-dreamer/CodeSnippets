#include "stdafx.h"
#include "Net/Utils.h"
#include "HttpClient.h"
#include "UrlUtils.h"

namespace Net {
namespace Utils {
;
wxString GetHttpContent(const wxString& urlStr)
{
	Reply reply = MakeRequest(urlStr);
	return reply.ReadDataToString();
}

Reply MakeRequest(const wxString& urlStr)
{
	CHECK_WXSTRING(urlStr);

	Request request = CreateDefaultRequest(urlStr);
	HttpClient client;
	return client.Get(request);
}

class AsyncRunner : public wxThread
{
	NO_COPY_CLASS(AsyncRunner);

public:
	AsyncRunner()
		: wxThread{wxTHREAD_DETACHED}
	{}

	void Start(std::function<void(void)> func)
	{
		CHECK(func);
		CHECK(!IsRunning());

		m_func = func;
		wxThread::Run();
	}

protected:
	virtual ExitCode Entry() override
	{
		m_func();
		return (wxThread::ExitCode)0;
	}

private:
	std::function<void(void)> m_func;
};

void MakeARequest(const wxString& urlStr)
{
	AsyncRunner* runner = new AsyncRunner;
	runner->Start([=]() { MakeRequest(urlStr); });
}

wxString GetRedirectUrl(const wxString& urlStr, wxString* name)
{
	CHECK_WXSTRING(urlStr);

	wxURL origUrl = UrlUtils::MakeUrl(urlStr);

	Reply reply = MakeRequest(urlStr);
	if (name)
		*name = reply.GetContentFilename();

	const wxURL& redirectUrl = reply.GetRequest().GetUrl();
	return (origUrl == redirectUrl) ? wxEmptyString : redirectUrl.BuildURI();
}

} // namespace Utils
} // namespace Net
