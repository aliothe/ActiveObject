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

        explicit ActiveObject();
        ~ActiveObject();
        void Send(const Message& m);
	void Send(const Message& m, const ErrorCB& on_error);

	ActiveObject(const ActiveObject&) = delete;
	ActiveObject& operator=(const ActiveObject&) = delete;

    private:
        void Run();

    private:
        bool done_;
	class WrapperMessage;
        morpheus::ConcurrentQueue<QueueData> queue_;
        std::unique_ptr<std::thread> thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
    };

}
#endif
