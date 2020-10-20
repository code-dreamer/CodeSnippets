#pragma once

#pragma warning(push)
#pragma warning(disable : 4640)	

template<typename T>
class Singleton
{
public:
	static T* GetInstance()	
	{
		G_ASSERT(!isDestructed_);
		
		(void)isDestructed_; // prevent removing is_destructed in Release configuration

		boost::mutex::scoped_lock lock( GetMutex() );
		static T instance;
		return &instance;
	}

private:
	static bool isDestructed_;

	static boost::mutex& GetMutex()	
	{
		static boost::mutex mutex;
		return mutex;
	}

	Singleton() {}
	~Singleton() 
	{ 
		isDestructed_ = true; 
	}
};

#pragma warning(pop)

// force creating mutex before main() is called
template<typename T>
bool Singleton<T>::isDestructed_ = (Singleton<T>::GetMutex(), false);
