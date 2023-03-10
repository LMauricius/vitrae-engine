#pragma once

#include "Vitrae/ResourceManager.h"

#include <memory>
#include <map>

namespace Vitrae
{
	template <class ResT> class ResourceManager;

	class SourceShaderStep;
	class GroupShaderStep;
	class SwitchShaderStep;
	class Shader;
	class Texture;
	class Material;
	class Mesh;
	class Model;
	class RenderStep;
	class RenderPlan;

	/*
	A HUB of multiple resource managers.
	One ResourceRoot must be used for all related resources
	*/
	class ResourceRoot
	{
	public:
		ResourceRoot();
		~ResourceRoot();

		/**
		Sets the manager for a particular resource type and
		takes its ownership.
		@param man The resource manager pointer to set
		*/
		template<class ResT>
		void setManager(Unique<ResourceManager<ResT>> &&man)
		{
			Unique<AnyResourceManager> &myvar = getManagerStorageVariable<ResT>();
			myvar = Unique<AnyResourceManager>(man.release());
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

		inline std::ostream &getErrStream() {return *mErrStream;}
		inline std::ostream &getInfoStream() {return *mInfoStream;}
		inline std::ostream &getWarningStream() {return *mWarningStream;}
		inline void setErrStream(std::ostream &os) {mErrStream = &os;}
		inline void setInfoStream(std::ostream &os) {mInfoStream = &os;}
		inline void setWarningStr(std::ostream &os) {mWarningStream = &os;}

	protected:

		/*
		Returns a variable to store the manager of a particular type.
		Is specialized to return member variables for defaultly supported types.
		*/
		template<class ResT>
		Unique<AnyResourceManager> &getManagerStorageVariable()
		{
			return mCustomManagers.at(ResourceManager<ResT>::getClassID());
		}

		Unique<AnyResourceManager> mShaderManPtr, mTextureManPtr, mMaterialManPtr, mMeshManPtr, mModelManPtr;
		Unique<AnyResourceManager> mSourceShaderStepManPtr, mGroupShaderStepManPtr, mSwitchShaderStepManPtr;
		std::map<size_t, Unique<AnyResourceManager>> mCustomManagers;

		std::ostream *mErrStream, *mInfoStream, *mWarningStream;
	};
	
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Texture>() {return mTextureManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Material>() {return mMaterialManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Mesh>() {return mMeshManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Model>() {return mModelManPtr;}

	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<SourceShaderStep>() {return mSourceShaderStepManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<GroupShaderStep>() {return mGroupShaderStepManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<SwitchShaderStep>() {return mSwitchShaderStepManPtr;}
	template<> Unique<AnyResourceManager> &ResourceRoot::getManagerStorageVariable<Shader>() {return mShaderManPtr;}
}