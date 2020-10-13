#include "stdafx.h"
#include "ScopeExit.h"

namespace Common {
;
ScopeExit::ScopeExit(const Callback& callback)
	: mCallback(callback)
	, mTaskEnabled(true)
{}

ScopeExit::~ScopeExit()
{
	if (mTaskEnabled) {
		DoTask();
	}
}

void ScopeExit::DoTask() 
{
	if (mCallback != nullptr) {
		mCallback(); 
		mTaskEnabled = false;
	}
}

void ScopeExit::EnableTask(bool enable)
{
	mTaskEnabled = enable;
}

} // namespace Common
