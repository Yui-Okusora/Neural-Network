#pragma once

#define NeuroSoraCore_BaseLayer

#include "NeuroSoraCore.hpp"
#include "Matrix/Matrix.hpp"

class Layer
{
public:
	virtual void feedforward(Matrix& input, Matrix& output) {}
	virtual void backpropagate(Matrix& inputGradient, Matrix& outputGradient){}
	static float eta; // overall layer learning rate
	static float alpha; // [0.0...n] multiplier of last weight change [momentum]
	static float reluParam;
};

