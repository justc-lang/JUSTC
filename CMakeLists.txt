# MIT License
#
# Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.14)

project(JUSTC)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(VCPKG_BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(VCPKG_TARGET_TRIPLET x64-windows-static)

include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)
if(NUM_PROCESSORS EQUAL 0)
    set(NUM_PROCESSORS 1)
endif()
set(CMAKE_BUILD_PARALLEL_LEVEL ${NUM_PROCESSORS})

if(MSVC)
    add_compile_options(/O2 /MP /EHsc)
else()
    add_compile_options(-O3 -pipe)
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation prefix")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

include(FetchContent)

set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(CEREAL_BUILD_SANDBOX OFF CACHE BOOL "Disable sandbox examples" FORCE)
set(CEREAL_BUILD_DOCS OFF CACHE BOOL "Disable documentation" FORCE)
set(CEREAL_BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(CEREAL_USE_BOOST OFF CACHE BOOL "" FORCE)
find_package(Cereal QUIET)
if(NOT Cereal_FOUND)
    message(STATUS "Cereal not found, downloading from GitHub...")
    FetchContent_Declare(
        cereal
        GIT_REPOSITORY https://github.com/USCiLab/cereal.git
        GIT_TAG v1.3.2
    )
    FetchContent_MakeAvailable(cereal)
    set(CEREAL_INCLUDE_DIR ${cereal_SOURCE_DIR}/include)
else()
    set(CEREAL_INCLUDE_DIR ${Cereal_INCLUDE_DIR})
endif()

find_package(cpr QUIET)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost CONFIG REQUIRED)

if(NOT cpr_FOUND)
    message(STATUS "CPR not found in system, building from source...")
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
        FetchContent_Declare(cpr
            GIT_REPOSITORY https://github.com/libcpr/cpr.git
            GIT_TAG 3591f20e82338d0a21733045f59accd2d476d84f # CPR 1.9.8
        )

        set(BUILD_CPR_TESTS OFF CACHE BOOL "Disable CPR tests")
        set(CPR_BUILD_TESTS OFF CACHE BOOL "Disable CPR tests")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries")
        set(CPR_BUILD_TESTS OFF CACHE BOOL "Disable CPR tests")

        FetchContent_MakeAvailable(cpr)
        set(CPR_BUILT_LOCALLY TRUE)
    else()
        message(WARNING "CPR auto-download requires CMake 3.14+. Please install CPR manually.")
        set(CPR_BUILT_LOCALLY FALSE)
    endif()
else()
    message(STATUS "Found system CPR: ${cpr_VERSION}")
endif()

