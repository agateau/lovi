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
#include <QTimer>
#include <QToolBar>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatLoader(std::make_unique<LogFormatLoader>())
    , mOpenLogAction(new QAction(this))
    , mSelectLogFormatAction(new QAction(this))
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
    setWindowTitle(QString("%1 - Lovi").arg(mLogPath));

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
    setWindowTitle("Lovi");

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
    mOpenLogAction->setText(tr("Open"));
    mOpenLogAction->setToolTip(tr("Open log file"));
    mOpenLogAction->setIcon(QIcon::fromTheme("document-open"));
    mOpenLogAction->setShortcut(QKeySequence::Open);
    connect(mOpenLogAction, &QAction::triggered, this, &MainWindow::showOpenLogDialog);

    mSelectLogFormatAction->setText(tr("Format"));
    mSelectLogFormatAction->setToolTip(tr("Select log format"));
    mSelectLogFormatAction->setIcon(QIcon::fromTheme("object-columns"));
    connect(mSelectLogFormatAction, &QAction::triggered, this, &MainWindow::showLogFormatDialog);

    mAutoScrollAction->setText(tr("Auto scroll"));
    mAutoScrollAction->setToolTip(tr("Automatically scroll down when new lines are logged"));
    mAutoScrollAction->setIcon(QIcon::fromTheme("go-bottom"));
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mOpenLogAction);
    mToolBar->addAction(mSelectLogFormatAction);
    mToolBar->addAction(mAutoScrollAction);
    mToolBar->setMovable(false);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        // Delay the call a bit to ensure the view has created the rows
        QTimer::singleShot(0, this, [this] {
            mTreeView->scrollToBottom();
        });
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

void MainWindow::showLogFormatDialog() {
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
