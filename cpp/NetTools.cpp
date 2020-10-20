#include "stdafx.h"
#include "NetTools.h"

//#include <cpprest/http_client.h>
//#include <cpprest/filestream.h>

namespace NetTools {
;
const wxString kUserAgent = wxS("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:27.0) Gecko/20100101 Firefox/27.0"); // "Firefox/27.0"

static size_t DownloadBlock(wxInputStream* in, void* writeBuff, size_t blockSize)
{
	CHECK_PTR(in);
	wxASSERT(in->IsOk());
	CHECK_PTR(writeBuff);
	//wxASSERT(out->IsOk());

	in->Read(writeBuff, blockSize);
	/*
	wxByte* buf = new wxByte[blockSize];
	in->Read(buf, blockSize);
	out->Write(buf, in->LastRead());
	delete[] buf;*/

	//wxASSERT(in->LastRead() == out->LastWrite());
	//return out->LastWrite();
	return in->LastRead();
}

wxCharTypeBuffer<char> Download(wxString urlStr, wxInt32 maxCount, const HeadersMap& headers)
{
	wxCharTypeBuffer<char> result;

	wxURL url(urlStr);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("Can't parse url '%s'"), urlStr);
		return result;
	}

	urlStr = url.BuildURI();

	wxString host = url.GetServer();

	wxHTTP http;
	http.SetMethod("GET");
	//http.SetHeader("GET", "/ HTTP/1.1");
	//http.SetHeader("GET", url.GetPath() + " " + "HTTP/1.1");
	/*http.SetHeader("Host", host);
	http.SetHeader("Connection", "close");
	http.SetHeader("Referer", urlStr);
	http.SetHeader("User-Agent", kUserAgent); // "Firefox/27.0"
	//http.SetHeader("Content-Length", "81920");*/

	for (auto it = headers.begin(); it != headers.end(); ++it)
		http.SetHeader(it->first, it->second);

	if (headers.find(wxS("Host")) == headers.end())
		http.SetHeader("Host", host);
	if (headers.find(wxS("Connection")) == headers.end())
		http.SetHeader("Connection", "close");
	if (headers.find(wxS("Referer")) == headers.end())
		http.SetHeader("Referer", urlStr);
	if (headers.find(wxS("User-Agent")) == headers.end())
		http.SetHeader("User-Agent", kUserAgent);

	//wxLogMessage("Connecting to '%s'", host);
	wxLogMessage("Connecting to '%s'", urlStr);
	http.SetTimeout(200);
	wxInt32 waitMs = 0;
	wxStopWatch sw;
	while (!http.Connect(host)) {
		waitMs += sw.Time();
		if (waitMs >= 6000) {
			wxLogMessage("Can't connect to '%s'", host);
			//wxFAIL;
			return result;
		}

		wxMilliSleep(200);
		waitMs += 200;
	}


	size_t ind;
	if ((ind = urlStr.find(host)) == wxString::npos) {
		wxFAIL;
		return result;
	}

	ind += host.Len();
	wxString path = urlStr.Mid(ind, urlStr.Len() - ind);

	wxScopedPtr<wxInputStream> in(http.GetInputStream(path));
	if (!in || !in->IsOk() || !in->CanRead()) {
		wxLogMessage("Can't get input stream for '%s'", urlStr);
		wxFAIL;
		return result;
	}

	const int responce = http.GetResponse();
	if (300 <= responce && responce < 400) {	// Redirection
		wxString redirectUrlStr = http.GetHeader(wxS("Location"));
		redirectUrlStr.Trim(true).Trim(false);
		return Download(redirectUrlStr, maxCount); // TODO: infinity recursion possible
	}

	//size_t downloaded = 0;
	size_t needToRead = maxCount > 0 ? maxCount : in->GetSize();
	if (UINT_MAX == needToRead) {
		wxMemoryBuffer buffer;
		const size_t blockSize = 8 * 1024;
		while (!in->Eof()) {
			wxMemoryBuffer currBuffer;
			size_t downloaded = DownloadBlock(in.get(), currBuffer.GetWriteBuf(blockSize), blockSize);
			currBuffer.UngetWriteBuf(downloaded);
			buffer.AppendData(currBuffer.GetData(), currBuffer.GetDataLen());
		}
		result = wxCharTypeBuffer<char>::CreateNonOwned((char*)buffer.GetData(), buffer.GetDataLen());
		
		if (http.GetHeader(wxS("Transfer-Encoding")).CmpNoCase(wxS("chunked")) == 0) {
			CombineChunkedResponse(result);
			/*
			//wxString str((char*)buffer.GetData(), wxConvAuto());
			//const wxString delimiter = wxS("\r\n");
			//wxArrayString tokens = wxStringTokenize(str, delimiter);
			//wxString regexpStr = R"(\r\n)";
			//wxRegEx r{;
			//r.Matches()

			//auto GetChunk = [](const wxCharTypeBuffer<char>& buffer, size_t& startInd) -> wxCharTypeBuffer<char> {
			//}

			buffer.Clear();

			const wxString delimiter = wxS("\r\n");
			const size_t delimiterLen = delimiter.Len();
			size_t len = result.length();
			size_t i = 0;
			size_t startInd = 0;
			while (i < len - 1) {
				const char* s = result.data() + i;
				startInd = i;
				while (i <= len - 2) {
					if (result[i] == delimiter[0] && result[i + 1] == delimiter[1])
						break;

					++i;
				}

				wxString chunkSizeStr{(const char*)result.data() + startInd, i - startInd};
				long chunkSize;
				bool ok = chunkSizeStr.ToLong(&chunkSize, 16);
				if (!ok) {
					FAIL;
					continue;
				}
				if (chunkSize <= 0)
					break;

				i += delimiterLen;
				s = result.data() + i;
				buffer.AppendData(result.data() + i, chunkSize);
				i += chunkSize + delimiterLen;
			}

			result = wxCharTypeBuffer<char>::CreateNonOwned((char*)buffer.GetData(), buffer.GetDataLen());
			*/
		}
	}
	else {
		result.extend(needToRead);
		in->Read(result.data(), needToRead);
		size_t readed = in->LastRead();
		result.shrink(readed);
	}
	return result;

	//wxString ss;
	//wxStringOutputStream out{&ss};
	//out.GetSize();
