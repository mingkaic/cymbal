
#include <algorithm>
#include <ostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <unordered_map>

#ifndef CYMBAL_EXPRESSION_H
#define CYMBAL_EXPRESSION_H

namespace cymbal
{

using id_t = uint16_t;

static const int8_t NALPHANUM = 'z' - 'a' + 10;

static const id_t NRESERVED = 4;

static const char MUL = '*';
static const char ADD = '+';

static const id_t MUL_ID = 2;
static const id_t ADD_ID = 3;

static const char RESERVED[NRESERVED] = {'0', '1', MUL, ADD};

static char num_to_char (int8_t num)
{
	if (num >= 10)
	{
		return 'a' + (num - 10);
	}
	return '0' + num;
}

static std::string num_to_sym (size_t num)
{
	if (num < NRESERVED)
	{
		switch (RESERVED[num])
		{
			case MUL_ID:
				return "*";
			case ADD_ID:
				return "+";
			default:
				return std::string(1, RESERVED[num]);
		}
	}
	num -= NRESERVED;
	std::string out = "<";
	for (; num > NALPHANUM; num /= NALPHANUM)
	{
		out.push_back(num_to_char(num % NALPHANUM));
	}
	out.push_back(num_to_char(num));
	out.push_back('>');
	return out;
}

struct Expression final
{
	using ptr_t = std::shared_ptr<Expression>;

	Expression (id_t id) : id_(id) {}

	Expression (id_t id, const std::vector<ptr_t>& subexps) :
		id_(id), subexps_(subexps) {}

	void print (std::ostream& os) const
	{
		if (subexps_.empty())
		{
			os << num_to_sym(id_);
			return;
		}
		bool use_brackets = subexps_.size() > 1;
		if (use_brackets)
		{
			os << '(';
		}
		if (nullptr != subexps_.front())
		{
			subexps_.front()->print(os);
		}
		for (size_t i = 1, n = subexps_.size(); i < n; ++i)
		{
			if (nullptr != subexps_[i])
			{
				os << num_to_sym(id_);
				subexps_[i]->print(os);
			}
		}
		if (use_brackets)
		{
			os << ')';
		}
	}

	ptr_t derive (id_t target) const
	{
		if (subexps_.empty())
		{
			if (id_ != target)
			{
				return std::make_shared<Expression>(0);
			}
			return std::make_shared<Expression>(1);
		}
		std::vector<ptr_t> subders;
		std::transform(subexps_.begin(), subexps_.end(),
				std::back_inserter(subders), [&](ptr_t subexp) { return subexp->derive(target); });
		switch (id_)
		{
			case MUL_ID:
			{
				size_t n = subexps_.size();
				std::vector<ptr_t> nextargs(n);
				std::vector<ptr_t> mulargs = subexps_;
				for (size_t i = 0; i < n; ++i)
				{
					mulargs[i] = subders[i];
					nextargs[i] = std::make_shared<Expression>(MUL_ID, mulargs);
					mulargs[i] = subexps_[i];
				}
				return std::make_shared<Expression>(ADD_ID, nextargs);
			}
			case ADD_ID:
				return std::make_shared<Expression>(ADD_ID, subders);
		}
		return nullptr;
	}

	id_t id_;

	std::vector<ptr_t> subexps_;
};

using exp_t = typename Expression::ptr_t;

std::ostream& operator << (std::ostream& os, const exp_t& exp);

static exp_t id_to_exp (id_t id)
{
	return std::make_shared<Expression>(NRESERVED + id);
}

template <typename T>
T calculate (const std::unordered_map<id_t,T>& values, const exp_t& exp)
{
	if (nullptr == exp)
	{
		return (T) 0;
	}
	if (exp->subexps_.empty())
	{
		if (exp->id_ < 2)
		{
			// return constants
			return exp->id_;
		}
		auto it = values.find(exp->id_);
		if (it == values.end())
		{
			return 0;
		}
		return it->second;
	}
	std::vector<T> args;
	args.reserve(exp->subexps_.size());
	std::transform(exp->subexps_.begin(), exp->subexps_.end(), std::back_inserter(args),
			[&values](const exp_t& e) { return calculate(values, e); });
	switch (exp->id_)
	{
		case MUL_ID:
			return std::accumulate(args.begin(), args.end(), 1, std::multiplies<T>());
		case ADD_ID:
			return std::accumulate(args.begin(), args.end(), 0);
		default:
			// todo: throw or log error
			return args.front();
	}
}

static exp_t optimize_exp (const exp_t& exp)
{
	if (nullptr == exp || exp->subexps_.empty())
	{
		return exp;
	}
	std::vector<exp_t> subs;
	subs.reserve(exp->subexps_.size());
	switch (exp->id_)
	{
		case MUL_ID:
			for (auto sub : exp->subexps_)
			{
				sub = optimize_exp(sub);
				switch (sub->id_)
				{
					case 0:
						return sub; // propagate 0
					case 1:
						// ignore 1
						break;
					default:
						subs.push_back(sub);
				}
			}
			break;
		case ADD_ID:
			for (auto sub : exp->subexps_)
			{
				sub = optimize_exp(sub);
				switch (sub->id_)
				{
					case 0:
						// ignore 0
						break;
					default:
						subs.push_back(optimize_exp(sub));
				}
			}
			break;
		default:
			return nullptr;
	}
	return std::make_shared<Expression>(subs.empty() ? 0 : exp->id_, subs);
}

} // cymbal

#endif // CYMBAL_EXPRESSION_H
