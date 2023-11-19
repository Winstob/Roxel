cmake_minimum_required(VERSION 3.5)
include(ExternalProject)

project(roxel)
set(PROJECT_NAME Roxel)

set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
externalproject_add(glfw_download
  GIT_REPOSITORY      https://github.com/glfw/glfw.git
  GIT_TAG             master
  SOURCE_DIR          ${GLFW_DIR}
  BUILD_COMMAND       ""
  INSTALL_COMMAND     ""
  )
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad)

# Subdirectories
add_subdirectory(src)
add_subdirectory(${GLFW_DIR})
add_subdirectory(${GLAD_DIR})

add_executable(${PROJECT_NAME}
  ${SRC}
  )

target_link_libraries(${PROJECT_NAME}
  glfw
  glad
  )
