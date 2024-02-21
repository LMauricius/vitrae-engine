#pragma once

#include <memory>

namespace Vitrae
{

    /**
     * @brief a class that acts like a unique ptr to void
     */
    class UniqueAnyPtr
    {
        void *mPtr;
        void (*mDeleter)(void *);

    public:
        // constructors
        template <class T>
        UniqueAnyPtr(T *ptr)
        {
            mPtr = ptr;
            mDeleter = [](void *ptr)
            {
                delete static_cast<T *>(ptr);
            };
        }

        template <class T>
        UniqueAnyPtr(std::unique_ptr<T> &&ptr)
        {
            mPtr = ptr.release();
            mDeleter = [](void *ptr)
            {
                delete static_cast<T *>(ptr);
            };
        }

        // move constructor
        UniqueAnyPtr(UniqueAnyPtr &&other)
        {
            mPtr = other.mPtr;
            mDeleter = other.mDeleter;
            other.mPtr = nullptr;
            other.mDeleter = nullptr;
        }

        ~UniqueAnyPtr()
        {
            if (mPtr != nullptr)
                (*mDeleter)(mPtr);
        }

        // move assignment
        template <class T>
        UniqueAnyPtr &operator=(T *ptr)
        {
            mPtr = ptr;
            mDeleter = [](void *ptr)
            {
                delete static_cast<T *>(ptr);
            };
            return *this;
        }

        template <class T>
        UniqueAnyPtr &operator=(std::unique_ptr<T> &&ptr)
        {
            mPtr = ptr.release();
            mDeleter = [](void *ptr)
            {
                delete static_cast<T *>(ptr);
            };
            return *this;
        }

        UniqueAnyPtr &operator=(UniqueAnyPtr &&other)
        {
            mPtr = other.mPtr;
            mDeleter = other.mDeleter;
            other.mPtr = nullptr;
            other.mDeleter = nullptr;
            return *this;
        }

        template <class T>
        T *get() const { return static_cast<T *>(mPtr); }
    };
}