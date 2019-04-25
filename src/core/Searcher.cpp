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
#include "Searcher.h"

#include "LogModel.h"

#include <QDebug>

using std::optional;

Searchable::~Searchable() {
}

Searcher::Searcher(QObject* parent) : QObject(parent) {
}

static optional<int>
search(Searchable* searchable, const Condition* condition, int startRow, int endRow, int delta) {
    for (int row = startRow; row != endRow; row += delta) {
        if (searchable->lineMatches(row, condition)) {
            return row;
        }
    }
    return {};
}

void Searcher::start(Searchable* searchable,
                     std::unique_ptr<Condition> condition,
                     SearchDirection direction,
                     int startRow) {
    bool isDown = direction == SearchDirection::Down;
    int delta = isDown ? 1 : -1;
    int begin = isDown ? 0 : (searchable->lineCount() - 1);
    int end = isDown ? searchable->lineCount() : -1;
    optional<int> row = search(searchable, condition.get(), startRow, end, delta);
    if (row.has_value()) {
        finished({SearchResponse::DirectHit, row.value()});
        return;
    }
    row = search(searchable, condition.get(), begin, startRow, delta);
    if (row.has_value()) {
        finished({isDown ? SearchResponse::WrappedDown : SearchResponse::WrappedUp, row.value()});
        return;
    }
    finished({});
}
