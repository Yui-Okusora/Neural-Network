#pragma once
#include "HeaderLibs.hpp"
#include "FullyConnLayer.hpp"
#include "ConvolutionLayer.hpp"
#include "PoolingLayer.hpp"
using namespace std;

class Net
{
public:
    Net(string save_file_name);
    void feedForward(Tensor inputVals);
    void backProp(Matrix targetVals, double learning_rate);
    double getRecentAverageError(void) const { return m_recentAverageError; }
    Matrix getOutputVals() {return m_outputVals;}
    Tensor normalize(Tensor inputVals, double minRange, double maxRange);
    void calcError(Matrix inputVals);
    void saveNet();
    void loadNet();
    static Matrix flatten(Tensor a);
private:
    string save_file_name;
    unsigned inputDepth = 1;
    double m_error;
	double m_recentAverageError;
	static double m_recentAverageSmoothingFactor;
    ConvolutionalLayer layer1;
    ConvolutionalLayer layer2;
    PoolingLayer layer3;
    FullyConnLayer layer4;
    FullyConnLayer layer5;
    Matrix m_outputVals;
    Tensor m_inputVals;
};

double Net::m_recentAverageSmoothingFactor = 100.0;

Tensor Net::normalize(Tensor inputVals, double minRange, double maxRange)
{
    for(unsigned i = 0; i < inputVals.size(); ++i)
    {
        for(unsigned j = 0; j < inputVals[i].getFlatted().size(); ++j)
        {
            inputVals[i].getFlatted()[j] = (inputVals[i].getFlatted()[j] + minRange) / (maxRange + minRange);
        }
    }
    return inputVals;
}

Matrix Net::flatten(Tensor a)
{
    vector<double> sum;
    for(unsigned depth = 0; depth < a.size(); ++depth)
    {
        Matrix tmp = a[depth];
        for(unsigned i = 0; i < tmp.getFlatted().size(); ++i)
        {
            sum.push_back(tmp.getFlatted()[i]);
        }
    }
    return Matrix(sum, 1, sum.size());
}

void Net::calcError(Matrix targetVals)
{
    m_error = 0.0;
    for(unsigned n = 0; n < m_outputVals.getCols(); ++n)
	{
		double delta = - targetVals.getValue(0, n) * log(m_outputVals.getValue(0, n));
		m_error += delta;
	}
	m_error /= m_outputVals.getCols();

	m_recentAverageError = 
			(m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
			/ (m_recentAverageSmoothingFactor + 1.0);
}

void Net::feedForward(Tensor inputVals)
{
    m_inputVals = inputVals;
    Tensor output = layer1.feedForward(inputVals);
    output = layer2.feedForward(output);
    output = layer3.feedForward(output);
    Matrix output2 = flatten(output);
    output2 = layer4.feedForward(output2);
    output2 = layer5.feedForward(output2);
    m_outputVals = output2;
    //saveNet();
}

void Net::backProp(Matrix targetVals, double learning_rate)
{
    ConvolutionalLayer::eta = learning_rate;
    FullyConnLayer::eta = learning_rate;
    auto grad1 = layer5.calcOutputGradients(targetVals);
    grad1 = layer4.backward(grad1);
    auto grad2 = layer3.calcPoolingGradient(grad1);
    grad2 = layer2.backward(grad2);
    grad2 = layer1.backward(grad2);
}

Net::Net(string save_file_name)
{
    this->save_file_name = save_file_name;
    layer1 = ConvolutionalLayer(28, 28, 1, 32, 3, ActivationType::Sigmoid);
    layer2 = ConvolutionalLayer(28, 28, 32, 32, 3, ActivationType::Sigmoid);
    layer3 = PoolingLayer(28, 28, 14, 14, 32);
    layer4 = FullyConnLayer(6272, 128, ActivationType::Sigmoid);
    layer5 = FullyConnLayer(128, 10, ActivationType::SoftMax);
    fstream tmp(save_file_name);
    if(tmp.good())
    {
        loadNet();
    }
}

void Net::saveNet()
{
    ofstream outStream;
    outStream.open(save_file_name + ".layer5.txt", ios::out);
    (outStream << layer5).close();
    outStream.open(save_file_name + ".layer4.txt", ios::out);
    (outStream << layer4).close();
    outStream.open(save_file_name + ".layer2.txt", ios::out);
    (outStream << layer2).close();
    outStream.open(save_file_name + ".layer1.txt", ios::out);
    (outStream << layer1).close();
    
}

void Net::loadNet()
{
    ifstream inpStream;
    inpStream.open(save_file_name + ".layer5.txt", ios::in);
    (inpStream >> layer5).close();
    inpStream.open(save_file_name + ".layer4.txt", ios::in);
    (inpStream >> layer4).close();
    inpStream.open(save_file_name + ".layer2.txt", ios::in);
    (inpStream >> layer2).close();
    inpStream.open(save_file_name + ".layer1.txt", ios::in);
    (inpStream >> layer1).close();
}