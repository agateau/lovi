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
#ifndef COLOR_H
#define COLOR_H

#include <QColor>

#include <optional>

class Color {
public:
    Color(const QColor& color);
    explicit Color(const QString& text);

    QColor toColor(const QString& matchingText) const;
    QString toString() const;
    bool isAuto() const;

    static Color createAuto();

private:
    Color();
    bool mIsAuto = false;
    QColor mColor;
};

bool operator==(const Color& c1, const Color& c2);

using OptionalColor = std::optional<Color>;

#endif // COLOR_H
