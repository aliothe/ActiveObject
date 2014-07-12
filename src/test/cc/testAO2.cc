#include <cstdlib>
#include <functional>
#include <cstdio>
#include <string>
#include "ActiveObject.h"

class Test{
public:
  void Write(std::string data, std::function<void(bool)> cb)
  {
    a.Send(
	     [cb](){
	       // Do job here ( such as write data, whatever)
	       // signal to caller
	       cb(true);
	     });
  }
private:
  morpheus::ActiveObject a;
};

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
