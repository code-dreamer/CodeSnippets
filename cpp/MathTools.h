#pragma once

#include <boost/numeric/conversion/cast.hpp>

namespace MathTools
{

KERNEL_API double Round(double value, uint32_t digitsAfterDecimal);

template<typename Target, typename Source>
Target SaturationCast(Source value) 
{
	try {
		return boost::numeric_cast<Target>(value);
	}
	catch (const boost::numeric::negative_overflow&) {
		G_ASSERT_MSG(false, "Negative overflow detected");
		return std::numeric_limits<Target>::min();
	}
	catch (const boost::numeric::positive_overflow&) {
		G_ASSERT_MSG(false, "Positive overflow detected");
		return std::numeric_limits<Target>::max();
	}

	catch (const boost::numeric::bad_numeric_cast&) {
		G_ASSERT_MSG(false, "Numeric cast failed");
		return Target();
	}
}

}
