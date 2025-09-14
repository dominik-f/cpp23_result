cmake_minimum_required(VERSION 3.25)
project(cpp23_result_tests CXX)

find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
	"tests.cpp"
)

target_compile_features(${PROJECT_NAME} PUBLIC
    cxx_std_23
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	GTest::gtest
	GTest::gmock
	GTest::gmock_main
	cpp23_result::cpp23_result
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
