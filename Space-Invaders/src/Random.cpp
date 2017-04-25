#include "Random.h"
#include <stdlib.h>

int Random::Int(int lower, int upper)
{
	return rand()%(upper-lower+1) + lower;
}
