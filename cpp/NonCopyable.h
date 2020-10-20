#pragma once

// usage:
// class MyClass : private NonCopyable {};

namespace NonCopyable_  // protection from unintended ADL
{

class KERNEL_API NonCopyable
{
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable&);
	NonCopyable(const NonCopyable&&);
	const NonCopyable& operator=(const NonCopyable&);
	const NonCopyable& operator=(const NonCopyable&&);
};

}

typedef NonCopyable_::NonCopyable NonCopyable;