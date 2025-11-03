#include "core.h"

#include <gmpxx.h>

Core::Core(QObject *parent)
    : QObject(parent) {}

Core::~Core() = default;

namespace {
bool isOperator(const QString &tok)
{
    return tok == "+" || tok == "-" || tok == "*" || tok == "/";
}

int precedence(const QString &op)
{
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

bool parseNumber(const QString &token, mpq_class &out)
{
    QString s = token.trimmed();
    if (s.isEmpty()) return false;

    int sign = 1;
    if (s.startsWith('+')) {
        s = s.mid(1);
    } else if (s.startsWith('-')) {
        sign = -1;
        s = s.mid(1);
    }

    if (s.isEmpty()) return false;

    const auto dot = s.indexOf('.');
    QString digits = s;
    qsizetype fracLen = 0;

    if (dot >= 0) {
        const QString intPart = s.left(dot);
        const QString fracPart = s.mid(dot + 1);
        digits = intPart + fracPart;
        fracLen = fracPart.length();
    }

    if (digits.isEmpty()) return false;

    for (QChar c : digits) {
        if (!c.isDigit()) return false;
    }

    mpz_class num;
    try {
        num = mpz_class(digits.toStdString(), 10);
    } catch (...) {
        return false;
    }

    if (sign < 0) num = -num;

    mpz_class den = 1;
    if (fracLen > 0) {
        mpz_class power = 1;
        for (qsizetype i = 0; i < fracLen; ++i) power *= 10;
        den = power;
    }

    mpq_class q(num, den);
    q.canonicalize();
    out = q;
    return true;
}

bool execute(std::vector<mpq_class> &vals, const QString &op)
{
    if (vals.size() < 2) return false;

    const mpq_class b = vals.back();
    vals.pop_back();
    const mpq_class a = vals.back();
    vals.pop_back();
    mpq_class r = 0;

    if (op == "+") r = a + b;
    else if (op == "-") r = a - b;
    else if (op == "*") r = a * b;
    else if (op == "/") {
        if (b == 0) return false;
        r = a / b;
    } else {
        return false;
    }

    vals.push_back(r);
    return true;
}

QString toDecimalString(const mpq_class &v, int maxFrac = 8)
{
    if (v == 0) return "0";

    mpz_class num = v.get_num();
    const mpz_class& den = v.get_den();

    const bool neg = num < 0;
    if (neg) num = -num;

    const mpz_class q = num / den;
    mpz_class r = num % den;

    QString intStr = QString::fromStdString(q.get_str());

    if (r == 0) {
        if (neg && intStr != "0") return "-" + intStr;
        return intStr;
    }

    QString fracStr;
    fracStr.reserve(maxFrac);

    for (int i = 0; i < maxFrac; ++i) {
        r *= 10;
        mpz_class digit = r / den;
        r %= den;
        fracStr.append(QChar('0' + static_cast<int>(digit.get_si())));
        if (r == 0) break;
    }

    if (r != 0) {
        if (const mpz_class next = (r * 10) / den; next >= 5) {
            int carry = 1;

            for (long long i = fracStr.size() - 1; i >= 0 && carry; --i) {
                if (const int d = fracStr.at(i).unicode() - '0' + carry; d >= 10) {
                    fracStr[i] = QChar('0');
                    carry = 1;
                } else {
                    fracStr[i] = QChar('0' + d);
                    carry = 0;
                }
            }

            if (carry) {
                for (long long i = intStr.size() - 1; i >= 0 && carry; --i) {
                    if (!intStr[i].isDigit()) continue;

                    if (const int d = intStr.at(i).unicode() - '0' + carry; d >= 10) {
                        intStr[i] = QChar('0');
                        carry = 1;
                    } else {
                        intStr[i] = QChar('0' + d);
                        carry = 0;
                    }
                }

                if (carry) intStr.prepend('1');
            }
        }
    }

    long long end = fracStr.size();
    while (end > 0 && fracStr.at(end - 1) == '0') --end;
    fracStr.truncate(end);

    if (fracStr.isEmpty()) {
        if (neg && intStr != "0") return "-" + intStr;
        return intStr;
    }

    QString res = intStr + "." + fracStr;
    if (neg) res.prepend('-');
    return res;
}
}

bool Core::isValid(const std::vector<QString> &exp)
{
    if (exp.empty()) return false;

    int balance = 0;
    bool expectOperand = true;

    for (const auto &tok : exp) {
        if (tok == "(") {
            if (!expectOperand) return false;
            balance++;
            expectOperand = true;
            continue;
        }

        if (tok == ")") {
            if (expectOperand) return false;
            if (balance <= 0) return false;
            balance--;
            expectOperand = false;
            continue;
        }

        if (isOperator(tok)) {
            if (expectOperand) return false;
            expectOperand = true;
            continue;
        }

        if (mpq_class _; !parseNumber(tok, _)) return false;
        if (!expectOperand) return false;
        expectOperand = false;
    }

    return !expectOperand && balance == 0;
}

QString Core::evaluate(const std::vector<QString> &exp)
{
    std::vector<mpq_class> values;
    std::vector<QString> ops;
    values.reserve(exp.size());
    ops.reserve(exp.size());

    auto applyTop = [&]() -> bool {
        const QString op = ops.back();
        ops.pop_back();
        return execute(values, op);
    };

    for (const auto &tok : exp) {
        if (tok == "(") {
            ops.push_back(tok);
            continue;
        }

        if (tok == ")") {
            while (!ops.empty() && ops.back() != "(") {
                if (!applyTop()) return "Error";
            }
            if (ops.empty()) return "Error";
            ops.pop_back();
            continue;
        }

        if (isOperator(tok)) {
            while (!ops.empty() && ops.back() != "(" && precedence(ops.back()) >= precedence(tok)) {
                if (!applyTop()) return "Error";
            }
            ops.push_back(tok);
            continue;
        }

        mpq_class v;
        if (!parseNumber(tok, v)) return "Error";
        values.push_back(v);
    }

    while (!ops.empty()) {
        if (!applyTop()) return "Error";
    }

    if (values.size() != 1) return "Error";

    return toDecimalString(values.back());
}
