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
#include "SearchBar.h"

#include "ConditionIO.h"
#include "ConditionLineEditChecker.h"
#include "Conditions.h"
#include "LogFormat.h"
#include "MainController.h"
#include "Searcher.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using std::unique_ptr;

SearchBar::SearchBar(QWidget* parent)
        : QWidget(parent)
        , mLineEdit(new QLineEdit(this))
        , mSearchButton(new QPushButton(this))
        , mLineEditChecker(std::make_unique<ConditionLineEditChecker>(mLineEdit)) {
    setupUi();
}

SearchBar::~SearchBar() {
}

void SearchBar::init(MainController* controller) {
    Q_ASSERT(!mController);
    Q_ASSERT(controller);
    mController = controller;
    connect(mController,
            &MainController::logFormatChanged,
            mLineEditChecker.get(),
            &ConditionLineEditChecker::setLogFormat);
}

void SearchBar::setupUi() {
    connect(mLineEdit, &QLineEdit::returnPressed, mSearchButton, &QPushButton::click);
    mSearchButton->setText(tr("Find"));
    connect(mSearchButton, &QPushButton::clicked, this, &SearchBar::start);

    auto* layout = new QHBoxLayout(this);
    layout->addWidget(mLineEdit);
    layout->addWidget(mSearchButton);
}

void SearchBar::start() {
    auto conditionOrError =
        ConditionIO::parse(mLineEdit->text(), mController->logFormat()->columnHash());
    if (std::holds_alternative<ConditionIO::ParseError>(conditionOrError)) {
        return;
    }
    auto condition = std::move(std::get<unique_ptr<Condition>>(conditionOrError));
    mController->startSearch(std::move(condition), SearchDirection::Down);
}
