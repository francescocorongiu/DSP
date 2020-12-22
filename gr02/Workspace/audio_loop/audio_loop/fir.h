#ifndef FIR_H_
#define FIR_H_


// ASM optimized filters, Q0.15 format
// unsigned short fir_asm(int *x, int *h, int *r, int *dbuffer, unsigned short nx, unsigned short nh);
// unsigned short fir2_asm(int *x, int *h, int *r, int *dbuffer, unsigned short nx, unsigned short nh);


void fir_linear_f(int *x, float *h, float *r, float *dbuffer, unsigned short nx, unsigned short nh);
void fir_circular_f(int *x, float *h, float *r, float *dbuffer, unsigned short nx, unsigned short nh);

void fir_linear_fixed(int *x, int *h, int *r, int *dbuffer, unsigned short nx, unsigned short nh, int y);
void fir_circular_fixed(int *x, int *h, int *r, int *dbuffer, unsigned short nx, unsigned short nh, int y);


// you can add here declarations of other FIR filter functions


#endif /* FIR_H_ */
