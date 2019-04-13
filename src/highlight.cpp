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
#include "highlight.h"

#include "conditionio.h"

HighlightColor::HighlightColor(const QColor& color) : mIsAuto(false), mColor(color) {
}

HighlightColor::HighlightColor(const QString& text) {
    if (text == "auto") {
        mIsAuto = true;
        return;
    }
    mColor = text;
}

static std::vector<QColor> generateAutoColorVector() {
    std::vector<QColor> colors;
    for (qreal hue = 0; hue < 1; hue += 1.0 / 20) {
        colors.push_back(QColor::fromHsvF(hue, 0.5, 0.9));
    }
    return colors;
}

QColor HighlightColor::toColor(const QString& matchingText) const {
    if (!mIsAuto) {
        return mColor;
    }
    static std::vector<QColor> autoColors = generateAutoColorVector();
    int sum = std::accumulate(
        matchingText.begin(), matchingText.end(), 0, [](int currentSum, const QChar& ch) {
            return currentSum + ch.toLatin1();
        });
    return autoColors.at(sum % autoColors.size());
}

QString HighlightColor::toString() const {
    return mIsAuto ? "auto" : mColor.name();
}

bool HighlightColor::isAuto() const {
    return mIsAuto;
}

HighlightColor HighlightColor::createAuto() {
    HighlightColor color;
    color.mIsAuto = true;
    return color;
}

HighlightColor::HighlightColor() {
}

Highlight::Highlight(LogFormat* logFormat) : mLogFormat(logFormat) {
    Q_ASSERT(mLogFormat);
}

void Highlight::setConditionDefinition(const QString& definition) {
    mConditionDefinition = definition;

    condition = ConditionIO::parse(definition, mLogFormat->columnHash());
    mLogFormat->emitHighlightChanged(this);
}

QString Highlight::conditionDefinition() const {
    return mConditionDefinition;
}

void Highlight::setScope(Highlight::ColorScope scope) {
    if (mScope == scope) {
        return;
    }
    mScope = scope;
    mLogFormat->emitHighlightChanged(this);
}

void Highlight::setBgColor(const OptionalHighlightColor& color) {
    mBgColor = color;
    mLogFormat->emitHighlightChanged(this);
}

void Highlight::setFgColor(const OptionalHighlightColor& color) {
    mFgColor = color;
    mLogFormat->emitHighlightChanged(this);
}
