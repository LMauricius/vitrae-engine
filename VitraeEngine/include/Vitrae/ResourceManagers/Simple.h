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
		// called when creating the resource
		void setup(ResT *, const ResT::SetupParams&) = 0;

		// called when needs loading (only while unloaded)
		void load(ResT *, const ResT::LoadParams&) = 0;

		// called when unloading (only while loaded)
		void unload(ResT *, const ResT::LoadParams&) = 0;
	};
	*/

	/**
	 * @brief A simple implementation of a resource manager.
	 * It loads resources when needed, and unloads and frees them when 
	 * no references to a Resource exist.
	 * @param ResT The type of resource
	 * @param Loader The loader of a resource.
	 * It must have functionality of an std::allocator class, along with 
	 * load(ResT*, const ResT::LoadParams&), unload(ResT*, const ResT::LoadParams&),
	 * and setup(ResT*, const ResT::SetupParams&) methods.
	 * Its value_type doesn't have to be equivalent to the ResT,
	 * just be static_cast-able to/from it
	*/
	template <class ResT, class Loader>
	class SimpleResourceManager : public ResourceManager<ResT>
	{
	protected:
		[[no_unique_address]] Loader mLoader;
		struct RefCountedWrapper: public RefCounted
		{
			RefCountedWrapper(ResT *ptr, ResT::LoadParams &&loadParams):
				RefCounted{0},
				ptr(ptr),
				loaded(false),
				loadParams(std::move(loadParams))
			{}

			bool loaded;
			ResT *ptr;
			[[no_unique_address]] ResT::LoadParams loadParams;
		};
		std::map<String, RefCountedWrapper> mResourcesByName;

		using real_inner_ptr = decltype(mResourcesByName)::iterator;
		static_assert(
			sizeof(real_inner_ptr) <= sizeof(typename resource_ptr<ResT>::inner_ptr),
			"Our inner hack isn't supported! Can't store inner pointer inside resource_ptr<>!"
		);

	public:
		SimpleResourceManager():
			mLoader()
		{}
		SimpleResourceManager(Loader&& allocator):
			mLoader(allocator)
		{}
		SimpleResourceManager(const Loader& allocator):
			mLoader(allocator)
		{}
		~SimpleResourceManager()
		{
			for (auto& p : mResourcesByName) {
				mLoader.deallocate(static_cast<Loader::value_type*>(p.second.ptr), 1);
			}
		}

		resource_ptr<ResT> createResource(const String &name, ResT::SetupParams &&setupParams, ResT::LoadParams &&loadParams)
		{
			auto res = mResourcesByName.insert(std::make_pair<String, RefCountedWrapper>(
				String(name),
				RefCountedWrapper(static_cast<ResT*>(mLoader.allocate(1)), std::move(loadParams))
			));

			if (!res.second) {
				throw std::runtime_error("Resource with this name already exists!");
			}

			mLoader.setup(static_cast<Loader::value_type*>(res.first->second.ptr), setupParams);
			real_inner_ptr it = res.first;
			return this->get_resource_ptr(toExt(it));
		}

		resource_ptr<ResT> getResource(const String &name)
		{
			auto it = mResourcesByName.find(name);

			if (it == mResourcesByName.end()) {
				throw std::runtime_error("Resource with this name doesn't exists!");
			}

			return this->get_resource_ptr(toExt(it));
		}

		void loadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr.get_inner_ptr());

			if (!it->second.loaded) {
				mLoader.load(static_cast<Loader::value_type*>(it->second.ptr), it->second.loadParams);
				it->second.loaded = true;
			}
		}

		void unloadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr.get_inner_ptr());

			if (it->second.loaded) {
				mLoader.unload(static_cast<Loader::value_type*>(it->second.ptr), it->second.loadParams);
				it->second.loaded = false;
			}
		}

	protected:
		resource_ptr<ResT>::inner_ptr toExt(real_inner_ptr it)
		{
			return *reinterpret_cast<resource_ptr<ResT>::inner_ptr*>(&it);
		}

		real_inner_ptr toReal(resource_ptr<ResT>::inner_ptr ptr)
		{
			return *reinterpret_cast<real_inner_ptr*>(&ptr);
		}

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
					mLoader.unload(static_cast<Loader::value_type*>(it->second.ptr), it->second.loadParams);
				}
				mLoader.deallocate(static_cast<Loader::value_type*>(it->second.ptr), 1);
				mResourcesByName.erase(it);
			}
		}
	};
}