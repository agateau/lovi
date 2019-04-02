#include "mainwindow.h"

#include "filelineprovider.h"
#include "logformat.h"
#include "logformatdialog.h"
#include "logformatloader.h"
#include "logmodel.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>())
    , mOpenLogAction(new QAction(this))
    , mOpenLogFormatAction(new QAction(this))
    , mAutoScrollAction(new QAction(this))
    , mToolBar(addToolBar(tr("Toolbar")))
    , mTreeView(new QTreeView(this)) {
    setupUi();
    setupActions();
}

MainWindow::~MainWindow() {
}

void MainWindow::loadLogFormat(const QString& filePath) {
    mLogFormatPath = filePath;
    mLogFormatLoader->load(filePath);
}

void MainWindow::loadLog(const QString &filePath) {
    mLogPath = filePath;

    auto fileLineProvider = std::make_unique<FileLineProvider>();
    fileLineProvider->setFilePath(filePath);
    mLineProvider = std::move(fileLineProvider);

    mLogModel = std::make_unique<LogModel>(mLineProvider.get());
    mLogModel->setLogFormat(mLogFormatLoader->logFormat());
    connect(mLogModel.get(), &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);

    connect(mLogFormatLoader.get(), &LogFormatLoader::logFormatChanged, mLogModel.get(), &LogModel::setLogFormat);

    mTreeView->setModel(mLogModel.get());
}

void MainWindow::setupUi() {
    mTreeView->setRootIsDecorated(false);
    mTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    mTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    setCentralWidget(mTreeView);

    QAction* copyAction = new QAction(this);
    copyAction->setText(tr("Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MainWindow::copySelectedLines);
    mTreeView->addAction(copyAction);

    resize(800, 600);
}

void MainWindow::setupActions() {
    mOpenLogAction->setText(tr("Open log"));
    mOpenLogAction->setIcon(QIcon::fromTheme("document-open"));
    mOpenLogAction->setShortcut(QKeySequence::Open);
    connect(mOpenLogAction, &QAction::triggered, this, &MainWindow::showOpenLogDialog);

    mOpenLogFormatAction->setText(tr("Open log format"));
    mOpenLogFormatAction->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenLogFormatAction, &QAction::triggered, this, &MainWindow::showOpenLogFormatDialog);

    mAutoScrollAction->setText(tr("Auto Scroll"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mOpenLogAction);
    mToolBar->addAction(mOpenLogFormatAction);
    mToolBar->addAction(mAutoScrollAction);
    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}

void MainWindow::showOpenLogDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilters({tr("Log files (*.log *.log.* *.txt)"),
                           tr("All files (*)")});
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

void MainWindow::showOpenLogFormatDialog() {
    LogFormatDialog dialog(mLogFormatPath, this);
    if (!dialog.exec()) {
        return;
    }
    loadLogFormat(dialog.logFormatPath());
}

void MainWindow::copySelectedLines() {
    QStringList list;
    auto selectedRows = mTreeView->selectionModel()->selectedRows();
    if (selectedRows.empty()) {
        int row = mTreeView->currentIndex().row();
        list << mLineProvider->lineAt(row);
    } else {
        for (const auto& index : selectedRows) {
            list << mLineProvider->lineAt(index.row());
        }
    }
    qApp->clipboard()->setText(list.join("\n"));
}
