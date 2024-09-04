/**
 * @file cast.h
 * @author Xuan Tho Do (tho.dok17@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-01
 *
 * @copyright Copyright (c) 2024 Kinis AI
 *
 */
#ifndef UTILS_CAST_H
#define UTILS_CAST_H

#include <stdint.h>

typedef union
{
	float f32;
	uint32_t u32;
} CAST_F32U32;

static inline uint32_t CAST_f32ToU32(float f32)
{
	CAST_F32U32 cast;
	cast.f32 = f32;
	return cast.u32;
}

static inline float CAST_u32ToF32(uint32_t u32)
{
	CAST_F32U32 cast;
	cast.u32 = u32;
	return cast.f32;
}

#endif // UTILS_CAST_H
