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
        { T::draw(chunks) -> std::template same_as<void> };
    };

    // An individual voxel drawer that runs based on the user's provided implementation of T::draw.
    // Designed as a callback for a loop-based function in TensorManager. Takes in a current position, and then a voxel to edit.
    template <class T, class Voxel>
    concept IsVoxelDrawer = requires(util::Vec3I pos, Voxel& voxel) {
        { T::draw(pos, voxel) -> std::template same_as<void> };
    };

    template <IsVoxel Voxel>
    class TensorManager {
    public:
        ~TensorManager() {
            flush_unloaded_chunks();
        }

        std::string tensor_directory;

        ChunkPosMap<Voxel> registered_chunks;

        /// @todo Fill in the bodies of these functions.

        // Reload or initialize new chunks that start actively running in the game immediately.
        void load_chunks(std::vector<util::Vec3I> chunks) {
            // 1. loop through each chunk position
            // 2. discard positions that are already loaded
            // 3. check if the chunk is being cached in unloaded_chunks and use that if possible
            // 3.1 - if not:
                // 4. get the chunk position's region file
                // 5. open the region file if not already opened
                // 6. check if that chunk is saved in the region file
                    // 6.1. if not, instantiate a new chunk
                    // 6.2. run run_chunk_write()
                // 7. if the chunk is saved in the region file, serialize that part of it into a new chunk
                // 8. append into loaded_chunks by position
        }

        // Remove the chunks at the selected positions and have them cached for disk writing.
        void unload_chunks(std::vector<util::Vec3I> chunks) {
            for (util::Vec3I& pos : chunks) {
                registered_chunks[pos].flags.disable(ChunkFlags::Values::IS_LOADED);
            }
        }

        // Flush cached chunks onto the disk.
        void flush_unloaded_chunks() {
            for (Chunk<Voxel>& chunk : registered_chunks) {
                if (chunk.flags.test(ChunkFlags::Values::IS_LOADED))
                    continue;

                chunk.serialize();
            }

            registered_chunks.clear();
        }

        // Used for quick generation of new terrain in a newly loaded chunk, determined by the game layer.
        template <IsChunkDrawer<Voxel> ChunkDrawer>
        void write_chunks(ChunkPosMap<Voxel>& chunks) {
            ChunkDrawer::draw(chunks);
        }

        // a quick editor to write a big cube (or a voxel-sized cube) or voxels in any location in the world, even if the chunks are not loaded at the moment
        template <IsVoxelDrawer<Voxel> VoxelDrawer>
        void write_voxel_cube(util::Vec3I first, util::Vec3I second, Voxel voxel_data) {
            // no need to confirm positions are valid since this function dynamically loads and unloads chunks if needed to perform this operation
            
            // 1. warn if size is too large (arbitrary number, not set yet)
            // 2. get chunk positions of lower and upper bound positions
            // 3. loop through each chunk, ensure at least all of them are in loaded_chunks or unloaded_chunks
            // 4. loop through each voxel, find its appropriate chunk, call Drawer::draw
        }
    };
}