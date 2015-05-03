#include <cstdlib>
#include <iostream>
#include "ActiveObject.h"

int main()
{
    morpheus::ActiveObject a;
    a.Send([](){ std::cout << "Hello world\n"; });
    return EXIT_SUCCESS;
}
