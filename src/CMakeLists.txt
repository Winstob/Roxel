set(SRC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/force.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Player/playersettings.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Player/player.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/octree.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/voxelset.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/world.hpp
  PARENT_SCOPE
  )

set(SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Player/player.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/octree.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/voxelset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/World/world.cpp
  PARENT_SCOPE
  )
