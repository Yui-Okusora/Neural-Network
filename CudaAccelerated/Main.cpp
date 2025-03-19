//#include "include/Matrix/Matrix.hpp"
#include "include/MemoryManager/MemoryManager.hpp"
#include "include/AdvancedMemory/AdvancedMemory.hpp"
#include "iostream"
#include <chrono>

#define SIZE size_t(268435456 * 3)
#define SIZE1 size_t(100)

void resetGPU();
void loadMem();

int main()
{
	loadMem();
	MemMng.initManager();

	std::cout << MemMng.getSysGranularity() << "\n";

	AdvancedMemory adMem1, adMem2;

	MemMng.createTmp(&adMem1, sizeof(unsigned int) * SIZE);
	MemMng.createTmp(&adMem2, sizeof(unsigned int) * SIZE);
	//adMem1.load(0, sizeof(unsigned int) * SIZE);
	//adMem2.load(0, sizeof(unsigned int) * SIZE);
	//adMem2.load(0, sizeof(float) * SIZE * SIZE1);
	
	//float* arrayA = nullptr;
	//arrayA = (float*)malloc(sizeof(float) * SIZE);

	for (unsigned long long i = 0; i < 1000; ++i) {
		ViewOfAdvancedMemory& view = adMem1.load(sizeof(unsigned int) * (SIZE / 1000) * i, sizeof(unsigned int) * (SIZE / 1000));
		for (unsigned long long j = 0; j < (SIZE / 1000); ++j)
			adMem1.at<unsigned int>(j, view) = i * (SIZE / 1000) + j + 1;
		adMem1.unload(view.lpMapAddress);
	}

	/*for (int i = 0; i < SIZE; ++i)
	{
		std::cout << adMem1.at<float>(i) << " ";
	}
	std::cout << "\n";*/

	//float* arrayB = nullptr;
	
	//arrayB = (float*)malloc(sizeof(float) * SIZE * SIZE1);

	std::cout << "Start first copy\n";

	std::chrono::high_resolution_clock::time_point start, end;
	double benchmarkDur;

	for (int i = 0; i < 0; ++i) {
		start = std::chrono::high_resolution_clock::now();

		ViewOfAdvancedMemory& view1 = adMem1.load(0, sizeof(unsigned int) * SIZE);
		ViewOfAdvancedMemory& view2 = adMem2.load(0, sizeof(unsigned int) * SIZE);

		memcpy(adMem2.getViewPtr(view2), adMem1.getViewPtr(view1), view1.dwMapViewSize);

		adMem1.unload(view1.lpMapAddress);
		adMem2.unload(view2.lpMapAddress);

		end = std::chrono::high_resolution_clock::now();

		benchmarkDur = std::chrono::duration<long double, std::ratio<1, 1>>(end - start).count();

		std::cout << benchmarkDur << "\n";
		std::cout << "Speed: " << (sizeof(unsigned int) * SIZE) / benchmarkDur << "B/s\n";
	}

	/*for (unsigned long long i = 0; i < 1000; ++i) {
		adMem1.load(sizeof(unsigned int) * (SIZE / 1000) * i, sizeof(unsigned int) * (SIZE / 1000));
		adMem2.load(sizeof(unsigned int) * (SIZE / 1000) * i, sizeof(unsigned int) * (SIZE / 1000));
		for (unsigned long long j = 0; j < (SIZE / 1000); ++j)
		{
			if (adMem2.at<unsigned int>(j) != i * (SIZE / 1000) + j + 1)
			{
				std::cout << "lost memory\n";
				break;
			}
		}
	}*/

	std::cout << "Delayed" << std::endl;
	Sleep(1000);

	std::cout << "Start second copy\n";

	for (int i = 0; i < 1; ++i) {
		start = std::chrono::high_resolution_clock::now();

		MemMng.memcopy(&adMem2, &adMem1, sizeof(unsigned int), adMem1.getFileSize());

		end = std::chrono::high_resolution_clock::now();

		benchmarkDur = std::chrono::duration<long double, std::ratio<1, 1>>(end - start).count();

		std::cout << benchmarkDur << "\n";
		std::cout << "Speed: " << (sizeof(unsigned int) * SIZE) / benchmarkDur << "B/s\n";
	}

	std::cout << MemMng.getUsedMemory() << "\n";

	//std::cout << std::chrono::duration<long double, std::micro>(end - start).count() << "\n";
	/*for (int i = 0; i < SIZE; ++i) {
		adMem2.load(sizeof(float) * SIZE1 * i, sizeof(float) * SIZE1);
		for(int j = 0; j < SIZE1; ++j)
			adMem2.at<float>(j) = i * SIZE1 + j + 1.0f;
	}*/
	
	/*Matrix A(arrayA, 1, SIZE);
	Matrix B(arrayB, SIZE, SIZE1);*/

	
	
	//A.dotProduct(B);
	
	
	//adMem2.load(0, sizeof(float) * SIZE);

	for (unsigned long long i = 0; i < 1000; ++i) {
		ViewOfAdvancedMemory& view1 =  adMem1.load(sizeof(unsigned int) * (SIZE / 1000) * i, sizeof(unsigned int) * (SIZE / 1000));
		ViewOfAdvancedMemory& view2 =  adMem2.load(sizeof(unsigned int) * (SIZE / 1000) * i, sizeof(unsigned int) * (SIZE / 1000));
		for (unsigned long long j = 0; j < (SIZE / 1000); ++j)
		{
			if (adMem2.at<unsigned int>(j, view2) != i * (SIZE / 1000) + j + 1)
			{
				std::cout << "lost memory\n";
				break;
			}
		}
		adMem1.unload(view1.lpMapAddress);
		adMem2.unload(view2.lpMapAddress);
	}
	
	for (unsigned long long i = 0; i < SIZE; ++i)
	{
		//std::cout << adMem2.at<float>(i) << "\n";
		break;
		
	}

	//free(arrayA);

	//system("pause");

	/*free(arrayA);
	free(arrayB);*/
	
	resetGPU();
	return 0;
}