//	in->Read(out);

	/*
	result.extend(needToRead);
	in->Read(result.data(), needToRead);
	size_t readed = in->LastRead();
	result.shrink(readed);
	*/
	
#if 0
	if (maxCount > 0) {
		result.extend((size_t)maxCount);
		in->Read(result.data(), (size_t)maxCount);
		size_t readed = in->LastRead();
		result.shrink(readed);
	}
	else {
		size_t size = in->GetSize();
		result.extend((size_t)maxCount);
		in->Read(result.data(), (size_t)maxCount);
		
		/*void* buffer = nullptr;
		size_t buffLen = 0;
		if (size != UINT_MAX) {
			result.extend(size);
			buffer = result.data();
			buffLen = size;
		}
		wxMemoryOutputStream out(buffer, buffLen);
		in->Read(out);
		downloaded = out.LastWrite();
		//result = wxCharTypeBuffer<char>::CreateOwned((char*)buffer, buffLen);
		*/
	}
#endif

//	return result;
}

wxCharTypeBuffer<char> CombineChunkedResponse(const wxCharTypeBuffer<char>& buffer)
{
	wxMemoryBuffer data;

	const wxString delimiter = wxS("\r\n");
	const size_t delimiterLen = delimiter.Len();
	size_t len = buffer.length();
	size_t i = 0;
	size_t startInd = 0;
	while (i < len - 1) {
		const char* s = buffer.data() + i;
		startInd = i;
		while (i <= len - 2) {
			if (buffer[i] == delimiter[0] && buffer[i + 1] == delimiter[1])
				break;

			++i;
		}

		wxString chunkSizeStr{(const char*)buffer.data() + startInd, i - startInd};
		long chunkSize;
		bool ok = chunkSizeStr.ToLong(&chunkSize, 16);
		if (!ok) {
			FAIL;
			continue;
		}
		if (chunkSize <= 0)
			break;

		i += delimiterLen;
		s = buffer.data() + i;
		data.AppendData(buffer.data() + i, chunkSize);
		i += chunkSize + delimiterLen;
	}

	wxCharTypeBuffer<char> result = wxCharTypeBuffer<char>::CreateNonOwned((char*)data.GetData(), data.GetDataLen());
	return result;
}

