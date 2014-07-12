#include <cstdlib>
#include "ActiveObject.h"

int main()
{
    morpheus::ActiveObject a;
    a.Send([](){ printf("Hello world\n");});
    return EXIT_SUCCESS;
}
