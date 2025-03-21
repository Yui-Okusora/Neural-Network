#pragma once
#include "HeaderLibs.hpp"
#include "AdvancedMemory/AdvancedMemory.hpp"
#include "MemoryManager/MemoryManager.hpp"

class Matrix
{
public:
    Matrix();
    Matrix(Matrix& a);
    Matrix(AdvancedMemory* ptr, unsigned rows, unsigned cols);
    Matrix(float* matrix, unsigned rows, unsigned cols);
    Matrix(unsigned rows, unsigned cols);
    
    ViewOfAdvancedMemory& load(size_t offset, size_t size);
    void unload(LPVOID viewAddress);
    float& getValue(unsigned row, unsigned col, const ViewOfAdvancedMemory& view);
    void* getViewPtr(const ViewOfAdvancedMemory& view) { return (void*)((char*)view.lpMapAddress + view.iViewDelta); }

    unsigned getRows() const {return n_rows;}
    unsigned getCols() const {return n_columns;}
    void setRows(unsigned rows) { n_rows = rows; }
    void setCols(unsigned cols) { n_columns = cols; }
    void resize(unsigned size);
    void print();
    void printFlat(std::ofstream &f);
    void read(std::ifstream &f);
    void operator=(Matrix &b);
    friend std::ostream &operator<<(std::ostream &out, Matrix &a);
    friend std::ofstream &operator<<(std::ofstream &out, Matrix &a);
    friend std::ifstream &operator>>(std::ifstream &in, Matrix &a);
    
    Matrix setRandomVals(float randVal(void));
    Matrix fill(const float &x);
    Matrix add(Matrix &b);
    Matrix add(const float &b);
    Matrix subtr(Matrix &b);
    Matrix transpose();
    Matrix multiply(Matrix &b);
    Matrix multiply(float x);
    Matrix dotProduct(Matrix &b);
    //Matrix addPadding(unsigned paddingThickness, unsigned top, unsigned bot, unsigned left, unsigned right);
    //Matrix correlate(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix convolute(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix pool(unsigned size, unsigned padding, unsigned stride);
    //MatrixRef submat(unsigned startRow, unsigned startCol, unsigned rowSize, unsigned colSize);
    Matrix rot180();
    float sum();
    ~Matrix();
private:
    unsigned n_columns = 0;
    unsigned n_rows = 0;
    AdvancedMemory m_values;
};

// --------------- INTERNAL ---------------

namespace YuiOkusora {
    namespace Math {
        namespace Mat {
            extern void addMatrix(Matrix* a, Matrix& b);
            extern void addVal2Matrix(Matrix* a, const float& b);
            extern void subtractMatrix(Matrix* a, Matrix& b);
            extern void transposeMatrix(Matrix* a);
            extern void multiplyVal2Matrix(Matrix* a, const float& b);
            extern void multiplyMat2Matrix(Matrix* a, Matrix& b);
            extern void dotProductMatrix(Matrix* a, Matrix& b);
            extern void rotateMatrix180(Matrix *a);
        }
    }
}

