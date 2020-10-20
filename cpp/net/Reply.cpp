#include "stdafx.h"
#include "Reply.h"

namespace Net {

const size_t kDownloadBufferSize = 8 * 1024;
//const size_t kDownloadBufferSize = 500;

Reply::Reply(const Request& request)
	: mEngine{new wxHTTP{}}
	, mRequest{request}
{}

Reply::~Reply()
{
	if (mStream)
		mStream.reset(); // delete mStream before mEngine
}

const Request& Reply::GetRequest() const
{
	return mRequest;
}

Reply::Reply(Reply&& other)
	: mEngine(std::move(other.mEngine))
	, mRequest(other.mRequest)
	, mStream(std::move(other.mStream))
	, mData(other.mData)
{}

Reply& Reply::operator=(Reply&& other)
{
	mEngine = std::move(other.mEngine);
	mRequest = other.mRequest;
	mStream = std::move(other.mStream);
	mData = other.mData;

	return *this;
}

void Reply::SetInternals(std::unique_ptr<wxHTTP> engine, std::unique_ptr<wxInputStream> stream)
{
	DCHECK(engine && stream);

	mEngine = std::move(engine);
	mStream = std::move(stream);
}

wxMemoryBuffer Reply::ReadData() const
{
	if (mData.IsEmpty()) {
		DCHECK(mStream && mStream->IsOk() && mStream->CanRead());

		size_t read = 0;
		for (;;) {
			wxByte buf[kDownloadBufferSize];
			mStream->Read(buf, kDownloadBufferSize);
			read = mStream->LastRead();
			if (read == 0)
				break;
			mData.AppendData(buf, read);
		}
	}

	return mData;
}

wxString Reply::ReadDataToString(const wxMBConv& converter) const
{
	wxMemoryBuffer data = ReadData();
	return{data, converter, data.GetDataLen()};
}

wxString Reply::GetContentFilename() const
{
	wxString filename;

	wxString headerName = GetHeaderName(KnownHeaders::ContentDisposition);
	wxString headerValue = mEngine->GetHeader(headerName);
	if (!headerValue.IsEmpty()) {
		wxString regexp = wxS(".*;.*filename=(.*)\\s*");
		wxRegEx regex{regexp};
		CHECK(regex.IsValid());
		if (regex.Matches(headerValue) && regex.GetMatchCount() > 1) {
			filename = regex.GetMatch(headerValue, 1);
			filename.Trim().Trim(true);
			filename.Replace(wxS(";"), wxEmptyString);
			filename.Replace(wxS("\""), wxEmptyString);
			/*if (filename.StartsWith(wxS("\"")))
				filename.Remove(0, 1);
			if (filename.EndsWith(wxS("\"")))
				filename.RemoveLast();
			if (filename.EndsWith(wxS(";")))
				filename.RemoveLast();*/
		}
	}

	return filename;
}

} // namespace Net
