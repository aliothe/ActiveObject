#include <chrono>
#include "ActiveObject.h"

namespace morpheus{ 

void with_error_handler(const ActiveObject::QueueData& m);

ActiveObject::ActiveObject()
  : done_(false),
    thread_(std::thread{[this]{ Run();}})
{}

ActiveObject::~ActiveObject()
{
  Send([this](){ done_ = true;});
  thread_.join();
}
  
void ActiveObject::Send(const Message& m)
{
    queue_.push(std::make_pair(m, ActiveObject::ErrorCB()));
    cv_.notify_one();
}

void ActiveObject::Send(const Message& m, const ErrorCB& on_error)
{
    queue_.push(std::make_pair(m, on_error));
    cv_.notify_one();
}
  
void ActiveObject::Run()
{
    while(!done_)
    {
        std::unique_lock<MutexType> lock(mutex_);
	// queue size check due to possible spurious wakeups ( not from being notified)
        cv_.wait(lock, [this](){return !queue_.empty();});	
	ActiveObject::QueueData m = queue_.pop();
	if(m.second)
	{
            with_error_handler(m);
	}
	else
	{
            // any exceptions better be handled in m
            // maybe allow the installation of a generic error handler in the AO
            // which is then always used in this case
            m.first();
	}
    }
}         
          
void with_error_handler(const ActiveObject::QueueData& m)
{  
    try
    {
        m.first();
    }
    catch(...)
    {
        m.second(std::current_exception());
    }  
}


}// end ns                           
