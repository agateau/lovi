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
#include "ConditionIO.h"

#include "Conditions.h"

#include <QCoreApplication>

static const QChar QUOTE = '"';

using std::optional;
using std::unique_ptr;
using std::variant;

namespace ConditionIO {

optional<QStringList> tokenize(const QString& text) {
    QStringList out;
    bool quoted = false;
    QString token;
    auto nextCharIsQuote = [&text](int idx) {
        return idx < text.length() - 1 && text.at(idx + 1) == QUOTE;
    };
    for (int idx = 0; idx < text.length(); ++idx) {
        const QChar ch = text.at(idx);
        if (quoted) {
            if (ch == QUOTE) {
                if (nextCharIsQuote(idx)) {
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
                if (nextCharIsQuote(idx)) {
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

variant<unique_ptr<Condition>, ParseError> parse(const QString& text,
                                                 const ColumnHash& columnHash) {
    QStringList tokens;
    {
        optional<QStringList> maybeTokens = ConditionIO::tokenize(text);
        if (!maybeTokens.has_value()) {
            return QCoreApplication::translate("ConditionIO", "Failed to parse '%1'").arg(text);
        }
        tokens = maybeTokens.value();
    }
    if (tokens.length() != 3) {
        return QCoreApplication::translate("ConditionIO", "Expected 3 tokens in '%1'").arg(text);
    }

    auto columnName = tokens.at(0);
    auto op = tokens.at(1);
    auto value = tokens.at(2);

    auto it = columnHash.find(columnName);
    if (it == columnHash.end()) {
        return QCoreApplication::translate("ConditionIO", "No column named '%1'").arg(columnName);
    }
    int column = *it;

    if (op == "=" || op == "==") {
        return std::make_unique<ExactCondition>(column, value);
    } else if (op == "contains") {
        return std::make_unique<ContainsCondition>(column, value);
    } else if (op == "~") {
        QRegularExpression regex(value);
        if (!regex.isValid()) {
            return QCoreApplication::translate("ConditionIO", "'%1' is not a valid regex: %2")
                .arg(value, regex.errorString());
        }
        return std::make_unique<RegExCondition>(column, regex);
    } else {
        return QCoreApplication::translate("ConditionIO", "Invalid operator '%1'").arg(op);
    }
}

} // namespace ConditionIO
