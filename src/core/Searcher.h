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

#include <memory>

class Condition;
class LogModel;

enum class SearchDirection { Up, Down };

struct SearchResponse {
    enum Result { NoHit, DirectHit, WrappedUp, WrappedDown };
    Result result = NoHit;
    int row = -1;
};

class Searcher : public QObject {
    Q_OBJECT
public:
    Searcher(QObject* parent = nullptr);

    void start(LogModel*, std::unique_ptr<Condition> condition, SearchDirection, int startRow);

signals:
    void finished(const SearchResponse& response);
};

#endif // SEARCHER_H
