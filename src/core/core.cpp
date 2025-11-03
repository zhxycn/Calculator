#include "core.h"

#include <sstream>
#include <gmpxx.h>

Core::Core(QObject *parent) : QObject(parent) {
    mpf_set_default_prec(256);
}

Core::~Core() = default;

namespace {
bool isOperator(const QString &tok) {
    return tok == "+" || tok == "-" || tok == "*" || tok == "/";
}

int precedence(const QString &op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

bool isNumber(const QString &tok) {
    bool ok = false;
    tok.toDouble(&ok);
    return ok;
}

bool execute(std::vector<mpf_class> &vals, const QString &op) {
    if (vals.size() < 2) return false;

    const mpf_class b = vals.back();
    vals.pop_back();
    const mpf_class a = vals.back();
    vals.pop_back();
    mpf_class r = 0;

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

QString format(const mpf_class &v) {
    std::ostringstream oss;
    oss << v;
    std::string s = oss.str();

    if (s == "-0" || s == "-0.0") s = "0";

    if (s.find('.') != std::string::npos) {
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
        if (s.empty()) s = "0";
    }

    return QString::fromStdString(s);
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

        if (!isNumber(tok)) return false;
        if (!expectOperand) return false;
        expectOperand = false;
    }

    return !expectOperand && balance == 0;
}

QString Core::evaluate(const std::vector<QString> &exp)
{
    std::vector<mpf_class> values;
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

        try {
            mpf_class v(tok.toStdString());
            values.push_back(v);
        } catch (...) {
            return "Error";
        }
    }

    while (!ops.empty()) {
        if (!applyTop()) return "Error";
    }

    if (values.size() != 1) return "Error";

    return format(values.back());
}
