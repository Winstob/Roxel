/* ---------------------------------------------------------------- *\
 * world.hpp
 * Author: Gavin Ralston
 * Date Created: 2023-12-12
\* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- *\
 * This is the interface between the world files on disk and the
 * cubes given to the game engine. The general format of the world
 * is as follows:
 *
 * The world is split up into zones, where each "zone" is a cube
 * of size determined by this program. Each axis of this zone is
 * (2^zone_depth_) voxels wide, and therefore contains
 * (2^(3*zone_depth_)) voxels in total.
 * Zone files (.zn) are formatted as a series of 16-bit integers,
 * where each number represents a voxel in that position of that
 * type. The position in the file matches with the position in the
 * world:
 *  The locations are laid out in a way similar to a hilbert curve.
 *  The first 1/8th of the data corresponds to the 0 quadrant of
 *  the current octree layer. The first 1/8th of this data
 *  corresponds to the 0 quadrant of the 0 quadrant of the current
 *  octree layer, and so on. This format allows fast and simple
 *  parsing. A diagram showing how each quadrant is laid out is
 *  shown below.
 *
 *              6 -------- 7
 *             /|         /|
 *            / |        / |
 *           /  |       /  |
 *          4 -------- 5   |
 * +Y       |   2 -----|-- 3
 *  |       |  /       |  /
 *  |  +Z   | /        | /
 *  |  /    |/         |/
 *  | /     0 -------- 1
 *  |/
 *  *---------- +X
 *
 * With a large world size, storage size becomes an issue. For this
 * reason, a modified version of Huffman encoding is used to store 
 * .zn files. For each set of equivalent voxel types, two integers
 * are stored in order: number of voxels in set
 * ( ceil((zone_depth_)/8)*8 bits)
 * and voxel type (16 bits).
\* ---------------------------------------------------------------- */
#ifndef WORLD_HPP
#define WORLD_HPP

#include <string>
#include "octree.hpp"
#include "anthrax_types.hpp"
#include "anthrax.hpp"

class World
{
public:
  World(std::string directory, Anthrax::Anthrax *anthrax_instance);
  void loadArea(Anthrax::vec3<int64_t> center, int load_distance);
  void getCubes(std::vector<Anthrax::Cube> *cube_vector);
private:
  const unsigned int num_layers_ = 32; // Number of layers in the octree - total world size in one axis is equal to 2^num_layers_
  const unsigned int zone_depth_ = 8; // Layer number of a zone - this determines the size of a zone 
                                      // A zone is a single file. The size of a zone in one axis is
                                      // equal to 2^zone_depth_.
  std::string directory_; // Location on disk containing this world's files
  Octree octree_; // Container for all voxels

  Anthrax::Anthrax *anthrax_instance_;
};
#endif // WORLD_HPP
