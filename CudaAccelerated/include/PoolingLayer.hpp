#pragma once
#include "Tensor.hpp"
using namespace std;

class PoolingLayer
{
public:
    PoolingLayer();
    PoolingLayer(unsigned InputHeight, unsigned InputWidth, unsigned OutputHeight, unsigned OutputWidth, unsigned InputChannels);
    Tensor feedForward(Tensor input);
    Tensor calcPoolingGradient(Tensor nextGradients);
    Tensor calcPoolingGradient(Matrix outputGradient);
    Tensor calcPoolingGradient(Matrix &nextWeights, Matrix &nextGradients);
    Tensor Matrix2Tensor(Matrix input);
    Tensor getInputGradient() {return m_gradients;}
    Tensor getOutputVals() {return m_outputVal;}
    Matrix calcHiddenGradients(Matrix &nextWeights, Matrix &nextGradients);
    const vector<Tensor> getKernels() {return vector<Tensor>(n_depth, Tensor(3, Matrix(2, 2).setMatrixVal(1.0)));}
private:
    double sumDerivativeOWeight(unsigned j, Matrix &nextWeights, Matrix &nextGradients);
    
    Tensor m_outputVal;
    Tensor m_inputVal;
    Tensor m_gradients;

    unsigned stride = 2;
    unsigned padding = 0;
    unsigned size = 2;
    unsigned n_depth;
    unsigned outputHeight;
    unsigned outputWidth;
    unsigned inputHeight;
    unsigned inputWidth;
};

PoolingLayer::PoolingLayer()
{

}

PoolingLayer::PoolingLayer(unsigned InputHeight, unsigned InputWidth, unsigned OutputHeight, unsigned OutputWidth, unsigned InputChannels)
{
    inputHeight = InputHeight;
    inputWidth = InputWidth;
    outputHeight = OutputHeight;
    outputWidth = OutputWidth;
    n_depth = InputChannels;
    m_gradients = Tensor(n_depth, Matrix(inputHeight, inputWidth));
    m_outputVal = Tensor(n_depth, Matrix(outputHeight, outputWidth));
}

Tensor PoolingLayer::Matrix2Tensor(Matrix input)
{
    unsigned index = 0;
    Tensor out(n_depth, Matrix(m_outputVal[0].getRows(), m_outputVal[0].getCols()));
    for(unsigned depth = 0; depth < n_depth; ++depth)
    {
        Matrix tmp(m_outputVal[0].getRows(), m_outputVal[0].getCols());
        for(unsigned i = 0; i < m_outputVal[0].getRows(); ++i)
        {
            for(unsigned j = 0; j < m_outputVal[0].getCols(); ++j)
            {
                tmp.getValue(i, j) = input.getFlatted()[index];
                ++index;
            }
        }
        out[depth] = tmp;
    }
    return out;
}

double PoolingLayer::sumDerivativeOWeight(unsigned j, Matrix &nextWeights, Matrix &nextGradients)
{
    double sum = 0.0;
    for(unsigned i = 0; i < nextWeights.getCols(); ++i)
    {
        sum += nextWeights.getValue(j, i) * nextGradients.getValue(0, i);
    }
    return sum;
}

Matrix PoolingLayer::calcHiddenGradients(Matrix &nextWeights, Matrix &nextGradients)
{
    Matrix currGradient(1, n_depth * outputHeight * outputWidth);
    for(unsigned i = 0; i < currGradient.getCols(); ++i)
    {
        double dow = sumDerivativeOWeight(i, nextWeights, nextGradients);
        currGradient.getValue(0, i) = dow;
    }
    return currGradient;
}

Tensor PoolingLayer::calcPoolingGradient(Tensor nextGradients)
{
    for(unsigned channel = 0; channel < n_depth; ++channel)
    {   
        Matrix input = nextGradients[channel];
        unsigned outHeight = m_gradients[channel].getRows() % 2;
        unsigned outWidth = m_gradients[channel].getCols() % 2;
        m_gradients[channel] = m_gradients[channel].addPadding(1, 0, outHeight, 0, outWidth);
        MatrixRef pool = m_gradients[channel].submat(0, 0, size, size);
        for(unsigned i = 0; i <= (pool.getOriginRows() - size) / stride; ++i)
        {
            for(unsigned j = 0; j <= (pool.getOriginCols() - size) / stride; ++j)
            {
                MatrixRef tmp = pool.move(i * stride, j * stride);
                bool flag = false;
                for(unsigned k = 0; k < size; ++k)
                {
                    for(unsigned g = 0; g < size; ++g)
                    {
                        if(tmp.getValue(k, g) == 1)
                        {
                            tmp.getValue(k, g) = input.getValue(i, j);
                            flag = true;
                            break;
                        }
                    }
                    if(flag) break;
                }
            }
        }
        outHeight = m_gradients[channel].getRows() - outHeight;
        outWidth = m_gradients[channel].getCols() - outWidth;
        m_gradients[channel] = Matrix(m_gradients[channel].submat(0, 0, outHeight, outWidth).getFlatted(), outHeight, outWidth);
    }
    return m_gradients;
}

Tensor PoolingLayer::calcPoolingGradient(Matrix outputGradient)
{
    return calcPoolingGradient(Matrix2Tensor(outputGradient));
}

Tensor PoolingLayer::calcPoolingGradient(Matrix &nextWeights, Matrix &nextGradients)
{
    Matrix gradient = calcHiddenGradients(nextWeights, nextGradients);
    return calcPoolingGradient(Matrix2Tensor(gradient));
}

Tensor PoolingLayer::feedForward(Tensor input)
{
    m_inputVal = input;
    for(unsigned i = 0; i < m_outputVal.size(); ++i)
    {
        m_inputVal[i] = m_inputVal[i].addPadding(1, 0, m_inputVal[i].getRows() % 2, 0, m_inputVal[i].getCols() % 2);
        if(padding != 0) m_inputVal[i] = m_inputVal[i].addPadding(padding);
        Matrix out(ceil((m_inputVal[i].getRows() - size + 1) / (double)stride), ceil((m_inputVal[i].getCols() - size + 1) / (double)stride));
        MatrixRef InputPtr = m_inputVal[i].submat(0, 0, size, size);
        MatrixRef gradientPtr = m_gradients[i].submat(0, 0, size, size);
        for(unsigned j = 0; j < out.getRows(); ++j)
        {
            for(unsigned k = 0; k < out.getCols(); ++k)
            {
                MatrixRef TInputPtr = InputPtr.move(j * stride, k * stride);
                MatrixRef TgradientPtr = gradientPtr.move(j * stride, k * stride);
                int max = INT_MIN;
                for(unsigned g = 0 ; g < size; ++g)
                {
                    for(unsigned l = 0 ; l < size; ++l)
                    {
                        if(TInputPtr.getValue(g, l) > max)
                        {
                            max = TInputPtr.getValue(g, l);
                            TgradientPtr.setAllValue(0.0);
                            TgradientPtr.getValue(g, l) = 1.0;
                        }
                    }
                }
                out.getValue(j, k) = max;
            }
        }
        m_outputVal[i] = out;
    }
    
    return m_outputVal;
}