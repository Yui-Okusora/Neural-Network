#include "include/Matrix/Matrix.hpp"
//#include "include/MemoryManager/MemoryManager.hpp"
//#include "include/AdvancedMemory/AdvancedMemory.hpp"
#include "iostream"
#include <chrono>

#define SIZE size_t(100)
#define SIZE1 size_t(100)

void resetGPU();
void loadMem();

int main()
{
	loadMem();
	MemMng.initManager();

	float a[] = {1,0,1,2,1,1,0,1,1,1,1,2};
	float b[] = {1,2,1,2,3,1,4,2,2};

	Matrix A(a, 4, 3);
	Matrix B(b, 3, 3);
	Matrix C;

	std::cout << A << "\n" << B << "\n";
	
	std::chrono::high_resolution_clock::time_point start, end;
	start = std::chrono::high_resolution_clock::now();

	C = A.dotProduct(B);

	end = std::chrono::high_resolution_clock::now();

	C.print();
	std::cout << "\n";

	std::cout << "Speed: " << std::chrono::duration<long double, std::micro>(end - start).count() << "\n";

	resetGPU();
	return 0;
}




