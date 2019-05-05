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
#include "ItemDelegate.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QPainter>

static constexpr qreal NOT_SELECTED_AND_OVER_ALPHA = 0.1;
static constexpr qreal SELECTED_AND_OVER_ALPHA = 0.3;
static constexpr qreal SELECTED_NOT_OVER_ALPHA = 0.2;

ItemDelegate::ItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {
}

void ItemDelegate::paint(QPainter* painter,
                         const QStyleOptionViewItem& option_,
                         const QModelIndex& index) const {
    auto option = option_;

    auto takeFlag = [](QStyle::State* state, QStyle::StateFlag flag) {
        if (state->testFlag(flag)) {
            state->setFlag(flag, false);
            return true;
        }
        return false;
    };

    auto isSelected = [&option](const QModelIndex& originIndex, int delta) -> bool {
        auto index = originIndex.sibling(originIndex.row() + delta, 0);
        if (!index.isValid()) {
            return false;
        }
        auto* itemView = static_cast<QAbstractItemView*>(option.styleObject);
        return itemView->selectionModel()->isSelected(index);
    };

    takeFlag(&option.state, QStyle::State_HasFocus);
    bool selected = takeFlag(&option.state, QStyle::State_Selected);
    bool mouseOver = takeFlag(&option.state, QStyle::State_MouseOver);
    QStyledItemDelegate::paint(painter, option, index);
    if (selected) {
        QRectF rect = option.rect;
        QColor color = option.palette.color(QPalette::ColorRole::Highlight);
        painter->setPen(color);
        bool atSelectionTop = !isSelected(index, -1);
        bool atSelectionBottom = !isSelected(index, 1);
        if (atSelectionTop) {
            painter->drawLine(rect.topLeft(), rect.topRight());
        }
        if (atSelectionBottom) {
            rect.setBottom(rect.bottom() - 1);
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        }
        color.setAlphaF(mouseOver ? SELECTED_AND_OVER_ALPHA : SELECTED_NOT_OVER_ALPHA);
        painter->fillRect(rect, color);
    } else if (mouseOver) {
        // mouse over but not selected
        QColor color = option.palette.color(QPalette::ColorRole::Text);
        color.setAlphaF(NOT_SELECTED_AND_OVER_ALPHA);
        painter->fillRect(option.rect, color);
    }
}
