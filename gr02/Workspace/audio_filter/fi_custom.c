#include "fi_custom.h"

int mpy_fi(int A, int B, int y) {

	long int C = (long)B * A;
	long int add = 0x1 << y - 1;
	C = C + add;
	C = C >> y;

	return C;
}

//ADD
// 0.5 + 0.6 = 1.1
// 1.05 Q015
