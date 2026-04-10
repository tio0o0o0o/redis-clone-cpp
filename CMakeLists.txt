cmake_minimum_required(VERSION 3.20)
project(redis-clone-cpp VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-fno-omit-frame-pointer -fdiagnostics-show-template-tree)

if (CMAKE_BUILD_TYPE STREQUAL "Debug") 
  add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)  
  add_link_options(-fsanitize=address -fsanitize=undefined)
endif()

include(FetchContent)

# Modules
FetchContent_Declare(
  googletest 
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(googletest)

include_directories(include)

# Main binary
add_executable(client
  playground/clienttest.cpp
)

add_library(resp2
  src/common/resp2.cpp
)

add_library(net
  src/common/socket.cpp 
  src/server/server.cpp 
  src/client/client.cpp 
)

add_executable(newclient
  playground/newclient.cpp
)

add_executable(newserver
  playground/newserver.cpp
)

target_link_libraries(newclient PRIVATE net)
target_link_libraries(newserver PRIVATE net)

# Testing binary
enable_testing()

add_executable(test_resp2
  tests/cpp/common/test_resp2.cpp
)

target_link_libraries(test_resp2 PRIVATE GTest::gtest_main resp2)

include(GoogleTest)

gtest_discover_tests(test_resp2)
