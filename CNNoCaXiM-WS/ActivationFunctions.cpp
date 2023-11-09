#include "ActivationFunctions.h"

DATA_PRECISION sigmoid(DATA_PRECISION& x)
{
	return (DATA_PRECISION)1.0 / ((DATA_PRECISION)1.0 + std::exp(-x));
}

DATA_PRECISION tanh(DATA_PRECISION& x)
{
	return (DATA_PRECISION)2.0 / ((DATA_PRECISION)1.0 + std::exp(-(DATA_PRECISION)2 * x)) - (DATA_PRECISION)1;
}

DATA_PRECISION relu(DATA_PRECISION& x)
{
	if (x < 0) x = (DATA_PRECISION)0.0;
	return x;
}