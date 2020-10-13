#pragma once

// Usage:
// class ConcreateSinglton : public Singleton<ConcreateSinglton>

template<typename T, typename ThreadModel = CComMultiThreadModel>
class Singleton
{
	NO_COPY_ASSIGN_CLASS(Singleton);

public:
	virtual ~Singleton()
	{}

	static T* InitPtr()
	{
		CComCritSecLock<ThreadModel::AutoCriticalSection> lock(mCs);

		if (!mInstance)
			mInstance.reset(new T());

		return mInstance.get();
	}

	static void ShutdownPtr()
	{
		CComCritSecLock<ThreadModel::AutoCriticalSection> lock(mCs);

		if (mInstance)
			mInstance.reset(nullptr);
	}

	static T* Get()	
	{
		CComCritSecLock<ThreadModel::AutoCriticalSection> lock(mCs);
		return mInstance.get();
	}
	
protected:
	Singleton() {}
	
private:
	static typename ThreadModel::AutoCriticalSection mCs;
	static std::unique_ptr<T> mInstance;
};

template<typename T, typename ThreadModel>
typename ThreadModel::AutoCriticalSection Singleton<T, ThreadModel>::mCs;

template<typename T, typename ThreadModel>
std::unique_ptr<T> Singleton<T, ThreadModel>::mInstance;
