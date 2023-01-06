#pragma once

#include <cstdint>
#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/UniqueCtr.h"

namespace Vitrae
{
	template <class ResT> class ResourceManager;

	template<class ResT>
	class resource_ptr
	{
	public:
		friend class ResourceManager<ResT>;

		using inner_ptr = uintptr_t;
		using inner_const_ptr = uintptr_t;

		resource_ptr(const resource_ptr& o):
			mResMan(o.mResMan),
			mPtr(o.mPtr)
		{
			mResMan->increaseCount(mPtr);
		}
		~resource_ptr()
		{
			mResMan->decreaseCount(mPtr);
		}

		resource_ptr& operator=(const resource_ptr& o)
		{
			mResMan = o.mResMan;
			mPtr = o.mPtr;
			mResMan->increaseCount(mPtr);
		}

		/*bool operator==(const resource_ptr&) const = default;
		bool operator!=(const resource_ptr&) const = default;
		bool operator>(const resource_ptr&) const = default;
		bool operator<(const resource_ptr&) const = default;
		bool operator>=(const resource_ptr&) const = default;
		bool operator<=(const resource_ptr&) const = default;*/

		ResT *operator->()
		{
			return mResMan->getResource(mPtr);
		}
		const ResT *operator->() const
		{
			return mResMan->getResource(mPtr);
		}
		ResT& operator*()
		{
			return *(mResMan->getResource(mPtr));
		}
		const ResT& operator*() const
		{
			return *(mResMan->getResource(mPtr));
		}

	protected:
		resource_ptr(ResourceManager<ResT> *resMan, inner_ptr ptr):
			mResMan(resMan),
			mPtr(ptr)
		{
			mResMan->increaseCount(mPtr);
		}

	private:
		inner_ptr mPtr;
		ResourceManager<ResT> *mResMan;
	};

	class AnyResourceManager
	{
	public:
		virtual ~AnyResourceManager() = 0;
	};

	template <class ResT>
	class ResourceManager: public ClassWithID<ResourceManager<ResT>>, public AnyResourceManager
	{
	public:
		friend class resource_ptr<ResT>;

		virtual ~ResourceManager() = 0;

		virtual resource_ptr<ResT> createResource(const String &name) = 0;
		virtual resource_ptr<ResT> getResource(const String &name) = 0;

		/**
		 * Can be called multiple times; loads the resource only once
		 * Call unloadResource() to undo this
		 * @param ptr The ptr
		 */
		virtual void loadResource(const resource_ptr<ResT> ptr) = 0;

		/**
		 * Can be called multiple times; unloads the resource only once
		 * Call loadResource() to undo this
		 * @param ptr The ptr
		 */
		virtual void unloadResource(const resource_ptr<ResT> ptr) = 0;

	protected:
		virtual ResT *getResource(const typename resource_ptr<ResT>::inner_ptr ptr) = 0;
		virtual void increaseCount(const typename resource_ptr<ResT>::inner_ptr ptr) = 0;
		virtual void decreaseCount(const typename resource_ptr<ResT>::inner_ptr ptr) = 0;
	};
}