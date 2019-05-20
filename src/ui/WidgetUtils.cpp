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
#include "WidgetUtils.h"

#include <QAction>
#include <QIcon>
#include <QLineEdit>
#include <QToolTip>

namespace WidgetUtils {

void addLineEditHelpIcon(QLineEdit* lineEdit, const QString& message) {
    auto* action = new QAction(QIcon::fromTheme("help-contents"), {}, lineEdit);
    QObject::connect(action, &QAction::triggered, action, [message] {
        QToolTip::showText(QCursor::pos(), message);
    });
    lineEdit->addAction(action, QLineEdit::TrailingPosition);
}

} // namespace WidgetUtils
