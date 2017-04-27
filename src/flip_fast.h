#pragma once
#include <stdint.h>
#include "macros_hell.h"

// The fastest flip function there is

namespace Flip_Fast
{
	extern uint64_t (*flip_fp[65])(const uint64_t, const uint64_t);
}

FORCE_INLINE uint64_t flip(const uint64_t P, const uint64_t O, const unsigned long move) { return Flip_Fast::flip_fp[move](P, O); }
