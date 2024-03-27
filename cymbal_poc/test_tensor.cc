
#ifndef DISABLE_TENSOR_TEST

#include <unordered_map>
#include <random>

#include "gtest/gtest.h"

#include "Fastor/Fastor.h"

#include "cymbal_poc/tensor.h"
#include "cymbal_poc/operation.h"


TEST(OPERATION, Einsum)
{
	std::minstd_rand rengine;
	std::uniform_int_distribution<int> dist(1, 256);
	auto gen = std::bind(dist, rengine);

	cymbal::Tensor<3> a(
			cymbal::Shape<3>({3, 4, 5}));
	cymbal::Tensor<4> b(
			cymbal::Shape<4>({5, 2, 4, 6}), a.shape_.nelems());
	auto result = cymbal::einsum(a, b, cymbal::indices_t<2>{
			cymbal::pidx_t{2, 0},
			cymbal::pidx_t{1, 2}});

	auto an = a.shape_.nelems();
	std::vector<int> data(an+b.shape_.nelems());
	auto dit = data.begin();
	std::generate(dit, data.end(), gen);

	Fastor::Tensor<int,3,4,5> fa(
			std::vector<int>(dit, dit+an));
	Fastor::Tensor<int,5,2,4,6> fb(
			std::vector<int>(dit+an, data.end()));
	auto fresult = Fastor::einsum<
		Fastor::Index<1,2,3>,
		Fastor::Index<3,4,2,5>>(fa, fb);
	Fastor::Tensor<int,3*2*6> want_result = fresult;
	std::vector<int> want;
	for (size_t i = 0; i < 3*2*6; ++i)
	{
		want.push_back(want_result(i));
	}

	std::unordered_map<cymbal::id_t,int> idmapping;
	for (size_t i = 0; i < an; ++i)
	{
		idmapping.emplace(a.exps_[i]->id_, data[i]);
	}
	for (size_t i = 0, n = b.shape_.nelems(); i < n; ++i)
	{
		idmapping.emplace(b.exps_[i]->id_, data[i+an]);
	}
	std::vector<int> got;
	got.reserve(result.exps_.size());
	std::transform(result.exps_.begin(), result.exps_.end(), std::back_inserter(got),
			[&idmapping](const cymbal::exp_t& id) { return cymbal::calculate(idmapping, id); });

	for (size_t i = 0, n = got.size(); i < n; ++i)
	{
		EXPECT_EQ(want[i], got[i]);
	}
}


#endif // DISABLE_TENSOR_TEST
