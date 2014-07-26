ActiveObject
============================

ActiveObject implementation a'la Herb Sutter

Example usage
=============

```
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
```

Or for async file io ( see testAsyncIO.cc)

```
    std::string testfilename = "testfile";
    std::string content = "Lorem ipsum dolor sit amet, wisi voluptua contentiones usu et, omittam torquatos dissentiunt no vis. Ei magna clita ornatus duo, oblique dolorum eos et. Pri id prodesset temporibus, mei eu velit graece invenire, veri affert complectitur has te. Ea mea legimus consequuntur, at atomorum senserit sed. Invidunt periculis ius te, at pro alienum quaestio, vim ut verear corrumpit percipitur. Ad pro laboramus intellegat, te cum cibo eloquentiam.\n";

    File::Write(testfilename, content, 
                [testfilename](File::ErrorMsg err)
                {
                    if(!err.empty())
                    {
                        printf("Error: %s\n", err.c_str());
                        return;
                    }
                    for(auto i = 0; i < 5; ++i)
                    {
                        File::Read(testfilename, 
                                   [](File::ErrorMsg err, File::RawData data)
                                   {
                                       if(!err.empty())
                                       {
                                           printf("Error: %s\n", err.c_str());
                                           return;
                                       }
                                       printf("read: %lu bytes\n", data.second);
                                       for(size_t i = 0; i < data.second; ++i)
                                       {
                                           printf("%c", data.first[i]);
                                       }
                                   });
                    }
                });

```

Prerequisite
============
* tested on linux 
* g++ wich groks c++11 - [g++]
* java ( used for building via gradle) - [java]

Build and test
==============
./gradlew test

License
-------

(The MIT License)

Copyright (c) 2013 Mathias Creutz &lt;mathiascreutz@yahoo.se&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[g++]: http://gcc.gnu.org/
[java]: http://www.oracle.com/technetwork/java/javase/overview/index.html

