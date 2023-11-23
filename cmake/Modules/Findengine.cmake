# Look for engine library to use

set(ENGINE_LIBRARY engine)
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/engine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/engine)
