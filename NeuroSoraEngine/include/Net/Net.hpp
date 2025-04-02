#pragma once

#define NeuroSoraCore_Net

#include "NeuroSoraCore.hpp"

#include "Matrix/Matrix.hpp"
#include "Layer/Layer.hpp"
#include "AdvancedMemory/AdvancedMemory.hpp"

class Net
{
public:
    Net();
    ~Net();
    void feedforward();
    void backpropagation(float learning_rate);
    double getRecentAverageError(void) const { return m_recentAverageError; }
    void calcError(Matrix& inputVals);
    Matrix m_outputVals;
    Matrix m_inputVals;
    double m_recentAverageError = 0;
    std::vector<Layer*> layers;
private:
    float m_error = 0;
	float m_recentAverageSmoothingFactor = 100.0f;
    Matrix m_gradients;
};
