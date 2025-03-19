
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include "iostream"

void loadMem()
{
	float* ptr;
	cudaMalloc(&ptr, sizeof(float));
	cudaFree(ptr);
}

void resetGPU()
{
	/*cudaDeviceProp DeviceProperties;
	cudaGetDeviceProperties(&DeviceProperties, 0);
	int GPUcount = 0;
	cudaGetDeviceCount(&GPUcount);
	std::cout << GPUcount;*/
	cudaDeviceReset();
}