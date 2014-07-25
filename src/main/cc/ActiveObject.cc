#include "ActiveObject.h"

namespace morpheus{ 
ActiveObject::ActiveObject()
    : done_(false), 
      thread_(std::unique_ptr<std::thread>(new std::thread([=]{ Run(); })))
{}

ActiveObject::~ActiveObject()
{
    Send([=](){ done_ = true;});
    thread_->join();
}

void ActiveObject::Send(const Message& m)
{
    queue_.push(m);
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.notify_one();
}

void ActiveObject::Run()
{
    while(!done_)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // due to possible spurious wakeups not due to being notified
        while(queue_.empty())
        {
            cv_.wait(lock);
        }
        Message m = queue_.pop();
        m();
    }
}                   
}                           
