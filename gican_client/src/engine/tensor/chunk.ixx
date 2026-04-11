module;

#include <type_traits>
#include <concepts>
#include <cstddef>
#include <vector>

export module engine.tensor:chunk;

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

        requires(T t, std::byte* buffer) {
            { t.serialize(buffer) } -> std::same_as<void>;
        } &&
        requires(T t, const std::byte* buffer) {
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

            /// @todo @note update for pallating support. replace with a method that gets the size based on the number of voxel type entries being pallated
            static constexpr std::size_t SERIALIZED_SIZE = CHUNK_VOLUME * Voxel::SERIALIZED_SIZE;

            using AsSerialzied = std::vector<std::byte>;

        // Update every time there is a change to the chunk system that requires a new file format.
        static constexpr std::size_t DATA_VERSION = 0;

        Chunk(AsSerialzied& serialized_form) {
            deserialize(serialized_form);
        }
        
        /// @todo Make internals for voxels a pallated system.
        util::CubeArray<Voxel, ChunkLength, CHUNK_LENGTH> voxels;

        // Flags dictate external behavior with the Chunk, not how the chunk works internally.
        // Can be modified by the engine or game layer depending on the specific flag.
        ChunkFlags flags;
    
        AsSerialzied serialize() const {
            AsSerialzied buffer;
            buffer.resize(SERIALIZED_SIZE); // 

            std::byte* ptr = buffer.data();
            for (const Voxel& voxel : voxels.items) {
                voxel.serialize(ptr);
                ptr += Voxel::SERIALIZED_SIZE;
            }

            return buffer;
        }

        void deserialize(AsSerialzied& serialized_form) {
            const std::byte* ptr = serialized_form.data();
            for (Voxel& voxel : voxels) {
                voxel.deserialize(ptr);
                ptr += Voxel::SERIALIZED_SIZE;
            }
        }
    };
}