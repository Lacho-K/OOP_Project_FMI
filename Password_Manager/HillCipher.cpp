#include "HillCipher.h"
#include "AsciiUtils.h"
#include "AutoCreator.hpp"
#include <ostream>
#include <istream>

const std::string HillCipher::ID = "hill";

Cipher* HillCipher::makeFromArgs(const std::vector<std::string>& args)
{
    int n = AsciiUtils::parseInt(args[0]);
    std::vector<std::vector<int>> key(n, std::vector<int>(n));

    int idx = 1;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            key[i][j] = AsciiUtils::parseInt(args[idx++]);
        }
    }

    return new HillCipher(key);
}

HillCipher::HillCipher(const std::vector<std::vector<int>>& key)
    : _key(key), _invKey(invertMatrix(key))
{}

std::string HillCipher::encrypt(const std::string& plain) const
{
    return process(plain, _key);
}

std::string HillCipher::decrypt(const std::string& coded) const
{
    return process(coded, _invKey);
}

std::string HillCipher::process(const std::string& text, const std::vector<std::vector<int>>& matrix) const
{
    int n = matrix.size();
    std::string padded = text;

    while (padded.size() % n != 0)
        padded += ' ';

    std::string result;

    for (size_t i = 0; i < padded.size(); i += n)
    {
        for (int row = 0; row < n; row++)
        {
            int sum = 0;
            for (int col = 0; col < n; col++)
            {
                sum += matrix[row][col] * AsciiUtils::decodeChar(padded[i + col]);
            }
            result += AsciiUtils::encodeChar(sum);
        }
    }

    return result;
}

void HillCipher::writeConfig(std::ostream& out) const
{
    int n = _key.size();
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            out.write(reinterpret_cast<const char*>(&_key[i][j]), sizeof(int));
        }
    }
}

void HillCipher::readConfig(std::istream& in)
{
    int n;
    in.read(reinterpret_cast<char*>(&n), sizeof(n));

    _key = std::vector<std::vector<int>>(n, std::vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            in.read(reinterpret_cast<char*>(&_key[i][j]), sizeof(int));
        }
    }
    _invKey = invertMatrix(_key);
}

int HillCipher::modInverse(int a)
{
    for (int x = 1; x < AsciiUtils::RANGE; x++)
    {
        if ((a * x) % AsciiUtils::RANGE == 1)
            return x;
    }
    throw std::runtime_error("Matrix is not invertible");
}

std::vector<std::vector<int>> HillCipher::invertMatrix(const std::vector<std::vector<int>>& m)
{
    int n = m.size();
    int det = determinant(m) % AsciiUtils::RANGE;
    if (det < 0)
        det += AsciiUtils::RANGE;

    int invDet = modInverse(det);

    std::vector<std::vector<int>> adj = adjugateMatrix(m);
    std::vector<std::vector<int>> inv(n, std::vector<int>(n));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int val = (adj[i][j] * invDet) % AsciiUtils::RANGE;
            if (val < 0)
                val += AsciiUtils::RANGE;
            inv[i][j] = val;
        }
    }

    return inv;
}

std::vector<std::vector<int>> HillCipher::adjugateMatrix(const std::vector<std::vector<int>>& m) const
{
    int n = m.size();
    std::vector<std::vector<int>> adj(n, std::vector<int>(n));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::vector<std::vector<int>> minor = getMinor(m, i, j);
            int sign = ((i + j) % 2 == 0) ? 1 : -1;
            adj[j][i] = sign * determinant(minor);
        }
    }

    return adj;
}

std::vector<std::vector<int>> HillCipher::getMinor(const std::vector<std::vector<int>>& m, int skipRow, int skipCol) const
{
    int n = m.size();
    std::vector<std::vector<int>> minor(n - 1, std::vector<int>(n - 1));

    for (int i = 0, mi = 0; i < n; i++)
    {
        if (i == skipRow) continue;
        for (int j = 0, mj = 0; j < n; j++)
        {
            if (j == skipCol) continue;
            minor[mi][mj++] = m[i][j];
        }
        mi++;
    }

    return minor;
}

int HillCipher::determinant(const std::vector<std::vector<int>>& m) const
{
    int n = m.size();
    if (n == 1)
        return m[0][0];

    int det = 0;
    for (int p = 0; p < n; p++)
    {
        std::vector<std::vector<int>> minor = getMinor(m, 0, p);
        int sign = (p % 2 == 0) ? 1 : -1;
        det += sign * m[0][p] * determinant(minor);
    }
    return det;
}

static AutoCreator<HillCipher> _;