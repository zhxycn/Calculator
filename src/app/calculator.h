#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>
#include <QPushButton>
#include <QKeyEvent>

#include "../core/core.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Calculator;
}
QT_END_NAMESPACE

class Calculator final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() override;

private slots:
    void onOperandClick();
    void onOperandClick(const QString &symbol);

    void onOperatorClick();
    void onOperatorClick(const QString &symbol);

    void onEqualClick();

    void onBackClick();

    void onClearClick();

private:
    Ui::Calculator *ui{};
    Core *core{};

    QString input{};
    QString operand{};
    std::vector<QString> exp{};

    bool isResult{false};

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // CALCULATOR_H
