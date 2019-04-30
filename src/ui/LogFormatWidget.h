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
#ifndef LOGFORMATWIDGET_H
#define LOGFORMATWIDGET_H

#include <QWidget>

#include <memory>

namespace Ui {
class LogFormatWidget;
}

class HighlightModel;
class LogFormat;
class LogFormatModel;
class MainController;
class SearchResponse;

class LogFormatWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogFormatWidget(MainController* controller, QWidget* parent = nullptr);
    ~LogFormatWidget();

private:
    void setupLogFormatSelector();
    void setupEditor();
    void setupSearchBar();

    void setLogFormat(LogFormat* logFormat);
    void onCurrentChanged(int row);
    void onCurrentHighlightChanged(const QModelIndex& index);
    void applyChanges();
    void onAddFormatClicked();
    void selectLogFormat(const QString& name);
    void onSearchFinished(const SearchResponse& response);

    MainController* const mController;
    const std::unique_ptr<Ui::LogFormatWidget> ui;
    const std::unique_ptr<LogFormatModel> mModel;
    const std::unique_ptr<HighlightModel> mHighlightModel;
};

#endif // LOGFORMATWIDGET_H
