#pragma once

#include "Net/Request.h"

namespace Net {

class Reply
{
	friend class HttpClient;

	//BOOST_MOVABLE_BUT_NOT_COPYABLE(Reply);

public:
	~Reply();

	Reply(Reply&& other);
	Reply& operator=(Reply&& other);

	const Request& GetRequest() const;

	// returns -1 if size is unknown
	//int GetDataSize() const;
	wxMemoryBuffer ReadData() const;
	//wxString ReadDataToString(const wxMBConv& converter = wxGet_wxConvUTF8()) const;
	wxString ReadDataToString(const wxMBConv& converter = wxConvAuto()) const;

	wxString GetContentFilename() const;

private:
	Reply(Reply& other);
	Reply(const Request& request);
	//wxHTTP& GetEngine();
	void SetInternals(std::unique_ptr<wxHTTP> engine, std::unique_ptr<wxInputStream> stream);
	//void SetStream(std::unique_ptr<wxInputStream> stream);

private:
	Request mRequest;
	std::unique_ptr<wxHTTP> mEngine;
	std::unique_ptr<wxInputStream> mStream;

	mutable wxMemoryBuffer mData;
};

} // namespace Net
