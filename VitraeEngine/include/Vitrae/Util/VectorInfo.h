#pragma once

#include <cstdlib>
#include <cstdint>
#include "assimp/types.h"
#include "glm/glm.hpp"

namespace Vitrae
{

    template <class VecT>
    struct VectorInfo
	{
		//using value_type = VecT;
		static const std::size_t NumComponents; // undefined
	};

    template <>
    struct VectorInfo<float>
	{
		using value_type = float;
		static constexpr std::size_t NumComponents = 1;
	};

    template <>
    struct VectorInfo<aiVector2D>
	{
		using value_type = ai_real;
		static constexpr std::size_t NumComponents = 2;
	};
    template <>
    struct VectorInfo<aiVector3D>
	{
		using value_type = ai_real;
		static constexpr std::size_t NumComponents = 3;
	};
    template <>
    struct VectorInfo<aiColor3D>
	{
		using value_type = ai_real;
		static constexpr std::size_t NumComponents = 3;
	};
    template <>
    struct VectorInfo<aiColor4D>
	{
		using value_type = ai_real;
		static constexpr std::size_t NumComponents = 4;
	};

    template <>
    struct VectorInfo<glm::vec1>
	{
		using value_type = float;
		static constexpr std::size_t NumComponents = 1;
	};
    template <>
    struct VectorInfo<glm::vec2>
	{
		using value_type = float;
		static constexpr std::size_t NumComponents = 2;
	};
    template <>
    struct VectorInfo<glm::vec3>
	{
		using value_type = float;
		static constexpr std::size_t NumComponents = 3;
	};
    template <>
    struct VectorInfo<glm::vec4>
	{
		using value_type = float;
		static constexpr std::size_t NumComponents = 4;
	};
}