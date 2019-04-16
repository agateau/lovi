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
#include "ColorWidget.h"

#include "Highlight.h"

#include <QColorDialog>

enum { NONE_IDX = 0, AUTO_IDX, CUSTOM_IDX };

ColorWidget::ColorWidget(QWidget* parent) : QComboBox(parent) {
    setEditable(false);
    addItem(tr("None"));
    addItem(tr("Auto"));
    addItem(tr("Custom..."));

    connect(this, qOverload<int>(&QComboBox::activated), this, &ColorWidget::onActivated);
}

void ColorWidget::setColor(const OptionalColor& color) {
    mColor = color;
    if (mColor.has_value()) {
        setCurrentIndex(mColor.value().isAuto() ? AUTO_IDX : CUSTOM_IDX);
    } else {
        setCurrentIndex(NONE_IDX);
    }
}

void ColorWidget::onActivated(int index) {
    if (index == NONE_IDX) {
        mColor = {};
        colorChanged(mColor);
        return;
    }

    if (index == AUTO_IDX) {
        mColor = Color::createAuto();
        colorChanged(mColor);
        return;
    }

    QColorDialog dialog(this);
    if (mColor.has_value()) {
        auto color = mColor.value();
        if (!color.isAuto()) {
            dialog.setCurrentColor(color.toColor({}));
        }
    }
    if (dialog.exec()) {
        mColor = Color(dialog.currentColor());
        colorChanged(mColor);
    }
}

OptionalColor ColorWidget::color() const {
    return mColor;
}
