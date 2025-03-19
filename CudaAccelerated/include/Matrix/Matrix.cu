#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "Matrix.hpp"
#include <chrono>
#include <iostream>

#define TILE_WIDTH constexpr 32

namespace YuiOkusora
{
	namespace Cuda
	{
		namespace Matrix
		{
			__global__ static void __cudaAddMatrix(float* a, float *b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;

				if (i < rows * cols)
					a[i] = a[i] + b[i];
				if (i < rows * cols - 1)
					a[i + 1] = a[i + 1] + b[i + 1];
			}

			__global__ static void __cudaAddVal2Matrix(float* a, float* b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;

				if (i < rows * cols)
					a[i] = a[i] + *b;
				if (i < rows * cols - 1)
					a[i + 1] = a[i + 1] + *b;
			}

			__global__ static void __cudaSubtractMatrix(float* a, float* b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;

				if (i < rows * cols)
					a[i] = a[i] - b[i];
				if (i < rows * cols - 1)
					a[i + 1] = a[i + 1] - b[i + 1];
			}
			
			__global__ static void __cudaTranspose(float* a, float* b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x); // rows
				unsigned j = (threadIdx.y + blockDim.y * blockIdx.y); // cols

				if (i < rows && j < cols) {
					unsigned idx = i * cols + j;
					unsigned ridx = j * rows + i;

					b[ridx] = a[idx];
				}

			}

			__global__ static void __cudaMultiplyVal2Matrix(float* a, float* b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;

				if (i < rows * cols)
					a[i] = a[i] * *b;
				if (i < rows * cols - 1)
					a[i + 1] = a[i + 1] * *b;
			}

			__global__ static void __cudaMultiplyMat2Matrix(float* a, float* b, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;

				if (i < rows * cols)
					a[i] = a[i] * b[i];
				if (i < rows * cols - 1)
					a[i + 1] = a[i + 1] * b[i + 1];
			}

			__global__ static void __cudaRotateMatrix180(float* a, size_t rows, size_t cols)
			{
				unsigned i = (threadIdx.x + blockDim.x * blockIdx.x) * 2;
				float tmp = 0;
				const size_t size = rows * cols;
				if (i < size / 2)
				{
					tmp = a[i];
					a[i] = a[size - i - 1];
					a[size - i - 1] = tmp;
				}
					
				if (i < (size / 2) - 1)
				{
					tmp = a[i + 1];
					a[i + 1] = a[size - i - 2];
					a[size - i - 2] = tmp;
				}
			}

