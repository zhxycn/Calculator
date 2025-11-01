#ifndef CALCULATOR_CORE_H
#define CALCULATOR_CORE_H

#include <QObject>

class Core final : public QObject
{
    Q_OBJECT

public:
    explicit Core(QObject *parent = nullptr);
    ~Core() override;

    static bool isValid(std::vector<QString> &exp);
    static QString evaluate(std::vector<QString> &exp);
};

#endif //CALCULATOR_CORE_H
