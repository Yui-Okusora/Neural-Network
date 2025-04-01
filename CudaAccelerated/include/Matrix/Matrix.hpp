#pragma once
#include "HeaderLibs.hpp"
#include "AdvancedMemory/AdvancedMemory.hpp"
#include "MemoryManager/MemoryManager.hpp"
#include <thread>

class Matrix
{
public:
    Matrix();
    Matrix(Matrix& a);
    Matrix(AdvancedMemory* ptr, unsigned rows, unsigned cols);
    Matrix(float* matrix, unsigned rows, unsigned cols);
    Matrix(unsigned rows, unsigned cols);

    ViewOfAdvancedMemory& load(size_t offset, size_t size);
    void unload(ViewOfAdvancedMemory& view);
    float& getValue(unsigned row, unsigned col, const ViewOfAdvancedMemory& view);
    void* getViewPtr(const ViewOfAdvancedMemory& view) { return m_values->getViewPtr(view); }

    unsigned getRows() const { return n_rows; }
    unsigned getCols() const { return n_columns; }
    void setRows(unsigned rows) { n_rows = rows; }
    void setCols(unsigned cols) { n_columns = cols; }

    void resize(unsigned size) { m_values->resize(sizeof(float) * size); }
    void print();
    void printFlat(std::ofstream& f);
    void read(std::ifstream& f);

    friend std::ostream& operator<<(std::ostream& out, Matrix& a);
    friend std::ofstream& operator<<(std::ofstream& out, Matrix& a);
    friend std::ifstream& operator>>(std::ifstream& in, Matrix& a);

    Matrix& operator=(Matrix& b);
    Matrix& operator=(Matrix&& b) noexcept;

    Matrix setRandomVals(float randVal(void));
    Matrix fill(const float& x);

    Matrix add(const Matrix& b);
    Matrix add(const float& b);
    Matrix subtr(const Matrix& b);
    Matrix transpose();
    Matrix multiply(const Matrix& b);
    Matrix multiply(float x);
    Matrix dotProduct(const Matrix& b);
    Matrix applyActivation(ActivationType activation, float reluParam = 0.015f);
    Matrix applyActivationDerivative(ActivationType activation, const Matrix* scalar = NULL, float reluParam = 0.015f);

    //Matrix setRandomVals(float randVal(void));
    //Matrix fill(const float& x);
    inline Matrix& add_nocopy(const Matrix& b);
    inline Matrix& add_nocopy(const float& b);
    inline Matrix& subtr_nocopy(const Matrix& b);
    inline Matrix& transpose_nocopy();
    inline Matrix& multiply_nocopy(const Matrix& b);
    inline Matrix& multiply_nocopy(float x);
    inline Matrix& dotProduct_nocopy(const Matrix& b);
    inline Matrix& applyActivation_nocopy(ActivationType activation, float reluParam = 0.015f);
    inline Matrix& applyActivationDerivative_nocopy(ActivationType activation, const Matrix* scalar = NULL, float reluParam = 0.015f);


    //Matrix addPadding(unsigned paddingThickness, unsigned top, unsigned bot, unsigned left, unsigned right);
    //Matrix correlate(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix convolute(Matrix kernel, unsigned padding, unsigned stride);
    //Matrix pool(unsigned size, unsigned padding, unsigned stride);
    //MatrixRef submat(unsigned startRow, unsigned startCol, unsigned rowSize, unsigned colSize);
    Matrix rot180();
    float sum();
    ~Matrix();
private:
    unsigned n_rows = 0;
    unsigned n_columns = 0;
    AdvancedMemory* m_values = nullptr;
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
            extern void dotProductMatrix(Matrix* a, const Matrix& b);
            extern void rotateMatrix180(Matrix *a);
            extern void applyActivationMatrix(Matrix* a, ActivationType acti, float reluParam);
            extern void applyActivationDerivativeMatrix(Matrix* a, ActivationType acti, float reluParam, Matrix* scalar);
        }
    }
}

