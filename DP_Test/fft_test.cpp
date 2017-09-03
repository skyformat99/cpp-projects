#include <cmath>
#include <string>
#include <iostream>
#include <complex>
#include <fstream>
#include <fftw3.h>
#include "Timer.h"
#include <mex.h>    //  Definitions for Matlab API

/**
 * Utilizing the exact same FFT algorithm that Matlab is using: FFTW, 
 * which can be found at 'http://www.fftw.org/'. 
 * 
 * The constants defined below were from the ER10b calibration, the
 * source of the test files used.
 *
 * To compile, run the following:
 *
 *   cl /O2 fft_test.cpp /link Timer.obj libfftw3l-3.lib libfftw3-3.lib libfftw3-3.lib
 *
 */

/** Number of points to use in FFT **/
//#define N_FFT 390625
//#define N_FFT 524288

#define DEBUG1

// Req'd for C++ calls
using namespace std;

/**
 * Function usage: 
 *   [mean_fft] = fft_test(waves)
 */
void mexFunction(int nlhs, mxArray *plhs[],
      int nrhs, const mxArray *prhs[])
{
    if (nrhs != 2 || nlhs != 1)
        mexErrMsgTxt("Usage: [mean_fft] = fft_test(waves,N_FFT)\n");

    int numVals, numVecs;
    numVecs = mxGetM(prhs[0]);
    numVals = mxGetN(prhs[0]);
    
    int N_FFT = (int)mxGetScalar(prhs[1]);
    
#ifdef DEBUG1
    mexPrintf("numVecs: %d numVals: %d N_FFT: %d\n",numVecs,numVals,N_FFT);
#endif
    
    int i,j;
    double *d, *temp;
    
    // Determine if we need to copy into a new array or not
    if (N_FFT == numVals) {
        d = mxGetPr(prhs[0]);
    } else {
        d = (double*)mxMalloc(sizeof(double)*N_FFT*numVecs);
        
        temp = mxGetPr(prhs[0]);
        
        for (j=0;j<numVecs;j++) {
          for (i=0;i<numVals;i++)
              d[i+(j*N_FFT)] = temp[i+(j*numVals)];
        
          for (i=numVals;i<N_FFT;i++)
              d[i+(j*N_FFT)] = 0;
        }
    }
    
    // Create output arry and get pointer
    if (!(plhs[0] = mxCreateDoubleMatrix(N_FFT,1,mxREAL)) )
        mexErrMsgTxt("Error: unable to create output array!");
    
    double* meanFFT;
    if ((meanFFT = mxGetPr(plhs[0])) == NULL)
        mexErrMsgTxt("Error: real data is null!");
    
//    double* meanFFT = (double*)mxMalloc(sizeof(double) * N_FFT);

#ifdef DEBUG1
    mexPrintf("Starting FFT computation\n");
#endif

    Timer t;
    t.reset();
    
    fftw_complex *out;
    fftw_plan p;

    double constant = 20. * log10( (4. / numVals) / 0.0015) + 64;

    for (i=0;i<N_FFT;i++)
      meanFFT[i] = 0.;

    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N_FFT);

    // Compute FFT over each input vector
    for (j=0;j<numVecs;j++) {
      p = fftw_plan_dft_r2c_1d(N_FFT, d+(j*N_FFT), out, FFTW_ESTIMATE);
      fftw_execute(p);

      for (i=0;i<N_FFT;i++)
        meanFFT[i] += 20. * log10( sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]));
      
      mexPrintf("  Vector %d (%0.3f sec)\n",j,t.elapsed());
    }
    
    for (i=0;i<N_FFT;i++)
        meanFFT[i] = (meanFFT[i] / numVecs) + constant;
    
#ifdef DEBUG1
    mexPrintf("Finished computing FFT (Elapsed Time: %0.3f sec)\n",t.elapsed());
#endif
    
    fftw_destroy_plan(p);
    fftw_free(out);
    
    if (N_FFT != numVals)
        mxFree(d);

    return;
}

int main (int argc, char* argv[]) {

  int N_FFT = 390625;
  
  // Load in the data
  fstream filestr ("collected_waves.txt", fstream::in);

  if (!filestr.is_open())
    throw "Error opening file stream";

  int numVecs, numVals;

  filestr >> numVecs;

  cout << "Number of vectors: " << numVecs << endl;

  int i,j;
  double sqrt2 = sqrt(2.);
  double temp;

  double** d = new double*[numVecs];

  for (j=0;j<numVecs;j++) {

    d[j] = new double[N_FFT];

    filestr >> numVals;
    cout << "  Number of values in vector: " << numVals << endl;

    for (i=0;i<numVals;i++) {
      filestr >> temp;
      d[j][i] = temp / sqrt2;
    }

    for (i=numVals;i<N_FFT;i++)
      d[j][i] = 0;
  }

  filestr.close();

  // Start timer, start computation
  Timer t;

  double* meanFFT = new double[N_FFT];
  double a,b;
  fftw_complex *out;
  fftw_plan p;

  double constant = 20. * log10( (4. / numVals) / 0.0015) + 64;

  for (i=0;i<N_FFT;i++)
    meanFFT[i] = 0.;

  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N_FFT);

  // Compute FFT over each input vector
  for (j=0;j<numVecs;j++) {
    p = fftw_plan_dft_r2c_1d(N_FFT, d[j], out, FFTW_ESTIMATE);
    fftw_execute(p);

    for (i=0;i<N_FFT;i++)
      meanFFT[i] += 20. * log10( sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]));

    t.print();
  }

  fftw_destroy_plan(p);
  fftw_free(out);

  for (i=0;i<N_FFT;i++)
    meanFFT[i] = (meanFFT[i] / numVecs) + constant;

  t.print("Executed FFTs ...");

  // Now write out the mean FFT
  fstream outfile ("output_fft.txt", fstream::out);

  if (!outfile.is_open())
    throw "Error opening file stream";

  for (i=0;i<N_FFT;i++)
    outfile << meanFFT[i] << endl;

  outfile.close();  

  return 0;
}
