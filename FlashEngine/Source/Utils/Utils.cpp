#include "pch.h"
#include "Utils.h"
#include <math.h>
#include <algorithm>

float srandom()
{
	float number = float(rand()) / float(RAND_MAX);
	number *= 2.0f;
	number -= 1.0f;
	return number;
}
