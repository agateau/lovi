#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "condition.h"

#include <QColor>
#include <QString>

#include <memory>

class Highlight {
public:
    std::unique_ptr<Condition> condition;
    QColor bgColor;
};

#endif // HIGHLIGHT_H
