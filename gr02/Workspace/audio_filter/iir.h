
#ifndef IIR_H_
#define IIR_H_



void iir_biquad1_f(int *x, float *b, float *a, float *r, float *dbuf1, float *dbuf2, unsigned short nx, unsigned short nq);
void iir_biquad2_f(int *x, float *b, float *a, float *r, float *dbuf, unsigned short nx, unsigned short nq);

void iir_biquad1_fix(int* x, int* b, int* a, int* r, int* dbuf1, int* dbuf2, unsigned short nx, unsigned short nq, int y);
void iir_biquad2_fix(int* x, int* b, int* a, int* r, int* dbuf, unsigned short nx, unsigned short nq, int y);

// you can add here declarations of other IIR filter functions

#endif /* IIR_H_ */
