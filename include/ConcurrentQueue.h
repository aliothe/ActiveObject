#ifndef __MORPHEUS_CONCURRENT_QUEUE__
#define __MORPHEUS_CONCURRENT_QUEUE__

#include <vector>
#include <mutex>

namespace morpheus{ 
    
    template<typename T>
    class ConcurrentQueue{
    public:
        explicit ConcurrentQueue()
        {}
        void push(const T& t)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace_back(t);
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

	ConcurrentQueue(const ConcurrentQueue&) = delete;
        ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    private:
        std::mutex mutex_;
        std::vector<T> queue_;        
    };

}

#endif
