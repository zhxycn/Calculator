#include "calculator.h"
#include "ui_calculator.h"

#include <QPushButton>

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::onOperandClick()
{
    const auto btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;

    const auto symbol = btn->property("data");
    if (symbol.isNull()) return;

    onOperandClick(symbol.toString());
}

void Calculator::onOperandClick(const QString &symbol)
{
    if (isResult) {
        input.clear();
        operand.clear();
        exp.clear();

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);

        isResult = false;
    }

    if (operand.isEmpty() && !exp.empty()) {
        if (const QString &prev = exp.back(); prev == ")") {
            input.append("*");
            exp.emplace_back("*");
            ui->inputBrowser->setText(input);
        }
    }

    operand.append(symbol);

    ui->operandBrowser->setText(operand);
}

void Calculator::onOperatorClick()
{
    const auto btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;

    const auto symbol = btn->property("data");
    if (symbol.isNull()) return;

    onOperatorClick(symbol.toString());
}

void Calculator::onOperatorClick(const QString &symbol)
{
    if (isResult) {
        input.clear();
        exp.clear();

        isResult = false;
    }

    if (symbol == "(") {
        if (!operand.isEmpty()) {
            input.append(operand);
            exp.push_back(operand);
            operand.clear();
            input.append("*");
            exp.emplace_back("*");
        } else if (!exp.empty()) {
            if (const QString &prev = exp.back(); !prev.isEmpty() && (prev.back().isDigit() || prev == ")")) {
                input.append("*");
                exp.emplace_back("*");
            }
        }

        input.append(symbol);
        exp.push_back(symbol);

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);
        return;
    }

    if (symbol == ")") {
        if (!operand.isEmpty()) {
            input.append(operand);
            exp.push_back(operand);
            operand.clear();
        }

        input.append(symbol);
        exp.push_back(symbol);

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);
        return;
    }

    if (symbol == "-") {
        bool prevIsOperator = true;

        if (!exp.empty()) {
            if (const QString &prev = exp.back(); !prev.isEmpty() && (prev.back().isDigit() || prev == ")"))
                prevIsOperator = false;
        }

        if (operand.isEmpty() && prevIsOperator) {
            operand.append(symbol);

            ui->operandBrowser->setText(operand);
            return;
        }
    }

    if (operand.isEmpty()) {
        bool prevIsOperand = false;

        if (!exp.empty()) {
            if (const QString &prev = exp.back(); !prev.isEmpty() && (prev.back().isDigit() || prev == ")"))
                prevIsOperand = true;
        }

        if (prevIsOperand) {
            input.append(symbol);
            exp.push_back(symbol);

            ui->inputBrowser->setText(input);
            ui->operandBrowser->setText(operand);
            return;
        }
    }

    if (!operand.isEmpty()) {
        input.append(operand);
        input.append(symbol);
        exp.push_back(operand);
        exp.push_back(symbol);
        operand.clear();

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);
    }
}

void Calculator::onEqualClick()
{
    if (isResult) {
        input.clear();
        exp.clear();

        isResult = false;
    }

    if (!operand.isEmpty()) {
        input.append(operand);
        exp.push_back(operand);
        operand.clear();
        input.append("=");

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);
    }

    if (Core::isValid(exp)) {
        operand = Core::evaluate(exp);

        ui->operandBrowser->setText(operand);
    } else {
        input.clear();
        operand.clear();

        ui->operandBrowser->setText("Error");
    }

    isResult = true;
}

void Calculator::onBackClick()
{
    if (input.isEmpty() && operand.isEmpty()) return;

    if (!operand.isEmpty()) {
        operand.chop(1);

        ui->operandBrowser->setText(operand);
        return;
    }

    if (!exp.empty()) {
        exp.pop_back();
        input.clear();

        for (const auto &tok : exp) {
            input.append(tok);
        }

        ui->inputBrowser->setText(input);
        ui->operandBrowser->setText(operand);
    }
}

void Calculator::onClearClick()
{
    input.clear();
    operand.clear();

    ui->inputBrowser->setText(input);
    ui->operandBrowser->setText(operand);
}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    if (const QString text = event->text(); !text.isEmpty()) {
        const QChar ch = text.at(0);

        if (QString("0123456789.").contains(ch)) {
            onOperandClick(QString(ch));
            return;
        }

        if (QString("+-*/()").contains(ch)) {
            onOperatorClick(QString(ch));
            return;
        }
    }

    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            onEqualClick();
            return;
        case Qt::Key_Backspace:
            onBackClick();
            return;
        case Qt::Key_Escape:
            onClearClick();
            return;
        default:
            break;
    }

    QMainWindow::keyPressEvent(event);
}
