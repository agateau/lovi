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
#include "LogFormatWidget.h"

#include "HighlightModel.h"
#include "LineEditChecker.h"
#include "LogFormat.h"
#include "LogFormatModel.h"
#include "LogFormatStore.h"
#include "MainController.h"
#include "Searcher.h"
#include "WidgetFloater.h"
#include "ui_LogFormatWidget.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QToolTip>

LogFormatWidget::LogFormatWidget(MainController* controller, QWidget* parent)
        : QWidget(parent)
        , mController(controller)
        , ui(std::make_unique<Ui::LogFormatWidget>())
        , mLogFormatModel(std::make_unique<LogFormatModel>(controller->logFormatStore()))
        , mHighlightModel(std::make_unique<HighlightModel>()) {
    Q_ASSERT(mController);
    ui->setupUi(this);
    setupLogFormatSelector();
    setupEditor();
    setupSearchBar();
    setLogFormat(mController->logFormat());
}

LogFormatWidget::~LogFormatWidget() {
}

void LogFormatWidget::setLogFormat(LogFormat* logFormat) {
    Q_ASSERT(logFormat);
    selectLogFormat(logFormat->name());
    ui->parserLineEdit->setText(logFormat->parserPattern());
    mHighlightModel->setLogFormat(logFormat);
}

void LogFormatWidget::setupLogFormatSelector() {
    ui->logFormatComboBox->setModel(mLogFormatModel.get());

    connect(ui->logFormatComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &LogFormatWidget::onCurrentLogFormatChanged);
    connect(ui->addFormatButton, &QToolButton::clicked, this, &LogFormatWidget::onAddFormatClicked);

    connect(mController, &MainController::logFormatChanged, this, &LogFormatWidget::setLogFormat);
}

void LogFormatWidget::setupEditor() {
    // Parser edit
    connect(ui->parserLineEdit,
            &QLineEdit::editingFinished,
            this,
            &LogFormatWidget::onParserEditingFinished);
    new LineEditChecker(ui->parserLineEdit, [](const QString& text) -> QString {
        QRegularExpression rx(text);
        return rx.isValid() ? QString() : rx.errorString();
    });

    // Highlight list
    ui->highlightListView->setModel(mHighlightModel.get());

    connect(ui->highlightListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &LogFormatWidget::onCurrentHighlightChanged);

    // Highlight list context menu
    auto removeHighlightAction = new QAction(tr("Remove Highlight"));
    connect(removeHighlightAction, &QAction::triggered, this, [this] {
        auto index = ui->highlightListView->currentIndex();
        if (!index.isValid()) {
            return;
        }
        mHighlightModel->logFormat()->removeHighlightAt(index.row());
    });
    ui->highlightListView->addAction(removeHighlightAction);
    ui->highlightListView->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Highlight add button
    auto addHighlightButton = new QToolButton;
    addHighlightButton->setIcon(QIcon::fromTheme("list-add"));
    connect(addHighlightButton, &QToolButton::pressed, this, [this] {
        Highlight* highlight = mHighlightModel->logFormat()->addHighlight();
        highlight->setBgColor(Color::createAuto());
    });

    auto floater = new WidgetFloater(ui->highlightListView);
    floater->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    floater->setChildWidget(addHighlightButton);
}

void LogFormatWidget::setupSearchBar() {
    ui->searchBar->setEnabled(false);
    ui->searchBar->layout()->setMargin(0);
    connect(mController->searcher(), &Searcher::finished, this, &LogFormatWidget::onSearchFinished);
    connect(ui->searchNextButton, &QToolButton::clicked, this, [this] {
        mController->startSearch(SearchDirection::Down);
    });
    connect(ui->searchPreviousButton, &QToolButton::clicked, this, [this] {
        mController->startSearch(SearchDirection::Up);
    });

    ui->searchNextButton->setShortcut(QKeySequence::Find);
}

void LogFormatWidget::onCurrentLogFormatChanged(int row) {
    QModelIndex index = mLogFormatModel->index(row);
    if (!index.isValid()) {
        return;
    }
    LogFormat* logFormat = mLogFormatModel->logFormatForIndex(index);
    mController->setLogFormat(logFormat);
}

void LogFormatWidget::onCurrentHighlightChanged(const QModelIndex& index) {
    auto logFormat = mHighlightModel->logFormat();
    auto* highlight = index.isValid() ? logFormat->editableHighlightAt(index.row()) : nullptr;
    mController->setCurrentHighlight(highlight);
    ui->highlightWidget->setHighlight(highlight);
    ui->searchBar->setEnabled(highlight);
}

void LogFormatWidget::onParserEditingFinished() {
    LogFormat* logFormat = mController->logFormat();
    Q_ASSERT(logFormat);
    logFormat->setParserPattern(ui->parserLineEdit->text());
}

void LogFormatWidget::onAddFormatClicked() {
    QString name = QInputDialog::getText(
        this, tr("Log format name"), tr("Enter a name for the new log format"));
    if (name.isEmpty()) {
        return;
    }
    auto error = mController->logFormatStore()->addLogFormat(name);
    if (error.has_value()) {
        QString message = error.value();
        QMessageBox box(this);
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("Could not add format."));
        box.setInformativeText(message);
        box.exec();
        return;
    }
    selectLogFormat(name);
}

void LogFormatWidget::selectLogFormat(const QString& name) {
    for (int row = 0; row < ui->logFormatComboBox->count(); ++row) {
        if (ui->logFormatComboBox->itemText(row) == name) {
            ui->logFormatComboBox->setCurrentIndex(row);
            return;
        }
    }
}

void LogFormatWidget::onSearchFinished(const SearchResponse& response) {
    QString text;
    switch (response.matchType) {
    case SearchMatchType::Direct:
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
    auto pos = ui->searchNextButton->mapToGlobal({ui->searchNextButton->width(), 0});
    QToolTip::showText(pos, text);
}
