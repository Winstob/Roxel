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
    //std::cout << conversion_map_["1"]["name"] << std::endl;
  }
  void setFile(std::string filename)
  {
    std::ifstream file(filename);
    file >> conversion_map_;
  }
  Anthrax::Cube convert(int id, Anthrax::vec3<float> position, int size)
  {
    std::string id_string = std::to_string(id);
    std::vector<float> ambient = conversion_map_[id_string]["ambient"];
    std::vector<float> diffuse = conversion_map_[id_string]["ambient"];
    std::vector<float> specular = conversion_map_[id_string]["ambient"];
    return Anthrax::Cube(position,
          size,
          Anthrax::vec3<float>(ambient[0], ambient[1], ambient[2]),
          Anthrax::vec3<float>(diffuse[0], diffuse[1], diffuse[2]),
          Anthrax::vec3<float>(specular[0], specular[1], specular[2]),
          conversion_map_[id_string]["shininess"], conversion_map_[id_string]["opacity"]);
  }
private:
  json conversion_map_;
};

#endif // CUBECONVERT_HPP
