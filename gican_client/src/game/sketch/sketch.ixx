/*

Sketches are 

@monoglint

*/
module;

#include <unordered_map>
#include <vector>
#include <cstddef>
#include <string>

export module game.sketch;

export namespace game::sketch {
    struct DemoStruct {

    };

    struct VoxelDataEntry {

    };

    class SketchEntries {
    private:
        std::unordered_map<std::string, std::size_t> str_id_map;
        std::vector<std::string> id_str_map;
        // std::vector<VoxelDataEntry> id_entry_map;
    };
}