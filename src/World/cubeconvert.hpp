/* ---------------------------------------------------------------- *\
 * cubeconvert.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-15
\* ---------------------------------------------------------------- */

#ifndef CUBECONVERT_HPP
#define CUBECONVERT_HPP

#include <string>
#include <fstream>
#include <cstring>
#include "nlohmann/json.hpp"
#include "cube.hpp"
#include <iostream>

using json = nlohmann::json;

class CubeConvert
{
public:
  CubeConvert() {}
  CubeConvert(std::string filename)
  {
    std::ifstream file(filename);
    file >> conversion_map_;
  }
  void setFile(std::string filename)
  {
    std::ifstream file(filename);
    file >> conversion_map_;
  }
  Anthrax::Cube convert(int id, Anthrax::vec3<float> position, int size)
  {
    std::string id_string = std::to_string(id);
    std::vector<float> color = conversion_map_[id_string]["color"];
    return Anthrax::Cube(id,
        position,
        size,
        Anthrax::vec3<float>(color[0], color[1], color[2]),
        conversion_map_[id_string]["reflectivity"],
        conversion_map_[id_string]["shininess"],
        conversion_map_[id_string]["opacity"]);
  }
private:
  json conversion_map_;
};

#endif // CUBECONVERT_HPP
