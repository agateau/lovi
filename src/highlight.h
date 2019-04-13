/*
 * Copyright 2019 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Lovi.
 *
 * Lovi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "conditions.h"

#include <QColor>
#include <QString>

#include <memory>
#include <optional>

class LogFormat;

class HighlightColor {
public:
    explicit HighlightColor(const QColor& color);
    explicit HighlightColor(const QString& text);

    QColor toColor(const QString& matchingText) const;
    QString toString() const;
    bool isAuto() const;

    static HighlightColor createAuto();

private:
    HighlightColor();
    bool mIsAuto = false;
    QColor mColor;
};

using OptionalHighlightColor = std::optional<HighlightColor>;

class Highlight {
public:
    enum ColorScope { Row, Cell };
    explicit Highlight(LogFormat* logFormat);

    void setConditionDefinition(const QString& definition);
    QString conditionDefinition() const;

    std::unique_ptr<Condition> condition;
    ColorScope scope;

    void setBgColor(const OptionalHighlightColor& color);

    OptionalHighlightColor bgColor() const {
        return mBgColor;
    }

    void setFgColor(const OptionalHighlightColor& color);

    OptionalHighlightColor fgColor() const {
        return mFgColor;
    }

private:
    LogFormat* const mLogFormat;
    QString mConditionDefinition;

    OptionalHighlightColor mBgColor;
    OptionalHighlightColor mFgColor;
};

#endif // HIGHLIGHT_H
