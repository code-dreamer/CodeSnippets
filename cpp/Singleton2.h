#pragma once

template <class T>
class SingletonBase
{
protected:
	SingletonBase() {}

public:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	
    static T& Instance()
	{
		static T obj;
		return obj;
	}
};

/*
class Single : public SingletonBase<Single>
{
public:
	void demo() { std::cout << "demo" << '\n'; }
};*/
