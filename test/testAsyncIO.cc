#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <exception>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <sstream>
#include <fstream>
#include <condition_variable>
#include <mutex>
#include "ActiveObject.h"

std::string get_exception_msg(std::exception_ptr e);
std::string getcwd();

/**
 *
 * Simple round robin thread pool using Active Objects
 *
 */
class ThreadPool{
public:        
    static ThreadPool& instance()
    {
        std::call_once(once_flag_,
                       [] 
                       {
                           instance_.reset(new ThreadPool);
                       });
        return *(instance_.get());
    }

    static void join()
    {
        delete instance_.release();
    }

    morpheus::ActiveObject& nextAO()
    {
        return pool_[nextAOIndex()];
    }

private:
    ThreadPool(int defaultPoolsize = 4)
        : poolIndex_(-1),
          poolsize_(defaultPoolsize),
          pool_(poolsize_)
    {}

    int nextAOIndex()
    {
        poolIndex_++;
        poolIndex_ = (poolsize_ == poolIndex_) ? 0 : poolIndex_;
        return poolIndex_;
    }
private:
    int poolIndex_;
    const int poolsize_;
    std::vector<morpheus::ActiveObject> pool_;
    static std::once_flag once_flag_;
    static std::unique_ptr<ThreadPool> instance_;
};

std::unique_ptr<ThreadPool> ThreadPool::instance_;
std::once_flag ThreadPool::once_flag_;

class File{
public:
    using Byte = char;
    using RawDataType = std::unique_ptr<Byte[]>;
    using RawDataSize = size_t;
    using RawData = std::pair<RawDataType, RawDataSize>;
    using ErrorMsg = std::string;

    static void Read(std::string filename, std::function<void(ErrorMsg, RawData)> cb)
    {
        ThreadPool::instance().nextAO().Send(
            [filename,cb]()
            {
                std::ifstream file(filename, std::ios::binary);
                if(!file)
                {
                    std::stringstream what;
                    what << "couldn't open file " << getcwd() << "/" << filename << "\n";
                    cb(what.str(), std::make_pair<File::RawDataType, File::RawDataSize>(nullptr,0));
                    return;
                }
                std::streampos begin = file.tellg();
                file.seekg(0,std::ios::end);
                std::streampos end = file.tellg();
                const long bytes = end - begin;
                file.seekg(std::ios::beg);
                RawDataType data = RawDataType(new Byte[bytes]);
                file.read(&data[0], bytes);
                if(!file)
                {
                    std::stringstream what;
                    what << "could only read  " << file.gcount() << " bytes from file, " << filename << "\n";
                    cb(what.str(), std::make_pair<File::RawDataType, File::RawDataSize>(nullptr,0));
                    return;
                }                
                cb("", std::make_pair<File::RawDataType, File::RawDataSize>(std::move(data),bytes));
            },
            [cb](std::exception_ptr e)
            {
                cb(get_exception_msg(e), std::make_pair<File::RawDataType, File::RawDataSize>(nullptr,0));
            });
    }

    static void Write(std::string filename, std::string data, std::function<void(ErrorMsg)> cb)
    {
        ThreadPool::instance().nextAO().Send(
            [filename,data,cb]()
            {
                std::ofstream file(filename, std::ios::binary);
                if(!file)
                {
                    std::stringstream what;
                    what << "couldn't open file " << getcwd() << "/" << filename << " for writing\n";
                    cb(what.str());
                    return;
                }
                file.write(data.data(), data.length());
                if(!file)
                {
                    std::stringstream what;
                    what << "failed to write to file " << getcwd() << "/" << filename << "\n";
                    cb(what.str());
                    return;
                }          
                file.close();
                cb("");
            },
            [cb](std::exception_ptr e)
            {
                cb("");
            });
    }            
};

std::string get_exception_msg(std::exception_ptr e)
{
    std::string msg;
    try
    {
        std::rethrow_exception(e);
    }
    catch(const std::exception& e)
    {
        msg = e.what();
    }
    catch(...)
    {
        msg = "caught something\n";
    }
    return msg;
}

std::string getcwd() 
{
    std::string result(1024,'\0');
    while( getcwd(&result[0], result.size()) == NULL) 
    {
        if( errno == ERANGE ) 
        {
            result.resize(result.size()*2);
        }
        else
        {
            throw std::runtime_error(strerror(errno));
        }
    }   
    result.resize(result.find('\0'));
    return result;
}


int main()
{
    std::string testfilename = "testfile";
    std::string content = "Lorem ipsum dolor sit amet, wisi voluptua contentiones usu et, omittam torquatos dissentiunt no vis. Ei magna clita ornatus duo, oblique dolorum eos et. Pri id prodesset temporibus, mei eu velit graece invenire, veri affert complectitur has te. Ea mea legimus consequuntur, at atomorum senserit sed. Invidunt periculis ius te, at pro alienum quaestio, vim ut verear corrumpit percipitur. Ad pro laboramus intellegat, te cum cibo eloquentiam.\n";

    std::mutex mutex;
    std::condition_variable cv;
    bool done = false;

    auto notify = [&mutex, &cv, &done]()
    {
        std::unique_lock<std::mutex> lock(mutex);
        done = true;
        cv.notify_one();
    };

    File::Write(testfilename, content, 
                [testfilename, notify](const File::ErrorMsg& err)
                {
                    if(!err.empty())
                    {
			std::cout << "Error: " << err.c_str() << "\n";
                        notify();
                        return;
                    }
                    for(auto i = 0; i < 10; ++i)
                    {
                        File::Read(testfilename, 
                                   [](const File::ErrorMsg& err, const File::RawData& data)
                                   {
                                       if(!err.empty())
                                       {
					   std::cout << "Error: " << err.c_str() << "\n";
                                           return;
                                       }
				       std::stringstream msg;
				       msg << "<read: " <<  data.second << " bytes>\n";
                                       for(size_t i = 0; i < data.second; ++i)
                                       {
					   msg << data.first[i];
                                       }
				       std::cout << msg.str() << "\n";
                                   });
                    }
                    notify();
                });
    // wait that read has been dispatched ( since that happens in the cb above)
    std::unique_lock<std::mutex> lock(mutex);
    while(!done)
    {
        std::cout << "<waiting for signal>\n";
        cv.wait(lock);
    }
    std::cout << "<joining>\n";
    // wait for all dispatched work to run to completion before exiting the process
    ThreadPool::join();
    std::cout << "<done>\n";
    return EXIT_SUCCESS;
}


