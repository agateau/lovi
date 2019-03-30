#include "mainwindow.h"

#include "config.h"
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

static unique_ptr<Config> loadConfig(const QString& filePath) {
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

    return Config::fromJsonDocument(doc);
}

MainWindow::MainWindow(LogModel* logModel, QWidget* parent)
    : QMainWindow(parent)
    , mConfigWatcher(new FileWatcher(this))
    , mModel(logModel) {

    createUi();
    createActions();

    connect(mModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);
    connect(mConfigWatcher, &FileWatcher::fileChanged, this, &MainWindow::reloadConfig);
}

MainWindow::~MainWindow() {

}

void MainWindow::loadConfig(const QString& filePath) {
    unique_ptr<Config> config = ::loadConfig(filePath);
    if (!config) {
        return;
    }
    mConfigWatcher->setFilePath(filePath);
    mModel->setConfig(config.get());
    mConfig = std::move(config);
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

void MainWindow::reloadConfig() {
    qInfo() << "Reloading config";
    loadConfig(mConfigWatcher->filePath());
}
