#ifndef EXAMPLEHPF_H
#define EXAMPLEHPF_H

extern IIRTransposeFilter ExampleHPFFilter;

#endif /* EXAMPLEHPF_H */

/* The following C-code fragment demonstrates how to call the filter routine
#include "IIR_Filter.h"
#include "ExampleHPF.h"

// NUM_SAMPLES defines the number of samples in one block of input data.
// This value should be changed as needed for the application
#define NUM_SAMPLES 100

{
   // Declare input and output sample arrays.
   int  inSamples[NUM_SAMPLES], outSamples[NUM_SAMPLES];


   // Call the IIRTransposeFilterInit routine to zero out the state variables
   IIRTransposeFilterInit( &ExampleHPFFilter );

   // Call BlockIIRTransposeFilter for each block of input samples
   // This routine would normally be called inside a FOR or a DO-WHILE loop
   // Only one instance has been shown
   BlockIIRTransposeFilter( &ExampleHPFFilter, &inSamples[0], &outSamples[0], NUM_SAMPLES );
}
*/
