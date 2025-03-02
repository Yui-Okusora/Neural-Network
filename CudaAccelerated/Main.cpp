#include "include/Matrix/Matrix.hpp"
#include "iostream"
#define SIZE 1000
#define SIZE1 1000

void resetGPU();

int main()
{
	float* arrayA = nullptr;
	arrayA = (float*)malloc(sizeof(float) * SIZE);

	for (int i = 0; i < SIZE; ++i) {
		arrayA[i] = i + 1;
	}

	float* arrayB = nullptr;
	
	arrayB = (float*)malloc(sizeof(float) * SIZE * SIZE1);

	for (int i = 0; i < SIZE * SIZE1; ++i) {
		arrayB[i] = i + 1;
	}

	Matrix A(arrayA, 1, SIZE);
	Matrix B(arrayB, SIZE, SIZE1);

	//system("pause");

	/*free(arrayA);
	free(arrayB);*/

	/*A.print();
	std::cout << "\n\n";
	B.print();
	std::cout << "\n\n";*/

	A.dotProduct(B);

	//C.print();
	
	resetGPU();
	return 0;
}