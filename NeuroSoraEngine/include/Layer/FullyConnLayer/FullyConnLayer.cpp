#include "FullyConnLayer.hpp"

namespace NeuroSoraCore
{
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
        Timer timer("FullyConnLayer::backpropagate");
        outputGradient = m_output->applyActivationDerivative(activationType, &inputGradient, Layer::reluParam);

        Matrix& tmp = weights->transpose();

        auto calcWeightsDelta = [](std::promise<void>& promise, std::unique_ptr<Matrix>& m_input, std::unique_ptr<Matrix>& weights, Matrix& inputGradient, Matrix& outputGradient, std::unique_ptr<Matrix>& old_weightsDelta)
            {
                Timer timer("FullyConnLayer::backpropagate::calcWeightsDelta");
                Matrix weightsDelta = m_input->transpose();
                weightsDelta.dotProduct_nocopy(outputGradient);
                weightsDelta.multiply_nocopy(Layer::eta);
                //Matrix& tmp = old_weightsDelta->multiply(Layer::alpha);
                weightsDelta.add_nocopy(old_weightsDelta->multiply(Layer::alpha));
                *old_weightsDelta = weightsDelta;
                weights->subtr_nocopy(weightsDelta);
                promise.set_value();
            };

        auto calcBiasDelta = [](std::promise<void>& promise, std::unique_ptr<Matrix>& bias, Matrix& outputGradient, std::unique_ptr<Matrix>& old_biasDelta)
            {
                Timer timer("FullyConnLayer::backpropagate::calcBiasDelta");
                Matrix biasDelta = outputGradient.multiply(Layer::eta);
                //Matrix& tmp = old_biasDelta->multiply(Layer::alpha);
                biasDelta.add_nocopy(old_biasDelta->multiply(Layer::alpha));
                *old_biasDelta = biasDelta;
                bias->subtr_nocopy(biasDelta);
                promise.set_value();
            };


        std::promise<void> task1, task2;

        threadPool1.enqueue(std::bind(calcWeightsDelta, std::ref(task1), std::ref(m_input), std::ref(weights), std::ref(inputGradient), std::ref(outputGradient), std::ref(old_weightsDelta)));
        threadPool1.enqueue(std::bind(calcBiasDelta, std::ref(task2), std::ref(bias), std::ref(outputGradient), std::ref(old_biasDelta)));

        task1.get_future().wait();
        task2.get_future().wait();

        outputGradient = outputGradient.dotProduct(tmp);
    }

    void FullyConnLayer::feedforward(Matrix& input, Matrix& output)
    {
        Timer timer("FullyConnLayer::feedforward");
        *m_input = input;
        Matrix& tmp = input.dotProduct(*weights);
        tmp.add_nocopy(*bias);
        tmp.applyActivation_nocopy(activationType, Layer::reluParam);
        output = tmp;
        *m_output = output;
    }

    void FullyConnLayer::createWeights(unsigned inpSize)
    {
        Timer timer("FullyConnLayer::createWeights");
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
            return 1.0f / (1.0f + exp(-x));
            break;
        case ActivationType::ReLU:
            return max(0.0f, x);
            break;
        case ActivationType::LReLU:
            return (x > 0.0f) ? x : reluParam * x;
            break;
        case ActivationType::SoftMax:
            return exp(x - maxC) / sumE_z;
        default:
            return 0.0f;
            break;
        }
    }

    float FullyConnLayer::activationFunctionDerivative(float x)
    {
        switch (activationType)
        {
        case ActivationType::Tanh:
            return 1.0f - x * x;
            break;
        case ActivationType::Sigmoid:
            return x * (1.0f - x);
            break;
        case ActivationType::ReLU:
            return (x > 0.0f) ? 1.0f : 0.0f;
            break;
        case ActivationType::LReLU:
            return (x > 0.0f) ? 1.0f : reluParam;
            break;
        case ActivationType::SoftMax:
            return x * (1.0f - x);
            break;
        default:
            return 0.0f;
            break;
        }
    }
}