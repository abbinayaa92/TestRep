#pragma once
#include <boost/thread.hpp>
#include <list>

template<typename T>
class ConcurrentQueue
{
private:
    std::list<T> the_queue;
    mutable boost::mutex the_mutex;
public:
    void push_back(const T& T)
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.push_back(T);
    }

    bool empty() const
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.empty();
    }

	int size() const {
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.size();
	}

    T& front()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.front();
    }
    
    T const& front() const
    {
        boost::mutex::scoped_lock lock(the_mutex);
        return the_queue.front();
    }

	T& back()
	{
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.back();
	}

    void pop_front()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.pop_front();
    }

	void pop_back()
    {
        boost::mutex::scoped_lock lock(the_mutex);
        the_queue.pop_back();
    }

	void resize(int newSize)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		the_queue.resize(newSize);
	}

	typename std::list<T>::iterator begin() {
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.begin();
	}
	typename std::list<T>::iterator end() {
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.end();
	}
};
