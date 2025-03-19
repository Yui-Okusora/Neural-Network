#include "Matrix.hpp"

Matrix::Matrix(Matrix& a) {
    n_columns = a.getCols();
    n_rows = a.getRows();

    MemMng.memcopy(&m_values, &a.m_values, sizeof(float) * n_columns * n_rows);
}

Matrix::Matrix(AdvancedMemory* ptr, unsigned rows, unsigned cols)
{
    n_columns = cols;
    n_rows = rows;

    MemMng.memcopy(&m_values, ptr, sizeof(float) * rows * cols);
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
    m_values.~AdvancedMemory();
}

void Matrix::operator=(const Matrix& b)
{
    n_rows = b.getRows();
    n_columns = b.getCols();
    MemMng.memcopy(&m_values, &const_cast<Matrix&>(b).m_values, sizeof(float) * n_rows * n_columns);
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

void Matrix::unload(LPVOID viewAddress)
{
    m_values.unload(viewAddress);
}

Matrix Matrix::setRandomVals(float randVal(void))
{
    size_t size = static_cast<size_t>(getRows()) * getCols();
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * size);
    for (unsigned i = 0; i < size; ++i)
    {
        m_values.at<float>(i, view) = randVal();
    }
    m_values.unload(view.lpMapAddress);
    return *this;
}

Matrix Matrix::fill(const float& x)
{
    //memset(m_values, x, sizeof(float) * n_rows * n_columns);
    return *this;
}

Matrix Matrix::add(Matrix& b)
{
    Matrix tmp(*this);
    YuiOkusora::Math::Mat::addMatrix(&tmp, b);
    return tmp;
}

Matrix Matrix::add(const float& b)
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

Matrix Matrix::multiply(float x)
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
    m_values.unload(view.lpMapAddress);
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
    m_values.unload(view.lpMapAddress);
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
    m_values.unload(view.lpMapAddress);
}

void Matrix::printFlat(std::ofstream& f)
{
    ViewOfAdvancedMemory& view = m_values.load(0, sizeof(float) * n_rows * n_columns);
    f << n_rows << " " << n_columns << " ";
    for (unsigned i = 0; i < n_rows * n_columns; ++i)
        f << m_values.at<float>(i, view) << " ";
    f << "\n";
    m_values.unload(view.lpMapAddress);
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