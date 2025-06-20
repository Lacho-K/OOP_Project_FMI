#include "HillCipher.h"
#include "AsciiUtils.h"
#include "AutoCreator.hpp"
#include <fstream>

const std::string HillCipher::ID = "hill";

// Четем n, след това n×n цели числа от файл с матрица
std::vector<std::vector<int>> HillCipher::parseKey(std::istream& in)
{
    int n = 0;
    in >> n;
    if (!in || n <= 0)
    {
        throw std::runtime_error("Невалиден размер на матрица във файла с ключ");
    }

    std::vector<std::vector<int>> key(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (!(in >> key[i][j]))
            {
                throw std::runtime_error("Непълна матрица във файла с ключ");
            }
        }
    }

    return key;
}

Cipher* HillCipher::makeFromArgs(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        throw std::invalid_argument("Empty file name for Hill cipher");
    }

    std::ifstream file(args[0]);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: \"" + args[0] + "\"");
    }

    return new HillCipher(parseKey(file));
}

// създава валиден обект, не променя текста при encrypt и decrypt, използва се 
//за createFromStream
HillCipher::HillCipher() 
    : _key{ {1, 0}, {0, 1} }, _invKey(invertMatrix(_key))
{
}

HillCipher::HillCipher(const std::vector<std::vector<int>>& key) 
    : _key(key), _invKey(invertMatrix(key))
{
}

std::string HillCipher::name() const
{
    return ID;
}

std::string HillCipher::encrypt(const std::string& plain) const
{
    return process(plain, _key);
}

std::string HillCipher::decrypt(const std::string& coded) const
{
    std::string decoded = process(coded, _invKey);

    // Премахваме всички интервали от края
    while (!decoded.empty() && decoded.back() == ' ')
    {
        decoded.pop_back();
    }

    return decoded;
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
    size_t n = 0;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    if (!in || n == 0)
    {
        throw std::runtime_error("Невалиден размер на матрица в конфигурацията");
    }

    _key.assign(n, std::vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            in.read(reinterpret_cast<char*>(&_key[i][j]), sizeof(int));
        }
    }

    _invKey = invertMatrix(_key);
}

std::string HillCipher::process(const std::string& text,
    const std::vector<std::vector<int>>& matrix) const
{
    size_t n = matrix.size();
    std::string output;

    for (size_t idx = 0; idx < text.size(); idx += n)
    {
        for (size_t row = 0; row < n; ++row)
        {
            int sum = 0;

            for (size_t col = 0; col < n; ++col)
            {
                size_t pos = idx + col;
                char ch = (pos < text.size()) ? text[pos] : ' ';
                sum += matrix[row][col] * AsciiUtils::decodeChar(ch);
            }

            output += AsciiUtils::encodeChar(sum);
        }
    }

    return output;
}

int HillCipher::modInverse(int a)
{
    a %= AsciiUtils::RANGE;
    if (a < 0)
    {
        a += AsciiUtils::RANGE;
    }

    for (int x = 1; x < AsciiUtils::RANGE; ++x)
    {
        if ((a * x) % AsciiUtils::RANGE == 1)
        {
            return x;
        }
    }

    throw std::runtime_error("can't find modular Inverse");
}

// Обратна матрица с метода на Гаус–Жордан (функцията е създадена с помощта на AI)
std::vector<std::vector<int>> HillCipher::invertMatrix(
    const std::vector<std::vector<int>>& a) const
{
    size_t n = a.size();
    std::vector<std::vector<int>> aug(n, std::vector<int>(2 * n, 0));

    // Създаваме разширена матрица [A | I]
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            int val = a[i][j] % AsciiUtils::RANGE;
            if (val < 0)
                val += AsciiUtils::RANGE;
            aug[i][j] = val;
        }
        aug[i][n + i] = 1;
    }

    for (size_t col = 0; col < n; ++col)
    {
        // Търсим главен елемент (pivot)
        size_t pivotRow = col;
        while (pivotRow < n && aug[pivotRow][col] == 0)
        {
            ++pivotRow;
        }

        if (pivotRow == n)
        {
            throw std::runtime_error("Cannot invert matrix");
        }

        if (pivotRow != col)
        {
            std::swap(aug[pivotRow], aug[col]);
        }

        // Правим така, че главният елемент да стане 1
        int invPivot = modInverse(aug[col][col]);
        for (size_t j = 0; j < 2 * n; ++j)
        {
            aug[col][j] = (aug[col][j] * invPivot) % AsciiUtils::RANGE;
        }

        // Премахваме елементи в останалите редове
        for (size_t row = 0; row < n; ++row)
        {
            if (row == col)
                continue;

            int factor = aug[row][col];

            if (factor == 0) 
                continue;

            for (size_t j = 0; j < 2 * n; ++j)
            {
                int value = aug[row][j] - factor * aug[col][j];
                value %= AsciiUtils::RANGE;
                if (value < 0) value += AsciiUtils::RANGE;
                aug[row][j] = value;
            }
        }
    }

    // Дясната половина е обратната матрица
    std::vector<std::vector<int>> inverse(n, std::vector<int>(n, 0));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            inverse[i][j] = aug[i][n + j];
        }
    }

    return inverse;
}

static AutoCreator<HillCipher> _;
