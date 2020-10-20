#include "stdafx.h"
#include "StackWalker.h"

namespace {

wxString FrameToString(const wxStackFrame& frame)
{
	wxString funcStr;
	funcStr = frame.GetName();
	/*size_t paramCount = frame.GetParamCount();
	for (size_t i = 0; i < paramCount; ++i) {
		if (i == 0)
			funcStr += wxS("(");

		wxString type, name, value;
		frame.GetParam(i, &type, &name, &value);

		if (!type.IsEmpty())
			funcStr += type + wxS(" ");
		if (!name.IsEmpty())
			funcStr += name + wxS(" ");
		if (!value.IsEmpty())
			funcStr += wxS("= ") + value + wxS(" ");

		if (i == paramCount - 1)
			funcStr += wxS(")");
		else
			funcStr += wxS(", ");
	}*/

	wxString res = wxString::Format(wxS("%d:%s %s"), frame.GetLevel()+1, frame.GetModule(), funcStr);
	if (frame.HasSourceLocation())
		res += wxString::Format(wxS(" %s: %d"), frame.GetFileName(), frame.GetLine());
	
	return res;
}

} // namespace

StackWalker::StackWalker(const char* appPath)
	: wxStackWalker(appPath)
{}

void StackWalker::OnStackFrame(const wxStackFrame& frame)
{
	wxString record = FrameToString(frame);
	m_stack.push_back(record);
}

const std::vector<wxString>& StackWalker::GetStack() const
{
	return m_stack;
}

void StackWalker::Walk(size_t skip, size_t maxDepth)
{
	m_stack.clear();
	wxStackWalker::Walk(skip, maxDepth);
}

wxString StackWalker::GetStackString() const
{
	wxString result;

	for (const wxString& record : m_stack)
		result += record + wxS("\r\n");

	return result;
}