#pragma once
#include "HeaderLibs.hpp"

using namespace std;


class MatrixRef
{
public:
    MatrixRef(){}
    MatrixRef(unsigned rows, unsigned cols);
    double* &getPointerRef(unsigned row, unsigned col);
    double &getValue(unsigned row, unsigned col){return *getPointerRef(row, col);}
    double getMax();
    unsigned &getCols(){return n_columns;}
    unsigned &getRows(){return n_rows;}
    unsigned &getOriginCols(){return orig_cols;}
    unsigned &getOriginRows(){return orig_rows;}
    vector<double> getFlatted();
    void setOrigin(unsigned rows, unsigned cols){orig_rows = rows; orig_cols = cols;}
    MatrixRef move(unsigned row, unsigned col);
    MatrixRef setAllValue(double x);
    void print();
private:
    vector<double*> m_refs;
    unsigned n_columns = 0;
    unsigned n_rows = 0;
    unsigned orig_cols = 0;
    unsigned orig_rows = 0;
    unsigned start_col = 0;
    unsigned start_row = 0;
};

vector<double> MatrixRef::getFlatted()
{
    vector<double> tmp(n_columns * n_rows, 0.0);
    for(unsigned i = 0; i < tmp.size(); ++i)
    {
        tmp[i] = *(m_refs[i]);
    }
    return tmp;
}

MatrixRef MatrixRef::setAllValue(double x)
{
    for(unsigned i = 0; i < m_refs.size(); ++i)
    {
        *(m_refs[i]) = x;
    }
    return *this;
}

void MatrixRef::print()
{
    for(unsigned i = 0; i < n_rows; ++i)
    {
        for(unsigned j = 0; j < n_columns; ++j)
        {
            cout << getValue(i, j) << " ";
        }
        cout << endl;
    }
}

MatrixRef MatrixRef::move(unsigned row, unsigned col)
{
    assert(start_row + row < orig_rows);
    assert(start_col + col < orig_cols);
    MatrixRef tmp = *this;
    tmp.start_col = start_col + col;
    tmp.start_row = start_row + row;
    for(unsigned i = 0; i < tmp.m_refs.size(); ++i)
    {
        tmp.m_refs[i] = tmp.m_refs[i] + (row * orig_cols + col);
    }
    return tmp;
}

double* &MatrixRef::getPointerRef(unsigned row, unsigned col)
{
    assert(row < n_rows);
    assert(col < n_columns);
    return m_refs[n_rows * row + col];
}

double MatrixRef::getMax()
{
    double maxx = numeric_limits<double>::min();
    for(unsigned i = 0; i < m_refs.size(); ++i)
    {
        maxx = max(maxx, *m_refs[i]);
    }
    return maxx;
}

MatrixRef::MatrixRef(unsigned rows, unsigned cols)
{
    n_rows = rows; 
    n_columns = cols;
    m_refs = vector<double*>(rows * cols, nullptr);
}