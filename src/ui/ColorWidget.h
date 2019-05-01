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
#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include "Highlight.h"

#include "Color.h"
#include "Vector.h"

#include <QPushButton>
#include <QToolButton>

#include <memory>

class QButtonGroup;
class QHBoxLayout;
class QVBoxLayout;

class ColorMenuWidget : public QWidget {
public:
    ColorMenuWidget();
    QHBoxLayout* addRow();

private:
    QVBoxLayout* const mLayout;
};

class ColorItem : public QToolButton {
public:
    ColorItem(const QString& text, const OptionalColor& color);
    OptionalColor color() const {
        return mColor;
    }

protected:
    OptionalColor mColor;
};

class SimpleColorItem : public ColorItem {
public:
    explicit SimpleColorItem(const OptionalColor& color);
    explicit SimpleColorItem(const QString& text, const OptionalColor& color);
};

class CustomColorItem : public ColorItem {
public:
    CustomColorItem();
    void setColor(const OptionalColor& color);
};

class ColorWidget : public QPushButton {
    Q_OBJECT
public:
    ColorWidget(QWidget* parent = nullptr);
    ~ColorWidget();

    void setColor(const OptionalColor& color);
    OptionalColor color() const;

signals:
    void colorChanged(const OptionalColor& color);

private:
    void setupMenu();
    void setupButtonGroup();
    void onActivated(QAbstractButton* button);
    void onAboutToShowMenu();
    void setCurrentItem(ColorItem* item);

    OptionalColor mColor;

    std::unique_ptr<QButtonGroup> mButtonGroup;
    std::unique_ptr<ColorItem> mNoneItem;
    std::unique_ptr<ColorItem> mAutoItem;
    std::unique_ptr<CustomColorItem> mCustomItem;
    stdq::Vector<std::unique_ptr<ColorItem>> mPredefinedColorItems;
};

#endif // COLORWIDGET_H
