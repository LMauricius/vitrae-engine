#pragma once

#include "Vitrae/Util/Types.h"
#include "Vitrae/Util/UniqueAnyPtr.h"
#include "Vitrae/Util/UniqueId.h"

#include "dynasma/managers/abstract.hpp"
#include "dynasma/cachers/abstract.hpp"
#include "dynasma/pointer.hpp"

#include <memory>
#include <map>
#include <any>

namespace Vitrae
{
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
	A HUB of multiple asset managers and other components.
	One ComponentRoot must be used for all related resources
	*/
	class ComponentRoot
	{
	public:
		ComponentRoot();
		~ComponentRoot();

		/**
		Sets the component of a particular type and
		takes its ownership.
		@param man The component pointer to set
		*/
		template <class T>
		void setComponent(Unique<T> &&comp)
		{
			auto &myvar = getGenericStorageVariable<T>();
			myvar = std::move(comp);
		}

		/**
		@return The component of a particular type T
		*/
		template <class T>
		T &getComponent()
		{
			auto &myvar = getGenericStorageVariable<T>();
			return *(myvar.template get<T>());
		}

		inline std::ostream &getErrStream() { return *mErrStream; }
		inline std::ostream &getInfoStream() { return *mInfoStream; }
		inline std::ostream &getWarningStream() { return *mWarningStream; }
		inline void setErrStream(std::ostream &os) { mErrStream = &os; }
		inline void setInfoStream(std::ostream &os) { mInfoStream = &os; }
		inline void setWarningStr(std::ostream &os) { mWarningStream = &os; }

	protected:
		/*
		Returns a variable to store the manager of a particular type.
		Is specialized to return member variables for defaultly supported types.
		*/
		template <class T>
		UniqueAnyPtr &getGenericStorageVariable()
		{
			return mCustomComponents.at(getClassID<T>());
		}

		std::map<size_t, UniqueAnyPtr> mCustomComponents;

		std::ostream *mErrStream, *mInfoStream, *mWarningStream;
	};
}