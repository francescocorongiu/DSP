
int mpy_fi(int* A, int* B, int* y) {

	long int C = (long) B * A;
	long int add = 0x1 << y - 1;
	C = C + add;
	C = C >> y;

	return C;
}
