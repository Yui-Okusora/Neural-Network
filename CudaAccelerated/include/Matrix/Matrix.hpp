#pragma once
#include "HeaderLibs.hpp"

//template <typename dataType>
class Matrix
{
public:
    Matrix(Matrix& a);
    Matrix(const double* matrix, unsigned rows, unsigned cols);
    Matrix(unsigned rows, unsigned cols);
    double* getFlatted() const {return m_values;}
    double* &getDataPtr() { return m_values; }
    double& getValue(unsigned row, unsigned col);
    double getConst(unsigned row, unsigned col) const;
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
    Matrix setRandomVals(double randVal(void));
    Matrix fill(const double &x);
    Matrix add(Matrix &b);
    Matrix add(const double &b);
    Matrix subtr(Matrix &b);
    Matrix transpose();
    Matrix multiply(Matrix &b);
    Matrix multiply(double x);
    Matrix dotProduct(Matrix &b);
    Matrix addPadding(unsigned paddingThickness, unsigned top, unsigned bot, unsigned left, unsigned right);
    Matrix correlate(Matrix kernel, unsigned padding, unsigned stride);
    Matrix convolute(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix pool(unsigned size, unsigned padding, unsigned stride);
    //MatrixRef submat(unsigned startRow, unsigned startCol, unsigned rowSize, unsigned colSize);
    Matrix rot180(); //cannot be used
    double sum();
    ~Matrix();
private:
    double* m_values = nullptr;
    unsigned n_columns = 0;
    unsigned n_rows = 0;
};

// --------------- INTERNAL ---------------

namespace YuiOkusora {
    namespace Math {
        namespace Mat {
            extern void fillMatrix(Matrix* a, const double& b);
            extern void addMatrix(Matrix* a, Matrix& b);
            extern void addVal2Matrix(Matrix* a, const double& b);
            extern void subtractMatrix(Matrix* a, Matrix& b);
            extern void transposeMatrix(Matrix* a);
            extern void multiplyVal2Matrix(Matrix* a, const double& b);
            extern void multiplyMat2Matrix(Matrix* a, Matrix& b);
            extern void dotProductMatrix(Matrix* a, Matrix& b);
            extern void rotateMatrix180(Matrix *a);
        }
    }
}

