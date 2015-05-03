#include <cstdlib>
#include <functional>
#include <cstdio>
#include <string>
#include <exception>
#include <iostream>
#include "ActiveObject.h"

void dump_exception(std::exception_ptr e);

class Test{
public:
    Test()
    {}
    void Write(const std::string& data, std::function<void(bool)> cb)
    {
        a.Send(
            [cb](){
                std::string().at(1); // this generates an std::out_of_range
                cb(true);
            },
            [cb](std::exception_ptr e)
            {
                dump_exception(e);
                cb(false);
            });
    }
private:
    morpheus::ActiveObject a;
};

void dump_exception(std::exception_ptr e)
{
    try
    {
        std::rethrow_exception(e);
    }
    catch(const std::exception& e)
    {
        printf("std::exception => %s\n", e.what());
    }
    catch(...)
    {
        printf("caught something\n");
    }
}

int main()
{
    Test t;
    t.Write("dandy", 
            [](bool success)
            {
                printf("Success: %i\n", success);
            });
    return EXIT_SUCCESS;
}


