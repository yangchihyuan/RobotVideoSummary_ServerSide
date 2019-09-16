#include "utility_compute.hpp"
#include <math.h>

float ComputeL2Norm(array<float,1536> array1, array<float,1536> array2)
{
    float temp;
    unsigned int size = array1.size();
    float sum = 0;
    for(unsigned int i=0 ; i<size ; i++)
    {
        temp = (array1[i] - array2[i]);
        sum += temp*temp;
    }
    return sqrt(sum);
}