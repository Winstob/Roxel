cmake_minimum_required(VERSION 3.5)

project(roxel)
set(PROJECT_NAME Roxel)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(engine REQUIRED)

# Subdirectories
add_subdirectory(src)

add_executable(${PROJECT_NAME}
  ${SRC}
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  engine
  )
