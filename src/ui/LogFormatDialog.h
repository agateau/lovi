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
#ifndef LOGFORMATDIALOG_H
#define LOGFORMATDIALOG_H

#include <QDialog>

#include <memory>

namespace Ui {
class LogFormatDialog;
}

class HighlightModel;
class LogFormat;
class LogFormatModel;
class LogFormatStore;

class LogFormatDialog : public QDialog {
    Q_OBJECT
public:
    explicit LogFormatDialog(LogFormatStore* store,
                             LogFormat* currentLogFormat,
                             QWidget* parent = nullptr);
    ~LogFormatDialog();

signals:
    void logFormatChanged(LogFormat* logFormat);

private:
    void setupSideBar(LogFormat* currentLogFormat);
    void setupEditor();

    void onCurrentChanged(int row);
    void onCurrentHighlightChanged(const QModelIndex& index);
    void applyChanges();
    void onAddFormatClicked();
    void selectLogFormat(const QString& name);

    const std::unique_ptr<Ui::LogFormatDialog> ui;
    const std::unique_ptr<LogFormatModel> mModel;
    const std::unique_ptr<HighlightModel> mHighlightModel;
    LogFormatStore* const mLogFormatStore;
};

#endif // LOGFORMATDIALOG_H
