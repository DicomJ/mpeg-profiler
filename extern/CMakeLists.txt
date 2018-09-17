include(ExternalProject)

Execute_Process(COMMAND git submodule update --init
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ExternalProject_Add(mpeg-mp4
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mpeg-mp4
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mpeg-mp4/build
    INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mpeg-mp4/build/install
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCXX_STD=${CXX_STD} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)

