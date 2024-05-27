#pragma once

#include <any>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace Vitrae
{
using PropertySpec = std::type_info;

/**
 * @brief A class that allows you to store and retrieve properties of any type.
 * @note Functionally similar to std::any, except with dynamic comparison method support
 */
class Property
{

    /**
     * A structure that contains function pointers to compare and convert properties.
     * Used to implement type-specific functionality in a generic way.
     * Similar to a vtable, except for any type
     */
    struct PropertyFuncTable
    {
        bool (*isEqual)(const Property &lhs, const Property &rhs);
        bool (*isLessThan)(const Property &lhs, const Property &rhs);
        bool (*toBool)(const Property &p);
        std::string (*toString)(const Property &p);
        std::size_t (*hash)(const Property &p);
    };

    /**
     * @brief Creates a PropertyFuncTable with the function pointers for the specified type.
     */
    template <class T> static constexpr PropertyFuncTable makeTable()
    {
        PropertyFuncTable table;

        // operator==
        if constexpr (requires(T lhs, T rhs) {
                          { lhs == rhs } -> std::convertible_to<bool>;
                      })
        {
            table.isEqual = [](const Property &lhs, const Property &rhs) -> bool {
                return (bool)(std::any_cast<T>(lhs.m_val) == std::any_cast<T>(rhs.m_val));
            };
        }
        else
        {
            table.isEqual = [](const Property &lhs, const Property &rhs) -> bool {
                std::stringstream ss;
                ss << "operator== is not implemented for type " << typeid(T).name();
                throw std::runtime_error(ss.str());
            };
        }

        // operator<
        if constexpr (requires(T lhs, T rhs) {
                          { lhs < rhs } -> std::convertible_to<bool>;
                      })
        {
            table.isLessThan = [](const Property &lhs, const Property &rhs) -> bool {
                return (bool)(std::any_cast<T>(lhs.m_val) < std::any_cast<T>(rhs.m_val));
            };
        }
        else
        {
            table.isLessThan = [](const Property &lhs, const Property &rhs) -> bool {
                std::stringstream ss;
                ss << "operator< is not implemented for type " << typeid(T).name();
                throw std::runtime_error(ss.str());
            };
        }

        // operator (bool)
        if constexpr (requires(T v) {
                          { bool(v) } -> std::convertible_to<bool>;
                      })
        {
            table.toBool = [](const Property &p) -> bool {
                return (bool)(std::any_cast<T>(p.m_val));
            };
        }
        else
        {
            table.toBool = [](const Property &p) -> bool {
                std::stringstream ss;
                ss << "operator bool is not implemented for type " << typeid(T).name();
                throw std::runtime_error(ss.str());
            };
        }

        // operator (std::string)
        if constexpr (requires(T v, std::stringstream ss) {
                          { ss << v };
                      })
        {
            table.toString = [](const Property &p) -> std::string {
                std::stringstream ss;
                ss << std::any_cast<T>(p.m_val);
                return ss.str();
            };
        }
        else
        {
            table.toString = [](const Property &p) -> std::string {
                std::stringstream ss;
                ss << "operator std::string is not implemented for type " << typeid(T).name();
                throw std::runtime_error(ss.str());
            };
        }

        // hash function
        if constexpr (requires(T v) {
                          { std::hash<T>{}(v) } -> std::convertible_to<std::size_t>;
                      })
        {
            table.hash = [](const Property &p) -> std::size_t {
                return std::hash<T>{}(std::any_cast<T>(p.m_val));
            };
        }
        else
        {
            table.hash = [](const Property &p) -> std::size_t {
                std::stringstream ss;
                ss << "std::hash is not implemented for type " << typeid(T).name();
                throw std::runtime_error(ss.str());
            };
        }

        return table;
    }

    /**
     * @returns a compile-time reference to the PropertyFuncTable for the specified type.
     */
    template <class T> static const PropertyFuncTable &getTableConstant()
    {
        static constexpr PropertyFuncTable table = makeTable<T>();
        return table;
    }

    /**
     * Underlying storage for any type
     */
    std::any m_val;
    /**
     * Pointer to the function table
     */
    const PropertyFuncTable *m_table;

  public:
    Property() : m_val(), m_table(&getTableConstant<void>())
    {
    }

    template <class T>
    Property(T val) : m_val(std::decay(val)), m_table(&getTableConstant<std::decay_t<T>>())
    {
    }
    inline Property(const Property &other) : m_val(other.m_val), m_table(other.m_table)
    {
    }
    inline Property(Property &&other) : m_val(std::move(other.m_val)), m_table(other.m_table)
    {
    }

    template <class T> Property &operator=(T val)
    {
        m_val = val;
        m_table = &getTableConstant<std::decay_t<T>>();
        return *this;
    }
    inline Property &operator=(const Property &other)
    {
        m_val = other.m_val;
        m_table = other.m_table;
        return *this;
    }
    inline Property &operator=(Property &&other)
    {
        m_val = std::move(other.m_val);
        m_table = other.m_table;
        return *this;
    }

    /**
     * @tparam T The type to retrieve the value as.
     * @return The value stored in the `std::any` object as type `T`.
     * @throws std::bad_any_cast If the stored value isn't of exact type `T`.
     */
    template <class T> T get() const
    {
        return std::any_cast<T>(m_val);
    }

    /**
     * @param other The Property object to compare with.
     * @return Whether the two Property objects are equal.
     * @throws std::bad_any_cast If the two Property objects have different types.
     * @throws std::runtime_error If the stored type is not comparable.
     */
    inline bool operator==(const Property &other) const
    {
        return m_table->isEqual(*this, other);
    }
    /**
     * @param other The Property object to compare with.
     * @return Whether the first Property object is less than the second.
     * @throws std::bad_any_cast If the two Property objects have different types.
     * @throws std::runtime_error If the stored type is not comparable.
     */
    inline bool operator<(const Property &other) const
    {
        return m_table->isLessThan(*this, other);
    }
    /**
     * @return Whether the stored value is truthy.
     * @throws std::runtime_error If the stored type is not convertible to bool.
     */
    inline operator bool() const
    {
        return m_table->toBool(*this);
    }
    /**
     * @returns A string representation of the stored value.
     * @throws std::runtime_error If the stored type is not convertible to std::string.
     */
    inline std::string toString() const
    {
        return m_table->toString(*this);
    }
    /**
     * @returns A hash value for the stored value.
     * @throws std::runtime_error If the stored type is not hashable.
     */
    inline std::size_t hash() const
    {
        return m_table->hash(*this) ^ (std::size_t)m_table;
    }
};

} // namespace Vitrae

namespace std
{
template <> struct hash<Vitrae::Property>
{
    size_t operator()(const Vitrae::Property &x) const
    {
        return x.hash();
    }
};
} // namespace std