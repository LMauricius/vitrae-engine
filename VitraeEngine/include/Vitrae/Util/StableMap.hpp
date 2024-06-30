#pragma once

#include <cstddef>
#include <map>
#include <utility>

namespace Vitrae
{

/**
 * @brief A map that expects its keys to be searched through often, but rarely changed
 * @note Modifying the values is fast
 *
 * @tparam KeyT the type of the key
 * @tparam MappedT the type of the value
 * @tparam CompT the comparison functor type
 */
template <class KeyT, class MappedT> class StableMap
{
    class StableMapIterator;
    class CStableMapIterator;

  public:
    using key_type = KeyT;
    using mapped_type = MappedT;
    using value_type = std::pair<const key_type &, mapped_type &>;
    using const_value_type = std::pair<const key_type &, const mapped_type &>;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = std::allocator<std::byte>;

    using pointer = std::pair<const key_type *, mapped_type *>;
    using const_pointer = std::pair<const key_type *, const mapped_type *>;
    using reference = std::pair<const key_type &, mapped_type &>;
    using const_reference = std::pair<const key_type &, const mapped_type &>;
    using iterator = StableMapIterator;
    using const_iterator = CStableMapIterator;

    StableMap() : m_data(nullptr), m_size(0){};

    StableMap(const StableMap &o)
    {
        m_size = o.m_size;
        m_data = new std::byte[getBufferSize(m_size)];
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);
        const KeyT *oKeyList = reinterpret_cast<const KeyT *>(o.m_data);
        m_valueList = reinterpret_cast<MappedT *>(m_data + getValueBufferOffset(m_size));
        for (std::size_t i = 0; i < m_size; ++i) {
            new (keyList + i) KeyT(oKeyList[i]);
            new (m_valueList + i) MappedT(o.m_valueList[i]);
        }
    }

    StableMap(StableMap &&o) : m_data(o.m_data), m_size(o.m_size), m_valueList(o.m_valueList)
    {
        o.m_size = 0;
        o.m_data = nullptr;
    }

    template <class InputItT>
    StableMap(InputItT first, InputItT last)
        requires requires(InputItT it) {
            { (*it).first } -> std::convertible_to<KeyT>;
            { (*it).second } -> std::convertible_to<MappedT>;
        }
    {
        m_size = std::distance(first, last);
        m_data = new std::byte[getBufferSize(m_size)];
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);
        m_valueList = reinterpret_cast<MappedT *>(m_data + getValueBufferOffset(m_size));

