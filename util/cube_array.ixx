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
    template <typename InputNum, typename OutputNum, std::size_t CUBE_LENGTH>
    requires IsArithmetic<InputNum> && IsArithmetic<OutputNum>
    [[nodiscard]]
    constexpr Vec3<OutputNum> cube_index_to_pos(InputNum index) {
        return { index % CUBE_LENGTH, (index / CUBE_LENGTH) % CUBE_LENGTH, index / (CUBE_LENGTH * CUBE_LENGTH) }; 
    }

    template <typename InputNum, typename OutputNum, std::size_t CUBE_LENGTH>
    requires IsArithmetic<InputNum> && IsArithmetic<OutputNum>
    [[nodiscard]]
    constexpr OutputNum cube_pos_to_index(Vec3<InputNum> pos) {
        return (pos.x) + (pos.y * CUBE_LENGTH) + (pos.z * CUBE_LENGTH * CUBE_LENGTH);
    }

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
            return get_by_index(cube_pos_to_index<Length, Length, LENGTH>(pos));
        }

        [[nodiscard]]
        T& get_by_index(Length index) {
            panic_assert(index < VOLUME, "Attempted to access a non-existent item.", true);
            
            return items[index];
        }

        // available for raw access. is backed by c++ safety, but does not conform to the engine's safety mechanisms
        std::array<T, VOLUME> items;
    };
}