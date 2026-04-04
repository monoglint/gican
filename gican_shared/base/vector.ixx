module;

#include <cstddef>
#include <type_traits>
#include <cmath>

export module base:vector;

export namespace base {
    template <typename T>
    requires std::is_arithmetic_v<T>
    struct Vector3 {
        T x;
        T y;
        T z;

        Vector3 operator-() {
            x = -x;
            y = -y;
            z = -z;
        }

        Vector3 operator+(const Vector3& other) {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3 operator-(const Vector3& other) {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator*(const Vector3& other) {
            return Vector3(x * other.x, y * other.y, z * other.z);
        }

        Vector3 operator*(const T& scale) {
            return Vector3(x * scale, y * scale, z * scale);
        }

        Vector3 operator/(const Vector3& other) {
            return Vector3(x / other.x, y / other.y, z / other.z);
        }

        Vector3 operator/(const T& scale) {
            return Vector3(x / scale, y / scale, z / scale);
        }

        [[nodiscard]]
        double magnitude() const {
            return std::sqrtf(x * x + y * y + z * z);
        }

        [[nodiscard]]
        Vector3 unit() const {
            return *this / magnitude();
        }

        [[nodiscard]]
        T dot(const Vector3 other) const {
            return x * other.x + y * other.y + z * other.z;
        }
    };

    static_assert(sizeof(float) == 4 && sizeof(double) == 8, "Due to libc++ constraints, Vector3F32 and Vector3F64 requires the use of 'float' and 'double' which do not follow proper sizing requirements with the given architecture.");

    using Vector3F32 = Vector3<float>;
    using Vector3F64 = Vector3<double>;

    using Vector3Int8 = Vector3<int8_t>;
    using Vector3Int16 = Vector3<int16_t>;
    using Vector3Int32 = Vector3<int32_t>;
    using Vector3Int64 = Vector3<int64_t>;

    using Vector3UInt8 = Vector3<uint8_t>;
    using Vector3UInt16 = Vector3<uint16_t>;
    using Vector3UInt32 = Vector3<uint32_t>;
    using Vector3UInt64 = Vector3<uint64_t>;
};