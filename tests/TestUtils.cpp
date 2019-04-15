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
#include "TestUtils.h"

std::ostream& operator<<(std::ostream& ostr, const QString& str) {
    ostr << '"' << str.toStdString() << '"';
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const QStringList& lst) {
    ostr << '{';
    for (const auto& str : lst) {
        ostr << '"' << str.toStdString() << "\", ";
    }
    ostr << '}';
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const QUrl& url) {
    ostr << '"' << url.toEncoded().constData() << '"';
    return ostr;
}
