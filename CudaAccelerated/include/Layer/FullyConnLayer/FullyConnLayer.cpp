#include "FullyConnLayer.hpp"
#include <thread>

FullyConnLayer::FullyConnLayer(unsigned inpNeuronNum, unsigned neuronNum, ActivationType activation)
{
    activationType = activation;
    m_input = std::make_unique<Matrix>(1, inpNeuronNum);
    m_output = std::make_unique<Matrix>(1, neuronNum);
    weights = std::make_unique<Matrix>(inpNeuronNum, neuronNum);
    bias = std::make_unique<Matrix>(1, neuronNum);
    old_weightsDelta = std::make_unique<Matrix>(inpNeuronNum, neuronNum);
    old_biasDelta = std::make_unique<Matrix>(1, neuronNum);
    createWeights(inpNeuronNum);
}

FullyConnLayer::FullyConnLayer()
{

}

void FullyConnLayer::backpropagate(Matrix& inputGradient, Matrix& outputGradient)
{
    outputGradient = m_output->applyActivationDerivative(activationType, &inputGradient, Layer::reluParam);

    auto calcWeightsDelta = [](Matrix& weightsDelta, std::unique_ptr<Matrix>&m_input, Matrix& inputGradient, Matrix& outputGradient, std::unique_ptr<Matrix>& old_weightsDelta)
        {
            weightsDelta = m_input->transpose();
            weightsDelta.dotProduct_nocopy(outputGradient);
            weightsDelta.multiply_nocopy(Layer::eta);
            weightsDelta.add_nocopy(old_weightsDelta->multiply(Layer::alpha));
            *old_weightsDelta = weightsDelta;
        };

    Matrix weightsDelta;

    auto& f = std::thread(calcWeightsDelta, std::ref(weightsDelta), std::ref(m_input), std::ref(inputGradient), std::ref(outputGradient), std::ref(old_weightsDelta));
    
    /*Matrix& weightsDelta = m_input->transpose();
    weightsDelta.dotProduct_nocopy(outputGradient);
    weightsDelta.multiply_nocopy(Layer::eta);
    weightsDelta.add_nocopy(old_weightsDelta->multiply(Layer::alpha));
    *old_weightsDelta = weightsDelta;*/

    Matrix& biasDelta = outputGradient.multiply(Layer::eta);
    biasDelta.add_nocopy(old_biasDelta->multiply(Layer::alpha));
    *old_biasDelta = biasDelta;

    outputGradient = outputGradient.dotProduct(weights->transpose());

    f.join();

    weights->subtr_nocopy(weightsDelta);
    bias->subtr_nocopy(biasDelta);
}

void FullyConnLayer::feedforward(Matrix& input, Matrix& output)
{
    *m_input = input;
    Matrix& tmp = input.dotProduct(*weights);
    tmp.add_nocopy(*bias);
    tmp.applyActivation_nocopy(activationType, Layer::reluParam);
    output = tmp;
    *m_output = output;
}

void FullyConnLayer::createWeights(unsigned inpSize)
{
    ViewOfAdvancedMemory& viewWeights = weights->load(0, sizeof(float) * weights->getRows() * weights->getCols());
    ViewOfAdvancedMemory& viewBias = bias->load(0, sizeof(float) * bias->getRows() * bias->getCols());
    for (unsigned i = 0; i < weights->getCols(); ++i)
    {
        for (unsigned j = 0; j < weights->getRows(); ++j)
        {
            weights->getValue(j, i, viewWeights) = randomWeight();
        }
        bias->getValue(0, i, viewBias) = randomWeight();
    }
    weights->unload(viewWeights);
    bias->unload(viewBias);
}

float FullyConnLayer::activationFunction(float x)
{
    switch (activationType)
    {
    case ActivationType::Tanh:
        return tanh(x);
        break;
    case ActivationType::Sigmoid:
        return 1.0 / (1.0 + exp(-x));
        break;
    case ActivationType::ReLU:
        return max(0.0, x);
        break;
    case ActivationType::LReLU:
        return (x > 0.0) ? x : reluParam * x;
        break;
    case ActivationType::SoftMax:
        return exp(x - maxC) / sumE_z;
    default:
        return 0.0;
        break;
    }
}

float FullyConnLayer::activationFunctionDerivative(float x)
{
    switch (activationType)
    {
    case ActivationType::Tanh:
        return 1.0 - x * x;
        break;
    case ActivationType::Sigmoid:
        return x * (1.0 - x);
        break;
    case ActivationType::ReLU:
        return (x > 0.0) ? 1.0 : 0.0;
        break;
    case ActivationType::LReLU:
        return (x > 0.0) ? 1.0 : reluParam;
        break;
    case ActivationType::SoftMax:
        return x * (1.0 - x);
        break;
    default:
        return 0.0;
        break;
    }
}