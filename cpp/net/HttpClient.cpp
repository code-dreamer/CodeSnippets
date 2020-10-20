#include "stdafx.h"
#include "HttpClient.h"
#include "UrlUtils.h"

//wxProtocol::ReadLine

namespace Net {
;
using namespace UrlUtils;

const size_t kMaxRedirections = 4;

bool ConnectToHost(wxHTTP& http, const wxString& host)
{
	const size_t maxWaitingTimeMs = 3000;	// 3 secs
	const size_t waitingIntervalMs = 500;	// 0.5 sec
	size_t totalWaitingTime = 0;

	while (!http.Connect(host)) {
		wxMilliSleep(waitingIntervalMs);
		totalWaitingTime += waitingIntervalMs;
		if (totalWaitingTime > maxWaitingTimeMs)
			return false;
	}

	return true;
}

struct ConnectData
{
	ConnectData(const Request& request_)
		: request{request_}
		, http{std::make_unique<wxHTTP>()}
	{}

	ConnectData(ConnectData&& other)
		: http{std::move(other.http)}
		, stream{std::move(other.stream)}
		, request{other.request}
	{}

	std::unique_ptr<wxHTTP> http;
	std::unique_ptr<wxInputStream> stream;
	Request request;

	wxDECLARE_NO_COPY_CLASS(ConnectData);
};

ConnectData AcquireInputStream(const Request& request, int& redirectionsRemained)
{
	ConnectData result{request};
	
	const wxURL& url = request.GetUrl();
	const wxString urlStr = url.BuildURI();
	const wxString host = url.GetServer();
	//http.SetHeader(wxS("Referer"), urlStr);
	//http.SetHeader(wxS("Host"), host);

	std::unique_ptr<wxHTTP>& http = result.http;
	std::unique_ptr<wxInputStream>& stream = result.stream;

	http->SetMethod(_S("GET"));
	
	const Request::HeadersMap& headers = request.GetHeaders();
	for (auto it = headers.begin(); it != headers.end(); ++it)
		http->SetHeader(it->first, it->second);

	if (!ConnectToHost(*http.get(), host))
		return std::move(result);
	
	wxString urlPath = UrlUtils::GetPath(url);
	stream.reset(http->GetInputStream(urlPath));
	//std::unique_ptr<wxInputStream> stream{http.GetInputStream(urlPath)};
	int response = http->GetResponse();

	if (100 <= response && response < 299) {
		//if (!stream->IsOk() || !stream->CanRead())
		return std::move(result);
	}

	if (300 <= response && response < 400) {	// Redirection
		if (--redirectionsRemained <= 0)
			return std::move(result);

		wxString redirectUrlStr = http->GetHeader(wxS("Location"));
		redirectUrlStr.Trim(true).Trim(false);
		wxURL redirectUrl = MakeUrl(redirectUrlStr);
		Request newRequest = request;
		newRequest.SetUrl(redirectUrl);
		return AcquireInputStream(newRequest, redirectionsRemained);
	}

	// some other error
	return std::move(result);
}

HttpClient::HttpClient()
{}

HttpClient::~HttpClient()
{}

Reply HttpClient::Get(const Request& request)
{
	int redirectionsRemained = kMaxRedirections;
	ConnectData connectData = AcquireInputStream(request, redirectionsRemained);
	DCHECK(connectData.stream);
	Reply reply{connectData.request};
	reply.SetInternals(std::move(connectData.http), std::move(connectData.stream));
	return reply;
}

Net::Request CreateDefaultRequest(const wxString& urlStr)
{
	CHECK_WXSTRING(urlStr);

	Request request{MakeUrl(urlStr)};
	if (!request.IsHeaderPresent(KnownHeaders::UserAgent)) {
		wxAppConsole* app = wxApp::GetInstance();
		DCHECK(app);
		if (app) {
			wxString appName = app->GetAppDisplayName();
			CHECK_WXSTRING(appName);
			request.SetHeader(KnownHeaders::UserAgent, appName);
		}
		//const wxString kUserAgent = wxS("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:27.0) Gecko/20100101 Firefox/27.0"); // "Firefox/27.0"
		//request.SetHeader(KnownHeaders::UserAgent, kUserAgent);
	}

	if (!request.IsHeaderPresent(KnownHeaders::Connection))
		request.SetHeader(KnownHeaders::Connection, _S("close"));

	if (!request.IsHeaderPresent(KnownHeaders::Referer))
		request.SetHeader(KnownHeaders::Referer, urlStr);

	return request;
}

} // namespace Net
