#pragma once
#include "HeaderLibs.hpp"
#include "AdvancedMemory/AdvancedMemory.hpp"
#include "MemoryManager/MemoryManager.hpp"

//template <typename dataType>
class Matrix
{
public:
    Matrix(Matrix& a);
    Matrix(float* matrix, unsigned rows, unsigned cols);
    Matrix(unsigned rows, unsigned cols);
    float* getFlatted() const {return m_values;}
    float* &getDataPtr() { return m_values; }
    float& getValue(unsigned row, unsigned col);
    float getConst(unsigned row, unsigned col) const;
    unsigned getRows() const {return n_rows;}
    unsigned getCols() const {return n_columns;}
    void setRows(unsigned rows) { n_rows = rows; }
    void setCols(unsigned cols) { n_columns = cols; }
    void print();
    friend std::ostream &operator<<(std::ostream &out, const Matrix &a);
    friend std::ofstream &operator<<(std::ofstream &out, const Matrix &a);
    friend std::ifstream &operator>>(std::ifstream &in, const Matrix &a);
    void printFlat(std::ofstream &f);
    void read(std::ifstream &f);
    void copyVals(const Matrix &b);
    void operator=(const Matrix &b);
    Matrix setRandomVals(float randVal(void));
    Matrix fill(const float &x);
    Matrix add(Matrix &b);
    Matrix add(const float &b);
    Matrix subtr(Matrix &b);
    Matrix transpose();
    Matrix multiply(Matrix &b);
    Matrix multiply(float x);
    Matrix dotProduct(Matrix &b);
    Matrix addPadding(unsigned paddingThickness, unsigned top, unsigned bot, unsigned left, unsigned right);
    Matrix correlate(Matrix kernel, unsigned padding, unsigned stride);
    Matrix convolute(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix pool(unsigned size, unsigned padding, unsigned stride);
    //MatrixRef submat(unsigned startRow, unsigned startCol, unsigned rowSize, unsigned colSize);
    Matrix rot180(); //cannot be used
    float sum();
    ~Matrix();
private:
    float* m_values = nullptr;
    unsigned n_columns = 0;
    unsigned n_rows = 0;
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

