#ifndef __MORPHEUS_TASK__
#define __MORPHEUS_TASK__
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "ConcurrentQueue"
namespace morpheus{ 

    class ActiveObject{
    public:
        using Message = std::function(void());
        ActiveObject()
            : done(false), 
              thread_(unique_ptr(new thread([=]{ this->Run(); }))), 
              notified(false);
        {}

        ~ActiveObject()
        {
            send([](){ done = true;});
        }

        void send(Message m)
        {
            queue_.push(m);
            std::unique_lock<std::mutex> lock(mutex_);
            toggleNotified();
            cv.notify_one();
        }

    private:
        void Run()
        {
            while(!done)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                while(!notified_)
                {
                    cv.wait(lock);
                }
                toggleNotified();
                Message m = queue_.pop();
                m();
            }
        }                                              

        private void toggleNotified()
        {
            notified_ = !notified_;
        }

    private:
        bool done_;
        ConcurrentQueue<Message> queue_;
        uniqe_ptr<std::thread> thread_;
        std::mutex mutex_;
        // due to possible spurious wakeups not due to being notified
        boolean notified_;        
    };

}
#endif
