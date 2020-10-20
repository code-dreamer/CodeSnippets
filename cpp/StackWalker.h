#pragma once

class StackWalker : public wxStackWalker
{
	wxDECLARE_NO_COPY_CLASS(StackWalker);

public:
	explicit StackWalker(const char* appPath);
	virtual ~StackWalker() {};

	virtual void Walk(size_t skip = 1, size_t maxDepth = wxSTACKWALKER_MAX_DEPTH) override;
	const std::vector<wxString>& GetStack() const;
	wxString GetStackString() const;

private:
	virtual void OnStackFrame(const wxStackFrame& frame) override;

private:
	std::vector<wxString> m_stack;
};

