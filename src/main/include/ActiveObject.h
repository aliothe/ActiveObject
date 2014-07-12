#ifndef __MORPHEUS_AO__
#define __MORPHEUS_AO__
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "ConcurrentQueue.h"
namespace morpheus{ 
    class ActiveObject{
    public:
        using Message = std::function<void()>;

        ActiveObject();
        ~ActiveObject();
        void Send(Message m);

    private:
        void Run();

    private:
        bool done_;
        morpheus::ConcurrentQueue<Message> queue_;
        std::unique_ptr<std::thread> thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
    };

}
#endif
