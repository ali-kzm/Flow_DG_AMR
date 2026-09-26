cmake_minimum_required(VERSION 3.20)

project(Flow_DG_AMR
    VERSION 0.1.0
    DESCRIPTION "DG flow solver with p4est AMR, MPI, and OpenCL"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FLOW_DG_AMR_BUILD_TESTS "Build project tests" ON)

find_package(MPI REQUIRED COMPONENTS C CXX)
find_package(OpenCL REQUIRED)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/p4est/CMakeLists.txt")
    message(FATAL_ERROR
        "p4est submodule is missing. Run: "
        "git submodule update --init --recursive"
    )
endif()

# p4est/libsc configuration.
# libsc controls whether p4est is compiled with MPI support.
set(SC_ENABLE_MPI ON CACHE BOOL "Enable MPI support in libsc/p4est" FORCE)
set(P4EST_BUILD_TESTING OFF CACHE BOOL "Disable p4est tests" FORCE)
set(P4EST_BUILD_EXAMPLES OFF CACHE BOOL "Disable p4est examples" FORCE)
set(enable_p6est OFF CACHE BOOL "Disable p6est" FORCE)
set(enable_p8est OFF CACHE BOOL "Disable p8est" FORCE)

add_subdirectory(lib/p4est EXCLUDE_FROM_ALL)

add_executable(flow_dg_amr
    src/main.cpp
    src/mesh2d.cpp
)

target_include_directories(flow_dg_amr
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(flow_dg_amr
    PRIVATE
        MPI::MPI_CXX
        OpenCL::OpenCL
        P4EST::P4EST
)

target_compile_features(flow_dg_amr PRIVATE cxx_std_20)

if(FLOW_DG_AMR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
