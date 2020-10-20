#pragma once

template<typename T>
class ConcurrentQueue
{
public:
	void Push(const T& value)
	{
		boost::mutex::scoped_lock lock(mutex_);
		queue_.push(value);
		lock.unlock();
		condition_.notify_one();
	}

	bool IsEmpty() const
	{
		boost::mutex::scoped_lock lock(mutex_);
		return queue_.empty();
	}

	bool TryToPop(T& poppedValue)
	{
		boost::mutex::scoped_lock lock(mutex_);
		if( queue_.empty() ) {
			return false;
		}

		poppedValue = queue_.front();
		queue_.pop();
		
		return true;
	}

	void WaitData()
	{
		boost::mutex::scoped_lock lock(mutex_);
		while( queue_.empty() ) {
			condition_.wait(lock);
		}
	}

	void WaitDataAndPop(T& poppedValue)
	{
		boost::mutex::scoped_lock lock(mutex_);
		while( queue_.empty() ) {
			condition_.wait(lock);
		}

		poppedValue = queue_.front();
		queue_.pop();
	}

private:
	std::queue<T> queue_;
	mutable boost::mutex mutex_;
	boost::condition_variable condition_;
};