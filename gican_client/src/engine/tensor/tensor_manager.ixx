/*

The terrain manager is responsible for serverside handling of chunks, serializing, deserializing, loading, unloading, and more.

*/

module;

#include <string>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <limits>

export module engine.tensor:tensor_manager;

import util;

export namespace engine::tensor {
    using ChunkSize = uint8_t;
    constexpr ChunkSize CHUNK_SIZE = 16;

    static_assert(CHUNK_SIZE < std::numeric_limits<ChunkSize>::max(), "ChunkSize does not support CHUNK_SIZE. Increase it to a longer type.");

    // To note for voxels, they are not set structs and are instead left open
    // for interpretation because even though this engine will probably only be used for this game,
    // I want to keep the engine purely independent.
    struct VoxelBase;

    template <class T>
    concept IsVoxel = std::is_base_of<VoxelBase, T>::value && requires(T t) {
        // a voxel class must contain a type that represents itself serialized.
        // this type should be bit based and MUST be architecture friendly
        typename T::AsSerialized;

        // a voxel class should have a serialize function
        { t.serialize() -> std::template same_as<T::AsSerialized> };

        // to note, it might be useful to also require a named static constructor for deserializing which could POTENTIALLY speed things up a little bit
        // that idea is probably not necessary though
        // @monoglint 5 Apr 2026
        { t.deserialize(T::AsSerialized) -> std::template same_as<void> };
    };

    template <IsVoxel Voxel>
    struct Chunk {
        /// @todo Replace with actual type
        using AsSerialized = nullptr_t;

        util::CubeArray<Voxel, ChunkSize, CHUNK_SIZE> voxels;
    
        AsSerialized serialize() {
            // note: serialize assuming the deserializer is of <Voxel>

            // 1. loop through all voxels
            // 2. serialize them individually
            // 3. append their serialized data to a buffer
            // 4. return the buffer
            
            /// @todo Replace with actual Serialized type
            return nullptr;
        }

        void deserialize(AsSerialized serialized) {
            // note: deserialize assuming the serializer is of <Voxel>
            
            // 1. iterate through bits in the buffer, select by sizeof(Voxel::AsSerialized)
            // 2. run Voxel::deserialize per voxel, append one-by-one to voxels
        }
    };

    template <IsVoxel Voxel>
    using ChunkPosMap = std::unordered_map<util::Vec3I, Chunk<Voxel>>;\
    
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

        // active chunks that exist in the running game
        ChunkPosMap<Voxel> loaded_chunks;

        // unloaded chunks that are not on the disk yet
        ChunkPosMap<Voxel> unloaded_chunks;

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
            // 1. loop through each chunk
            // 2. move them over to discarded_chunks
        }

        // Flush cached chunks onto the disk.
        void flush_unloaded_chunks() {
            // loop through all chunks
            // serialize to their appropriate region files
            // clear
        }

        // Used for quick generation of new terrain in a newly loaded chunk, determined by the game layer.
        template <IsChunkDrawer<Voxel> ChunkDrawer>
        void write_chunks(ChunkPosMap<Voxel>& chunks) {
            ChunkDrawer::draw(chunks);
        }

        // a quick editor to write a big cube (or a voxel-sized cube) or voxels in any location in the world, even if the chunks are not loaded at the moment
        template <IsVoxelDrawer<Voxel> VoxelDrawer>
        void write_voxels(util::Vec3I first, util::Vec3I second, Voxel voxel_data) {
            // no need to confirm positions are valid since this function dynamically loads and unloads chunks if needed to perform this operation
            
            // 1. warn if size is too large (arbitrary number, not set yet)
            // 2. get chunk positions of lower and upper bound positions
            // 3. loop through each chunk, ensure at least all of them are in loaded_chunks or unloaded_chunks
            // 4. loop through each voxel, find its appropriate chunk, call Drawer::draw
        }
    };
}