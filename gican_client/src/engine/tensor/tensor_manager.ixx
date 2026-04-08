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
#include <fstream>

#include "zlc/zlibcomplete.hpp"

export module engine.tensor:tensor_manager;

import util;

export namespace engine::tensor {
    // To note for voxels, they are not set structs and are instead left open
    // for interpretation because even though this engine will probably only be used for this game,
    // I want to keep the engine purely independent.
    struct VoxelBase;

    template <class T>
    concept IsVoxel = 
        // A voxel class must inherit from VoxelBase for explicitness.
        std::is_base_of<VoxelBase, T>::value && 
        
        // A voxel must contain a constant that reads how much it promises to write and read from file buffers.
        std::same_as<decltype(T::SERIALIZED_SIZE), std::size_t> &&

        // An important number to keep track of to prevent loading a file format where it is not supported version-wise.
        std::same_as<decltype(T::DATA_VERSION), std::size_t> &&

        requires(T t, char* buffer) {
            { t.serialize(buffer) } -> std::same_as<void>;
        } &&
        requires(T t, const char* buffer) {
            { t.deserialize(buffer) } -> std::same_as<void>;
        };

    enum class _ChunkFlags : std::uint8_t {
        NONE = 0,

        // Whether or not the chunk should be actively modified and displayed in-game.
        // If disabled, the chunk is elegible for being cleaned up from RAM and serialized to the disk.
        IS_LOADED = 1 << 0,

        // Marked as true the moment a change is made to the chunk.
        // The engine does not interact with this variable. The game layer on top should set this flag when needed.
        IS_SERIALIZABLE = 1 << 1,
    };

    using ChunkFlags = util::BitFlags<_ChunkFlags>;

    template <IsVoxel Voxel>
    class Chunk {
    public:
        using ChunkLength = uint16_t;
        using ChunkVolume = uint16_t;

        /// @warning GROUNDS FOR DATA_VERSION CHANGE
        static constexpr ChunkLength CHUNK_LENGTH = 16;
        static constexpr ChunkVolume CHUNK_VOLUME = CHUNK_LENGTH * CHUNK_LENGTH * CHUNK_LENGTH;

        static constexpr std::size_t SERIALIZED_SIZE = CHUNK_VOLUME * Voxel::SERIALIZED_SIZE;

        using AsSerialzied = std::string;

        // Update every time there is a change to the chunk system that requires a new file format.
        static constexpr std::size_t DATA_VERSION = 0;
        
        /// @todo Make internals for voxels a pallated system.
        util::CubeArray<Voxel, ChunkLength, CHUNK_LENGTH> voxels;

        // Flags dictate external behavior with the Chunk, not how the chunk works internally.
        // Can be modified by the engine or game layer depending on the specific flag.
        ChunkFlags flags;
    
        AsSerialzied serialize() const {
            AsSerialzied buffer(SERIALIZED_SIZE);

            char* ptr = buffer.data();
            for (const Voxel& voxel : voxels.items) {
                voxel.serialize(ptr);
                ptr += Voxel::SERIALIZED_SIZE;
            }

            return buffer;
        }

        void deserialize(AsSerialzied& buffer) {
            const char* ptr = buffer.data();
            for (Voxel& voxel : voxels) {
                voxel.deserialize(ptr);
                ptr += Voxel::SERIALIZED_SIZE;
            }
        }
    };

    /*
    
        REGION FILE FORMAT
        
        repeat (REGION_VOLUME) {
            u32 - chunk pos in rest of file per CHUNK_ALIGNMENT
        }
    */
    // A RAM representation of an existing region file for fast incremental disk editing
    template <IsVoxel Voxel>
    class Region {
    public:
        // Change every time a major file-based change occurs to the Region struct.
        // This variable will be smashed together with the other data versions of Chunk and Voxel to make a unique tag.
        static constexpr std::size_t DATA_VERSION = 0;

        // The character used to delimit the numbers in their vec3 position.
        static constexpr char FILE_NAME_POS_DELIMITER = '-';

        // The size of each buffer chunk the file system should use when reading and writing.
        static constexpr std::size_t FSTREAM_BUFFER_SIZE = 5e6; // 5 megs. magic number, change however

        /// @warning DATA_VERSION should increment if these change:
        static constexpr std::size_t REGION_LENGTH = 32; // in chunks
        static constexpr std::size_t REGION_VOLUME = REGION_LENGTH * REGION_LENGTH * REGION_LENGTH;

        Region(std::string region_directory, util::Vec3I region_pos)
            : region_pos(region_pos) {
                stream.open(region_directory + '/' + region_pos.to_string(FILE_NAME_POS_DELIMITER), std::ios::in | std::ios::out | std::ios::binary);
            }

        const util::Vec3I region_pos;

        // Note: registers new index if there is none found
        std::size_t find_chunk_index(util::Vec3I chunk_pos) {

        }

        /* u32 - #bytes of chunk for quick access - n - bytes chunk takes up  */
        void write_chunk(util::Vec3I chunk_pos, Chunk<Voxel>& chunk) {
            std::size_t index = find_chunk_index(chunk_pos);
            std::string serialized = chunk.serialize();
            std::string length_indicator;

            /// @todo: to_append needs to include 4 bytes in the beginning stating how long the chunk of memory is

            std::string to_append = compressor.compress(serialized);

            stream.seekp(index);
            stream.write(to_append.data(), to_append.length());
        }

        void load_chunk(util::Vec3I chunk_pos, Chunk<Voxel>& chunk) {
            std::size_t index = find_chunk_index(chunk_pos);
            stream.seekg(index);

            // read first 4 bits to get buffer length.
            /// @warning @todo THIS BECOMES ENDIAN-DEPENDENT.
            std::uint32_t buffer_length;
            stream.read(reinterpret_cast<char*>(&buffer_length), sizeof(buffer_length));

            std::string compressed;
            stream.read(compressed.data(), buffer_length);

            std::string decompressed = decompressor.decompress(compressed);
            chunk.deserialize(decompressed);
        }

    private:
        std::fstream stream;

        zlibcomplete::ZLibCompressor compressor;
        zlibcomplete::ZLibDecompressor decompressor;
    };

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