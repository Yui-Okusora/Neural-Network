#pragma once
#include "HeaderLibs.hpp"
#include "Tensor.hpp"
using namespace std;

class ConvolutionalLayer
{
public:
	ConvolutionalLayer();
	ConvolutionalLayer(unsigned InpHeight, unsigned InpWidth, unsigned InpDepth, unsigned OutNum, unsigned kernelSize, ActivationType Activation);
	Tensor feedForward(Tensor &input);
	Tensor backward(Tensor outputGradient);
	void createKernel(unsigned size);
	friend ofstream &operator<<(ofstream &out, ConvolutionalLayer &a);
	friend ifstream &operator>>(ifstream &in, ConvolutionalLayer &a);
	static double eta; // [0.0...1.0] overall net training rate
	static double alpha; // [0.0...n] multiplier of last weight change [momentum]
	static double reluParam;
private:
    
	double activationFunction(double x);
	double activationFunctionDerivative(double x);
	static double sigmoid(double x);
	static double randomWeight(void) { return rand() / double(RAND_MAX) / 1.0; }
    Tensor m_outputVal;
	Tensor m_inputVal;
    vector<Tensor> kernels;
	vector<Tensor> deltaKernels;
	Tensor biases;
	Tensor deltaBiases;
	Tensor m_gradients;
    unsigned stride = 1;
    unsigned padding = 1;
	unsigned n_depth; // j
	unsigned kernelsNum; // i
	unsigned kernelSize = 3;
	unsigned inpHeight;
	unsigned inpWidth;
	ActivationType activationType;
};

double ConvolutionalLayer::eta = 0.13887;
double ConvolutionalLayer::alpha = 0;
double ConvolutionalLayer::reluParam = 0.1;

Tensor ConvolutionalLayer::backward(Tensor outputGradient)
{
	vector<Tensor> kernels_gradient(kernelsNum, Tensor(kernelSize, kernelSize, n_depth));
	Tensor input_gradient(inpHeight, inpWidth, kernelsNum);

	for(unsigned i = 0; i < kernelsNum; ++i)
	{
		for(unsigned j = 0; j < n_depth; ++j)
		{
			Matrix activeFuncDeriv(m_inputVal[j]);
			for(unsigned k = 0; k < activeFuncDeriv.getFlatted().size(); ++k)
				activeFuncDeriv.getFlatted()[k] = activationFunctionDerivative(activeFuncDeriv.getFlatted()[k]);

			kernels_gradient[i][j] = m_inputVal[j].correlate(outputGradient[i], padding, stride);
			Matrix tmp = outputGradient[i].convolute(kernels[i][j], padding, stride);
			input_gradient[j] = input_gradient[j].add(tmp.multiply(activeFuncDeriv));
		}
	}
	for(unsigned i = 0; i < kernelsNum; ++i)
	{
		for(unsigned j = 0; j < n_depth; ++j)
		{
			deltaKernels[i][j] = kernels_gradient[i][j].multiply(eta).add(deltaKernels[i][j].multiply(alpha));
			kernels[i][j] = kernels[i][j].subtr(deltaKernels[i][j]);
		}
		deltaBiases[i] = outputGradient[i].multiply(eta).add(deltaBiases[i].multiply(alpha));
		biases[i] = biases[i].subtr(deltaBiases[i]);
	}
	return input_gradient;
}

Tensor ConvolutionalLayer::feedForward(Tensor &input)
{
	assert(input.size() == n_depth);
	assert(input[0].getCols() == inpWidth);
	assert(input[0].getRows() == inpHeight);
	m_inputVal = input;
	m_inputVal = m_inputVal.addPadding(padding);
	for(unsigned kernelNum = 0; kernelNum < kernels.size(); ++kernelNum)
	{
		Tensor kernel = kernels[kernelNum];
		//Zi = Bi
		Matrix sum(biases[kernelNum]);

		sum = sum.add(input.correlate(kernel, padding, stride).sum());

		//Yi = activate(Zi)
		for(unsigned i = 0; i < sum.getFlatted().size(); ++i)
		{
			double &tmp = sum.getFlatted()[i];
			tmp = activationFunction(tmp);
		}

		assert(m_outputVal[kernelNum].getCols() == sum.getCols());
		assert(m_outputVal[kernelNum].getRows() == sum.getRows());
		m_outputVal[kernelNum] = sum;
	}
	m_inputVal = input;
	return m_outputVal;
}

ConvolutionalLayer::ConvolutionalLayer(unsigned InpHeight, unsigned InpWidth, unsigned InpDepth, unsigned OutNum, unsigned KernelSize, ActivationType Activation)
{
	assert(Activation != ActivationType::SoftMax);
	activationType = Activation;
	n_depth = InpDepth;
	kernelsNum = OutNum;
	inpHeight = InpHeight;
	inpWidth = InpWidth;
	kernelSize = KernelSize;
	createKernel(KernelSize);
	Matrix tmp((InpHeight - KernelSize + 2 * padding) / stride + 1, (InpWidth - KernelSize + 2 * padding) / stride + 1);

	m_gradients = Tensor(OutNum, tmp);
	m_outputVal = m_gradients;
	deltaBiases = m_gradients;
	biases = m_gradients;
	for(unsigned outputDepth = 0; outputDepth < OutNum; ++outputDepth)
	{
		biases[outputDepth] = tmp.setRandomVals(randomWeight);
	}
}

ConvolutionalLayer::ConvolutionalLayer()
{

}

void ConvolutionalLayer::createKernel(unsigned size)
{
	for(unsigned kernelNum = 0; kernelNum < kernelsNum; ++kernelNum)
	{
		Tensor tmp(n_depth, Matrix(size, size));
		deltaKernels.push_back(tmp);
		for(unsigned i = 0; i < n_depth; ++i)
		{
			tmp[i] = tmp[i].setRandomVals(randomWeight);
		}
		kernels.push_back(tmp);
	}
}

double ConvolutionalLayer::sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

double ConvolutionalLayer::activationFunction(double x)
{
	switch (activationType)
	{
	case ActivationType::Tanh:
		return tanh(x);
		break;
	case ActivationType::Sigmoid:
		return sigmoid(x);
		break;
	case ActivationType::ReLU:
		return max(0.0, x);
		break;
	case ActivationType::LReLU:
		return (x > 0.0) ? x : reluParam * x;
		break;
	default:
		return 0.0;
		break;
	}
}

double ConvolutionalLayer::activationFunctionDerivative(double x)
{
	switch (activationType)
	{
	case ActivationType::Tanh:
		return 1.0 - x * x;
		break;
	case ActivationType::Sigmoid:
		{
			//double sig = sigmoid(x);
			//return sig * (1.0 - sig);
			return x * (1.0 - x);
		}
		break;
	case ActivationType::ReLU:
		return (x > 0.0) ? 1.0 : 0.0;
		break;
	case ActivationType::LReLU:
		return (x > 0.0) ? 1.0 : reluParam;
	default:
		return 0.0;
		break;
	}
}

ofstream &operator<<(ofstream &out, ConvolutionalLayer &a)
{
	//out << a.kernelsNum << endl;
	for(unsigned i = 0; i < a.kernelsNum; ++i)
		out << a.kernels[i];
	for(unsigned i = 0; i < a.kernelsNum; ++i)
		out << a.biases[i];
	return out;
}

ifstream &operator>>(ifstream &in, ConvolutionalLayer &a)
{
	for(unsigned i = 0; i < a.kernelsNum; ++i)
		in >> a.kernels[i];
	for(unsigned i = 0; i < a.kernelsNum; ++i)
		in >> a.biases[i];
	return in;
}
