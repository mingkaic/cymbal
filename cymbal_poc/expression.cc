
#include <algorithm>
#include <ostream>

#include "cymbal_poc/expression.h"

#ifdef CYMBAL_EXPRESSION_H

namespace cymbal
{

std::ostream& operator << (std::ostream& os, const exp_t& exp)
{
	if (exp == nullptr)
	{
		os << "[nil]";
	}
	else
	{
		exp->print(os);
	}
	return os;
}

} // cymbal

#endif // CYMBAL_EXPRESSION_H
