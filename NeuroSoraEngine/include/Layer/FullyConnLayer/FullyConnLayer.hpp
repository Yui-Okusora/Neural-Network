#pragma once

#define NeuroSoraCore_FCLayer

#include "NeuroSoraCore.hpp"
#include "Layer/Layer.hpp"

namespace NeuroSoraCore
{

    class FullyConnLayer : public Layer
    {
    public:
        FullyConnLayer(unsigned inpNeuronNum, unsigned neuronNum, ActivationType activation);
        FullyConnLayer();
        void feedforward(Matrix& input, Matrix& output);
        void backpropagate(Matrix& inputGradient, Matrix& outputGradient);
    private:
        void createWeights(unsigned inpSize);
        float activationFunction(float x);
        float activationFunctionDerivative(float x);
        static float randomWeight(void) { return rand() / float(RAND_MAX); }
        float sumE_z = 0.0;
        float maxC = -9999999;
        std::unique_ptr<Matrix> m_output;
        std::unique_ptr<Matrix> m_input;
        std::unique_ptr<Matrix> weights;
        std::unique_ptr<Matrix> bias;
        std::unique_ptr<Matrix> old_weightsDelta;
        std::unique_ptr<Matrix> old_biasDelta;
        ActivationType activationType = ActivationType::Sigmoid;
    };
}

