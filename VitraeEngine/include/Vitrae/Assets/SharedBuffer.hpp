#pragma once

#include "Vitrae/ComponentRoot.h"
#include "Vitrae/Util/NonCopyable.h"

#include "dynasma/keepers/abstract.hpp"
#include "dynasma/managers/abstract.hpp"
#include "dynasma/pointer.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace Vitrae
{
class Renderer;

using BufferUsageHints = std::uint8_t;
struct BufferUsageHint
{
    static constexpr BufferUsageHints HOST_INIT = 1 << 0;
    static constexpr BufferUsageHints HOST_WRITE = 1 << 1;
    static constexpr BufferUsageHints HOST_READ = 1 << 2;
    static constexpr BufferUsageHints GPU_DRAW = 1 << 3;
    static constexpr BufferUsageHints GPU_COMPUTE = 1 << 4;
};

/**
 * A shared buffer is a data storage object that can easily be shared between CPU and GPU.
 * The raw shared buffer can be used to store any type of data.
 */
class RawSharedBuffer : public dynasma::PolymorphicBase
{
  public:
    struct SetupParams
    {
        Renderer &renderer;
        ComponentRoot &root;
        BufferUsageHints usage = BufferUsageHint::HOST_INIT | BufferUsageHint::GPU_DRAW;
        std::size_t size = 0;
    };

    RawSharedBuffer();
    virtual ~RawSharedBuffer() = 0;

    virtual void synchronize() = 0;
    virtual std::size_t memory_cost() const = 0;

    void resize(std::size_t size);

    inline std::size_t size() const { return m_size; }
    const Byte *data() const;
    Byte *data();

    std::span<const Byte> operator[](std::pair<std::size_t, std::size_t> slice) const;
    std::span<Byte> operator[](std::pair<std::size_t, std::size_t> slice);

  protected:
    /// @brief The current specified size of the buffer
    std::size_t m_size;
    /// @brief A pointer to the underlying buffer. nullptr if it needs to be requested
    mutable Byte *m_bufferPtr;
    /// @brief  The range of the buffer that needs to be synchronized
    mutable std::pair<std::size_t, std::size_t> m_dirtySpan;

    /**
     * @brief This function is called only while m_bufferPtr is nullptr, and has to set m_bufferPtr
     * to valid m_size long span of memory
     */
    virtual void requestBufferPtr() const = 0;
    /**
     * @brief This function is called before m_size gets changed, and ensures m_bufferPtr is
     * either nullptr or points to a valid m_size long span of memory
     */
    virtual void requestResizeBuffer(std::size_t size) const = 0;
};

struct RawSharedBufferKeeperSeed
{
    using Asset = RawSharedBuffer;
    std::variant<RawSharedBuffer::SetupParams> kernel;
    inline std::size_t load_cost() const { return 1; }
};

using RawSharedBufferKeeper = dynasma::AbstractKeeper<RawSharedBufferKeeperSeed>;

/**
 * A SharedBufferPtr is used to access a shared buffer, with a safer underlying type.
 * @tparam HeaderT the type stored at the start of the buffer. Use void if there is no header.
 * @tparam ElementT the type stored in the array after the header. Use void if there is no FAM
 */
template <class HeaderT, class ElementT> class SharedBufferPtr
{
    static constexpr bool HAS_HEADER = !std::is_same_v<HeaderT, void>;
    static constexpr bool HAS_FAM_ELEMENTS = !std::is_same_v<ElementT, void>;

    // Location of the FAM adjusted for alignment
    static constexpr std::ptrdiff_t getFirstElementOffset()
        requires HAS_FAM_ELEMENTS
    {
        if constexpr (HAS_HEADER)
            return ((sizeof(HeaderT) - 1) / alignof(ElementT) + 1) * alignof(ElementT);
        else
            return 0;
    }

  public:
    static constexpr std::size_t calcMinimumBufferSize(std::size_t numElements)
        requires HAS_FAM_ELEMENTS
    {
        if constexpr (HAS_HEADER)
            return getFirstElementOffset() + sizeof(ElementT) * numElements;
        else
            return sizeof(ElementT) * numElements;
    }
    static constexpr std::size_t calcMinimumBufferSize()
    {
        if constexpr (HAS_HEADER)
            return sizeof(HeaderT);
        else
            return 0;
    }

    /**
     * Constructs a SharedBufferPtr with a new RawSharedBuffer allocated from the Keeper in the root
     * with enough size for the HeaderT
     */
    SharedBufferPtr(ComponentRoot &root, BufferUsageHint usage)
        : m_buffer(root.getComponent<RawSharedBufferKeeper>().new_asset(RawSharedBufferKeeperSeed{
              .kernel = RawSharedBuffer::SetupParams{.renderer = root.getComponent<Renderer>(),
                                                     .root = root,
                                                     .usage = usage,
                                                     .size = calcMinimumBufferSize()}}))
    {}

    /**
     * Constructs a SharedBufferPtr with a new RawSharedBuffer allocated from the Keeper in the root
     * with the specified number of elements
     */
    SharedBufferPtr(ComponentRoot &root, BufferUsageHint usage, std::size_t numElements)
        requires HAS_FAM_ELEMENTS
        : m_buffer(root.getComponent<RawSharedBufferKeeper>().new_asset(RawSharedBufferKeeperSeed{
              .kernel = RawSharedBuffer::SetupParams{.renderer = root.getComponent<Renderer>(),
                                                     .root = root,
                                                     .usage = usage,
                                                     .size = calcMinimumBufferSize(numElements)}}))
    {}

    /**
     * Constructs a SharedBufferPtr from a RawSharedBuffer FirmPtr
     */
    SharedBufferPtr(dynasma::FirmPtr<RawSharedBuffer> buffer) : m_buffer(buffer) {}

    /**
     * Resizes the underlying RawSharedBuffer to contain the given number of FAM elements
     */
    void resizeElements(std::size_t numElements)
        requires HAS_FAM_ELEMENTS
    {
        m_buffer->resize(calcMinimumBufferSize(numElements));
    }

    /**
     * @returns the number of FAM elements in the underlying RawSharedBuffer
     */
    std::size_t byteSize() const { return m_buffer->size(); }

    /**
     * @returns the number of FAM elements in the underlying RawSharedBuffer
     */
    std::size_t numElements() const
        requires HAS_FAM_ELEMENTS
    {
        return (m_buffer->size() - getFirstElementOffset()) / sizeof(ElementT);
    }

    /**
     * @returns The header of the buffer
     */
    const HeaderT &getHeader() const
        requires HAS_HEADER
    {
        return *reinterpret_cast<const HeaderT *>(m_buffer->data());
    }
    HeaderT &getHeader()
        requires HAS_HEADER
    {
        return *reinterpret_cast<HeaderT *>((*m_buffer)[{0, sizeof(HeaderT)}].data());
    }

    /**
     * @returns The FAM element at the given index
     */
    const ElementT &getElement(std::size_t index) const
        requires HAS_FAM_ELEMENTS
    {
        return *reinterpret_cast<const ElementT *>(m_buffer->data() + getFirstElementOffset() +
                                                   sizeof(ElementT) * index);
    }
    ElementT &getElement(std::size_t index)
        requires HAS_FAM_ELEMENTS
    {
        return *reinterpret_cast<ElementT *>(
            (*m_buffer)[{getFirstElementOffset() + sizeof(ElementT) * index,
                         getFirstElementOffset() + sizeof(ElementT) * index + sizeof(ElementT)}]
                .data());
    }

    /**
     * @returns the underlying RawSharedBuffer, type agnostic
     */
    dynasma::FirmPtr<const RawSharedBuffer> getRawBuffer() const { return m_buffer; }

  protected:
    dynasma::FirmPtr<RawSharedBuffer> m_buffer;
};

} // namespace Vitrae