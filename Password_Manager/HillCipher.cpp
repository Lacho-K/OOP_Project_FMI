#include "HillCipher.h"
#include "AsciiUtils.h"
#include "AutoCreator.hpp"
#include <fstream>

const std::string HillCipher::ID = "hill";

Cipher* HillCipher::makeFromArgs(const std::vector<std::string>& args)
{
    if (args.empty())
        throw std::invalid_argument("Missing key file path for HillCipher");

    std::ifstream ifs(args[0]);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open key file: " + args[0]);
    }

    return new HillCipher(parseKey(ifs));
}

std::vector<std::vector<int>> HillCipher::parseKey(std::istream& in)
{
    int n;
    in >> n;

    if (!in || n <= 0)
        throw std::runtime_error("Invalid matrix size");

    std::vector<std::vector<int>> matrix(n, std::vector<int>(n));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            in >> matrix[i][j];
            if (!in)
                throw std::runtime_error("Invalid or incomplete matrix in file");
        }
    }

    return matrix;
}

HillCipher::HillCipher()
    : _key{ {1, 0}, {0, 1} },
    _invKey(invertMatrix(_key))
{}

HillCipher::HillCipher(const std::vector<std::vector<int>>& key)
    : _key(key),
    _invKey(invertMatrix(key))
{}

std::string HillCipher::encrypt(const std::string& plain) const
{
    return process(plain, _key);
}

std::string HillCipher::decrypt(const std::string& coded) const
{
    return process(coded, _invKey);
}

std::string HillCipher::name() const
{
    return ID;
}

std::string HillCipher::process(const std::string& text, const std::vector<std::vector<int>>& matrix) const
{
    size_t n = matrix.size();
    std::string paddedText = text;

    while (paddedText.length() % n != 0)
        paddedText += ' ';

    std::string result;
    result.reserve(paddedText.length());

    for (size_t i = 0; i < paddedText.length(); i += n)
    {
        for (size_t row = 0; row < n; ++row)
        {
            size_t sum = 0;
            for (size_t col = 0; col < n; ++col)
            {
                sum += static_cast<size_t>(matrix[row][col]) * AsciiUtils::decodeChar(paddedText[i + col]);
            }
            result += AsciiUtils::encodeChar(static_cast<int>(sum));
        }
    }
    return result;
}

void HillCipher::writeConfig(std::ostream& out) const
{
    size_t n = _key.size();
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            out.write(reinterpret_cast<const char*>(&_key[i][j]), sizeof(int));
        }
    }
}

void HillCipher::readConfig(std::istream& in)
{
    size_t n;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    if (!in || n == 0) {
        throw std::runtime_error("Invalid matrix size during config read");
    }

    _key.assign(n, std::vector<int>(n));
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            in.read(reinterpret_cast<char*>(&_key[i][j]), sizeof(int));
            if (!in) {
                throw std::runtime_error("Incomplete matrix during config read");
            }
        }
    }
    _invKey = invertMatrix(_key);
}

int HillCipher::modInverse(int a)
{
    a = (a % AsciiUtils::RANGE + AsciiUtils::RANGE) % AsciiUtils::RANGE;
    for (int x = 1; x < AsciiUtils::RANGE; ++x)
    {
        if (((static_cast<size_t>(a) * x) % AsciiUtils::RANGE) == 1)
            return x;
    }
    throw std::runtime_error("Matrix determinant not invertible (GCD(det, RANGE) != 1)");
}

std::vector<std::vector<int>> HillCipher::invertMatrix(const std::vector<std::vector<int>>& m) const
{
    size_t n = m.size();
    if (n == 0) {
        throw std::invalid_argument("Cannot invert an empty matrix");
    }

    int det = determinant(m);
    int detModRange = (det % AsciiUtils::RANGE + AsciiUtils::RANGE) % AsciiUtils::RANGE;

    if (detModRange == 0) {
        throw std::runtime_error("Matrix is not invertible (determinant is 0 mod RANGE)");
    }

    int invDet = modInverse(detModRange);

    std::vector<std::vector<int>> adj = adjugateMatrix(m);
    std::vector<std::vector<int>> inv(n, std::vector<int>(n));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            int val = adj[i][j] * invDet;
            int finalVal = static_cast<int>((val % AsciiUtils::RANGE + AsciiUtils::RANGE) % AsciiUtils::RANGE);
            inv[i][j] = finalVal;
        }
    }
    return inv;
}

std::vector<std::vector<int>> HillCipher::adjugateMatrix(const std::vector<std::vector<int>>& m) const
{
    size_t n = m.size();
    if (n == 0) return {};
    if (n == 1) return { {1} };

    std::vector<std::vector<int>> adj(n, std::vector<int>(n));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            std::vector<std::vector<int>> minor = getMinor(m, i, j);
            int cofactor = determinant(minor);
            if ((i + j) % 2 == 1) {
                cofactor = -cofactor;
            }
            adj[j][i] = cofactor;
        }
    }
    return adj;
}

std::vector<std::vector<int>> HillCipher::getMinor(const std::vector<std::vector<int>>& m, size_t skipRow, size_t skipCol) const
{
    size_t n = m.size();
    std::vector<std::vector<int>> minor(n - 1, std::vector<int>(n - 1));

    size_t minorRow = 0;
    for (size_t i = 0; i < n; ++i)
    {
        if (i == skipRow) continue;
        size_t minorCol = 0;
        for (size_t j = 0; j < n; ++j)
        {
            if (j == skipCol) continue;
            minor[minorRow][minorCol++] = m[i][j];
        }
        minorRow++;
    }
    return minor;
}

int HillCipher::determinant(const std::vector<std::vector<int>>& m) const
{
    size_t n = m.size();

    if (n == 0) return 0;
    if (n == 1) return m[0][0];

    size_t det = 0;
    for (size_t p = 0; p < n; ++p)
    {
        std::vector<std::vector<int>> minor = getMinor(m, 0, p);
        size_t term = static_cast<size_t>(m[0][p]) * determinant(minor);
        if (p % 2 == 1) {
            det = (det + AsciiUtils::RANGE - (term % AsciiUtils::RANGE)) % AsciiUtils::RANGE;
        }
        else {
            det = (det + (term % AsciiUtils::RANGE)) % AsciiUtils::RANGE;
        }
    }
    return static_cast<int>(det);
}

static AutoCreator<HillCipher> _;