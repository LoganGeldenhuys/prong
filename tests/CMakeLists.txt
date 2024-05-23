cmake_minimum_required(VERSION 3.15) # Set the minimum version of CMake
project(ProngTests VERSION 0.1 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Find the required packages
find_package(Boost REQUIRED COMPONENTS system) # Find Boost, system component for ASIO
find_package(OpenSSL REQUIRED) # Find OpenSSL
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Include the parent include directory to mimic installed state
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Include CTest to enable testing
include(CTest)

# Enable testing and define test configurations
enable_testing()

# Option to build tests, default is ON
option(BUILD_TESTS "Build the tests." ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB test-sources CONFIGURE_DEPENDS src/*.cpp)

add_custom_target(prong-test)

if(BUILD_TESTS)
	foreach (source IN LISTS test-sources)
	  get_filename_component(test "${source}" NAME_WE)
			add_executable(prong-test-${test} ${source})

			# Include directories for target
			target_include_directories(prong-test-${test} PRIVATE ${Boost_INCLUDE_DIRS})
			target_include_directories(prong-test-${test} PRIVATE ${OPENSSL_INCLUDE_DIR})

			# Link libraries to executable
			target_link_libraries(prong-test-${test}
					OpenSSL::SSL
					OpenSSL::Crypto
					${Boost_LIBRARIES}
					)

			# Add the tests to CTest
			add_test(NAME prong-test-${test} COMMAND prong-test-${test})
			add_dependencies(prong-test prong-test-${test})
	endforeach()
endif()

