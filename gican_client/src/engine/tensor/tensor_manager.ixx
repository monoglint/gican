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

export module engine.tensor:tensor_manager;

import util;

export namespace engine::tensor {
    namespace io {
        // This variable can change across versions. The header for the region file should start with the size of the chunks.

        constexpr std::uint32_t BUFFER_SIZE = 1024 * 8;

        /*
        
        NOTE
        ALL REGION FILE FORMATS SHOULD HAVE THEIR FIRST 4 BYTES DEDICATED TO HOW BIG EACH BUFFER CHUNK IS.
        
        */

        // This class performs live writing,
        class RegionBufferWriter {
        public:
            RegionBufferWriter(std::string path, std::uint32_t BUFFER_SIZE)
                : BUFFER_SIZE(BUFFER_SIZE)
            {
                // removing .open() and passing path into stream initialization is okay if there are no reasons later to have .open() individual
                stream.open(path, std::ios::binary);

            }

            ~RegionBufferWriter() {
                flush_to_stream();
            }

            const std::uint32_t BUFFER_SIZE;

            // Writing needs a given position for incremental writing.
            void write_at() {

            }
            
            void flush_to_stream() {
                /// @todo impl compressor
                
                // for now, just assume buffer is the final output:
                stream.write(buffer.data(), buffer.size());
            }
        private:
            std::vector<char> buffer;
            std::ofstream stream;
            /// @todo impl compressor
        };

        class RegionBufferReader {

        };
    }

    // To note for voxels, they are not set structs and are instead left open
    // for interpretation because even though this engine will probably only be used for this game,
    // I want to keep the engine purely independent.
    struct VoxelBase;

    template <class T>
    concept IsVoxel = 
        // A voxel class must inherit from VoxelBase for explicitness.
        std::is_base_of<VoxelBase, T>::value && 
        
        // A voxel must contain a constant that reads how much it promises to write and read from file buffers.
        std::same_as<decltype(T::SERIALIZED_BYTE_COUNT), std::size_t> &&

        // An important number to keep track of to prevent loading a file format where it is not supported version-wise.
        std::same_as<decltype(T::DATA_VERSION), std::size_t> &&

        requires(T t, const std::ofstream& stream) {
            { t.serialize(stream) } -> std::same_as<void>;
        } &&
        requires(T t, const std::ifstream& stream) {
            { t.deserialize(stream) } -> std::same_as<void>;
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
        static constexpr std::size_t SERIALIZED_BYTE_COUNT = CHUNK_VOLUME * Voxel::SERIALIZED_BYTE_COUNT;

        // Update every time there is a change to the chunk system that requires a new file format.
        static constexpr std::size_t DATA_VERSION = 0;
        
        util::CubeArray<Voxel, ChunkLength, CHUNK_LENGTH> voxels;

        // Flags dictate external behavior with the Chunk, not how the chunk works internally.
        // Can be modified by the engine or game layer depending on the specific flag.
        ChunkFlags flags;
    
        // Saves to a state in the form of bytes.
        void serialize(std::ofstream& buffer) const {
            for (const Voxel& voxel : voxels.items) {
                voxel.serialize(buffer);
            }
        }

        // Loads a saved state in the form of bytes.
        void deserialize(std::ifstream& stream) {
            for (Voxel& voxel : voxels) {
                voxel.deserialize(stream);
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
                stream.rdbuf()->pubsetbuf(buffer, FSTREAM_BUFFER_SIZE);
                stream.open(region_directory + '/' + region_pos.to_string(FILE_NAME_POS_DELIMITER));
            }

        const util::Vec3I region_pos;

        // Returns how many bytes forward from the start of the file a
        std::size_t find_or_make_chunk_registry(util::Vec3I chunk_pos) {

        }

        void write_chunk(util::Vec3I chunk_pos, Chunk<Voxel>& chunk) {

        }
    private:
        char buffer[FSTREAM_BUFFER_SIZE];
        std::fstream stream;
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