void Request(wxString urlStr, const HeadersMap& headers)
{
	wxURL url(urlStr);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("Can't parse url '%s'"), urlStr);
		return;
	}

	urlStr = url.BuildURI();

	wxString host = url.GetServer();

	wxHTTP http;
	http.SetMethod("GET");
	http.SetHeader("Host", host);
	http.SetHeader("Connection", "close");
	http.SetHeader("Referer", urlStr);
	http.SetHeader("User-Agent", kUserAgent);

	for (auto it = headers.begin(); it != headers.end(); ++it)
		http.SetHeader(it->first, it->second);

	wxLogMessage("Connecting to '%s'", urlStr);
	http.SetTimeout(200);
	wxInt32 waitMs = 0;
	wxStopWatch sw;
	while (!http.Connect(host)) {
		waitMs += sw.Time();
		if (waitMs >= 6000) {
			wxLogMessage("Can't connect to '%s'", host);
			//wxFAIL;
			return;
		}

		wxMilliSleep(200);
		waitMs += 200;
	}

	size_t ind;
	if ((ind = urlStr.find(host)) == wxString::npos) {
		wxFAIL;
		return;
	}

	ind += host.Len();
	wxString path = urlStr.Mid(ind, urlStr.Len() - ind);

	wxScopedPtr<wxInputStream> in(http.GetInputStream(path));
	if (!in || !in->IsOk() || !in->CanRead()) {
		wxLogMessage("Can't get input stream for '%s'", urlStr);
		wxFAIL;
		return;
	}
}

class AsyncRunner : public wxThread 
{ 
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

void ARequest(const wxString& urlStr, const HeadersMap& headers)
{
	auto func = [=]() {
		Request(urlStr, headers);
	};
	AsyncRunner* runner = new AsyncRunner;
	runner->Start(func);
	/*
	std::async(std::launch::async, [=]() {
		Request(urlStr, headers);
	});*/
//	std::async(std::launch::async, Request, urlStr, headers);
	//Request(urlStr, headers);
}

wxString GetRedirectUrlImpl(wxString urlStr)
{
	wxURL url(urlStr);
	if (url.GetError() != wxURL_NOERR) {
		wxLogError(_("Can't parse url '%s'"), urlStr);
		return urlStr;
	}

	urlStr = url.BuildURI();

	wxString host = url.GetServer();

	wxHTTP http;
	//http.SetHeader("GET", "/ HTTP/1.1");
	http.SetMethod("GET");
	http.SetHeader("Host", host);
	http.SetHeader("Referer", urlStr);
	http.SetHeader("Connection", "close");
	http.SetHeader("User-Agent", kUserAgent);

	wxInt32 waitMs = 0;
	while (!http.Connect(host)) {
		wxMilliSleep(200);
		waitMs += 200;
		if (waitMs >= 10000) {
			wxLogMessage("Can't connect to '%s'", host);
			wxFAIL;
			return urlStr;
		}
	}

	size_t ind;
	if ((ind = urlStr.find(host)) == wxString::npos) {
		wxFAIL;
		return wxEmptyString;
	}

	ind += host.Len();
	wxString path = urlStr.Mid(ind, urlStr.Len() - ind);

	wxScopedPtr<wxInputStream> in(http.GetInputStream(path));
	if (!in || !in->IsOk() || !in->CanRead()) {
		wxLogMessage("Can't connect to '%s'", urlStr);
		wxFAIL;
		return urlStr;
	}

	const int responce = http.GetResponse();
	if (300 <= responce && responce < 400) {	// Redirection
		wxString redirectUrlStr = http.GetHeader(wxS("Location"));
		redirectUrlStr.Trim(true).Trim(false);
		return GetRedirectUrlImpl(redirectUrlStr);
	}

	return urlStr;
}

wxString GetRedirectUrl(wxString urlStr)
{
	CHECK_WXSTRING(urlStr);

	wxString redirectUrl = GetRedirectUrlImpl(urlStr);
	if (redirectUrl == urlStr)
		redirectUrl.Clear();

	return redirectUrl;
}

wxString DownloadToString(const wxString& urlStr, wxInt32 maxCount, const HeadersMap& headers, const wxMBConv& converter)
{
	wxCharTypeBuffer<char> data = Download(urlStr, maxCount, headers);
	return{data, converter, data.length()};
}

std::string DownloadToStdString(const wxString& urlStr, wxInt32 maxCount, const HeadersMap& headers)
{
	wxCharTypeBuffer<char> data = Download(urlStr, maxCount, headers);
	
	return{data.data(), data.length()};
}

bool DownloadToFile(const wxString& url, const wxString& outFilePath)
{
	CHECK_WXSTRING(url);
	CHECK_WXSTRING(outFilePath);

	wxCharTypeBuffer<char> fileData = NetTools::Download(url);
	if (fileData.length() <= 0)
		return false;

	wxFile file;
	if (!wxFileExists(outFilePath)) {
		if (!file.Create(outFilePath, true))
			return false;
	}
	if (!file.Open(outFilePath, wxFile::OpenMode::write))
		return false;
	size_t written = file.Write(fileData.data(), fileData.length());
	return (written == fileData.length());
}

} // namespace NetTools
