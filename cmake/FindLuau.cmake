set(LUAU_INCLUDE_DIR /data/data/com.termux/files/usr/local/include)
set(LUAU_LIBRARY_DIR /data/data/com.termux/files/usr/local/lib)

find_path(LUAU_INCLUDE_DIRS
  NAMES lua.h luacode.h Luau/Compiler.h
  PATHS ${LUAU_INCLUDE_DIR}
  NO_DEFAULT_PATH
)

find_library(LUAU_AST_LIB NAMES Luau.Ast libLuau.Ast PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_BYTECODE_LIB NAMES Luau.Bytecode libLuau.Bytecode PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_CODEGEN_LIB NAMES Luau.CodeGen libLuau.CodeGen PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_COMMON_LIB NAMES Luau.Common libLuau.Common PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_COMPILER_LIB NAMES Luau.Compiler libLuau.Compiler PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_CONFIG_LIB NAMES Luau.Config libLuau.Config PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_REQUIRE_LIB NAMES Luau.Require libLuau.Require PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_VM_LIB NAMES Luau.VM libLuau.VM PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)
find_library(LUAU_ANALYSIS_LIB NAMES Luau.Analysis libLuau.Analysis PATHS ${LUAU_LIBRARY_DIR} NO_DEFAULT_PATH)

set(LUAU_LIBRARIES
  ${LUAU_AST_LIB}
  ${LUAU_BYTECODE_LIB}
  ${LUAU_CODEGEN_LIB}
  ${LUAU_COMMON_LIB}
  ${LUAU_COMPILER_LIB}
  ${LUAU_CONFIG_LIB}
  ${LUAU_REQUIRE_LIB}
  ${LUAU_VM_LIB}
  ${LUAU_ANALYSIS_LIB}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Luau REQUIRED_VARS LUAU_INCLUDE_DIRS LUAU_LIBRARIES)

if(Luau_FOUND)
  add_library(Luau::Luau UNKNOWN IMPORTED)
  set_target_properties(Luau::Luau PROPERTIES
    IMPORTED_LOCATION "${LUAU_COMPILER_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${LUAU_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${LUAU_LIBRARIES}"
  )
endif()
