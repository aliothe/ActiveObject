#ifndef __MORPHEUS_AO__
#define __MORPHEUS_AO__
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <exception>
#include <utility>
#include "ConcurrentQueue.h"
namespace morpheus{ 
    class ActiveObject{
    public:
        using Message = std::function<void()>;
	using ErrorCB = std::function<void(std::exception_ptr)>;
	using QueueData = std::pair<Message, ErrorCB>;
        using MutexType = std::mutex;

        explicit ActiveObject();
        ~ActiveObject();
        void Send(const Message& m);
	void Send(const Message& m, const ErrorCB& on_error);

	ActiveObject(const ActiveObject&) = delete;
	ActiveObject& operator=(const ActiveObject&) = delete;

    private:
        void Run();
	void waitUntilRunning();
	
    private:
        bool done_;
	bool running_;
        morpheus::ConcurrentQueue<QueueData> queue_;
        std::thread thread_;
        // used for condition variable
        MutexType mutex_;	
        std::condition_variable cv_;
    };

}
#endif
