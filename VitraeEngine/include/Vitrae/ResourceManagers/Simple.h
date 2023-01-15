#pragma once

#include "Vitrae/ResourceManager.h"
#include "Vitrae/Util/RefCounted.h"

#include <map>
#include <list>
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
		std::list<RefCountedWrapper> mResources;
		std::map<String, resource_ptr<ResT>> mResourcesByName;

		using real_inner_ptr = decltype(mResources)::iterator;
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
			mResourcesByName.clear();
			for (auto& w : mResources) {
				mLoader.deallocate(static_cast<Loader::value_type*>(w.ptr), 1);
			}
		}

		resource_ptr<ResT> createUnnamedResource(ResT::SetupParams &&setupParams, ResT::LoadParams &&loadParams)
		{
			mResources.push_back(
				RefCountedWrapper(static_cast<ResT*>(mLoader.allocate(1)), std::move(loadParams))
			);

			mLoader.setup(static_cast<Loader::value_type*>(mResources.back().ptr), setupParams);
			real_inner_ptr it = mResources.end();
			it--;
			return this->get_resource_ptr(toExt(it));
		}

		resource_ptr<ResT> createNamedResource(const String &name, ResT::SetupParams &&setupParams, ResT::LoadParams &&loadParams)
		{
			auto insRes = mResourcesByName.insert(std::make_pair<String, resource_ptr<ResT>>(
				String(name),
				createUnnamedResource(std::move(setupParams), std::move(loadParams))
			));

			if (!insRes.second) {
				mLoader.deallocate(static_cast<Loader::value_type*>(mResources.back().ptr), 1);
				throw std::runtime_error("Resource with this name already exists!");
			}

			return insRes.first->second;
		}

		resource_ptr<ResT> getResource(const String &name)
		{
			auto it = mResourcesByName.find(name);

			if (it == mResourcesByName.end()) {
				throw std::runtime_error("Resource with this name doesn't exist!");
			}

			return it->second;
		}

		void forgetResource(const String &name)
		{
			mResourcesByName.erase(name);
		}

		void loadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr.get_inner_ptr());

			if (!it->loaded) {
				mLoader.load(static_cast<Loader::value_type*>(it->ptr), it->loadParams);
				it->loaded = true;
			}
		}

		void unloadResource(const resource_ptr<ResT> ptr)
		{
			real_inner_ptr it = toReal(ptr.get_inner_ptr());

			if (it->loaded) {
				mLoader.unload(static_cast<Loader::value_type*>(it->ptr), it->loadParams);
				it->loaded = false;
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
			return it->ptr;
		}
		void increaseCount(const resource_ptr<ResT>::inner_ptr ptr)
		{
			real_inner_ptr it = toReal(ptr);
			it->count++;
		}
		void decreaseCount(const resource_ptr<ResT>::inner_ptr ptr)
		{
			real_inner_ptr it = toReal(ptr);
			it->count--;
		
			if (it->count == 0) {
				if (it->loaded) {
					mLoader.unload(static_cast<Loader::value_type*>(it->ptr), it->loadParams);
				}
				mLoader.deallocate(static_cast<Loader::value_type*>(it->ptr), 1);
			}
		}
	};
}