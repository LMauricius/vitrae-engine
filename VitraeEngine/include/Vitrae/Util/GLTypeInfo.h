#pragma once

#include <cstdlib>
#include "glad/glad.h"

namespace Vitrae
{

    template <class T>
    struct GLTypeInfo
	{
		static const GLenum GlTypeId; // undefined
	};

	template<>
    struct GLTypeInfo<std::int8_t>
	{
		static constexpr GLenum GlTypeId = GL_BYTE;
	};
    template<>
    struct GLTypeInfo<std::uint8_t>
	{
		static constexpr GLenum GlTypeId = GL_UNSIGNED_BYTE;
	};
    template<>
    struct GLTypeInfo<std::int16_t>
	{
		static constexpr GLenum GlTypeId = GL_SHORT;
	};
    template<>
    struct GLTypeInfo<std::uint16_t>
	{
		static constexpr GLenum GlTypeId = GL_UNSIGNED_SHORT;
	};
    template<>
    struct GLTypeInfo<std::int32_t>
	{
		static constexpr GLenum GlTypeId = GL_INT;
	};
    template<>
    struct GLTypeInfo<std::uint32_t>
	{
		static constexpr GLenum GlTypeId = GL_UNSIGNED_INT;
	};

    template<>
    struct GLTypeInfo<float>
	{
		static constexpr GLenum GlTypeId = GL_FLOAT;
	};
    template<>
    struct GLTypeInfo<double>
	{
		static constexpr GLenum GlTypeId = GL_DOUBLE;
	};
}