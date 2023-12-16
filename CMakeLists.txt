cmake_minimum_required(VERSION 3.5)

project(roxel)
set(PROJECT_NAME Roxel)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "-static") 
elseif(UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS "-static")
endif()

find_package(anthrax REQUIRED)
find_package(nlohmann_json REQUIRED)

# Subdirectories
add_subdirectory(src)

add_executable(${PROJECT_NAME}
  ${SRC}
  ${SRC_HEADERS}
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  anthrax
  nlohmann_json
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )
