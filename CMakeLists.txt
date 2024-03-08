cmake_minimum_required(VERSION 3.5)
project(prong VERSION 0.1 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Interface library for the prong header-only library
add_library(prong INTERFACE)

# Specify include directory for the prong interface library
target_include_directories(prong INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Interface library for the prong_integrations header-only library
add_library(prong_integrations INTERFACE)

# Specify include directory for the prong_integrations interface library
target_include_directories(prong_integrations INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add Boost include directories to the target
target_include_directories(https_client_demo PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(https_client_demo PRIVATE ${OPENSSL_INCLUDE_DIR})

# Set the export names for the targets
set(EXPORT_NAME_PRONG ProngTargets)
set(EXPORT_NAME_PRONG_INTEGRATIONS ProngIntegrationsTargets)

# Install the prong library target and create export set
install(TARGETS prong
    EXPORT ${EXPORT_NAME_PRONG}
    INCLUDES DESTINATION include
)

# Install the prong_integrations library target and create export set
install(TARGETS prong_integrations
    EXPORT ${EXPORT_NAME_PRONG_INTEGRATIONS}
    INCLUDES DESTINATION include
)

# Install the header files for prong and prong_integrations directories
install(DIRECTORY include/prong/ DESTINATION include/prong)
install(DIRECTORY include/prong_integrations/ DESTINATION include/prong_integrations)

# Install the master header files
install(FILES include/prong.h DESTINATION include)
install(FILES include/prong_integrations.h DESTINATION include)

# Generate and install the export sets
install(EXPORT ${EXPORT_NAME_PRONG}
    FILE ${EXPORT_NAME_PRONG}.cmake
    NAMESPACE Prong::
    DESTINATION lib/cmake/prong
)

install(EXPORT ${EXPORT_NAME_PRONG_INTEGRATIONS}
    FILE ${EXPORT_NAME_PRONG_INTEGRATIONS}.cmake
    NAMESPACE Prong::
    DESTINATION lib/cmake/prong_integrations
)

# Include module to generate package config files for both libraries
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ProngConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
write_basic_package_version_file(
    ProngIntegrationsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the config and version files for both libraries
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ProngConfigVersion.cmake"
    DESTINATION lib/cmake/prong
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ProngIntegrationsConfigVersion.cmake"
    DESTINATION lib/cmake/prong_integrations
)
