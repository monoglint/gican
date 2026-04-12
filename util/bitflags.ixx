/*

util:bitflags

an enum options utility that allows additional functionality to be applied to specific enums

specific example

    enum class _StorageSpecifierFlags : uint8_t {
        NONE        = 0,
        STATIC      = 1 << 0,
    };

    using StorageSpecifierFlags = util::BitFlags<_StorageSpecifierFlags>;

@monoglint
7 April 2026

*/

module;

#include <cstddef>
#include <type_traits>

export module util:bitflags;

export namespace util {
    template <typename T>
    concept IsBitFlagSafe = 
        std::is_enum_v<T> && 
        std::is_unsigned_v<std::underlying_type_t<T>> &&
        requires {
            static_cast<std::underlying_type_t<T>>(T::NONE) == 0;
        };

    template <IsBitFlagSafe Enum>
    class BitFlags {
    public:
        // for external access
        using Values = Enum;
        using UnderlyingType = std::underlying_type_t<Enum>;

        constexpr BitFlags(Enum value)
            : value(value)
        {}

        constexpr BitFlags()
            : value(Enum::NONE)
        {}
        
        Enum value;

        constexpr operator Enum() const {
            return value;
        }

        constexpr BitFlags operator~() const noexcept {
            return static_cast<Enum>(~static_cast<UnderlyingType>(value));
        }

        constexpr BitFlags operator|(const BitFlags& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) | static_cast<UnderlyingType>(other.value));
        }

        constexpr BitFlags operator|(const Enum& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) | static_cast<UnderlyingType>(other));
        }

        constexpr BitFlags operator&(const BitFlags& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) & static_cast<UnderlyingType>(other.value));
        }

        constexpr BitFlags operator&(const Enum& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) & static_cast<UnderlyingType>(other));
        }

        constexpr BitFlags operator^(const BitFlags& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) ^ static_cast<UnderlyingType>(other.value));
        }

        constexpr BitFlags operator^(const Enum& other) const noexcept {
            return static_cast<Enum>(static_cast<UnderlyingType>(value) ^ static_cast<UnderlyingType>(other));
        }

        constexpr BitFlags& operator^=(const BitFlags& other) noexcept {
            *this = *this ^ other;
            return *this;
        }

        constexpr BitFlags& operator^=(const Enum& other) noexcept {
            *this = *this ^ other;
            return *this;
        }

        constexpr BitFlags& operator&=(const BitFlags& other) noexcept {
            *this = *this & other;
            return *this;
        }

        constexpr BitFlags& operator&=(const Enum& other) noexcept {
            *this = *this & other;
            return *this;
        }

        constexpr BitFlags& operator|=(const BitFlags& other) noexcept {
            *this = *this | other;
            return *this;
        }

        constexpr BitFlags& operator|=(const Enum& other) noexcept {
            *this = *this | other;
            return *this;
        }

        constexpr void enable(const BitFlags& flag) const noexcept {
            *this |= flag;
        }

        constexpr bool has(const BitFlags& flag) const noexcept {
            return (*this & flag) == flag;
        }

        constexpr void toggle(const BitFlags& flag) const noexcept {
            *this ^= flag;
        }

        constexpr void disable(const BitFlags& flag) const noexcept {
            *this &= ~flag;
        }
    };
}