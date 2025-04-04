#include "Net.hpp"

namespace NeuroSoraCore
{

    float Layer::eta = 0.0015f;
    float Layer::alpha = 0.5f;
    float Layer::reluParam = 0.01f;


    Net::Net()
    {

    }

    Net::~Net()
    {
        for (auto& layer : layers)
            delete layer;
    }

    void Net::calcError(Matrix& targetVals)
    {
        m_gradients = m_outputVals;
        ViewOfAdvancedMemory& viewTarget = targetVals.load(0, sizeof(float) * targetVals.getRows() * targetVals.getCols());
        ViewOfAdvancedMemory& viewOutput = m_outputVals.load(0, sizeof(float) * m_outputVals.getRows() * m_outputVals.getCols());
        ViewOfAdvancedMemory& viewGradient = m_gradients.load(0, sizeof(float) * m_gradients.getRows() * m_gradients.getCols());
        for (unsigned i = 0; i < targetVals.getCols(); ++i)
        {
            float tmp = m_outputVals.getValue(0, i, viewOutput) - targetVals.getValue(0, i, viewTarget);
            m_gradients.getValue(0, i, viewGradient) = tmp;
            m_error += tmp;
        }
        targetVals.unload(viewTarget);
        m_outputVals.unload(viewOutput);
        m_gradients.unload(viewGradient);
        m_error /= targetVals.getCols();

        m_recentAverageError =
            (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);
    }

    void Net::feedforward()
    {
        m_outputVals = m_inputVals;
        for (auto& layer : layers)
        {
            layer->feedforward(m_outputVals, m_outputVals);
        }
    }

    void Net::backpropagation(float learning_rate)
    {
        Layer::eta = learning_rate;
        for (size_t i = layers.size(); i > 0; --i)
        {
            layers[i - 1]->backpropagate(m_gradients, m_gradients);
        }
    }

}