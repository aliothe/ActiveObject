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
    void push(const T& t)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.emplace_back(t);
    }

    T pop()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      T v = queue_.front();
      queue_.pop_front();
      return v;
    }

    bool empty() const
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.empty();
    }

    size_t size() const
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.size();
    }
	
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

  private:
    mutable std::mutex mutex_;
    std::deque<T> queue_;
  };

}

#endif
