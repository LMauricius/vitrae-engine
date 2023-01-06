#pragma once

#include "ResourceManager.h"

#include <memory>
#include <map>

namespace Vitrae
{
	template <class ResT> class ResourceManager;

	class Material;

	/*
	A HUB of multiple resource managers.
	One ResourceRoot must be used for all related resources
	*/
	class ResourceRoot
	{
	public:
		ResourceRoot();

		/**
		Sets the manager for a particular resource type and
		takes its ownership.
		@param man The resource manager pointer to set
		*/
		template<class ResT>
		void setManager(ResourceManager<ResT> *man)
		{
			Unique<AnyResourceManager> &myvar = getManagerStorageVariable<ResT>();
			myvar = Unique<AnyResourceManager>(man);
		}

		/**
		@return The resource manager for a particular type ResT
		*/
		template<class ResT>
		ResourceManager<ResT> &getManager()
		{
			Unique<AnyResourceManager> &myvar = getManagerStorageVariable<ResT>();
			return *static_cast<ResourceManager<ResT>*>(myvar.get());
		}

	protected:

		/*
		Returns a variable to store the manager of a particular type.
		Is specialized to return member variables for defaultly supported types.
		*/
		template<class ResT>
		Unique<AnyResourceManager> &getManagerStorageVariable()
		{
			return mCustomManagers[ResourceManager<ResT>::getClassID()];
		}

		Unique<AnyResourceManager> mMaterialManPtr;
		std::map<size_t, Unique<AnyResourceManager>> mCustomManagers;
	};

	template<>
	Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Material>() {return mMaterialManPtr;}
}