// Examples of bit shifting and masking to select a field
//
#include <stdio.h>

main()
{
    short int y = 0xAbcd;
    printf("%hx\n", y);
    short int z = (y >> 12);  // We want bits y[12:...]
    printf("%hx\n", z);
    short int w = z & 0x000f; // We want bits y[12:15]
    printf("%hx\n", w);

	// Get bits y[9:11]
    printf("%hx, %hx, %hx\n", y, y >> 9, (y >> 9) & 0x0007);

    // Get bits y[0:8] -- if you want a negative number,
    // either sign extend the result or subtract 2^n from
    // the result (you can figure out n).
    //
    printf("%hx, %hx\n", y, y & 0x01ff);
}
