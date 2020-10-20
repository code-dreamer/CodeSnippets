#pragma once

namespace CppTools
{
	template<typename T>
	void SafeDelete(T*& ptr) 
	{
		typedef char TypeIsCompleteCheck[sizeof(T)];
		if (ptr != nullptr) {
			delete ptr;
			ptr = nullptr;
		}
	}

	template<typename T>
	void SafeDeleteA(T*& ptr) 
	{
		typedef char TypeIsCompleteCheck[sizeof(T)];
		if (ptr != nullptr) {
			delete[] ptr;
			ptr = nullptr;
		}
	}

	template<class T, std::size_t N> 
	char(&LenghtOf(T(&)[N]))[N];

	template<typename T>
	QString GetClassName(const T* const obj)
	{
		return QLatin1String(typeid(obj).name());
	}

	template<typename T>
	QString GetClassName(const T& obj)
	{
		return QLatin1String(typeid(obj).name());
	}

	template<typename T>
	std::shared_ptr<T> SharedPtrFromWeak(std::weak_ptr<T> ptr)
	{
		G_ASSERT( !ptr.expired() );
		return ptr.lock();
	}

	// Measure function call time
	template <class Function>
	__int64 TimedCall(Function&& calledFunc)
	{
		QElapsedTimer elapsedTimer;
		elapsedTimer.start();
		
		calledFunc();
		
		return elapsedTimer.elapsed();
	}
}

#define STATIC_ARRAY_SIZE(arr) sizeof(CppTools::LenghtOf(arr))

#define SAFE_DELETE(ptr)		\
	if (ptr != nullptr) {		\
		delete ptr;				\
		ptr = nullptr;			\
	}

#define SAFE_DELETEA(ptr)		\
	if (ptr != nullptr) {		\
		delete[] ptr;			\
		ptr = nullptr;			\
	}