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
#include "conditionio.h"

static const QChar QUOTE = '"';

namespace ConditionIO {

std::optional<QStringList> tokenize(const QString& text) {
    QStringList out;
    bool quoted = false;
    QString token;
    for (int idx = 0; idx < text.length(); ++idx) {
        const QChar ch = text.at(idx);
        auto nextCharIsQuote = [&text, idx] {
            return idx < text.length() - 1 && text.at(idx + 1) == QUOTE;
        };
        if (quoted) {
            if (ch == QUOTE) {
                if (nextCharIsQuote()) {
                    token.append(QUOTE);
                    ++idx;
                } else {
                    quoted = false;
                }
            } else {
                token.append(ch);
            }
        } else {
            if (ch == QUOTE) {
                if (nextCharIsQuote()) {
                    token.append(QUOTE);
                    ++idx;
                } else {
                    quoted = true;
                }
            } else if (ch.isSpace()) {
                if (!token.isEmpty()) {
                    out.append(token);
                    token.clear();
                }
            } else {
                token.append(ch);
            }
        }
    }
    if (quoted) {
        // Unfinished quote
        return {};
    }
    if (!token.isEmpty()) {
        out.append(token);
    }
    return std::move(out);
}

} // namespace ConditionIO
