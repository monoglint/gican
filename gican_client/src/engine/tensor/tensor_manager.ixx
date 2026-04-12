/*

The tensor manager is responsible for serverside handling of chunks, serializing, deserializing, loading, unloading, and more.
To note, this is the engine side of the project. This means that there are no mentions of sketches or any other data relating to a live ingame voxel.

For that, this file is built to be the base architecture that game/ can build on.
*/


module;

#include <cstring>
#include <string>
#include <cstddef>
#include <unordered_map>

export module engine.tensor:tensor_manager;

import util;
import :chunk;
import :region;

export namespace engine::tensor {

    // A hashmap of chunks identified by their vector3 position.
    template <IsVoxel Voxel>
    using ChunkPosMap = std::unordered_map<util::Vec3I, Chunk<Voxel>>;
    
    // A chunk drawer can draw individual voxels based on the user's provided implementation of T::draw.
    // Designed to take in a list of chunks, and permits any form of external modification.
    template <class T, class Voxel>
    concept IsChunkDrawer = requires(ChunkPosMap<Voxel>& chunks) {
        { T::draw(chunks) } -> std::same_as<void>;
    };

    // An individual voxel drawer that runs based on the user's provided implementation of T::draw.
    // Designed as a callback for a loop-based function in TensorManager. Takes in a current position, and then a voxel to edit.
    template <class T, class Voxel>
    concept IsVoxelDrawer = requires(util::Vec3I pos, Voxel& voxel) {
        { T::draw(pos, voxel) } -> std::same_as<void>;
    };

    template <IsVoxel Voxel, IsChunkDrawer<Voxel> DefaultGenerator>
    class TensorManager {
    public:
        TensorManager(std::string region_directory)
            : region_directory(region_directory)
        {}

        ~TensorManager() {
            flush_unloaded_chunks();
        }

        TensorManager(const TensorManager&) = delete;
        TensorManager& operator=(const TensorManager&) = delete;

        TensorManager(TensorManager&&) = delete;
        TensorManager& operator=(const TensorManager&&) = delete;

        const std::string region_directory;

        /// @todo Fill in the bodies of these functions.

        // Reload or initialize new chunks that start actively running in the game immediately.
        void load_chunks(std::vector<util::Vec3I> chunks) {
            ChunkPosMap<Voxel> to_generate;

            for (util::Vec3I& chunk_pos : chunks) {
                auto itr = present_chunks.find(chunk_pos);

                if (itr != present_chunks.end()) {
                    itr->second.flags.enable();
                    continue;
                }

                util::Vec3I region_pos = get_region_pos(chunk_pos);
                RegionFileManager<Voxel>& region = get_region(region_pos);
                present_chunks[chunk_pos] = region.load_chunk(chunk_pos - region_pos);

                Chunk<Voxel>& loaded_chunk = present_chunks.at(chunk_pos);

                loaded_chunk.flags.enable(ChunkFlags::Values::IS_LOADED);

                // If a chunk is new/uninitialized, 
                if (loaded_chunk.flags.has(ChunkFlags::Values::HAS_BEEN_GENERATED))
                    continue;

                to_generate[chunk_pos] = loaded_chunk;
                loaded_chunk.flags.enable(ChunkFlags::Values::HAS_BEEN_GENERATED);
            }

            write_chunks<DefaultGenerator>(to_generate);
        }

        // Remove the chunks at the selected positions and have them cached for disk writing.
        void unload_chunks(std::vector<util::Vec3I> chunks) {
            for (util::Vec3I& pos : chunks) {
                auto itr = present_chunks.find(pos);

                if (itr != present_chunks.end())
                    itr->second.flags.disable(ChunkFlags::Values::IS_LOADED);
            }
        }

        // Flush cached chunks onto the disk.
        void flush_unloaded_chunks() {
            for (auto& [chunk_pos, chunk] : present_chunks) {
                if (!chunk.flags.has(ChunkFlags::Values::IS_SAVE_PENDING))
                    continue;

                util::Vec3I region_pos = get_region_pos(chunk_pos);
                RegionFileManager<Voxel>& region = get_region(region_pos);
                region.write_chunk(chunk_pos - region_pos, chunk);

                present_chunks.erase(chunk_pos);
            }
        }

        // Used for quick generation of new terrain in a newly loaded chunk, determined by the game layer.
        template <IsChunkDrawer<Voxel> ChunkDrawer>
        void write_chunks(ChunkPosMap<Voxel>& chunks) {
            ChunkDrawer::draw(chunks);
        }

        // a quick editor to write a big cube (or a voxel-sized cube) or voxels in any location in the world, even if the chunks are not loaded at the moment
        template <IsVoxelDrawer<Voxel> VoxelDrawer>
        void write_voxels_cube(util::Vec3I first, util::Vec3I second, Voxel voxel_data) {
            // no need to confirm positions are valid since this function dynamically loads and unloads chunks if needed to perform this operation
            
            // 1. warn if size is too large (arbitrary number, not set yet)
            // 2. get chunk positions of lower and upper bound positions
            // 3. loop through each chunk, ensure at least all of them are in loaded_chunks or unloaded_chunks
            // 4. loop through each voxel, find its appropriate chunk, call Drawer::draw
        }

        util::Vec3I get_region_pos(util::Vec3I chunk_pos) {
            return chunk_pos / RegionFileManager<Voxel>::REGION_LENGTH;
        }

        RegionFileManager<Voxel>& get_region(util::Vec3I region_pos) {
            auto itr = regions.find(region_pos);

            if (itr != regions.end())
                return itr;

            regions[region_pos] = RegionFileManager<Voxel>(region_directory, region_pos);

            return regions[region_pos];
        }
    private:
        // All chunks, loaded or unloaded, present in ram.
        ChunkPosMap<Voxel> present_chunks;
        std::unordered_map<util::Vec3I, RegionFileManager<Voxel>> regions;
    };
}