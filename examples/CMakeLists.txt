cmake_minimum_required(VERSION 3.15) # Set the minimum version of CMake
project(ProngExamples VERSION 0.1 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Find the required packages
find_package(Boost REQUIRED COMPONENTS system) # Find Boost, system component for ASIO
find_package(OpenSSL REQUIRED) # Find OpenSSL
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Include the parent include directory to mimic installed state
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# List of example programs
set(EXAMPLE_PROGRAMS 
    chains_and_runnables
    openai_chat_model
    prompt_type_safety
    simple_chat_model
)

# Loop to create executables for each example
foreach(example ${EXAMPLE_PROGRAMS})
    add_executable(${example} ${example}.cpp) # Create executable from each source file

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
