# CMakeList.txt : CMake project for khang_exchange, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Project build
project ("khang_exchange")

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Force C++ 11 standard, so websocketpp doesn't look for Boost. 
# Note: Under Mingw, seems to have a warning that Boost.Thread is still required.
add_definitions(-D_WEBSOCKETPP_CPP11_STRICT_)

# Packages
find_package(fmt CONFIG REQUIRED)
find_package(websocketpp REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(asio REQUIRED)

# Add source to this project's executable.
add_executable (khang_exchange "khang_exchange.cpp" "khang_exchange.hpp" "myServer.hpp" "myServer.cpp")
target_link_libraries(khang_exchange PRIVATE fmt::fmt)
target_link_libraries(khang_exchange PRIVATE websocketpp::websocketpp)
target_link_libraries(khang_exchange PRIVATE jsoncpp_lib)
target_link_libraries(khang_exchange PRIVATE asio)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET khang_exchange PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
