#pragma once

namespace SyncUtils {
;
class ScopedThread
{	
public:
	explicit ScopedThread(std::thread t);
	~ScopedThread();

	ScopedThread(const ScopedThread&) = delete;
	ScopedThread& operator=(const ScopedThread&) = delete;

private:
	std::thread _t;
};

class HierarchicalMutex
{
public:
	explicit HierarchicalMutex(unsigned long value);

	void Lock();
	void Unlock();
	bool TryLock();

private:
	void CheckForHierarchyViolation();
	void UpdateHierarchyValue();

	std::mutex _internalMutex;
	unsigned long const _hierarchyValue;
	unsigned _previousHierarchyValue;
	static thread_local unsigned long ThisThreadHierarchyValue;
};

};
