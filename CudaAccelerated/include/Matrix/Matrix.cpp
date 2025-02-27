#include "Matrix.hpp"

Matrix::Matrix(Matrix& a) {
    n_columns = a.getCols();
    n_rows = a.getRows();

    m_values = (double*)calloc(n_rows * n_columns, sizeof(double));
    memcpy(m_values, a.getFlatted(), sizeof(double) * n_rows * n_columns);
}

Matrix::Matrix(const double* matrix, unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;
    
    m_values = (double*)calloc(rows * cols, sizeof(double));
    memcpy(m_values, matrix, sizeof(double) * rows * cols);
}

Matrix::Matrix(unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;
    
    m_values = (double*)calloc(rows * cols, sizeof(double));
}

Matrix::~Matrix()
{
    if (m_values != nullptr) free(m_values);
}

void Matrix::copyVals(const Matrix& b)
{
    n_rows = b.getRows();
    n_columns = b.getCols();
    if (m_values != nullptr) free(m_values);
    m_values = (double*)malloc(sizeof(double) * n_rows * n_columns);
    memcpy(m_values, b.getFlatted(), sizeof(double) * n_rows * n_columns);
}

void Matrix::operator=(const Matrix& b)
{
    n_rows = b.getRows();
    n_columns = b.getCols();
    if (m_values != nullptr) free(m_values);
    m_values = (double*)malloc(sizeof(double) * n_rows * n_columns);
    memcpy(m_values, b.getFlatted(), sizeof(double) * n_rows * n_columns);
}

double& Matrix::getValue(unsigned row, unsigned col)
{
    return m_values[n_columns * row + col];
}

double Matrix::getConst(unsigned row, unsigned col) const
{
    return m_values[n_columns * row + col];
}

Matrix Matrix::setRandomVals(double randVal(void))
{
    size_t size = getRows() * getCols();
    for (unsigned i = 0; i < size; ++i)
    {
        m_values[i] = randVal();
    }
    return *this;
}

Matrix Matrix::fill(const double& x)
{
    Matrix tmp = *this;
    for (unsigned i = n_columns * n_rows; i > 0; --i)
    {
        tmp.getFlatted()[i] = x;
    }
    return tmp;
}

Matrix Matrix::add(Matrix& b)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::addMatrix(&tmp, b);
    return tmp;
}

Matrix Matrix::add(const double& b)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::addVal2Matrix(&tmp, b);
    return tmp;
}

Matrix Matrix::subtr(Matrix& b)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::subtractMatrix(&tmp, b);
    return tmp;
}

Matrix Matrix::transpose()
{
    Matrix tmp(*this);
    
    YuiOkusora::Math::Mat::transposeMatrix(&tmp);

    unsigned tmp1 = tmp.getCols();
    tmp.setCols(tmp.getRows());
    tmp.setRows(tmp1);

    return tmp;
}

Matrix Matrix::multiply(double x)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::multiplyVal2Matrix(&tmp, x);
    return tmp;
}

Matrix Matrix::multiply(Matrix& b)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::multiplyMat2Matrix(&tmp, b);
    return tmp;
}

Matrix Matrix::dotProduct(Matrix& b)
{
    
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::dotProductMatrix(&tmp, b);

    tmp.setCols(b.getRows());

    return tmp;
}

Matrix Matrix::addPadding(unsigned paddingThickness = 0, unsigned top = 1, unsigned bot = 1, unsigned left = 1, unsigned right = 1)
{
    Matrix tmp(n_rows + (top + bot) * paddingThickness, n_columns + (left + right) * paddingThickness);
    for (unsigned i = top * paddingThickness; i < n_rows + top * paddingThickness; ++i)
    {
        for (unsigned j = left * paddingThickness; j < n_columns + left * paddingThickness; ++j)
        {
            double tmp2 = getValue(i - top * paddingThickness, j - left * paddingThickness);
            tmp.getValue(i, j) = tmp2;
        }
    }
    return tmp;
}

