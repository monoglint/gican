module;

#include <cstddef>
#include <cstdint>

export module voxel:chunk;

import base;

export namespace voxel {
    // Data representing the live data of a placed block in a given location.
    struct Voxel {

    };

    using Pos = uint8_t;
    using Vector3Pos = base::Vector3<Pos>;

    template <std::size_t CHUNK_SIZE>
    struct Chunk {
        static constexpr std::size_t VOXEL_COUNT = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

        [[nodiscard]]
        Voxel& get_at(Vector3Pos pos) {
            base::panic_assert(pos.x < CHUNK_SIZE && pos.y < CHUNK_SIZE && pos.z < CHUNK_SIZE, "Attempted to access an out-of-bounds voxel.");
            
            std::size_t index = (pos.x) + (pos.y * CHUNK_SIZE) + (pos.z * CHUNK_SIZE * CHUNK_SIZE);

            // Use other method for its safety check to prevent UB.
            return get_by_index(index);
        }

        [[nodiscard]]
        Voxel& get_by_index(std::size_t index) {
            base::panic_assert(index < VOXEL_COUNT, "Attempted to access a non-existent voxel.");
            
            return voxels[index];
        }
    
    private:
        
        Voxel voxels[VOXEL_COUNT];
    };

    using StdChunk = Chunk<32>;
};