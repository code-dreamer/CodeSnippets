#pragma once

// Usage:
//func()
//{
	//SCOPE_EXIT { 
	//	call_at_the_end_of_current_scope();	
	//};
	
	// some logic
//}

//ScopeExit e = [&]() { logic(); };
//e.DoTask();

namespace Common {
;
class ScopeExit
{
	NO_COPY_ASSIGN_CLASS(ScopeExit);

	typedef std::function<void(void)> Callback;

public:
	ScopeExit(const Callback& callback);
	~ScopeExit();

public:
	void DoTask() ;
	void EnableTask(bool enable);

private:
	std::function<void(void)> mCallback;
	bool mTaskEnabled;
};
} // namespace Common

#define EXIT_SCOPE_CREATE_UNIQ_NAME2(line) scopeExit_E1FB5B84_B1FC_4A16_8DA4_EE4542AF5109##line
#define EXIT_SCOPE_CREATE_UNIQ_NAME(line) EXIT_SCOPE_CREATE_UNIQ_NAME2(line)
#define SCOPE_EXIT Common::ScopeExit EXIT_SCOPE_CREATE_UNIQ_NAME(__LINE__) = [&]()
