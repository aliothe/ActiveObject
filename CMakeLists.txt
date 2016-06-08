cmake_minimum_required (VERSION 3.0.2)
project (LibActiveObject)
find_package (Threads)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -O2")
endif()

add_library(ActiveObject "${PROJECT_SOURCE_DIR}/src/ActiveObject.cc")

include_directories ("${PROJECT_SOURCE_DIR}/include")

add_executable(TestAO "${PROJECT_SOURCE_DIR}/test/testAO.cc")
add_executable(TestAO2 "${PROJECT_SOURCE_DIR}/test/testAO2.cc")
add_executable(TestAOException "${PROJECT_SOURCE_DIR}/test/testAOException.cc")
add_executable(TestAsyncIO "${PROJECT_SOURCE_DIR}/test/testAsyncIO.cc")

install (TARGETS ActiveObject DESTINATION lib)
install (FILES include/ActiveObject.h include/ConcurrentQueue.h DESTINATION include)

target_link_libraries(TestAO ActiveObject ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestAO2 ActiveObject ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestAOException ActiveObject ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(TestAsyncIO ActiveObject ${CMAKE_THREAD_LIBS_INIT})


