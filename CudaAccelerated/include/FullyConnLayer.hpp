#pragma once
#include "Matrix.hpp"

class FullyConnLayer
{
public:
    FullyConnLayer();
    FullyConnLayer(unsigned inpNeuronNum, unsigned neuronNum, ActivationType activation);
    Matrix feedForward(Matrix input);
    Matrix backward(Matrix outputGradient);
    Matrix calcOutputGradients(Matrix targetVals);
    friend ofstream &operator<<(ofstream &out, FullyConnLayer &a);
    friend ifstream &operator>>(ifstream &in, FullyConnLayer &a);
    static double eta; // [0.0...1.0] overall net training rate
	static double alpha; // [0.0...n] multiplier of last weight change [momentum]
	static double reluParam;
private:
    void createWeights(unsigned inpSize);
    double sumDerivativeOWeight(unsigned j, Matrix &nextWeights, Matrix &nextGradients);
	double activationFunction(double x);
	double activationFunctionDerivative(double x);
    double softmax(double x);
	static double sigmoid(double x);
	static double randomWeight(void) { return rand() / double(RAND_MAX)/ 1.0; }
    double sumE_z = 0.0;
    double maxC;
    Matrix m_output;
    Matrix m_input;
    Matrix weights;
    Matrix deltaWeights;
    Matrix m_gradients;
    ActivationType activationType;
};

double FullyConnLayer::eta = 0.13887; // overall net learning rate
double FullyConnLayer::alpha = 0; // momentum, multiplier of last deltaWeight, [0.0..n]
double FullyConnLayer::reluParam = 0.01;

Matrix FullyConnLayer::calcOutputGradients(Matrix targetVals)
{

    for(unsigned i = 0; i < m_output.getCols(); ++i)
    {
        double delta = m_output.getValue(0, i) - targetVals.getValue(0, i);
        m_gradients.getValue(0, i) = delta * activationFunctionDerivative(m_output.getValue(0, i));
    }
    return backward(m_gradients);
}

Matrix FullyConnLayer::backward(Matrix outputGradient)
{
    Matrix weight_gradient(weights.getRows(), weights.getCols());
    Matrix input_gradient(m_input.getRows(), m_input.getCols());
    for(unsigned i = 0; i < m_input.getCols(); ++i)
        for(unsigned j = 0; j < weights.getCols(); ++j)
        {
            weight_gradient.getValue(i, j) = outputGradient.getValue(0, j) * m_input.getValue(0, i);
            input_gradient.getValue(0, i) += weights.getValue(i, j) * outputGradient.getValue(0, j) * activationFunctionDerivative(m_input.getValue(0, i));
        }
    for(unsigned i = 0; i < m_input.getCols(); ++i)
        for(unsigned j = 0; j < weights.getCols(); ++j)
        {
            deltaWeights.getValue(i, j) = weight_gradient.getValue(i, j) * eta + deltaWeights.getValue(i, j) * alpha;
            weights.getValue(i, j) -= deltaWeights.getValue(i, j);
        }

    return input_gradient;
}

Matrix FullyConnLayer::feedForward(Matrix input)
{
    input.getFlatted().push_back(1.0);
    input.getCols() += 1;
    m_input = input;
    m_output = input.dot(weights);
    if(activationType == ActivationType::SoftMax)
    {
        maxC = -1000000000000;
        for(unsigned i = 0; i < m_output.getFlatted().size(); ++i)
        {
            if(maxC < m_output.getFlatted()[i]) maxC = m_output.getFlatted()[i];
        }
        sumE_z = 0.0;
        for(unsigned i = 0; i < m_output.getFlatted().size(); ++i)
        {
            sumE_z += exp(m_output.getFlatted()[i] - maxC);
        }
    }
    for(unsigned i = 0; i < m_output.getRows(); ++i)
        for(unsigned j = 0; j < m_output.getCols(); ++j)
            m_output.getValue(i, j) = activationFunction(m_output.getValue(i, j));
    
    return m_output;
}

FullyConnLayer::FullyConnLayer(unsigned inpNeuronNum, unsigned neuronNum, ActivationType activation)
{
    activationType = activation;
    m_output = Matrix(1, neuronNum);
    m_gradients = Matrix(1, neuronNum);
    createWeights(inpNeuronNum);
}

FullyConnLayer::FullyConnLayer()
{
    
}

void FullyConnLayer::createWeights(unsigned inpSize)
{
    Matrix tmp(inpSize + 1, m_output.getCols());
    weights = tmp;
    deltaWeights = tmp;
    for(unsigned i = 0; i < weights.getRows(); ++i)
    {
        for(unsigned j = 0; j < weights.getCols(); ++j)
        {
            weights.getValue(i, j) = randomWeight();
        }
    }
}

double FullyConnLayer::sigmoid(double x)
{
	return 1.0 / (1.0 + exp(-x));
}

double FullyConnLayer::softmax(double x)
{
    return exp(x - maxC) / sumE_z;
}

double FullyConnLayer::activationFunction(double x)
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
    case ActivationType::SoftMax:
        return softmax(x);
	default:
		return 0.0;
		break;
	}
}

double FullyConnLayer::activationFunctionDerivative(double x)
{
	switch (activationType)
	{
	case ActivationType::Tanh:
		return 1.0 - x * x;
		break;
	case ActivationType::Sigmoid:
		{
            return x * (1.0 - x);
		}
		break;
	case ActivationType::ReLU:
		return (x > 0.0) ? 1.0 : 0.0;
		break;
	case ActivationType::LReLU:
		return (x > 0.0) ? 1.0 : reluParam;
    case ActivationType::SoftMax:
        {
            //double sm = softmax(x);
            //return sm * (1.0 - sm);
            return x * (1.0 - x);
        }
	default:
		return 0.0;
		break;
	}
}

ofstream &operator<<(ofstream &out, FullyConnLayer &a)
{
    out << a.weights;
    return out;
}

ifstream &operator>>(ifstream &in, FullyConnLayer &a)
{
    in >> a.weights;
    return in;
}