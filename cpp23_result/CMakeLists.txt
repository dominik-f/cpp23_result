# CMakeList.txt : CMake project for cpp23_result, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.20)
project(cpp23_result VERSION 1.0 LANGUAGES CXX)

# Add source to this project's executable.
add_executable (cpp23_result "cpp23_result.cpp" "cpp23_result.h" "result.h")

set_target_properties(cpp23_result PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED true
)
