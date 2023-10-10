cmake_minimum_required (VERSION 3.22)
project (ExtendedSparseAddressSpace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_PREFIX_PATH "../install")
find_package(VulkanHeaders REQUIRED CONFIG)
message(STATUS "Vulkan Headers Version: ${VulkanHeaders_VERSION}")

add_compile_definitions($<$<CONFIG:Debug>:DEBUG> VK_ENABLE_BETA_EXTENSIONS)
    
set(TARGET "ExtendedSparseAddressSpaceImage")

add_executable(${TARGET} main.cpp)

target_link_libraries(
    ${TARGET} PRIVATE
    vulkan
    Vulkan::Headers)
