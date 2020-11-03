/*
 * Copyright 2020 Aurélien Gâteau <mail@agateau.com>
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
#include "ConditionLineEdit.h"

#include "ConditionLineEditChecker.h"
#include "WidgetUtils.h"

static constexpr char HIGHLIGHT_SYNTAX_HELP[] = QT_TRANSLATE_NOOP(
    "ConditionLineEdit",
    "<html>The syntax for a condition is:\n"
    "\n"
    "<pre>[COLUMN] [OPERATOR] [CRITERIA]</pre>\n"
    "\n"
    "<p><b>COLUMN</b>: The name of a column defined in the log parser.</p>"
    "<p><b>OPERATOR</b>: One of:<ul>"
    "  <li>= or ==: exact match</li>"
    "  <li>contains: the cell must contain the \"criteria\" string</li>"
    "  <li>~: the cell must match the \"criteria\" regular expression</li>"
    "</ul></p>"
    "<p><b>CRITERIA</b>: The string to match. If it contains spaces, surround it with "
    "<b>\"</b>. To use a literal <b>\"</b>, escape it by prefixing it with another <b>\"</b>.</p>"
    "</html>");

ConditionLineEdit::ConditionLineEdit(QWidget* parent)
        : QLineEdit(parent), mLineEditChecker(std::make_unique<ConditionLineEditChecker>(this)) {
    WidgetUtils::addLineEditHelpIcon(this, tr(HIGHLIGHT_SYNTAX_HELP));
}

ConditionLineEdit::~ConditionLineEdit() = default;

void ConditionLineEdit::setLogFormat(const LogFormat* logFormat) {
    mLineEditChecker->setLogFormat(logFormat);
}
