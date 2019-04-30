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
#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>

class Condition;

enum class SearchDirection { Up, Down };
enum class SearchMatchType { None, Direct, HitTop, HitBottom };

struct SearchResponse {
    SearchMatchType matchType = SearchMatchType::None;
    int row = -1;
};

Q_DECLARE_METATYPE(SearchResponse)

class Searchable {
public:
    virtual ~Searchable();

    virtual int lineCount() const = 0;

    virtual bool lineMatches(int row, const Condition* condition) const = 0;
};

class Searcher : public QObject {
    Q_OBJECT
public:
    Searcher(QObject* parent = nullptr);

    void start(const Searchable*, const Condition* condition, SearchDirection, int startRow);

signals:
    void finished(const SearchResponse& response);
};

#endif // SEARCHER_H