message(STATUS "Installing QuickJS CMake...")
if(NOT MSVC)
    FetchContent_Declare(quickjscmake
        GIT_REPOSITORY https://github.com/robloach/quickjs-cmake.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(quickjscmake)
endif()

if(NOT MSVC)
    # MSVC compatibility for QuickJS - not needed since disabled
endif()

message(STATUS "Installing Luau...")
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    FetchContent_Declare(luau
        GIT_REPOSITORY https://github.com/Roblox/luau.git
        GIT_TAG 0.699
    )

    set(LUAU_BUILD_CLI OFF CACHE BOOL "Disable CLI")
    set(LUAU_BUILD_TESTS OFF CACHE BOOL "Disable tests")
    set(LUAU_BUILD_WEB OFF CACHE BOOL "Disable web build")

    FetchContent_MakeAvailable(luau)

    if(luau_POPULATED)
        message(STATUS "Luau successfully configured")

        add_library(luau_lib STATIC
            ${luau_SOURCE_DIR}/Ast/src/Ast.cpp
            ${luau_SOURCE_DIR}/Ast/src/Confusables.cpp
            ${luau_SOURCE_DIR}/Ast/src/Lexer.cpp
            ${luau_SOURCE_DIR}/Ast/src/Location.cpp
            ${luau_SOURCE_DIR}/Ast/src/Parser.cpp
            ${luau_SOURCE_DIR}/Common/src/StringUtils.cpp
            ${luau_SOURCE_DIR}/Ast/src/TimeTrace.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Builtins.cpp
            ${luau_SOURCE_DIR}/Compiler/src/BuiltinFolding.cpp
            ${luau_SOURCE_DIR}/Compiler/src/BytecodeBuilder.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Compiler.cpp
            ${luau_SOURCE_DIR}/Compiler/src/ConstantFolding.cpp
            ${luau_SOURCE_DIR}/Compiler/src/CostModel.cpp
            ${luau_SOURCE_DIR}/Compiler/src/lcode.cpp
            ${luau_SOURCE_DIR}/Compiler/src/TableShape.cpp
            ${luau_SOURCE_DIR}/Compiler/src/ValueTracking.cpp
            ${luau_SOURCE_DIR}/VM/src/lapi.cpp
            ${luau_SOURCE_DIR}/VM/src/laux.cpp
            ${luau_SOURCE_DIR}/VM/src/lbaselib.cpp
            ${luau_SOURCE_DIR}/VM/src/lbitlib.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuiltins.cpp
            ${luau_SOURCE_DIR}/VM/src/lcorolib.cpp
            ${luau_SOURCE_DIR}/VM/src/ldblib.cpp
            ${luau_SOURCE_DIR}/VM/src/ldebug.cpp
            ${luau_SOURCE_DIR}/VM/src/ldo.cpp
            ${luau_SOURCE_DIR}/VM/src/lfunc.cpp
            ${luau_SOURCE_DIR}/VM/src/lgc.cpp
            ${luau_SOURCE_DIR}/VM/src/linit.cpp
            ${luau_SOURCE_DIR}/VM/src/lmathlib.cpp
            ${luau_SOURCE_DIR}/VM/src/lmem.cpp
            ${luau_SOURCE_DIR}/VM/src/lobject.cpp
            ${luau_SOURCE_DIR}/VM/src/loslib.cpp
            ${luau_SOURCE_DIR}/VM/src/lperf.cpp
            ${luau_SOURCE_DIR}/VM/src/lstate.cpp
            ${luau_SOURCE_DIR}/VM/src/lstring.cpp
            ${luau_SOURCE_DIR}/VM/src/lstrlib.cpp
            ${luau_SOURCE_DIR}/VM/src/ltable.cpp
            ${luau_SOURCE_DIR}/VM/src/ltablib.cpp
            ${luau_SOURCE_DIR}/VM/src/ltm.cpp
            ${luau_SOURCE_DIR}/VM/src/ludata.cpp
            ${luau_SOURCE_DIR}/VM/src/lutf8lib.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmexecute.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmload.cpp
            ${luau_SOURCE_DIR}/VM/src/lvmutils.cpp
            ${luau_SOURCE_DIR}/Ast/src/Allocator.cpp
            ${luau_SOURCE_DIR}/Ast/src/Cst.cpp
            ${luau_SOURCE_DIR}/Ast/src/PrettyPrinter.cpp
            ${luau_SOURCE_DIR}/Compiler/src/Types.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuffer.cpp
            ${luau_SOURCE_DIR}/VM/src/lbuflib.cpp
            ${luau_SOURCE_DIR}/VM/src/lgcdebug.cpp
            ${luau_SOURCE_DIR}/VM/src/lnumprint.cpp
            ${luau_SOURCE_DIR}/VM/src/lveclib.cpp
        )

        target_include_directories(luau_lib PUBLIC
            ${luau_SOURCE_DIR}/Ast/include
            ${luau_SOURCE_DIR}/Compiler/include
            ${luau_SOURCE_DIR}/VM/include
            ${luau_SOURCE_DIR}/Common/include
        )

        target_compile_definitions(luau_lib PRIVATE
            LUA_USE_LINUX
            LUAU_FASTCALL
        )

        if(MSVC)
            target_compile_options(luau_lib PRIVATE /W0)
        else()
            target_compile_options(luau_lib PRIVATE -w)
        endif()

        message(STATUS "Luau built from source")
    endif()
else()
    message(WARNING "Please use CMake 3.14+")
endif()

find_package(ICU REQUIRED COMPONENTS uc i18n)

include_directories(third-party)

set(CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/keywords.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/fetch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/unicode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/builtins.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/cpptypes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/justb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/compiler/justb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/loader/justb.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/serializer/json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/serializer/justo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/serializer/xml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/serializer/yaml.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/parser/json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/parser/justo.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lang/js.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/lang/luau.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/built-in/http/http.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/built-in/math/math.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/built-in/binary/binary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/built-in/string/string.cpp
)

if(MSVC)
    list(REMOVE_ITEM CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core/lang/js.cpp)
endif()

add_library(justc_core STATIC ${CORE_SOURCES})

target_include_directories(justc_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
)

target_include_directories(justc_core SYSTEM PRIVATE ${CEREAL_INCLUDE_DIR})

target_link_libraries(justc_core PRIVATE luau_lib)

if(NOT MSVC)
    target_link_libraries(justc_core PRIVATE quickjs cpr)
else()
    target_link_libraries(justc_core PRIVATE cpr::cpr)
endif()
target_include_directories(justc_core SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(justc_core PRIVATE ICU::uc ICU::i18n)

add_library(justc_lib SHARED core/entry/lib.cpp)
target_compile_definitions(justc_lib PRIVATE JUSTC_BUILD)
target_include_directories(justc_lib PUBLIC core)
target_include_directories(justc_lib SYSTEM PRIVATE ${CEREAL_INCLUDE_DIR})
target_link_libraries(justc_lib PUBLIC justc_core)

if(NOT MSVC)
    set_target_properties(justc_lib PROPERTIES OUTPUT_NAME "justc")
else()
    set_target_properties(justc_lib PROPERTIES OUTPUT_NAME "libjustc")
endif()

add_executable(justc_cli core/entry/cli.cpp)
target_include_directories(justc_cli SYSTEM PRIVATE ${CEREAL_INCLUDE_DIR})
target_link_libraries(justc_cli PRIVATE justc_core)
set_target_properties(justc_cli PROPERTIES OUTPUT_NAME "justc")

if(MSVC)
    target_compile_options(justc_core PRIVATE /std:c++20)
    target_compile_options(justc_cli PRIVATE /std:c++20)
else()
    target_compile_options(justc_core PRIVATE -std=c++17)
    target_compile_options(justc_cli PRIVATE -std=c++17)
endif()


find_library(QUADMATH_LIB quadmath)
if(QUADMATH_LIB)
    target_link_libraries(justc_core PRIVATE ${QUADMATH_LIB})
    target_link_libraries(justc_cli PRIVATE ${QUADMATH_LIB})
    target_link_libraries(justc_lib PRIVATE ${QUADMATH_LIB})
    target_compile_definitions(justc_core PRIVATE JUSTC_HAS_QUADMATH=1)
else()
    target_compile_definitions(justc_core PRIVATE JUSTC_HAS_QUADMATH=0)
endif()

set_property(TARGET justc_core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(TARGET justc_lib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(TARGET justc_cli PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

install(TARGETS justc_cli RUNTIME DESTINATION bin)
