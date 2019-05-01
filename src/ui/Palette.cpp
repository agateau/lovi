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
#include "Palette.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

using std::optional;

static optional<int> toInt(const QString& txt) {
    bool ok;
    int value = txt.toInt(&ok);
    return ok ? value : optional<int>{};
}

optional<Palette> Palette::load(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << path << file.error();
        return {};
    }
    QTextStream stream(&file);
    Palette palette;

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        QStringList tokens = line.split(QRegularExpression("\\s+"));
        if (tokens.length() < 2) {
            continue;
        }
        if (tokens.at(0) == "Columns:") {
            auto columns = toInt(tokens.at(1));
            if (!columns.has_value()) {
                qWarning() << "Failed to read column count from" << path << "line:" << line;
                return {};
            }
            palette.columns = columns.value();
        }
        if (tokens.length() >= 3) {
            auto red = toInt(tokens.at(0));
            if (!red.has_value()) {
                continue;
            }
            auto green = toInt(tokens.at(1));
            if (!green.has_value()) {
                qWarning() << "Invalid green value in" << line;
                return {};
            }
            auto blue = toInt(tokens.at(2));
            if (!green.has_value()) {
                qWarning() << "Invalid red value in" << line;
                return {};
            }
            QColor color{red.value(), green.value(), blue.value()};
            palette.colors.push_back(color);
        }
    }
    if (palette.colors.size() == 0) {
        qWarning() << "No color found in" << path;
        return {};
    }

    if (palette.columns == 0) {
        palette.columns = palette.colors.size();
    }
    return std::move(palette);
}
