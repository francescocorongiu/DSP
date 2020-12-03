#include "fir.h"
#include "fi_custom.h"
#include "iir.h"
#include "samples.h"
#include "fir_coeffs.h"
#include "iir_coeffs.h"
#include "main.h"


#define BUFF_LEN	256


// define FIR buffers
int buffer_fir[FIR_LEN];
int buffer_asm[FIR_LEN + 2];  //ASM implementation requires 2 extra memory locations

float fbuffer_fir[FIR_LEN];
float fbufferc_fir[FIR_LEN + 1];

// define IIR buffers
float fbuffer1[3 * BIQ_LEN + 1];
float fbuffer2[3 * BIQ_LEN + 1];
float fbuffer_iir2[3 * BIQ_LEN + 1];


// define output buffers
int output[BUFF_LEN];

float output1f[BUFF_LEN];
float output2f[BUFF_LEN];

int output1_fix[BUFF_LEN];
int output2_fix[BUFF_LEN];

float output1i[BUFF_LEN];
float output2i[BUFF_LEN];


// for computing statistics
float err1, err2, err3, err4, power;

// compute total squared error between sequences s1 and s2 (both integer)
float compute_error_int(int* s1, int* s2, int len)
{
	int i;
	float acc = 0, tmp;
	for (i = 0; i < len; i++)
	{
		tmp = (float)s1[i] - s2[i];
		acc += tmp * tmp;
	}
	return acc;
}

// compute total squared error between sequences s1 (integer) and s2 (floating point)
float compute_error_intf(int* s1, float* s2, int len)
{
	int i;
	float acc = 0, tmp;

	for (i = 0; i < len; i++)
	{
		tmp = (float)s1[i] - s2[i];
		acc += tmp * tmp;
	}
	return acc;
}

// compute total squared error between sequences s1 and s2 (both floating point)
float compute_error_f(float* s1, float* s2, int len)
{
	int i;
	float acc = 0, tmp;

	for (i = 0; i < len; i++)
	{
		tmp = s1[i] - s2[i];
		acc += tmp * tmp;
	}
	return acc;
}

int main(void) {

	long i;

	// zero out buffers
	for (i = 0; i < FIR_LEN; i++)
	{
		buffer_fir[i] = 0;
		fbuffer_fir[i] = 0;
		fbufferc_fir[i] = 0;
	}
	fbufferc_fir[FIR_LEN] = 0;
	for (i = 0; i < FIR_LEN + 2; i++)
	{
		buffer_asm[i] = 0;
	}
	for (i = 0; i < 3 * BIQ_LEN + 1; i++)
	{
		fbuffer1[i] = 0;
		fbuffer2[i] = 0;
		fbuffer_iir2[i] = 0;
	}

	err1 = 0;
	err2 = 0;
	err3 = 0;
	err4 = 0;
	power = 0;

	for (i = 0; i < DATA_LEN / BUFF_LEN; i++)
	{
		// write here code to process samples from &samples[i*BUFF_LEN] to &samples[i*BUFF_LEN + BUFF_LEN - 1]

		// FIR floating point version
		//fir_asm(&samples[i * BUFF_LEN], h, output, buffer_asm, BUFF_LEN, FIR_LEN);

		fir_circular_f(&samples[i * BUFF_LEN], hf, output2f, fbufferc_fir, BUFF_LEN, FIR_LEN);
		fir_linear_f(&samples[i * BUFF_LEN], hf, output1f, fbuffer_fir, BUFF_LEN, FIR_LEN);
		// FIR fixed point version
		fir_circular_fixed(&samples[i * BUFF_LEN], h, output2_fix, buffer_fir, BUFF_LEN, FIR_LEN,15);
		fir_linear_fixed(&samples[i * BUFF_LEN], h, output1_fix, buffer_fir, BUFF_LEN, FIR_LEN,15);

		// IIR floating point
		iir_biquad1_f(&samples[i * BUFF_LEN], bf, af, output1i, fbuffer1, fbuffer2, BUFF_LEN, BIQ_LEN);
		iir_biquad2_f(&samples[i * BUFF_LEN], bf, af, output2i, fbuffer_iir2, BUFF_LEN, BIQ_LEN);


		// compute error between floating point and fixed point FIR linear versions
		err1 += compute_error_intf(output1_fix, output1f, BUFF_LEN);

		// compute error between FIR linear and FIR circular
		err2 += compute_error_f(output1f, output2f, BUFF_LEN);

		// compute error between IIR form1 and IIR form2
		err3 += compute_error_f(output1i, output2i, BUFF_LEN);

	}

	err1 /= DATA_LEN;
	err2 /= DATA_LEN;
	err3 /= DATA_LEN;




	for (i = 0; i < DATA_LEN; i++)
		power += (float)samples[i] * (float)samples[i];

	power /= DATA_LEN;

	return 0;
}



