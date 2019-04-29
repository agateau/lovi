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
#include "ui_SearchBar.h"

#include <QLineEdit>

using std::unique_ptr;

template <class T> static unique_ptr<T> initUi(QWidget* parent) {
    auto ui = std::make_unique<T>();
    ui->setupUi(parent);
    return ui;
}

SearchBar::SearchBar(QWidget* parent) : QWidget(parent), ui(initUi<Ui::SearchBar>(this)) {
    setupUi();
}

SearchBar::~SearchBar() {
}

void SearchBar::init(MainController* controller, QLineEdit* lineEdit) {
    Q_ASSERT(!mController);
    Q_ASSERT(controller);
    mController = controller;
    connect(mController->searcher(), &Searcher::finished, this, &SearchBar::onFinished);
    mLineEdit = lineEdit;
}

void SearchBar::setupUi() {
    layout()->setSpacing(0);
    layout()->setMargin(0);
    ui->resultLabel->hide();
    connect(ui->nextButton, &QToolButton::clicked, this, [this] { start(SearchDirection::Down); });
    connect(
        ui->previousButton, &QToolButton::clicked, this, [this] { start(SearchDirection::Up); });

    ui->nextButton->setShortcut(QKeySequence::Find);
}

void SearchBar::start(SearchDirection direction) {
    auto conditionOrError =
        ConditionIO::parse(mLineEdit->text(), mController->logFormat()->columnHash());
    if (std::holds_alternative<ConditionIO::ParseError>(conditionOrError)) {
        return;
    }
    auto condition = std::move(std::get<unique_ptr<Condition>>(conditionOrError));
    mController->startSearch(std::move(condition), direction);
}

void SearchBar::onFinished(const SearchResponse& response) {
    QString text;
    switch (response.matchType) {
    case SearchMatchType::Direct:
        ui->resultLabel->hide();
        return;
    case SearchMatchType::None:
        text = tr("No match found");
        break;
    case SearchMatchType::HitTop:
        text = tr("Hit top, continuing at bottom");
        break;
    case SearchMatchType::HitBottom:
        text = tr("Hit bottom, continuing at top");
        break;
    }
    ui->resultLabel->show();
    ui->resultLabel->setText(text);
}
