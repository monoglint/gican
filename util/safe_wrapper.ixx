/*

util:safe_wrapper

This class provides type safety to basic numeric or other primitive types to ensure that they are not interchanged where not appropriate.

e.g.
using ChunkPos = util::SafeWrapper<util::Vec3I, struct ChunkPosWrapTag>;

@monoglint
11 April 2026

*/

module;

#include <concepts>
#include <utility>

export module util:safe_wrapper;

export namespace util {
    template <class UniqueTag, typename T>
    class SafeWrapper {
    public:
        using Underlying = T;
        using Tag = UniqueTag;

        template <typename _T>
        requires std::convertible_to<_T, T>
        explicit constexpr SafeWrapper(_T underlying)
            : underlying(underlying)
        {}

        [[nodiscard]]
        constexpr T& get() {
            return underlying;
        }

        [[nodiscard]]
        constexpr const T& get() const {
            return underlying;
        }

        constexpr SafeWrapper operator+(const SafeWrapper& other) const {
            return SafeWrapper{get() + other.get()};
        }

        constexpr SafeWrapper operator-(const SafeWrapper& other) const {
            return SafeWrapper{get() - other.get()};
        }

        constexpr SafeWrapper operator*(const SafeWrapper& other) const {
            return SafeWrapper{get() * other.get()};
        }

        constexpr SafeWrapper operator/(const SafeWrapper& other) const {
            return SafeWrapper{get() / other.get()};
        }

        constexpr bool operator<=>(const SafeWrapper& other) const = default;
    private:
        T underlying;
    };
}

namespace std {
    template <class Tag, typename T>
    requires requires (const T& t) {
        { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
    }
    struct hash<util::SafeWrapper<Tag, T>> {
        std::size_t operator()(const util::SafeWrapper<Tag, T>& obj) const noexcept {
            return std::hash<T>{}(obj.get());
        }
    };
}