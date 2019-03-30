#include "mainwindow.h"

#include "logformat.h"
#include "filewatcher.h"
#include "logmodel.h"

#include <QAction>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QToolBar>
#include <QTreeView>

using std::optional;
using std::unique_ptr;

static optional<QByteArray> readFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << filePath << file.errorString();
        return {};
    }
    return file.readAll();
}

static unique_ptr<LogFormat> loadLogFormat(const QString& filePath) {
    optional<QByteArray> json = readFile(filePath);
    if (!json.has_value()) {
        return {};
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << filePath << ":" << error.errorString();
        return {};
    }

    return LogFormat::fromJsonDocument(doc);
}

MainWindow::MainWindow(LogModel* logModel, QWidget* parent)
    : QMainWindow(parent)
    , mLogFormatWatcher(new FileWatcher(this))
    , mModel(logModel) {

    createUi();
    createActions();

    connect(mModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);
    connect(mLogFormatWatcher, &FileWatcher::fileChanged, this, &MainWindow::reloadLogFormat);
}

MainWindow::~MainWindow() {

}

void MainWindow::loadLogFormat(const QString& filePath) {
    unique_ptr<LogFormat> logFormat = ::loadLogFormat(filePath);
    if (!logFormat) {
        return;
    }
    mLogFormatWatcher->setFilePath(filePath);
    mModel->setLogFormat(logFormat.get());
    mLogFormat = std::move(logFormat);
}

void MainWindow::createUi() {
    mToolBar = addToolBar(tr("Toolbar"));

    mTreeView = new QTreeView();
    mTreeView->setModel(mModel);
    mTreeView->setRootIsDecorated(false);
    setCentralWidget(mTreeView);
}

void MainWindow::createActions() {
    mAutoScrollAction = new QAction("Auto Scroll");
    mAutoScrollAction->setCheckable(true);
    connect(mAutoScrollAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled) {
            mTreeView->scrollToBottom();
        }
    });

    mToolBar->addAction(mAutoScrollAction);
}

void MainWindow::onRowsInserted() {
    if (mAutoScrollAction->isChecked()) {
        mTreeView->scrollToBottom();
    }
}

void MainWindow::reloadLogFormat() {
    qInfo() << "Reloading log format";
    loadLogFormat(mLogFormatWatcher->filePath());
}
