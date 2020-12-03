#include "fir.h"
#include "fi_custom.h"
#include "iir.h"
#include "samples.h"
#include "fir_coeffs.h"
#include "iir_coeffs.h"
#include "main.h"


#define BUFF_LEN	256


// define FIR buffers

//fir fixed p.
int buffer_fix_l_fir[FIR_LEN];
int buffer_fix_c_fir[FIR_LEN];

//ASM implementation requires 2 extra memory locations
int buffer_asm[FIR_LEN + 2];  

//fir floating p.
float buffer_f_lin_fir[FIR_LEN];
float buffer_f_cir_fir[FIR_LEN + 1];

// define IIR buffers
float buffer_f_f1_1_iir[3 * BIQ_LEN + 1];
float buffer_f_f1_2_iir[3 * BIQ_LEN + 1];
float buffer_f_f2_iir[3 * BIQ_LEN + 1];

int buffer_fix_1_iir_f1[3 * BIQ_LEN + 1];
int buffer_fix_2_iir_f1[3 * BIQ_LEN + 1];
int buffer_fix_iir_f2[3 * BIQ_LEN + 1];

// define output buffers

float output1f_fir[BUFF_LEN];
float output2f_fir[BUFF_LEN];

int output1_fix_fir[BUFF_LEN];
int output2_fix_fir[BUFF_LEN];

float output_f1_iir[BUFF_LEN];
float output_f2_iir[BUFF_LEN];

int output_f1_iir_fix[BUFF_LEN];
int output_f2_iir_fix[BUFF_LEN];


// for computing statistics
float	err_fir_lin_float_vs_fixed,
		err_fir_float_lin_vs_circ,
		err_iir_float_form1_vs_form2,
		err_fir_circ_float_vs_fixed,
		err_iir_f1_float_vs_fixed,
		err_iir_f2_float_vs_fixed,
		err4,
		power;

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
		buffer_fix_l_fir[i] = 0;
		buffer_fix_c_fir[i] = 0;
		buffer_f_lin_fir[i] = 0;
		buffer_f_cir_fir[i] = 0;
	}
	buffer_f_cir_fir[FIR_LEN] = 0;
	for (i = 0; i < FIR_LEN + 2; i++)
	{
		buffer_asm[i] = 0;
	}
	for (i = 0; i < 3 * BIQ_LEN + 1; i++)
	{
		//float
		buffer_f_f1_1_iir[i] = 0;
		buffer_f_f1_2_iir[i] = 0;
		buffer_f_f2_iir[i] = 0;
		//fixed
		buffer_fix_2_iir_f1[i] = 0;
		buffer_fix_iir_f2[i] = 0;
		buffer_fix_1_iir_f1[i] = 0;
	}

	err_fir_lin_float_vs_fixed = 0;
	err_fir_float_lin_vs_circ = 0;
	err_iir_float_form1_vs_form2 = 0;
	err_fir_circ_float_vs_fixed = 0;
	err_iir_f1_float_vs_fixed = 0;
	err_iir_f2_float_vs_fixed = 0;
	err4 = 0;
	power = 0;

	for (i = 0; i < DATA_LEN / BUFF_LEN; i++)
	{
		// write here code to process samples from &samples[i*BUFF_LEN] to &samples[i*BUFF_LEN + BUFF_LEN - 1]

		// FIR floating point version
		//fir_asm(&samples[i * BUFF_LEN], h, output, buffer_asm, BUFF_LEN, FIR_LEN);

		fir_circular_f(&samples[i * BUFF_LEN], hf, output2f_fir, buffer_f_cir_fir, BUFF_LEN, FIR_LEN);
		fir_linear_f(&samples[i * BUFF_LEN], hf, output1f_fir, buffer_f_lin_fir, BUFF_LEN, FIR_LEN);
		// FIR fixed point version
		fir_circular_fixed(&samples[i * BUFF_LEN], h, output2_fix_fir, buffer_fix_c_fir, BUFF_LEN, FIR_LEN, 15);
		fir_linear_fixed(&samples[i * BUFF_LEN], h, output1_fix_fir, buffer_fix_l_fir, BUFF_LEN, FIR_LEN, 15);

		// IIR floating point
		iir_biquad1_f(&samples[i * BUFF_LEN], bf, af, output_f1_iir, buffer_f_f1_1_iir, buffer_f_f1_2_iir, BUFF_LEN, BIQ_LEN);
		iir_biquad2_f(&samples[i * BUFF_LEN], bf, af, output_f2_iir, buffer_f_f2_iir, BUFF_LEN, BIQ_LEN);

		// IIR fixed point
		iir_biquad1_fix(&samples[i * BUFF_LEN], b, a, output_f1_iir_fix, buffer_fix_1_iir_f1, buffer_fix_2_iir_f1, BUFF_LEN, BIQ_LEN, 12);
		iir_biquad2_fix(&samples[i * BUFF_LEN], b, a, output_f2_iir_fix, buffer_fix_iir_f2, BUFF_LEN, BIQ_LEN, 12);


		// compute error between floating point and fixed point FIR linear versions
		err_fir_lin_float_vs_fixed += compute_error_intf(output1_fix_fir, output1f_fir, BUFF_LEN);

		// compute error between floating point and fixed point FIR circular versions
		err_fir_circ_float_vs_fixed += compute_error_intf(output2_fix_fir, output2f_fir, BUFF_LEN);

		// compute error between FIR linear and FIR circular
		err_fir_float_lin_vs_circ += compute_error_f(output1f_fir, output2f_fir, BUFF_LEN);

		// compute error between IIR form1 and IIR form2
		err_iir_float_form1_vs_form2 += compute_error_f(output_f1_iir, output_f2_iir, BUFF_LEN);

		// compute error between floating point and fixed point IIR form1 and IIR form1
		err_iir_f1_float_vs_fixed += compute_error_intf(output_f1_iir_fix, output_f1_iir, BUFF_LEN);

		// compute error between floating point and fixed point IIR form2 and IIR form2
		err_iir_f2_float_vs_fixed += compute_error_intf(output_f2_iir_fix, output_f2_iir, BUFF_LEN);

	}
	float output_f1_iir_fix_val1 = (float)output_f1_iir_fix[0];
	float output_f1_iir_fix_val2 = (float)output_f1_iir_fix[1];
	float output_f1_iir_fix_val3 = (float)output_f1_iir_fix[2];

	err_fir_circ_float_vs_fixed /= DATA_LEN;
	err_fir_lin_float_vs_fixed /= DATA_LEN;
	err_fir_float_lin_vs_circ /= DATA_LEN;
	err_iir_float_form1_vs_form2 /= DATA_LEN;
	err_iir_f2_float_vs_fixed /= DATA_LEN;
	err_iir_f1_float_vs_fixed /= DATA_LEN;



	for (i = 0; i < DATA_LEN; i++)
		power += (float)samples[i] * (float)samples[i];

	power /= DATA_LEN;

	return 0;
}



