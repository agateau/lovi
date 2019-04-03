#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "conditions.h"

#include <QColor>
#include <QString>

#include <memory>
#include <optional>

class HighlightColor {
public:
    explicit HighlightColor(const QString& text);

    QColor toColor(const QString& matchingText) const;

private:
    bool mIsAuto = false;
    QColor mColor;
};

class Highlight {
public:
    std::unique_ptr<Condition> condition;
    std::optional<HighlightColor> bgColor;
    std::optional<HighlightColor> fgColor;
};

#endif // HIGHLIGHT_H
