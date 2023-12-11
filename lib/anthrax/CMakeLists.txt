project(Anthrax)

set(PROJECT_NAME anthrax)

set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
#externalproject_add(glfw_download
#  GIT_REPOSITORY      https://github.com/glfw/glfw.git
#  GIT_TAG             master
#  SOURCE_DIR          ${GLFW_DIR}
#  BUILD_COMMAND       ""
#  INSTALL_COMMAND     ""
#  )
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC)

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad)
#externalproject_add(glm_download
#  GIT_REPOSITORY      https://github.com/g-truc/glm.git
#  GIT_TAG             master
#  SOURCE_DIR          ${GLM_DIR}
#  BUILD_COMMAND       ""
#  INSTALL_COMMAND     ""
#  )

set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

add_subdirectory(${GLFW_DIR})
add_subdirectory(${GLAD_DIR})
add_subdirectory(${GLM_DIR})

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/anthrax.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/shader.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cube.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/anthrax_types.hpp
  )

set(SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/anthrax.cpp
  )

set(SHADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/internalshaders.hpp
  )

add_library(${PROJECT_NAME}
  ${SRC}
  ${HEADERS}
  ${SHADERS}
  )

target_link_libraries(${PROJECT_NAME}
  glfw
  glad
  glm
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
