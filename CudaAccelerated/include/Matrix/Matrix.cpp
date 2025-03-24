#include "Matrix.hpp"

Matrix::Matrix()
{

}

Matrix::Matrix(Matrix& a) {
    n_columns = a.getCols();
    n_rows = a.getRows();

    MemMng.memcopy(&m_values, &a.m_values, sizeof(float), sizeof(float) * n_columns * n_rows);
}

Matrix::Matrix(AdvancedMemory* ptr, unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;

    MemMng.memcopy(&m_values, ptr, sizeof(float), sizeof(float) * rows * cols);
}

Matrix::Matrix(float* matrix, unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;

    MemMng.memcopy(&m_values, matrix, sizeof(float) * rows * cols);
    
    //m_values = matrix;
}

Matrix::Matrix(unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;
    
    MemMng.createTmp(&m_values, sizeof(float) * rows * cols);
}

Matrix::~Matrix()
{
    //std::cout << "Matrix destroyed " << m_values.getID() << "\n";
    //m_values.~AdvancedMemory();
}

Matrix& Matrix::operator=(Matrix& b)
{
    n_rows = b.getRows();
    n_columns = b.getCols();
    MemMng.memcopy(&m_values, &b.m_values, sizeof(float), sizeof(float) * n_rows * n_columns);
    return *this;
}

float& Matrix::getValue(unsigned row, unsigned col, const ViewOfAdvancedMemory& view)
{
    return m_values.at<float>((size_t)n_columns * row + col, view);
}

void Matrix::resize(unsigned size)
{
    m_values.resize(sizeof(float) * size);
}

ViewOfAdvancedMemory& Matrix::load(size_t offset, size_t size)
{
    return m_values.load(offset, size);
}

void Matrix::unload(ViewOfAdvancedMemory& view)
{
    m_values.unload(view);
}

Matrix Matrix::setRandomVals(float randVal(void))
{
    size_t size = static_cast<size_t>(getRows()) * getCols();
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * size);
    for (unsigned i = 0; i < size; ++i)
    {
        m_values.at<float>(i, view) = randVal();
    }
    m_values.unload(view);
    return *this;
}

Matrix Matrix::fill(const float& x)
{
    //memset(m_values, x, sizeof(float) * n_rows * n_columns);
    return *this;
}

Matrix& Matrix::add_nocopy(Matrix& b)
{
    if (n_rows != b.getRows() || n_columns != b.getCols())
        throw std::out_of_range("Adding matrixes are not the same dimensions\n");
    YuiOkusora::Math::Mat::addMatrix(this, b);
    return *this;
}

Matrix Matrix::add(Matrix& b)
{
    Matrix tmp(*this);
    tmp.add_nocopy(b);
    return tmp;
}

Matrix& Matrix::add_nocopy(const float& b)
{
    YuiOkusora::Math::Mat::addVal2Matrix(this, b);
    return *this;
}

Matrix Matrix::add(const float& b)
{
    Matrix tmp(*this);
    tmp.add_nocopy(b);
    return tmp;
}

Matrix& Matrix::subtr_nocopy(Matrix& b)
{
    if (n_rows != b.getRows() || n_columns != b.getCols())
        throw std::out_of_range("Subtracting matrixes are not the same dimensions\n");
    YuiOkusora::Math::Mat::subtractMatrix(this, b);
    return *this;
}

Matrix Matrix::subtr(Matrix& b)
{
    Matrix tmp(*this);
    tmp.subtr_nocopy(b);
    return tmp;
}

Matrix& Matrix::transpose_nocopy()
{
    YuiOkusora::Math::Mat::transposeMatrix(this);

    unsigned tmp1 = getCols();
    setCols(getRows());
    setRows(tmp1);

    return *this;
}

Matrix Matrix::transpose()
{
    Matrix tmp(*this);
    tmp.transpose_nocopy();
    return tmp;
}

Matrix& Matrix::multiply_nocopy(float x)
{
    YuiOkusora::Math::Mat::multiplyVal2Matrix(this, x);
    return *this;
}

Matrix Matrix::multiply(float x)
{
    Matrix tmp(*this);
    tmp.multiply_nocopy(x);
    return tmp;
}

Matrix& Matrix::multiply_nocopy(Matrix& b)
{
    if (n_rows != b.getRows() || n_columns != b.getCols())
        throw std::out_of_range("Scaling matrixes are not the same dimensions\n");
    YuiOkusora::Math::Mat::multiplyMat2Matrix(this, b);
    return *this;
}

Matrix Matrix::multiply(Matrix& b)
{
    Matrix tmp(*this);
    tmp.multiply_nocopy(b);
    return tmp;
}

Matrix& Matrix::dotProduct_nocopy(Matrix& b)
{
    if (n_columns != b.getRows())
        throw std::out_of_range("Multipling matrix A columns not equal matrix B rows\n");
    YuiOkusora::Math::Mat::dotProductMatrix(this, b);
    setCols(b.getCols());

    return *this;
}

Matrix Matrix::dotProduct(Matrix& b)
{
    Matrix tmp(*this);
    tmp.dotProduct_nocopy(b);
    return tmp;
}

Matrix& Matrix::applyActivation_nocopy(ActivationType activation, float reluParam)
{
    YuiOkusora::Math::Mat::applyActivationMatrix(this, activation, reluParam);
    return *this;
}

Matrix Matrix::applyActivation(ActivationType activation, float reluParam)
{
    Matrix tmp(*this);
    tmp.applyActivation(activation, reluParam);
    return tmp;
}

Matrix& Matrix::applyActivationDerivative_nocopy(ActivationType activation, Matrix* scalar, float reluParam)
{
    if (scalar != NULL)
        if (n_rows != scalar->getRows() || n_columns != scalar->getCols())
            throw std::out_of_range("Matrixes are not the same dimensions\n");
    YuiOkusora::Math::Mat::applyActivationDerivativeMatrix(this, activation, reluParam, scalar);
    return *this;
}

Matrix Matrix::applyActivationDerivative(ActivationType activation, Matrix* scalar, float reluParam)
{
    Matrix tmp(*this);
    tmp.applyActivationDerivative_nocopy(activation, scalar, reluParam);
    return tmp;
}

Matrix Matrix::rot180()
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::rotateMatrix180(&tmp);
    return tmp;
}

float Matrix::sum()
{
    size_t size = static_cast<size_t>(getRows()) * getCols();
    float sum = 0.0;
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * n_rows * n_columns);
    for (unsigned i = 0; i < size; ++i)
    {
        sum += m_values.at<float>(i, view);
    }
    m_values.unload(view);
    return sum;
}

void Matrix::print()
{
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * n_rows * n_columns);
    for (unsigned i = 0; i < n_rows; ++i)
    {
        for (unsigned j = 0; j < n_columns; ++j)
            std::cout << std::setw(3) << getValue(i,j, view) << " ";
        std::cout << "\n";
    }
    m_values.unload(view);
}

void Matrix::read(std::ifstream& f)
{
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * n_rows * n_columns);
    f >> n_rows >> n_columns;
    size_t size = static_cast<size_t>(getRows()) * getCols();
    for (unsigned i = 0; i < size; ++i)
    {
        f >> m_values.at<float>(i, view);
    }
    m_values.unload(view);
}

void Matrix::printFlat(std::ofstream& f)
{
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * n_rows * n_columns);
    f << n_rows << " " << n_columns << " ";
    for (unsigned i = 0; i < n_rows * n_columns; ++i)
        f << m_values.at<float>(i, view) << " ";
    f << "\n";
    m_values.unload(view);
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