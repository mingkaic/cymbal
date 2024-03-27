
#include <algorithm>
#include <array>
#include <string>

#include "cisab/fmts/format.h"

#ifndef CYMBAL_SHAPE_H
#define CYMBAL_SHAPE_H

namespace cymbal
{

template <uint8_t RANK>
struct Shape final
{
	Shape (const std::array<size_t,RANK>& dims) : dims_(dims) {}

	std::string to_string (void) const
	{
		return cisab::fmts::Humanize<std::array<size_t,RANK>>(dims_);
	}

	std::array<size_t,RANK> index_to_coord (size_t index) const
	{
		std::array<size_t,RANK> coord;
		coord.fill(0);
		for (uint8_t j = 0; j < RANK; ++j)
		{
			coord[RANK-1-j] = index % dims_[RANK-1-j];
			index /= dims_[RANK-1-j];
		}
		return coord;
	}

	size_t coord_to_index (const std::array<size_t,RANK>& coord) const
	{
		size_t index = 0;
		size_t multiple = 1;
		for (int16_t j = RANK-1; j >= 0; --j)
		{
			index += coord[j] * multiple;
			multiple *= dims_[j];
		}
		return index;
	}

	size_t nelems (void) const
	{
		return std::accumulate(dims_.begin(), dims_.end(), 1, std::multiplies<size_t>());
	}

	std::array<size_t,RANK> dims_;
};

} // cymbal

#endif // CYMBAL_SHAPE_H
