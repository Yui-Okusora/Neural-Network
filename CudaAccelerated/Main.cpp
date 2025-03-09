#include "include/Matrix/Matrix.hpp"
#include "include/MemoryManager/MemoryManager.hpp"
#include "include/AdvancedMemory/AdvancedMemory.hpp"
#include "iostream"

#define SIZE 10
#define SIZE1 10

void resetGPU();

int main()
{
	MemMng.initManager();
	
	int* arrayA = nullptr;
	arrayA = (int*)malloc(sizeof(int) * SIZE);

	for (int i = 0; i < SIZE; ++i) {
		arrayA[i] = i + 1;
	}

	float* arrayB = nullptr;
	
	arrayB = (float*)malloc(sizeof(float) * SIZE * SIZE1);

	for (int i = 0; i < SIZE * SIZE1; ++i) {
		arrayB[i] = i + 1.0f;
	}
	free(arrayB);
	//Matrix A(arrayA, 1, SIZE);
	//Matrix B(arrayB, SIZE, SIZE1);

	AdvancedMemory adMem1;
	MemMng.createTmp(&adMem1, sizeof(int) * SIZE);
	adMem1.load(0, sizeof(int) * SIZE);
	memcpy(adMem1.getViewPtr(), arrayA, sizeof(int) * SIZE);

	adMem1.at<int>(9) = 2;

	for (int i = 0; i < SIZE; ++i)
	{
		std::cout << adMem1.at<int>(i) << " ";
	}

	free(arrayA);

	system("pause");

	/*free(arrayA);
	free(arrayB);*/

	/*A.print();
	std::cout << "\n\n";
	B.print();
	std::cout << "\n\n";*/

	//A.dotProduct(B);

	

	//C.print();
	
	resetGPU();
	return 0;
}