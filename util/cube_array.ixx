module;

#include <cstddef>
#include <array>

export module util:cube_array;

import :geometry;
import :panic;

// c++ hates me using std::is_arithmetic for some reason at the moment
template <typename T>
concept IsArithmetic = requires(T x) { x + x; };

export namespace util {
    // Designed in mind of graphics programming.
    template <typename T, typename Size, Size SIZE>
    requires IsArithmetic<Size>
    class CubeArray {
    public:
        [[nodiscard]]
        T& get_by_pos(Vec3<Size> pos) {
            panic_assert(pos.x < SIZE && pos.y < SIZE && pos.z < SIZE, "Attempted to access an out-of-bounds item.", true);
            // Use other method for its safety check to prevent UB.
            return get_by_index(pos_to_index(pos));
        }

        [[nodiscard]]
        T& get_by_index(Size index) {
            panic_assert(index < SIZE, "Attempted to access a non-existent item.", true);
            
            return items[index];
        }

        [[nodiscard]]
        Vec3<Size> index_to_pos(Size index) const {
            return { index % SIZE, (index / SIZE) % SIZE, index / (SIZE * SIZE) }; 
        }

        [[nodiscard]]
        Size pos_to_index(Vec3<Size> pos) const {
            return (pos.x) + (pos.Y * SIZE) + (pos.z * SIZE * SIZE);
        }

        // available for raw access. is backed by c++ safety, but does not conform to the engine's safety mechanisms
        std::array<T, SIZE> items;
    };
}