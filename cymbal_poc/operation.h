
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

#include "cymbal_poc/expression.h"
#include "cymbal_poc/shape.h"
#include "cymbal_poc/tensor.h"

namespace cymbal
{

using pidx_t = std::pair<uint8_t,uint8_t>;

template <size_t N>
using indices_t = std::array<pidx_t,N>;

template <size_t LRANK, size_t RRANK, size_t NINDICES>
Tensor<LRANK+RRANK-2*NINDICES> einsum (
		const Tensor<LRANK>& lhs, const Tensor<RRANK>& rhs,
		const indices_t<NINDICES>& indices)
{
	std::array<size_t,NINDICES> join_dims;
	std::array<bool,LRANK> lfree;
	std::array<bool,RRANK> rfree;
	lfree.fill(true); rfree.fill(true);
	for (uint8_t j = 0; j < NINDICES; ++j)
	{
		auto lidx = indices[j].first;
		auto ridx = indices[j].second;
		assert(lhs.shape_.dims_[lidx] == rhs.shape_.dims_[ridx]);
		join_dims[j] = lhs.shape_.dims_[lidx];
		lfree[lidx] = false;
		rfree[ridx] = false;
	}
	const uint8_t LFREE = LRANK-NINDICES;
	const uint8_t RFREE = RRANK-NINDICES;
	std::array<size_t,LFREE+RFREE> outdims;
	std::array<uint8_t,LFREE> lfree_indices;
	std::array<uint8_t,RFREE> rfree_indices;
	std::array<size_t,LFREE> lfree_dims;
	std::array<size_t,RFREE> rfree_dims;
	for (uint8_t j = 0, k = 0; j < LRANK; ++j)
	{
		if (lfree[j])
		{
			outdims[k] = lhs.shape_.dims_[j];
			lfree_indices[k] = j;
			lfree_dims[k++] = lhs.shape_.dims_[j];
		}
	}
	for (uint8_t j = 0, k = 0; j < RRANK; ++j)
	{
		if (rfree[j])
		{
			outdims[LFREE+k] = rhs.shape_.dims_[j];
			rfree_indices[k] = j;
			rfree_dims[k++] = rhs.shape_.dims_[j];
		}
	}
	Shape<LFREE+RFREE> outshape(outdims);
	Shape<NINDICES> join_shape(join_dims);
	Shape<LFREE> lfree_shape(lfree_dims);
	Shape<RFREE> rfree_shape(rfree_dims);
	auto njoin = join_shape.nelems();
	auto nlfree = lfree_shape.nelems();
	auto nrfree = rfree_shape.nelems();
	std::vector<exp_t> lmat(nlfree * njoin);
	std::vector<exp_t> rmat(nrfree * njoin);
	for (size_t i = 0, n = lhs.shape_.nelems(); i < n; ++i)
	{
		auto lcoord = lhs.shape_.index_to_coord(i);
		std::array<size_t,NINDICES> join_coord;
		for (uint8_t j = 0; j < NINDICES; ++j)
		{
			join_coord[j] = lcoord[indices[j].first];
		}
		std::array<size_t,LFREE> free_coord;
		for (uint8_t j = 0; j < LFREE; ++j)
		{
			free_coord[j] = lcoord[lfree_indices[j]];
		}
		auto jidx = join_shape.coord_to_index(join_coord);
		auto fidx = lfree_shape.coord_to_index(free_coord);
		lmat[jidx + fidx * njoin] = lhs.exps_[i];
	}
	for (size_t i = 0, n = rhs.shape_.nelems(); i < n; ++i)
	{
		auto rcoord = rhs.shape_.index_to_coord(i);
		std::array<size_t,NINDICES> join_coord;
		for (uint8_t j = 0; j < NINDICES; ++j)
		{
			join_coord[j] = rcoord[indices[j].second];
		}
		std::array<size_t,RFREE> free_coord;
		for (uint8_t j = 0; j < RFREE; ++j)
		{
			free_coord[j] = rcoord[rfree_indices[j]];
		}
		auto jidx = join_shape.coord_to_index(join_coord);
		auto fidx = rfree_shape.coord_to_index(free_coord);
		rmat[jidx + fidx * njoin] = rhs.exps_[i];
	}
	std::vector<exp_t> out;
	out.reserve(nlfree * nrfree);
	for (size_t y = 0; y < nlfree; ++y)
	{
		for (size_t x = 0; x < nrfree; ++x)
		{
			exp_t outer = std::make_shared<Expression>(ADD_ID);
			for (size_t i = 0; i < njoin; ++i)
			{
				exp_t inner = std::make_shared<Expression>(MUL_ID, std::vector<exp_t>{
					lmat[i + y * njoin],
					rmat[i + x * njoin],
				});
				outer->subexps_.push_back(inner);
			}
			out.push_back(outer);
		}
	}
	return Tensor<LFREE+RFREE>(outshape, out);
}

template <size_t RANK>
Tensor<RANK> permute (
		const Tensor<RANK>& arg,
		const std::array<uint8_t,RANK>& indices)
{
	std::array<size_t,RANK> outdims;
	outdims.fill(1);
	for (uint8_t j = 0; j < RANK; ++j)
	{
		outdims[j] = arg.shape_.dims_[indices[j]];
	}
	std::array<size_t,RANK> pcoord;
	size_t n = arg.shape_.nelems();
	Shape<RANK> outshape(outdims);
	std::vector<exp_t> out(n);
	for (size_t i = 0; i < n; ++i)
	{
		auto coord = arg.shape_.index_to_coord(i);
		pcoord.fill(0);
		for (uint8_t j = 0; j < RANK; ++j)
		{
			pcoord[j] = coord[indices[j]];
		}
		out[outshape.coord_to_index(pcoord)] = arg.exps_[i];
	}
	return Tensor<RANK>(outshape, out);
}

template <size_t RANK>
Tensor<RANK> cwise_prod (
		const Tensor<RANK>& lhs,
		const Tensor<RANK>& rhs)
{
	size_t n = lhs.shape_.nelems();
	assert(n == rhs.shape_.nelems());
	std::vector<exp_t> out;
	out.reserve(n);
	for (size_t i = 0; i < n; ++i)
	{
		exp_t id = std::make_shared<Expression>(MUL_ID);
		id->subexps_.push_back(lhs.exps_[i]);
		id->subexps_.push_back(rhs.exps_[i]);
		out.push_back(id);
	}
	return Tensor<RANK>(lhs.shape_, out);
}

template <size_t RANK>
cymbal::Tensor<RANK> optimize (const cymbal::Tensor<RANK>& arg)
{
	size_t n = arg.shape_.nelems();
	std::vector<exp_t> out;
	out.reserve(n);
	for (size_t i = 0; i < n; ++i)
	{
		out.push_back(optimize_exp(arg.exps_[i]));
	}
	return cymbal::Tensor<RANK>(arg.shape_, out);
}

template <size_t ORANK,size_t TRANK>
cymbal::Tensor<ORANK+TRANK> jacobian (
		const cymbal::Tensor<ORANK>& op,
		const cymbal::Tensor<TRANK>& target)
{
	size_t ntargs = target.shape_.nelems();
	size_t nops = op.shape_.nelems();
	std::vector<exp_t> out;
	out.reserve(nops * ntargs);
	for (size_t i = 0; i < ntargs; ++i)
	{
		auto& tid = target.exps_[i]->id_;
		assert(tid >= NRESERVED);
		for (size_t j = 0; j < nops; ++j)
		{
			out.push_back(op.exps_[j]->derive(tid));
		}
	}
	std::array<size_t,ORANK+TRANK> outdims;
	std::copy(target.shape_.dims_.begin(), target.shape_.dims_.end(), outdims.begin());
	std::copy(op.shape_.dims_.begin(), op.shape_.dims_.end(), outdims.begin()+TRANK);
	return cymbal::Tensor<ORANK+TRANK>(Shape<ORANK+TRANK>(outdims), out);
}

} // cymbal
