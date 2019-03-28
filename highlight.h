#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "condition.h"

#include <QString>

#include <memory>

class Highlight {
public:
    std::unique_ptr<Condition> condition;
    QString bgColor;
};

#endif // HIGHLIGHT_H
