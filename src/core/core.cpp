#include "core.h"

Core::Core(QObject *parent) : QObject(parent) {}

Core::~Core() = default;

bool Core::isValid(std::vector<QString> &exp)
{
    return true;
}

QString Core::evaluate(std::vector<QString> &exp)
{
    return "";
}
