
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include "iostream"

void resetGPU()
{
	/*cudaDeviceProp DeviceProperties;
	cudaGetDeviceProperties(&DeviceProperties, 0);
	int GPUcount = 0;
	cudaGetDeviceCount(&GPUcount);
	std::cout << GPUcount;*/
	cudaDeviceReset();
}