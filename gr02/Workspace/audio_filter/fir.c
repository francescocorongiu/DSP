#include "fi_custom.h"
#include "fir.h"

// FIR filter implemented using linear buffer and inting point arithmetic
// x: pointer to input buffer
// h: pointer to filter coefficients
// r: poiter to output buffer
// dbuffer: pointer to internal delay line (must be at least nh samples)
// nx: number of samples in input buffer
// nh: number of filter coefficients

void fir_linear_f(int* x, float* h, float* r, float* dbuffer, unsigned short nx,
	unsigned short nh) {
	int i, j;
	float acc;

	for (i = 0; i < nx; i++) {
		// load new sample in delay line
		dbuffer[0] = x[i];
		// initialize accumulator with first tap product
		acc = dbuffer[0] * h[0];
		// rest of cycle
		for (j = 1; j < nh; j++) {
			acc += dbuffer[j] * h[j];
		}
		for (j = nh - 2; j >= 0; j--) {
			// shift delay line
			dbuffer[j + 1] = dbuffer[j];
		}
		// write output
		r[i] = acc;
	}
}

// FIR filter implemented using circular buffer
// x: pointer to input buffer
// h: pointer to filter coefficients
// r: poiter to output buffer
// dbuffer: pointer to internal circular buffer (must be at least nh+1 samples)
//		dbuffer[0...nh-1]: stores actual circular buffer
//		dbuffer[nh]: store value of circular pointer
// nx: number of samples in input buffer
// nh: number of filter coefficients

void fir_circular_f(int* x, float* h, float* r, float* dbuffer,
	unsigned short nx, unsigned short nh) {
	int i, j, k, P;
	float acc;

	// recover initial value of circular pointer
	// this implementation assumes that dbuffer has one extra location (nh + 1 locations)
	// the last location is used to store the value of the circular pointer
	P = dbuffer[nh];

	for (i = 0; i < nx; i++) {
		dbuffer[P] = x[i]; //load new sample in buffer, position P

		// initialize accumulator with first tap product
		acc = dbuffer[P] * h[0];

		// cycle with circular pointer: from P+1 to end of the buffer
		for (j = 1, k = P + 1; k < nh; j++, k++) {
			acc += dbuffer[k] * h[j];
		}

		// from the start of the buffer to P-1
		for (k = 0; k < P; j++, k++) {
			acc += dbuffer[k] * h[j];
		}

		// update circular pointer
		if (--P < 0)
			P = nh - 1;

		// write the output
		r[i] = acc;
	}
	// store last value of circular pointer
	dbuffer[nh] = P;
}

//y= number of fixed point bits (WL-1)
void fir_linear_fixed(int* x, int* h, int* r, int* dbuffer, unsigned short nx, unsigned short nh, int y){
	int i, j;
	long acc;

	for (i = 0; i < nx; i++) {
		// load new sample in delay line
		dbuffer[0] = x[i];
		// initialize accumulator with first tap product
		acc = dbuffer[0]*h[0];
		// rest of cycle
		for (j = 1; j < nh; j++) {
			acc += dbuffer[j]*h[j];
		}
		for (j = nh - 2; j >= 0; j--) {
			// shift delay line
			dbuffer[j + 1] = dbuffer[j];
		}
		// write output
		long int add = 0x1 << y - 1;
		acc = acc + add;
		acc = acc >> y;
		r[i] = (int)acc;
	}
}


void fir_circular_fixed(int* x, int* h, int* r, int* dbuffer, unsigned short nx, unsigned short nh, int y)
{
	int i, j, k, P;
	long acc;

	// recover initial value of circular pointer
	// this implementation assumes that dbuffer has one extra location (nh + 1 locations)
	// the last location is used to store the value of the circular pointer
	P = dbuffer[nh];

	for (i = 0; i < nx; i++) {
		dbuffer[P] = x[i]; //load new sample in buffer, position P

		// initialize accumulator with first tap product
		acc = dbuffer[P] * h[0];

		// cycle with circular pointer: from P+1 to end of the buffer
		for (j = 1, k = P + 1; k < nh; j++, k++) {
			acc += dbuffer[k] * h[j];
		}

		// from the start of the buffer to P-1
		for (k = 0; k < P; j++, k++) {
			acc += dbuffer[k] * h[j];
		}

		// update circular pointer
		if (--P < 0)
			P = nh - 1;

		// write the output
		long int add = 0x1 << y - 1;
		acc = acc + add;
		acc = acc >> y;
		r[i] = (int) acc;
	}
	// store last value of circular pointer
	dbuffer[nh] = P;
}
