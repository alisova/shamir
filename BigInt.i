%module BigInt
%{
/* Includes the header in the wrapper code */
#include "BigInt.h"
%}
/* Parse the header file to generate wrappers */
%include "BigInt.h"
