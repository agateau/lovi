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
#include "mainwindow.h"

#include "config.h"
#include "filelineprovider.h"
#include "logformat.h"
#include "logformatdialog.h"
#include "logformatstore.h"
#include "logmodel.h"
#include "stdinlineprovider.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>

static const int MAX_RECENT_FILES = 10;

MainWindow::MainWindow(Config* config, LogFormatStore* store, QWidget* parent)
        : QMainWindow(parent)
        , mConfig(config)
        , mLogFormatStore(store)
        , mOpenLogAction(new QAction(this))
        , mSelectLogFormatAction(new QAction(this))
        , mAutoScrollAction(new QAction(this))
        , mCopyLinesAction(new QAction(this))
        , mRecentFilesMenu(new QMenu(this))
        , mToolBar(addToolBar(tr("Toolbar")))
        , mTreeView(new QTreeView(this)) {
    setupActions();
    setupUi();
}

MainWindow::~MainWindow() {
}

void MainWindow::loadLogFormat(const QString& logFormatName) {
    auto logFormat = mLogFormatStore->byName(logFormatName);
    if (!logFormat) {
        qWarning() << "No log format named" << logFormatName;
        return;
    }
    setLogFormat(logFormat);
}

void MainWindow::loadLog(const QString& filePath) {
    bool isStdin = filePath == "-";

    mLogPath = filePath;
    QString titlePath = isStdin ? "<stdin>" : mLogPath;
    setWindowTitle(QString("%1 - Lovi").arg(titlePath));
    if (!isStdin) {
        addLogToRecentFiles();
    }

    createLineProvider();

    mLogModel = std::make_unique<LogModel>(mLineProvider.get());

    if (!isStdin) {
        QString logFormatName = mConfig->logFormatForFile().value(mLogPath);
        if (!logFormatName.isEmpty()) {
            loadLogFormat(logFormatName);
        }
    }

    connect(mLogModel.get(), &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);

    mTreeView->setModel(mLogModel.get());

    // Must be done here because the selectionModel is (re)set by setModel()
    connect(mTreeView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::onSelectionChanged);
    onSelectionChanged();
}

void MainWindow::setupUi() {
    setWindowTitle("Lovi");

    mTreeView->setRootIsDecorated(false);
    mTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    mTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mTreeView->addAction(mCopyLinesAction);
    setCentralWidget(mTreeView);

    mToolBar->addAction(mOpenLogAction);
    mToolBar->addAction(mSelectLogFormatAction);
    mToolBar->addAction(mAutoScrollAction);

    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    resize(800, 600);
}

static void appendShortcutToToolTip(QAction* action) {
    auto shortcut = action->shortcut().toString(QKeySequence::NativeText);
    action->setToolTip(QString("%1 (%2)").arg(action->toolTip(), shortcut));
}

void MainWindow::setupActions() {
    mOpenLogAction->setText(tr("Open"));
    mOpenLogAction->setToolTip(tr("Open log file"));
    mOpenLogAction->setIcon(QIcon::fromTheme("document-open"));
    mOpenLogAction->setShortcut(QKeySequence::Open);
    mOpenLogAction->setMenu(mRecentFilesMenu);
    connect(mOpenLogAction, &QAction::triggered, this, &MainWindow::showOpenLogDialog);
    connect(mRecentFilesMenu, &QMenu::aboutToShow, this, &MainWindow::fillRecentFilesMenu);

    mSelectLogFormatAction->setText(tr("Format"));
    mSelectLogFormatAction->setToolTip(tr("Select log format"));
    mSelectLogFormatAction->setShortcut(Qt::SHIFT | Qt::Key_F);
    mSelectLogFormatAction->setIcon(QIcon::fromTheme("object-columns"));
    connect(mSelectLogFormatAction, &QAction::triggered, this, &MainWindow::showLogFormatDialog);

    mAutoScrollAction->setText(tr("Auto scroll"));
    mAutoScrollAction->setShortcut(Qt::SHIFT | Qt::Key_S);
    mAutoScrollAction->setToolTip(tr("Automatically scroll down when new lines are logged"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    for (auto action : {mOpenLogAction, mSelectLogFormatAction, mAutoScrollAction}) {
        appendShortcutToToolTip(action);
    }

    mCopyLinesAction->setText(tr("Copy"));
    mCopyLinesAction->setShortcut(QKeySequence::Copy);
    mCopyLinesAction->setEnabled(false);
    connect(mCopyLinesAction, &QAction::triggered, this, &MainWindow::copySelectedLines);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        // Delay the call a bit to ensure the view has created the rows
        QTimer::singleShot(0, this, [this] { mTreeView->scrollToBottom(); });
    }
}

void MainWindow::onSelectionChanged() {
    int lineCount = mTreeView->selectionModel()->selectedRows().count();
    mCopyLinesAction->setEnabled(lineCount > 0);
    mCopyLinesAction->setText(tr("Copy %n line(s)", "", lineCount));
}

void MainWindow::showOpenLogDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({tr("Log files (*.log *.log.* *.txt)"), tr("All files (*)")});
    dialog.setWindowTitle(tr("Open log file"));
    if (!mLogPath.isEmpty()) {
        QString logDir = QFileInfo(mLogPath).absolutePath();
        dialog.setDirectory(logDir);
    }
    if (!dialog.exec()) {
        return;
    }
    loadLog(dialog.selectedFiles().first());
}

void MainWindow::showLogFormatDialog() {
    if (mLogFormatDialog) {
        mLogFormatDialog->show();
        mLogFormatDialog->activateWindow();
        return;
    }
    mLogFormatDialog = new LogFormatDialog(mLogFormatStore, mLogFormat, this);
    connect(mLogFormatDialog.data(),
            &LogFormatDialog::logFormatChanged,
            this,
            &MainWindow::setLogFormat);
    mLogFormatDialog->show();
}

void MainWindow::copySelectedLines() {
    auto selectedRows = mTreeView->selectionModel()->selectedRows();
    if (selectedRows.empty()) {
        return;
    }
    QStringList list;
    for (const auto& index : selectedRows) {
        list << mLineProvider->lineAt(index.row());
    }
    qApp->clipboard()->setText(list.join("\n"));
}

void MainWindow::addLogToRecentFiles() {
    QStringList files = mConfig->recentLogFiles();
    files.removeOne(mLogPath);
    files.insert(0, mLogPath);
    while (files.length() > MAX_RECENT_FILES) {
        files.takeLast();
    }
    mConfig->setRecentLogFiles(files);
}

void MainWindow::fillRecentFilesMenu() {
    mRecentFilesMenu->clear();
    for (const auto& filePath : mConfig->recentLogFiles()) {
        mRecentFilesMenu->addAction(filePath, this, [this, filePath] { loadLog(filePath); });
    }
}

void MainWindow::createLineProvider() {
    if (mLogPath == "-") {
        mLineProvider = std::make_unique<StdinLineProvider>();
    } else {
        mLineProvider = std::make_unique<FileLineProvider>(mLogPath);
    }
}

void MainWindow::setLogFormat(LogFormat* logFormat) {
    mLogFormat = logFormat;
    mLogModel->setLogFormat(mLogFormat);
    if (!mLogPath.isEmpty() && !mLogFormat->name.isEmpty()) {
        mConfig->setLogFormatForFile(mLogPath, mLogFormat->name);
    }
}
