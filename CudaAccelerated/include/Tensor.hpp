#pragma once
#include "Matrix.hpp"

using namespace std;
class Tensor
{
public:
    Tensor(){}
    Tensor(unsigned depth, Matrix _n);
    Tensor(unsigned rows, unsigned cols, unsigned depth);
    Tensor correlate(Tensor &kernels, unsigned padding, unsigned stride);
    Tensor convolute(Tensor &kernels, unsigned padding, unsigned stride);
    Tensor addPadding(unsigned paddingThickness, unsigned top, unsigned bot, unsigned left, unsigned right);
    Matrix &operator[](size_t index){ assert(index < n_depth); return m_values[index];}
    Matrix sum();
    size_t size(){return m_values.size();}
    unsigned &getRows(){return n_rows;}
    unsigned &getCols(){return n_columns;}
    unsigned &getDepth(){return n_depth;}
    void printFlat(ofstream &out);
    void read(ifstream &in);
    friend ofstream &operator<<(ofstream &out, Tensor &a);
    friend ifstream &operator>>(ifstream &in, Tensor &a);
private:
    vector<Matrix> m_values;
    unsigned n_rows;
    unsigned n_columns;
    unsigned n_depth;
};

Matrix Tensor::sum()
{
    Matrix tmp(n_rows, n_columns);
    for(unsigned i = 0; i < n_depth; ++i)
    {
        tmp = tmp.add((*this)[i]);
    }
    return tmp;
}

Tensor Tensor::addPadding(unsigned paddingThickness, unsigned top = 1, unsigned bot = 1, unsigned left = 1, unsigned right = 1)
{
    Tensor tmp = *this;
    tmp.n_rows += (top + bot) * paddingThickness;
    tmp.n_columns += (left + right) * paddingThickness;
    for(unsigned i = 0; i < n_depth; ++i)
    {
        tmp[i] = tmp[i].addPadding(paddingThickness, top, bot, left, right);
    }
    return tmp;
}

Tensor Tensor::correlate(Tensor &kernels, unsigned padding, unsigned stride)
{
    assert(kernels.n_columns == kernels.n_rows);
    assert(n_depth == kernels.n_depth);
    Tensor inp = *this;
    Tensor out((inp.n_rows - kernels.n_rows + 4 * padding) / stride - 1, (inp.n_columns - kernels.n_columns + 4 * padding) / stride - 1, n_depth);
    for(unsigned i = 0; i < n_depth; ++i)
    {
        out[i] = inp[i].correlate(kernels[i], padding, stride);
    }
    return out;
}

Tensor Tensor::convolute(Tensor &kernels, unsigned padding, unsigned stride)
{
    assert(kernels.n_columns == kernels.n_rows);
    assert(n_depth == kernels.n_depth);
    Tensor inp = *this;
    Tensor out((inp.n_rows - kernels.n_rows + 4 * padding) / stride - 1, (inp.n_columns - kernels.n_columns + 4 * padding) / stride - 1, n_depth);
    for(unsigned i = 0; i < n_depth; ++i)
    {
        out[i] = inp[i].convolute(kernels[i], padding, stride);
    }
    return out;
}

Tensor::Tensor(unsigned rows, unsigned cols, unsigned depth)
{
    n_columns = cols;
    n_rows = rows;
    n_depth = depth;
    m_values = vector<Matrix>(depth, Matrix(rows, cols));
}

Tensor::Tensor(unsigned depth, Matrix _n)
{
    n_depth = depth;
    m_values = vector<Matrix>(depth, _n);
    n_columns = _n.getCols();
    n_rows = _n.getRows();
}

void Tensor::printFlat(ofstream &out)
{
    out << n_rows << " " << n_columns << " " << n_depth << endl;
    for(unsigned i = 0; i < n_depth; ++i)
    {
        out << m_values[i];
    }
}

void Tensor::read(ifstream &in)
{
    in >> n_rows >> n_columns >> n_depth;
    vector<Matrix> tmp;
    Matrix inp;
    for(unsigned i = 0; i < n_depth; ++i)
    {
        in >> inp;
        tmp.push_back(inp);
    }
    m_values = tmp;
}

ofstream &operator<<(ofstream &out, Tensor &a)
{
    a.printFlat(out);
    return out;
}

ifstream &operator>>(ifstream &in, Tensor &a)
{
    a.read(in);
    return in;
}