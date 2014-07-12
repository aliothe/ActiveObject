#ifndef __MORPHEUS_CONCURRENT_QUEUE__
#define __MORPHEUS_CONCURRENT_QUEUE__

#include <deque>
#include <mutex>

namespace morpheus{ 
    
    template<typename T>
    class ConcurrentQueue{
    public:
        explicit ConcurrentQueue()
        {}
        void push(T t)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push_front(t);
        }

        T pop()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            T v = queue_.back();
            queue_.pop_back();
            return v;
        }

        bool empty()
        {
            return queue_.empty();
        }

	ConcurrentQueue(ConcurrentQueue&) = delete;
        void operator=(ConcurrentQueue&) = delete;

    private:        
        std::mutex mutex_;
        std::deque<T> queue_;        
    };

}

#endif
