#include "stdafx.h"
#include "MathTools.h"

double MathTools::Round(const double value, const uint32_t digitsAfterDecimal)
{
	return floor(value * pow(10., (int)digitsAfterDecimal) + .5) / pow(10., (int)digitsAfterDecimal);
}
