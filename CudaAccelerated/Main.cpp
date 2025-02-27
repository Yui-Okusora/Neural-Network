#include "include/Matrix/Matrix.hpp"
#include "iostream"
#define SIZE 10
#define SIZE1 5

void resetGPU();

int main()
{
	double* arrayA = nullptr;
	arrayA = (double*)malloc(sizeof(double) * 12);

	for (int i = 0; i < SIZE; ++i) {
		arrayA[i] = i + 1;
	}

	double* arrayB = nullptr;
	
	arrayB = (double*)malloc(sizeof(double) * 9);

	for (int i = 0; i < SIZE * SIZE1; ++i) {
		arrayB[i] = i + 1;
	}

	Matrix A(arrayA, 1, SIZE);
	Matrix B(arrayB, SIZE, SIZE1);

	/*A.print();
	std::cout << "\n\n";
	B.print();
	std::cout << "\n\n";*/

	Matrix C = A.dotProduct(B);

	//C.print();
	
	resetGPU();
	return 0;
}