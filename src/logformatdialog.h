#ifndef LOGFORMATDIALOG_H
#define LOGFORMATDIALOG_H

#include <QDialog>

#include <memory>

class QFileSystemModel;

namespace Ui {
class LogFormatDialog;
}

class LogFormatDialog : public QDialog {
    Q_OBJECT
public:
    explicit LogFormatDialog(const QString& logFormatPath, QWidget* parent = nullptr);
    ~LogFormatDialog();

    QString logFormatPath() const;

private:
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    const std::unique_ptr<Ui::LogFormatDialog> ui;
    const std::unique_ptr<QFileSystemModel> mModel;
    const QString mDirPath;

    QString mInitialLogFormatPath;
};

#endif // LOGFORMATDIALOG_H
