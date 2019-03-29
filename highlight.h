#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "conditions.h"

#include <QColor>
#include <QString>

#include <memory>

class Highlight {
public:
    std::unique_ptr<Condition> condition;
    QColor bgColor;
    QColor fgColor;
};

#endif // HIGHLIGHT_H
