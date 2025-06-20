#include <math.h>
#include <stdint.h>

const float Vref = 3.3; // Reference voltage (in volts)
float result;
int max_count;

float count2volt(int res, int count){

	    max_count = pow(2, res) - 1;

	    result = Vref / (max_count * count);

	    return result;
}
