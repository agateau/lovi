#include "logformatdialog.h"
#include "ui_logformatdialog.h"

#include <QFileSystemModel>
#include <QStandardPaths>

static QString logFormatDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

LogFormatDialog::LogFormatDialog(const QString& logFormatPath, QWidget* parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::LogFormatDialog>())
    , mModel(std::make_unique<QFileSystemModel>())
    , mDirPath(logFormatDirPath())
    , mInitialLogFormatPath(logFormatPath) {
    ui->setupUi(this);

    mModel->setFilter(QDir::Files);
    mModel->sort(0);
    connect(mModel.get(), &QAbstractItemModel::rowsInserted, this, &LogFormatDialog::onRowsInserted);
    ui->listView->setModel(mModel.get());
    ui->listView->setRootIndex(mModel->setRootPath(mDirPath));
}

LogFormatDialog::~LogFormatDialog() {
}

QString LogFormatDialog::logFormatPath() const {
    auto index = ui->listView->currentIndex();
    if (!index.isValid()) {
        return {};
    }
    return index.data(QFileSystemModel::FilePathRole).toString();
}

void LogFormatDialog::onRowsInserted(const QModelIndex& parent, int first, int last) {
    if (mInitialLogFormatPath.isEmpty()) {
        return;
    }
    for (int row = first; row <= last; ++row) {
        auto index = mModel->index(row, 0, parent);
        if (index.data(QFileSystemModel::FilePathRole).toString() == mInitialLogFormatPath) {
            ui->listView->setCurrentIndex(index);
            mInitialLogFormatPath.clear();
            return;
        }
    }
}
