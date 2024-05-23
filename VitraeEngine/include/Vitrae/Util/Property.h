#pragma once

#include <any>
#include <sstream>
#include <stdexcept>
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
};
} // namespace Vitrae