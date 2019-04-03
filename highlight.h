#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "conditions.h"

#include <QColor>
#include <QString>

#include <memory>
#include <optional>

class Highlight {
public:
    std::unique_ptr<Condition> condition;
    std::optional<QColor> bgColor;
    std::optional<QColor> fgColor;
};

#endif // HIGHLIGHT_H
