#include "stdafx.h"
#include "SyncUtils.h"

namespace SyncUtils {
;

// ScopedThread // 

ScopedThread::ScopedThread(std::thread t)
	: _t(std::move(t))
{
	if (!_t.joinable())
		throw std::logic_error("Expect joinable thread");
}

ScopedThread::~ScopedThread()
{
	_t.join();
}

////////////////////////////////////////////////////////////////////////////////


// HierarchicalMutex // 
thread_local unsigned long HierarchicalMutex::ThisThreadHierarchyValue(ULONG_MAX);

HierarchicalMutex::HierarchicalMutex(unsigned long value)
	: _hierarchyValue(value)
	, _previousHierarchyValue(0)
{}

void HierarchicalMutex::CheckForHierarchyViolation()
{
	if (ThisThreadHierarchyValue <= _hierarchyValue)
	{
		throw std::logic_error("mutex hierarchy violated");
	}
}

void HierarchicalMutex::UpdateHierarchyValue()
{
	_previousHierarchyValue = ThisThreadHierarchyValue;
	ThisThreadHierarchyValue = _hierarchyValue;
}

void HierarchicalMutex::Lock()
{
	CheckForHierarchyViolation();
	_internalMutex.lock();
	UpdateHierarchyValue();
}

void open_connection()
{}

void HierarchicalMutex::Unlock()
{
	ThisThreadHierarchyValue = _previousHierarchyValue;
	_internalMutex.unlock();

	std::once_flag connection_init_flag;
	std::call_once(connection_init_flag, open_connection);
}

bool HierarchicalMutex::TryLock()
{
	CheckForHierarchyViolation();
	if (!_internalMutex.try_lock())
		return false;

	UpdateHierarchyValue();
	return true;
}
////////////////////////////////////////////////////////////////////////////////

};

