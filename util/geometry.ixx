module;

#include <cstddef>
#include <type_traits>
#include <cmath>
#include <functional>

export module util:geometry;

import :hash;

export namespace util {
    template <typename T>
    requires std::is_arithmetic_v<T>
    struct Vec3 {
        T x;
        T y;
        T z;

        Vec3 operator-() {
            x = -x;
            y = -y;
            z = -z;
        }

        Vec3 operator+(const Vec3& other) {
            return Vec3(x + other.x, y + other.y, z + other.z);
        }

        Vec3 operator-(const Vec3& other) {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }

        Vec3 operator*(const Vec3& other) {
            return Vec3(x * other.x, y * other.y, z * other.z);
        }

        Vec3 operator*(const T& scale) {
            return Vec3(x * scale, y * scale, z * scale);
        }

        Vec3 operator/(const Vec3& other) {
            return Vec3(x / other.x, y / other.y, z / other.z);
        }

        Vec3 operator/(const T& scale) {
            return Vec3(x / scale, y / scale, z / scale);
        }

        Vec3 operator==(const Vec3& other) {
            return x == other.x && y == other.y && z == other.z;
        }

        Vec3 operator!=(const Vec3& other) {
            return !(*this == other);
        }

        [[nodiscard]]
        double magnitude() const {
            return std::sqrtf(x * x + y * y + z * z);
        }

        [[nodiscard]]
        Vec3 unit() const {
            return *this / magnitude();
        }

        [[nodiscard]]
        T dot(const Vec3 other) const {
            return x * other.x + y * other.y + z * other.z;
        }
    };

    static_assert(sizeof(float) == 4 && sizeof(double) == 8, "Due to libc++ constraints, Vec3F32 and Vec3F64 requires the use of 'float' and 'double' which do not follow proper sizing requirements with the given architecture.");

    using Vec3F32 = Vec3<float>;
    using Vec3F64 = Vec3<double>;

    using Vec3I8 = Vec3<int8_t>;
    using Vec3I16 = Vec3<int16_t>;
    using Vec3I32 = Vec3<int32_t>;
    using Vec3I64 = Vec3<int64_t>;

    using Vec3U8 = Vec3<uint8_t>;
    using Vec3U16 = Vec3<uint16_t>;
    using Vec3U32 = Vec3<uint32_t>;
    using Vec3U64 = Vec3<uint64_t>;

    using Vec3F = Vec3F64;
    using Vec3I = Vec3I64;
    using Vec3U = Vec3U64;
};

namespace std {
    template<typename T>
    struct hash<util::Vec3<T>> {
        std::size_t operator()(const util::Vec3<T>& vec) const {
            std::size_t x_hash = std::hash<T>(vec.x);
            std::size_t y_hash = std::hash<T>(vec.y);
            std::size_t z_hash = std::hash<T>(vec.z);

            util::combine_hashes(x_hash, y_hash);
            util::combine_hashes(x_hash, z_hash);
            return x_hash;
        }
    };
}