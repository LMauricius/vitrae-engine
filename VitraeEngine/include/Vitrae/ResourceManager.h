#pragma once

#include <cstdint>
#include <concepts>
#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/UniqueCtr.h"

namespace Vitrae
{
	template <class ResT> class ResourceManager;

	class void_resource_ptr
	{
	public:
		virtual ~void_resource_ptr() = 0;
		virtual void_resource_ptr* new_copy() const = 0;
	};

	template<class ResT>
	class resource_ptr final: public void_resource_ptr
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

			return *this;
		}

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

		inline inner_ptr get_inner_ptr() const
		{
			return mPtr;
		}

		void_resource_ptr* new_copy() const
		{
			return new resource_ptr(*this);
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

	template<class ResT>
	class casted_resource_ptr
	{
	public:
		casted_resource_ptr(casted_resource_ptr&&) = default;
		casted_resource_ptr(const casted_resource_ptr& o):
			mInner((*o.mInner).new_copy()),
			mRawInner(o.mRawInner)
		{}
		~casted_resource_ptr() = default;

		casted_resource_ptr& operator=(casted_resource_ptr&& o) = default;
		casted_resource_ptr& operator=(const casted_resource_ptr& o)
		{
			mInner = Unique((*o.mInner).new_copy());
			mRawInner = o.mRawInner;
			return *this;
		}

		ResT *operator->()
		{
			return mRawInner;
		}
		const ResT *operator->() const
		{
			return mRawInner;
		}
		ResT& operator*()
		{
			return *mRawInner;
		}
		const ResT& operator*() const
		{
			return *mRawInner;
		}

		template<class T, class U> requires std::convertible_to<T, U>
		friend casted_resource_ptr<T> static_pointer_cast(casted_resource_ptr<U>&& o)
		{
			return Vitrae::casted_resource_ptr<T>(std::move(o.mInner), static_cast<T*>(o.mRawInner));
		}

		template<class T, class U> requires std::convertible_to<T, U>
		friend casted_resource_ptr<T> static_pointer_cast(const casted_resource_ptr<U>& o)
		{
			return Vitrae::casted_resource_ptr<T>((*o.mInner).new_copy(), static_cast<T*>(o.mRawInner));
		}

		template<class T, class U> requires std::convertible_to<T, U>
		friend casted_resource_ptr<T> static_pointer_cast(const resource_ptr<U>& o)
		{
			return Vitrae::casted_resource_ptr<T>(o.new_copy(), static_cast<ResT*>(&*o));
		}

	protected:
		Unique<void_resource_ptr> mInner;
		ResT *mRawInner;

		casted_resource_ptr(Unique<void_resource_ptr> inner, ResT *rawInner):
			mInner(inner),
			mRawInner(rawInner)
		{
		}
	};

	class AnyResourceManager
	{
	public:
		virtual ~AnyResourceManager() = 0;
	};


	/**
	 * @brief Creates and loads resources on demand
	 * @param ResT The type of resource. The resource must have a LoadParams type defined in its scope.
	 */
	template <class ResT>
	class ResourceManager: public ClassWithID<ResourceManager<ResT>>, public AnyResourceManager
	{
	public:
		friend class resource_ptr<ResT>;

		virtual ~ResourceManager() = 0;

		virtual resource_ptr<ResT> createUnnamedResource(ResT::SetupParams &&setupParams, ResT::LoadParams &&loadParams) = 0;
		virtual resource_ptr<ResT> createNamedResource(const String &name, ResT::SetupParams &&setupParams, ResT::LoadParams &&loadParams) = 0;
		virtual resource_ptr<ResT> getResource(const String &name) = 0;
		virtual void forgetResource(const String &name) = 0;

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

		resource_ptr<ResT> get_resource_ptr(const typename resource_ptr<ResT>::inner_ptr ptr)
		{
			return resource_ptr<ResT>(this, ptr);
		}
	};
}