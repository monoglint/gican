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
    // Designed in mind of graphics programming - customizable types for the given external purpose of the CubeArray.
    template <typename T, typename Length, Length LENGTH>
    requires IsArithmetic<Length>
    class CubeArray {
    public:
        static constexpr Length VOLUME = LENGTH * LENGTH * LENGTH;

        [[nodiscard]]
        T& get_by_pos(Vec3<Length> pos) {
            panic_assert(pos.x < LENGTH && pos.y < LENGTH && pos.z < LENGTH, "Attempted to access an out-of-bounds item.", true);
            // Use other method for its safety check to prevent UB.
            return get_by_index(pos_to_index(pos));
        }

        [[nodiscard]]
        T& get_by_index(Length index) {
            panic_assert(index < VOLUME, "Attempted to access a non-existent item.", true);
            
            return items[index];
        }

        [[nodiscard]]
        Vec3<Length> index_to_pos(Length index) const {
            return { index % LENGTH, (index / LENGTH) % LENGTH, index / (LENGTH * LENGTH) }; 
        }

        [[nodiscard]]
        Length pos_to_index(Vec3<Length> pos) const {
            return (pos.x) + (pos.Y * LENGTH) + (pos.z * LENGTH * LENGTH);
        }

        // available for raw access. is backed by c++ safety, but does not conform to the engine's safety mechanisms
        std::array<T, VOLUME> items;
    };
}