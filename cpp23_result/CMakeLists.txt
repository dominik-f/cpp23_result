cmake_minimum_required(VERSION 3.25)
project(cpp23_result CXX)

# Add source to this project's executable.
add_library(${PROJECT_NAME} SHARED
	"cpp23_result.cpp"
	"cpp23_result.h"
	"result.h"
)
add_library("cpp23_result::cpp23_result" ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC
    cxx_std_23
)

target_include_directories(${PROJECT_NAME}
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	PUBLIC
		$<INSTALL_INTERFACE:include/result>
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/df
)

if(${BUILD_TESTING})
	enable_testing()
	add_subdirectory(tests)
endif()