			__global__ static void __cudaDotProductMatrix(float* a, float* b, float* c, size_t Arows, size_t Acols, size_t Bcols)
			{
				unsigned i = threadIdx.y + blockIdx.y * blockDim.y; // rows
				unsigned j = threadIdx.x + blockIdx.x * blockDim.x; // cols
				unsigned k = threadIdx.z + blockIdx.z * blockDim.z;

				if (i >= Arows || j >= Bcols) return;

				c[i * Bcols + j] += a[i * Acols + k] * b[k * Bcols + j];
			}
			
		}
	}
	namespace Math
	{
		namespace Mat
		{
			void addMatrix(Matrix *a, Matrix &b)
			{
				float* aPtr, *bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float) * a->getCols() * a->getRows());

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());
				ViewOfAdvancedMemory& viewB = b.load(0, sizeof(float) * a->getCols() * a->getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, b.getViewPtr(viewB), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				b.unload(viewB.lpMapAddress);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 2));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaAddMatrix<<<blockNum, threadsPerBlock>>>(aPtr, bPtr, a->getRows(), a->getCols());
				
				cudaDeviceSynchronize();
				
				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
				cudaFree(bPtr);
				
			}

			void addVal2Matrix(Matrix* a, const float &b)
			{
				float* aPtr, * bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float));

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, &b, sizeof(float), cudaMemcpyHostToDevice);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 2));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaAddVal2Matrix << <blockNum, threadsPerBlock >> > (aPtr, bPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
				cudaFree(bPtr);

			}

			void subtractMatrix(Matrix* a, Matrix& b)
			{
				float* aPtr, * bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float) * a->getCols() * a->getRows());

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());
				ViewOfAdvancedMemory& viewB = b.load(0, sizeof(float) * a->getCols() * a->getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, b.getViewPtr(viewB), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				b.unload(viewB.lpMapAddress);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 2));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaSubtractMatrix << <blockNum, threadsPerBlock >> > (aPtr, bPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
				cudaFree(bPtr);

			}
		
			void transposeMatrix(Matrix* a)
			{
				float* aPtr, *bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float) * a->getCols() * a->getRows());

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);

				dim3 blockNum(0,0,1), threadNum(0,0,1);
				for (int i = 1; i <= 32;++i)
				{
					threadNum.x = i;
					blockNum.x = ceil(float(a->getRows()) / float(i));
					if (blockNum.x <= 208) break;
				}
				
				for (int i = 1; i <= 32;++i)
				{
					threadNum.y = i;
					blockNum.y = ceil(float(a->getCols()) / float(i));
					if (blockNum.y <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaTranspose<<<blockNum, threadNum>>>(aPtr, bPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), bPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);

				cudaFree(aPtr);
				cudaFree(bPtr);

			}
		
			void multiplyVal2Matrix(Matrix* a, const float& b)
			{
				float* aPtr, * bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float));

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, &b, sizeof(float), cudaMemcpyHostToDevice);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 2));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaMultiplyVal2Matrix << <blockNum, threadsPerBlock >> > (aPtr, bPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
				cudaFree(bPtr);
			}

			void multiplyMat2Matrix(Matrix* a, Matrix& b)
			{
				float* aPtr, * bPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());
				cudaMalloc(&bPtr, sizeof(float) * b.getCols() * b.getRows());

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());
				ViewOfAdvancedMemory& viewB = b.load(0, sizeof(float) * b.getCols() * b.getRows());

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, b.getViewPtr(viewB), sizeof(float) * b.getCols() * b.getRows(), cudaMemcpyHostToDevice);

				b.unload(viewB.lpMapAddress);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 2));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaMultiplyMat2Matrix << <blockNum, threadsPerBlock >> > (aPtr, bPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
				cudaFree(bPtr);
			}

			void dotProductMatrix(Matrix* a, Matrix& b)
			{
				float* aPtr = nullptr, * bPtr = nullptr, * cPtr = nullptr;
				auto aSIZE = a->getRows() * a->getCols(), bSIZE = b.getRows() * b.getCols(), cSIZE = a->getRows() * b.getCols();

				std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

				cudaMalloc(&aPtr, sizeof(float) * (static_cast<size_t>(aSIZE) + bSIZE + cSIZE));
				bPtr = aPtr + bSIZE;
				cPtr = aPtr + cSIZE;

				cudaMemset(cPtr, 0, sizeof(float) * cSIZE);

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * aSIZE);
				ViewOfAdvancedMemory& viewB = b.load(0, sizeof(float) * bSIZE);

				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * aSIZE, cudaMemcpyHostToDevice);
				cudaMemcpy(bPtr, b.getViewPtr(viewB), sizeof(float) * bSIZE, cudaMemcpyHostToDevice);

				a->unload(viewA.lpMapAddress);
				b.unload(viewB.lpMapAddress);

				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

				std::cout << std::chrono::duration<long long, std::nano>(end - start).count() << "\n";

				dim3 blockNum(0,0,1), threadNum(0,0,1);

				for (unsigned i = 1; i <= 16; i = i << 2)
				{
					threadNum.z = i;
					blockNum.z = (unsigned)ceil(float(a->getCols()) / float(threadNum.z));
					if (blockNum.z <= 26) break;
				}

				for (unsigned i = 1; i <= 32 / sqrt(threadNum.z); i = i << 1)
				{
					threadNum.y = i;
					blockNum.y = (unsigned)ceil(float(a->getRows()) / float(threadNum.y));
					if (blockNum.y <= 26) break;
				}

				for (unsigned i = 1; i <= 1024; i = i << 1)
				{
					float tmp = i / float(threadNum.y * threadNum.z);
					blockNum.x = (unsigned)ceil(float(b.getCols()) / tmp);
					threadNum.x = (unsigned)ceil(tmp);
					if (blockNum.x <= 26) break;
				}

				//threadNum.y = 1024 / threadNum.x;
				//blockNum.y = (unsigned)ceil(float(b.getCols()) / float(threadNum.y));

				YuiOkusora::Cuda::Matrix::__cudaDotProductMatrix<<<blockNum, threadNum>>>(aPtr, bPtr, cPtr, a->getRows(), a->getCols(), b.getCols());

				cudaDeviceSynchronize();

				a->resize(cSIZE);
				ViewOfAdvancedMemory& viewC = a->load(0, sizeof(float) * cSIZE);

				cudaMemcpy(a->getViewPtr(viewC), cPtr, sizeof(float) * cSIZE, cudaMemcpyDeviceToHost);
				a->unload(viewC.lpMapAddress);

				cudaFree(aPtr);
				cudaFree(bPtr);
				cudaFree(cPtr);
			}

			void rotateMatrix180(Matrix* a)
			{
				float* aPtr;
				cudaMalloc(&aPtr, sizeof(float) * a->getCols() * a->getRows());

				ViewOfAdvancedMemory& viewA = a->load(0, sizeof(float) * a->getCols() * a->getRows());
				
				cudaMemcpy(aPtr, a->getViewPtr(viewA), sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyHostToDevice);

				unsigned blockNum = 1, threadsPerBlock = 0;
				for (int i = 0; i <= 5; ++i) {
					threadsPerBlock = 32 << i;
					blockNum = ceil(float(a->getCols() * a->getRows()) / float(threadsPerBlock * 4));
					if (blockNum <= 208) break;
				}

				YuiOkusora::Cuda::Matrix::__cudaRotateMatrix180 << <blockNum, threadsPerBlock >> > (aPtr, a->getRows(), a->getCols());

				cudaDeviceSynchronize();

				cudaMemcpy(a->getViewPtr(viewA), aPtr, sizeof(float) * a->getCols() * a->getRows(), cudaMemcpyDeviceToHost);
				a->unload(viewA.lpMapAddress);
				
				cudaFree(aPtr);
			}
		}
	}
};