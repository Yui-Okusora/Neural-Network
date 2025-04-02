#include "include/NeuroSoraCore.hpp"

#include "AdvancedMemory/AdvancedMemory.hpp"
#include "MemoryManager/MemoryManager.hpp"
#include "Net/Net.hpp"
#include "Layer/FullyConnLayer/FullyConnLayer.hpp"

#define SIZE size_t(100)
#define SIZE1 size_t(100)

void resetGPU();
void loadMem();

int main()
{
	loadMem();
	MemMng.initManager();
	MemMng.setTmpDir("tmp\\");

	srand((unsigned int)time(0));

	Net net1;

	std::chrono::high_resolution_clock::time_point start, end;

	net1.layers.push_back(new FullyConnLayer(2,3, ActivationType::Tanh));
	net1.layers.push_back(new FullyConnLayer(3,3, ActivationType::Tanh));
	net1.layers.push_back(new FullyConnLayer(3,3, ActivationType::Tanh));
	net1.layers.push_back(new FullyConnLayer(3,1, ActivationType::Tanh));

	Matrix input(1, 2),target(1, 1);

	for (int i = 1; i <= 20; ++i)
	{
		int randomVal1 = (rand() % 10 + 1) / 10;
		int randomVal2 = (rand() % 10 + 1) / 10;
		int trueAns = randomVal1 ^ randomVal2;
		ViewOfAdvancedMemory& viewInput = input.load(0, sizeof(float) * input.getCols());
		input.getValue(0, 0, viewInput) = (float)randomVal1;
		input.getValue(0, 1, viewInput) = (float)randomVal2;
		input.unload(viewInput);

		ViewOfAdvancedMemory& viewTarget = target.load(0, sizeof(float) * target.getCols());
		target.getValue(0, 0, viewTarget) = (float)trueAns;
		//target.getValue(0, 1, viewTarget) = !trueAns;
		target.unload(viewTarget);

		net1.m_inputVals = input;

		start = std::chrono::high_resolution_clock::now();

		net1.feedforward();
		net1.calcError(target);

		net1.backpropagation(0.027f);

		end = std::chrono::high_resolution_clock::now();

		std::cout << "Completed after " << std::chrono::duration<long double, std::micro>(end - start).count() << "us\n";

		if (i % 10 == 0) {
			std::cout << "\nEpoch: " << i << "\nInput:\n";
			input.print();
			std::cout << "\nTarget:\n";
			target.print();
			std::cout << "\nOutput:\n";
			net1.m_outputVals.print();
			std::cout << "\nError: ";
			std::cout << net1.m_recentAverageError << std::endl;
		}
	}

	resetGPU();
	system("pause");
	return 0;
}




