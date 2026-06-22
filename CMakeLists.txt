cmake_minimum_required(VERSION 3.20)
project(OxygenDeobf VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LUAU_INCLUDE_DIR /data/data/com.termux/files/usr/local/include)
set(LUAU_LIBRARY_DIR /data/data/com.termux/files/usr/local/lib)

set(SOURCES
    src/main.cpp
    src/Core/LuauVM.cpp
    src/Core/Sandbox.cpp
    src/Core/Pipeline.cpp
    src/Core/VariableRenamer.cpp
    src/Core/ErrorFixer.cpp
    src/Utils/Logger.cpp
    src/Utils/FileUtils.cpp
)

add_executable(OxygenDeobf ${SOURCES})
target_include_directories(OxygenDeobf PRIVATE src ${LUAU_INCLUDE_DIR})
target_link_libraries(OxygenDeobf PRIVATE
    ${LUAU_LIBRARY_DIR}/libLuau.Ast.a
    ${LUAU_LIBRARY_DIR}/libLuau.Bytecode.a
    ${LUAU_LIBRARY_DIR}/libLuau.CodeGen.a
    ${LUAU_LIBRARY_DIR}/libLuau.Common.a
    ${LUAU_LIBRARY_DIR}/libLuau.Compiler.a
    ${LUAU_LIBRARY_DIR}/libLuau.Config.a
    ${LUAU_LIBRARY_DIR}/libLuau.Require.a
    ${LUAU_LIBRARY_DIR}/libLuau.VM.a
    ${LUAU_LIBRARY_DIR}/libLuau.Analysis.a
    ${LUAU_LIBRARY_DIR}/libisocline.a
)