        int i = 0;
        for (const auto &keyVal : container) {
            new (keyList + i) KeyT(keyVal.first);
            new (m_valueList + i) MappedT(keyVal.second);
            ++i;
        }
    }

    template <class OKeyT, class OMappedT>
    StableMap(std::map<OKeyT, OMappedT> orderedList)
        requires std::convertible_to<OKeyT, KeyT> && std::convertible_to<OMappedT, MappedT>
    {
        m_size = list.size();
        m_data = new std::byte[getBufferSize(m_size)];
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);
        m_valueList = reinterpret_cast<MappedT *>(m_data + getValueBufferOffset(m_size));

        int i = 0;
        for (const auto &keyVal : list) {
            new (keyList + i) KeyT(keyVal.first);
            new (m_valueList + i) MappedT(keyVal.second);
            ++i;
        }
    }

    ~StableMap()
    {
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);
        for (std::size_t i = 0; i < m_size; ++i) {
            keyList[i].~KeyT();
            m_valueList[i].~MappedT();
        }
        delete[] m_data;
    }

    StableMap &operator=(const StableMap &o)
    {
        m_size = o.m_size;
        m_data = new std::byte[getBufferSize(m_size)];
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);
        const KeyT *oKeyList = reinterpret_cast<const KeyT *>(o.m_data);
        m_valueList = reinterpret_cast<MappedT *>(m_data + getValueBufferOffset(m_size));
        for (std::size_t i = 0; i < m_size; ++i) {
            new (keyList + i) KeyT(oKeyList[i]);
            new (m_valueList + i) MappedT(o.m_valueList[i]);
        }
    }
    StableMap &operator=(StableMap &&o)
    {
        m_size = o.m_size;
        m_data = o.m_data;
        m_valueList = o.m_valueList;
        o.m_size = 0;
        o.m_data = nullptr;
        return *this;
    }

    std::size_t size() const { return m_size; }
    std::span<const KeyT> keys() const { return std::span<const KeyT>(getKeyList(), m_size); }
    std::span<MappedT> values() { return std::span<MappedT>(m_valueList, m_size); }
    std::span<const MappedT> values() const
    {
        return std::span<const MappedT>(m_valueList, m_size);
    }

    StableMapIterator begin() { return StableMapIterator(getKeyList(), m_valueList); }
    StableMapIterator end()
    {
        return StableMapIterator(getKeyList() + m_size, m_valueList + m_size);
    }
    CStableMapIterator begin() const { return CStableMapIterator(getKeyList(), m_valueList); }
    CStableMapIterator end() const
    {
        return CStableMapIterator(getKeyList() + m_size, m_valueList + m_size);
    }
    CStableMapIterator cbegin() const { return CStableMapIterator(getKeyList(), m_valueList); }
    CStableMapIterator cend() const
    {
        return CStableMapIterator(getKeyList() + m_size, m_valueList + m_size);
    }

    MappedT &operator[](const KeyT &key)
    {
        std::size_t ind = findClosestIndex(key);
        if (getKeyList()[ind] == key) {
            return m_valueList[ind];
        } else {
            realloc_w_uninit(ind);
            new (getKeyList() + ind) KeyT(key);
            new (m_valueList + ind) MappedT();
            return m_valueList[ind];
        }
    }
    const MappedT &operator[](const KeyT &key) const { return at(key); }

    MappedT &at(const KeyT &key)
    {
        std::size_t ind = findClosestIndex(key);
        if (getKeyList()[ind] == key) {
            return m_valueList[ind];
        }
        throw std::out_of_range("Key not found");
    }

    const MappedT &at(const KeyT &key) const
    {
        std::size_t ind = findClosestIndex(key);
        if (getKeyList()[ind] == key) {
            return m_valueList[ind];
        }
        throw std::out_of_range("Key not found");
    }

    std::pair<iterator, bool> emplace(const KeyT &key, const MappedT &value)
    {
        std::size_t ind = findClosestIndex(key);
        if (getKeyList()[ind] == key) {
            return std::make_pair(iterator(keyList + ind, m_valueList + ind), false);
        } else {
            realloc_w_uninit(ind);

            // insert
            new (getKeyList() + ind) KeyT(key);
            new (m_valueList + ind) MappedT(value);

            return std::make_pair(iterator(getKeyList() + ind, m_valueList + ind), true);
        }
    }

    std::size_t erase(const KeyT &key)
    {
        std::size_t ind = findClosestIndex(key);
        if (getKeyList()[ind] == key) {
            realloc_w_erased(ind);
            return 1;
        } else {
            return 0;
        }
    }

  protected:
    std::size_t m_size;
    std::byte *m_data;    // starts with keys, also contains values. Owned by the map
    MappedT *m_valueList; // pointer to a portion of m_data buffer

    static constexpr std::size_t getValueBufferOffset(std::size_t numElements)
    {
        return (alignof(MappedT) < alignof(KeyT))
                   ? numElements * sizeof(KeyT)
                   : (numElements * sizeof(KeyT) + alignof(MappedT) - 1) / alignof(MappedT) *
                         alignof(MappedT);
    }

    static constexpr std::size_t getBufferSize(std::size_t numElements)
    {
        return getValueBufferOffset(numElements) + numElements * sizeof(MappedT);
    }

    std::size_t findClosestIndex(const KeyT &key) const
    {
        // uses binary search to find the index of the closest key
        const KeyT *keyList = getKeyList();
        std::size_t leftIndex = 0;
        std::size_t rightIndex = m_size;
        while (leftIndex < rightIndex) {
            std::size_t midIndex = (leftIndex + rightIndex) / 2;
            if (key < keyList[midIndex]) {
                rightIndex = midIndex;
            } else {
                leftIndex = midIndex + 1;
            }
        }
        return leftIndex;
    }

    const KeyT *getKeyList() const { return reinterpret_cast<const KeyT *>(m_data); }

    void realloc_buf(std::size_t newSize)
    {
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);

        std::byte *newData = new std::byte[getBufferSize(newSize)];
        KeyT *newKeyList = reinterpret_cast<KeyT *>(newData);
        MappedT *newValueList =
            reinterpret_cast<MappedT *>(newData + getValueBufferOffset(newSize));

        if (newSize > m_size) {
            for (std::size_t i = 0; i < m_size; ++i) {
                new (newKeyList + i) KeyT(keyList[i]);
                new (newValueList + i) MappedT(m_valueList[i]);
                keyList[i].~KeyT();
                m_valueList[i].~MappedT();
            }
            for (std::size_t i = m_size; i < newSize; ++i) {
                new (newKeyList + i) KeyT();
                new (newValueList + i) MappedT();
            }
        } else {
            for (std::size_t i = 0; i < newSize; ++i) {
                new (newKeyList + i) KeyT(keyList[i]);
                new (newValueList + i) MappedT(m_valueList[i]);
                keyList[i].~KeyT();
                m_valueList[i].~MappedT();
            }
            for (std::size_t i = newSize; i < m_size; ++i) {
                keyList[i].~KeyT();
                m_valueList[i].~MappedT();
            }
        }

        delete[] m_data;
        m_data = newData;
        m_valueList = newValueList;
        m_size = newSize;
    }

    void realloc_w_erased(difference_type erasingIndex)
    {
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);

        std::byte *newData = new std::byte[getBufferSize(m_size - 1)];
        KeyT *newKeyList = reinterpret_cast<KeyT *>(newData);
        MappedT *newValueList =
            reinterpret_cast<MappedT *>(newData + getValueBufferOffset(m_size - 1));

        // move data before
        for (std::size_t i = 0; i < erasingIndex; ++i) {
            new (newKeyList + i) KeyT(std::move(keyList[i]));
            new (newValueList + i) MappedT(std::move(m_valueList[i]));
            keyList[i].~KeyT();
            m_valueList[i].~MappedT();
        }

        // erase
        keyList[erasingIndex].~KeyT();
        m_valueList[erasingIndex].~MappedT();

        // move data after
        for (std::size_t i = erasingIndex + 1; i < m_size; ++i) {
            new (newKeyList + i - 1) KeyT(std::move(keyList[i]));
            new (newValueList + i - 1) MappedT(std::move(m_valueList[i]));
            keyList[i].~KeyT();
            m_valueList[i].~MappedT();
        }

        delete[] m_data;
        m_data = newData;
        m_valueList = newValueList;
        --m_size;
    }

    void realloc_w_uninit(difference_type uninitIndex)
    {
        KeyT *keyList = reinterpret_cast<KeyT *>(m_data);

        std::byte *newData = new std::byte[getBufferSize(m_size + 1)];
        KeyT *newKeyList = reinterpret_cast<KeyT *>(newData);
        MappedT *newValueList =
            reinterpret_cast<MappedT *>(newData + getValueBufferOffset(m_size + 1));

        // move data before
        for (std::size_t i = 0; i < uninitIndex; ++i) {
            new (newKeyList + i) KeyT(std::move(keyList[i]));
            new (newValueList + i) MappedT(std::move(m_valueList[i]));
            keyList[i].~KeyT();
            m_valueList[i].~MappedT();
        }

        // move data after
        for (std::size_t i = uninitIndex; i < m_size; ++i) {
            new (newKeyList + i + 1) KeyT(std::move(keyList[i]));
            new (newValueList + i + 1) MappedT(std::move(m_valueList[i]));
            keyList[i].~KeyT();
            m_valueList[i].~MappedT();
        }

        delete[] m_data;
        m_data = newData;
        m_valueList = newValueList;
        ++m_size;
    }

    class StableMapIterator
    {
        const key_type *mp_key;
        mapped_type *mp_value;

      public:
        using iterator_category = std::bidirectional_iterator_tag;

        using StableMap::difference_type;
        using StableMap::pointer;
        using StableMap::reference;
        using StableMap::value_type;

        StableMapIterator() : mp_key(nullptr), mp_value(nullptr) {}
        StableMapIterator(const key_type *key, mapped_type *value) : mp_key(key), mp_value(value) {}
        StableMapIterator(const StableMapIterator &) = default;
        StableMapIterator(StableMapIterator &&) = default;

        auto operator=(const StableMapIterator &other) = default;
        auto operator=(StableMapIterator &&other) = default;

        auto operator++()
        {
            mp_key++;
            mp_value++;
            return *this;
        }
        auto operator--()
        {
            mp_key--;
            mp_value--;
            return *this;
        }

        auto operator==(const StableMapIterator &other) const { return mp_key == other.mp_key; }
        auto operator<=>(const StableMapIterator &other) const { return mp_key <=> other.mp_key; }

        auto operator*() const { return value_type(*mp_key, *mp_value); }
        auto operator->() const { return value_type(*mp_key, *mp_value); }
    };

    class CStableMapIterator
    {
        const key_type *mp_key;
        const mapped_type *mp_value;

      public:
        using iterator_category = std::bidirectional_iterator_tag;

        using StableMap::difference_type;
        using pointer = StableMap::const_pointer;
        using reference = StableMap::const_reference;
        using value_type = StableMap::const_value_type;

        CStableMapIterator() : mp_key(nullptr), mp_value(nullptr) {}
        CStableMapIterator(const key_type *key, const mapped_type *value)
            : mp_key(key), mp_value(value)
        {}
        CStableMapIterator(const CStableMapIterator &) = default;
        CStableMapIterator(CStableMapIterator &&) = default;

        auto operator=(const CStableMapIterator &other) = default;
        auto operator=(CStableMapIterator &&other) = default;

        auto operator++()
        {
            mp_key++;
            mp_value++;
            return *this;
        }
        auto operator--()
        {
            mp_key--;
            mp_value--;
            return *this;
        }

        auto operator==(const CStableMapIterator &other) const { return mp_key == other.mp_key; }
        auto operator<=>(const CStableMapIterator &other) const { return mp_key <=> other.mp_key; }

        auto operator*() const { return value_type(*mp_key, *mp_value); }
        auto operator->() const { return value_type(*mp_key, *mp_value); }
    };
};

} // namespace Vitrae