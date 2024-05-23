cmake_minimum_required(VERSION 3.15) # Set the minimum version of CMake
project(ProngExamples VERSION 0.1 LANGUAGES CXX)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

file(GLOB example-sources CONFIGURE_DEPENDS src/*.cpp)

foreach (source IN LISTS example-sources)
	get_filename_component(example "${source}" NAME_WE)
	add_executable(${example} ${source})

	# Include directories for your target
	target_include_directories(${example} PRIVATE ${Boost_INCLUDE_DIRS})
	target_include_directories(${example} PRIVATE ${OPENSSL_INCLUDE_DIR})

	# Link libraries to your executable
	target_link_libraries(${example}
			OpenSSL::SSL
			OpenSSL::Crypto
			${Boost_LIBRARIES}
			)
endforeach()

