/*

The tensor manager is responsible for serverside handling of chunks, serializing, deserializing, loading, unloading, and more.
To note, this is the engine side of the project. This means that there are no mentions of sketches or any other data relating to a live ingame voxel.

For that, this file is built to be the base architecture that game/ can build on.
*/

module;

#include <string>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <array>

export module engine.tensor:tensor_manager;

import util;

export namespace engine::tensor {
    using ChunkLength = uint16_t;
    using ChunkVolume = uint16_t;
    constexpr ChunkLength CHUNK_LENGTH = 16;
    constexpr ChunkVolume CHUNK_VOLUME = CHUNK_LENGTH * CHUNK_LENGTH * CHUNK_LENGTH;

    // To note for voxels, they are not set structs and are instead left open
    // for interpretation because even though this engine will probably only be used for this game,
    // I want to keep the engine purely independent.
    struct VoxelBase;

    template <class T>
    concept IsVoxel = 
        std::is_base_of<VoxelBase, T>::value && 
        std::same_as<decltype(T::SERIALIZED_BYTE_COUNT), std::size_t> &&
        requires(T t, std::byte* ptr) {
            { t.serialize(ptr) } -> std::same_as<void>;

            { t.deserialize(ptr) } -> std::same_as<void>;
    };

    template <IsVoxel Voxel>
    struct Chunk {
        /// @todo Replace with actual type
        using AsSerialized = std::array<std::byte, CHUNK_VOLUME * Voxel::SERIALIZED_BYTE_COUNT>;

        util::CubeArray<Voxel, ChunkLength, CHUNK_LENGTH> voxels;
    
        // Saves to a state in the form of bytes.
        AsSerialized serialize() {
            AsSerialized container {};
            std::byte* ptr = container.data();

            for (ChunkLength index; Voxel& voxel : voxels.items) {
                voxels.items[index].serialize(ptr);
                ptr += Voxel::SERIALIZED_BYTE_COUNT;
            }

            return container;
        }

        // Loads a saved state in the form of bytes.
        void deserialize(AsSerialized serialized) {
            std::byte* ptr = serialized.data();
            for (ChunkVolume index = 0; index < serialized.size(); index++) {
                voxels.items[index].deserialize(ptr);
                ptr += Voxel::SERIALIZED_BYTE_COUNT;
            }
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

        /*
        
        GET-BACK-TO-NOTE

        Instead of moving chunks from loaded_chunks to unloaded_chunks and vise verca,

        STORE ALL CHUNKS IN ONE MAP, USE VECTORS TO STORE KEYS OF LOADED AND UNLOADED CHUNKS. BIG MEMORY AND PROCESS SAVER!!!
        
        */
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
            for (util::Vec3I& pos : chunks) {
                unloaded_chunks[pos] = loaded_chunks[pos];
                loaded_chunks.erase(pos);
            }
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