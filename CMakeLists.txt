cmake_minimum_required(VERSION 3.15)
project(Froggy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Recommend vcpkg for dependencies:
#   vcpkg install glfw3 glm glad tinyobjloader
#
# Configure with:
# cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(tinyobjloader CONFIG QUIET) # optional if installed via vcpkg
# glad is often added via vcpkg as well or you can include glad.c in src
find_package(glad CONFIG QUIET)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC "src/*.cpp")

add_executable(Froggy ${SRC})

# If you installed via vcpkg and these are available as targets:
if (TARGET glfw)
  target_link_libraries(Froggy PRIVATE glfw OpenGL::GL glm::glm)
else()
  target_link_libraries(Froggy PRIVATE OpenGL::GL)
endif()

if (TARGET tinyobjloader::tinyobjloader)
  target_link_libraries(Froggy PRIVATE tinyobjloader::tinyobjloader)
else()
  # no-op; we will expect tiny_obj_loader.h in include/ if not installed
endif()

if (TARGET glad::glad)
  target_link_libraries(Froggy PRIVATE glad::glad)
endif()

# On some systems you may need to link pthread or other libs, handle as needed.
