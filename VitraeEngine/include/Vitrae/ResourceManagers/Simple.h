#pragma once

#include "Vitrae/ResourceManager.h"
#include "Vitrae/Util/RefCounted.h"

#include <map>
#include <memory>
#include <stdexcept>

namespace Vitrae
{
	/*
	// example loader
	template<class ResT>
	class Loader: public std::allocator<ResT>
	{
		void load(ResT *) = 0;
		void unload(ResT *) = 0;
	};
	*/

	/**
	 * @brief A simple implementation of a resource manager.
	 * It loads resources when needed, and unloads and frees them when 
	 * no references to a Resource exist.
	 * @param ResT The type of resource
	 * @param Loader The loader of a resource.
	 * It must have functionality of an std::allocator class, along with 
	 * load(ResT*) and unload(ResT*) methods.
	*/
	template <class ResT, class Loader>
	class SimpleResourceManager : public ResourceManager<ResT>
	{
	public:
		SimpleResourceManager():
			mLoader()
		{}
		SimpleResourceManager(Allocator&& allocator):
			mLoader(allocator)
		{}
		SimpleResourceManager(const Allocator& allocator):
			mLoader(allocator)
		{}
		~SimpleResourceManager()
		{
			for (auto& p : mResourcesByName) {
				mLoader.deallocate(p.second.ptr);
			}
		}

		resource_ptr<ResT> createResource(const String &name)
		{
			auto res = mResourcesByName.insert({
				name,
				RefCountedWrapper(mLoader.allocate());
			});

			if (!res.second) {
				throw std::runtime_error("Resource with this name already exists!");
			}

			real_inner_ptr it = res.first;
			return toExt(it);
		}

		resource_ptr<ResT> getResource(const String &name)
		{
			auto it = mResourcesByName.find(name);

			if (it == mResourcesByName.end()) {
				throw std::runtime_error("Resource with this name doesn't exists!");
			}

			return toExt(it);
		}

		void loadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr);

			if (!it->second.loaded) {
				mLoader.load(it->second.ptr);
				it->second.loaded = true;
			}
		}

		void unloadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr);

			if (it->second.loaded) {
				mLoader.unload(it->second.ptr);
				it->second.loaded = false;
			}
		}

	protected:
		using real_inner_ptr = typeof(mResourcesByName)::iterator;
		static_assert(
			sizeof(real_inner_ptr) <= sizeof(resource_ptr<ResT>::inner_ptr),
			"Our inner hack isn't supported! Can't store inner pointer inside resource_ptr<>!"
		);

		resource_ptr<ResT>::inner_ptr toExt(real_inner_ptr it)
		{
			return reinterpret_cast<resource_ptr<ResT>::inner_ptr>(it);
		}
		real_inner_ptr toReal(resource_ptr<ResT>::inner_ptr ptr)
		{
			return reinterpret_cast<real_inner_ptr>(ptr);
		}

		class RefCountedWrapper: public RefCounted
		{
			RefCountedWrapper(ResT *ptr):
				RefCounted{0},
				ptr(ptr),
				loaded(false)
			{}

			bool loaded;
			ResT *ptr;
		};

		ResT *getResource(const resource_ptr<ResT>::inner_ptr ptr)
		{
			real_inner_ptr it = toReal(ptr);
			return it->second.ptr;
		}
		void increaseCount(const resource_ptr<ResT>::inner_ptr ptr)
		{
			real_inner_ptr it = toReal(ptr);
			it->second.count++;
		}
		void decreaseCount(const resource_ptr<ResT>::inner_ptr ptr)
		{
			real_inner_ptr it = toReal(ptr);
			it->second.count--;
		
			if (it->second.count == 0) {
				if (it->second.loaded) {
					mLoader.unload(it->second.ptr);
				}
				mLoader.deallocate(it->second.ptr);
				mResourcesByName.erase(it);
			}
		}

		[[no_unique_address]] Loader mLoader;
		std::map<String, RefCountedWrapper> mResourcesByName;
	};
}