Matrix Matrix::correlate(Matrix kernel, unsigned padding, unsigned stride)
{
    assert(kernel.n_columns == kernel.n_rows);
    Matrix tmp = addPadding(padding);
    Matrix out((tmp.n_rows - kernel.n_rows + 2 * padding) / stride - 1, (tmp.n_columns - kernel.n_columns + 2 * padding) / stride - 1);
    for (unsigned inp_row = 0; inp_row <= (tmp.n_rows - kernel.n_rows); inp_row += stride)
    {
        for (unsigned inp_col = 0; inp_col <= (tmp.n_columns - kernel.n_columns); inp_col += stride)
        {
            double sum = 0;
            for (unsigned k = 0; k < kernel.n_rows; ++k)
            {
                for (unsigned g = 0; g < kernel.n_columns; ++g)
                {
                    sum += kernel.getValue(k, g) * tmp.getValue(inp_row + k, inp_col + g);
                }
            }
            out.getValue(inp_row / stride, inp_col / stride) = sum;

        }

    }
    return out;
}

Matrix Matrix::convolute(Matrix kernel, unsigned padding, unsigned stride)
{
    kernel = kernel.rot180();
    return correlate(kernel, padding, stride);
}

/*Matrix Matrix::pool(unsigned size, unsigned padding, unsigned stride)
{
    Matrix tmp = addPadding(1, 0, n_rows % 2, 0, n_columns % 2);
    if (padding != 0) tmp = addPadding(padding);
    Matrix out(ceil((tmp.n_rows - size + 1) / (double)stride), ceil((tmp.n_columns - size + 1) / (double)stride));
    MatrixRef ptr = tmp.submat(0, 0, 2, 2);
    for (unsigned i = 0; i < out.getRows(); ++i)
    {
        for (unsigned j = 0; j < out.getCols(); ++j)
        {
            out.getValue(i, j) = ptr.move(i * stride, j * stride).getMax();

        }
    }
    return out;
}*/

/*MatrixRef Matrix::submat(unsigned startRow, unsigned startCol, unsigned rowSize, unsigned colSize)
{
    MatrixRef tmp(rowSize, colSize);
    tmp.setOrigin(n_rows, n_columns);
    for (unsigned i = startRow; i < startRow + rowSize; ++i)
    {
        for (unsigned j = startCol; j < startCol + colSize; ++j)
        {
            tmp.getPointerRef(i - startRow, j - startCol) = &getValue(i, j);
        }
    }
    return tmp;
}*/

Matrix Matrix::rot180()
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::rotateMatrix180(&tmp);
    return tmp;
}

double Matrix::sum()
{
    size_t size = getRows() * getCols();
    double sum = 0.0;
    for (unsigned i = 0; i < size; ++i)
    {
        sum += m_values[i];
    }
    return sum;
}

void Matrix::print()
{
    for (unsigned i = 0; i < n_rows; ++i)
    {
        for (unsigned j = 0; j < n_columns; ++j)
            std::cout << std::setw(3) << getValue(i,j) << " ";
        std::cout << "\n";
    }
}

std::ostream& operator<<(std::ostream& out, Matrix& a)
{
    a.print();
    return out;
}

std::ofstream& operator<<(std::ofstream& out, Matrix& a)
{
    a.printFlat(out);
    return out;
}

std::ifstream& operator>>(std::ifstream& in, Matrix& a)
{
    a.read(in);
    return in;
}

void Matrix::read(std::ifstream& f)
{
    f >> n_rows >> n_columns;
    size_t size = getRows() * getCols();
    for (unsigned i = 0; i < size; ++i)
    {
        f >> m_values[i];
    }
}

void Matrix::printFlat(std::ofstream& f)
{
    f << n_rows << " " << n_columns << " ";
    for (unsigned i = 0; i < n_rows * n_columns; ++i)
        f << m_values[i] << " ";
    f << "\n";
}