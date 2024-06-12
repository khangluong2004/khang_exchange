# CMakeList.txt : CMake project for khang_exchange, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(fmt_DIR "D:\\C_C++\\khang_exchange\\khang_exchange\\out\\build\\x86-debug\\vcpkg_installed\\vcpkg\\pkgs\\fmt_x86-windows\\share\\fmt")

project ("khang_exchange")
find_package(fmt CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (khang_exchange "khang_exchange.cpp" "khang_exchange.h")
target_link_libraries(khang_exchange PRIVATE fmt::fmt)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET khang_exchange PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
