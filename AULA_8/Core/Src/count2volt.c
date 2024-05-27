#include <math.h>
#include <stdint.h>

#define Vref 3.3 // Reference voltage (in volts)

int max_count;

float count2volt(uint16_t res, uint16_t count){

	    max_count = pow(2, res) - 1;

	    return(Vref / max_count * count);
}
