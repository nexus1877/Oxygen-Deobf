cmake_minimum_required(VERSION 3.20)
project(OxygenDeobf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Luau REQUIRED)

set(SOURCES
    src/main.cpp
    src/Core/LuauVM.cpp
    src/Core/Sandbox.cpp
    src/Core/Pipeline.cpp
    src/Utils/Logger.cpp
    src/Utils/FileUtils.cpp
    src/Obfuscators/IronBrew.cpp
    src/Obfuscators/MoonSec.cpp
    src/Obfuscators/Prometheus.cpp
    src/Obfuscators/WeAreDevs.cpp
)

add_executable(OxygenDeobf ${SOURCES})
target_include_directories(OxygenDeobf PRIVATE src)
target_link_libraries(OxygenDeobf PRIVATE Luau::Luau)
