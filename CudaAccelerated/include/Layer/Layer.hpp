#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <variant>
#include "Neuron.hpp"
#include "ConvolutionNeuron.hpp"
using namespace std;

class DenseLayer
{
public:
    DenseLayer() {}
    DenseLayer(unsigned numNeuron, ActivationType activationFunc);
    vector<Neuron>& getLayer() {return m_neurons;}
    unsigned size() {return m_neurons.size();}
private:
    vector<Neuron> m_neurons;
};

DenseLayer::DenseLayer(unsigned numNeuron, ActivationType activationFunc)
{
    for(unsigned n = 0; n <= numNeuron; ++n)
    {
        m_neurons.push_back(Neuron(n, activationFunc));
    }
    m_neurons.back().setOutputVal(1.0);
}

class ConvolutionLayer
{
public:
    ConvolutionLayer() {}
    ConvolutionLayer(unsigned numNeuron, ActivationType activationFunc);
    vector<ConvolutionLayer> &getLayer() {return m_neurons;}
    unsigned size() {return m_neurons.size();}
private:
    vector<ConvolutionLayer> m_neurons;
};


typedef variant<DenseLayer, ConvolutionLayer> LayerType;
class Layers
{
public:
    void push_back(LayerType layer) {m_layers.push_back(layer);}
    
    LayerType &back() {return m_layers.back();}
private:
    vector<LayerType> m_layers;
};