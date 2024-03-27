
#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

#include "cymbal_poc/expression.h"
#include "cymbal_poc/shape.h"

#ifndef CYMBAL_TENSOR_H
#define CYMBAL_TENSOR_H

namespace cymbal
{

template <size_t RANK>
struct Tensor final
{
	Tensor (const Shape<RANK>& shape, id_t id_begin = 0) : shape_(shape)
	{
		std::vector<id_t> ids(shape.nelems());
		std::iota(ids.begin(), ids.end(), id_begin);
		std::transform(ids.begin(), ids.end(), std::back_inserter(exps_), id_to_exp);
	}

	Tensor (const Shape<RANK>& shape, const std::vector<id_t>& ids) : shape_(shape)
	{
		std::transform(ids.begin(), ids.end(), std::back_inserter(exps_), id_to_exp);
	}

	Tensor (const Shape<RANK>& shape, const std::vector<exp_t>& exps) : shape_(shape), exps_(exps) {}

	Shape<RANK> shape_;

	std::vector<exp_t> exps_;
};

} // cymbal

#endif // CYMBAL_TENSOR_H
