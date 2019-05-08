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

#include "ColorUtils.h"
#include "Highlight.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QDebug>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>

static constexpr char NONE_COLOR_ICON[] = "paint-none";
static constexpr char AUTO_COLOR_ICON[] = "colormanagement";

static QIcon createColorIcon(const QColor& color, const QSize& iconSize) {
    QPixmap pix(iconSize);
    pix.fill(Qt::transparent);
    {
        QPainter painter(&pix);
        painter.setRenderHints(QPainter::Antialiasing);
        QRectF rect = QRectF(pix.rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        painter.setBrush(color);
        painter.setPen(color.darker());
        painter.drawRoundedRect(rect, 5, 5);
    }
    QIcon icon;
    icon.addPixmap(pix);
    return icon;
}

static QIcon createIcon(const OptionalColor& color, const QSize& iconSize) {
    if (!color.has_value()) {
        return QIcon::fromTheme(NONE_COLOR_ICON);
    }
    if (color->isAuto()) {
        return QIcon::fromTheme(AUTO_COLOR_ICON);
    }
    QColor col = color.value().toColor({});
    return createColorIcon(col, iconSize);
}

//# ColorMenuWidget
ColorMenuWidget::ColorMenuWidget() : mLayout(new QVBoxLayout(this)) {
    mLayout->setSpacing(0);
    mLayout->setMargin(0);
}

QHBoxLayout* ColorMenuWidget::addRow() {
    auto* layout = new QHBoxLayout;
    mLayout->addLayout(layout);
    layout->setMargin(0);
    layout->setMargin(0);
    return layout;
}

//# ColorItem
ColorItem::ColorItem(const QString& text, const OptionalColor& color) : mColor(color) {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setAutoRaise(true);
    setCheckable(true);
    if (!text.isEmpty()) {
        setText(text);
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
}

//# SimpleColorItem
SimpleColorItem::SimpleColorItem(const OptionalColor& color) : SimpleColorItem({}, color) {
}

SimpleColorItem::SimpleColorItem(const QString& text, const OptionalColor& color)
        : ColorItem(text, color) {
    setIcon(createIcon(color, iconSize()));
}

//# CustomColorItem
CustomColorItem::CustomColorItem() : ColorItem(tr("Custom..."), {}) {
}

void CustomColorItem::setColor(const OptionalColor& color) {
    mColor = color;
    setIcon(createIcon(color, iconSize()));
}

void CustomColorItem::resetIcon() {
    setIcon({});
}

//# ColorWidget
ColorWidget::ColorWidget(QWidget* parent)
        : QPushButton(parent)
        , mButtonGroup(std::make_unique<QButtonGroup>())
        , mNoneItem(std::make_unique<SimpleColorItem>(tr("None"), OptionalColor()))
        , mAutoItem(std::make_unique<SimpleColorItem>(tr("Auto"), Color::createAuto()))
        , mCustomItem(std::make_unique<CustomColorItem>()) {
    setupMenu();
    setupButtonGroup();
    setColor({});
}

ColorWidget::~ColorWidget() {
}

void ColorWidget::setupMenu() {
    static std::optional<Palette> paletteOrNone = ColorUtils::loadGimpPalette(":/palette.gpl");

    auto* widget = new ColorMenuWidget;

    auto createMenu = [this, widget] {
        auto* menu = new QMenu(this);
        connect(menu, &QMenu::aboutToShow, this, &ColorWidget::onAboutToShowMenu);

        auto* action = new QWidgetAction(this);
        action->setDefaultWidget(widget);
        menu->addAction(action);
        setMenu(menu);
    };

    auto createPredefinedItems = [this, widget](const Palette& palette) {
        int column = 0;
        QHBoxLayout* row = nullptr;
        for (const auto& color : palette.colors) {
            if (column == 0) {
                row = widget->addRow();
            }
            auto item = std::make_unique<SimpleColorItem>(color);
            row->addWidget(item.get());
            mPredefinedColorItems.push_back(std::move(item));
            column = (column + 1) % std::min(palette.columns, 8);
        }
    };

    createMenu();

    auto* row = widget->addRow();
    row->addWidget(mNoneItem.get());
    row->addWidget(mAutoItem.get());
    if (paletteOrNone.has_value()) {
        createPredefinedItems(paletteOrNone.value());
    }
    row = widget->addRow();
    row->addWidget(mCustomItem.get());
    widget->adjustSize();
}

void ColorWidget::setupButtonGroup() {
    mButtonGroup->setExclusive(true);
    mButtonGroup->addButton(mNoneItem.get());
    mButtonGroup->addButton(mAutoItem.get());
    for (const auto& item : mPredefinedColorItems) {
        mButtonGroup->addButton(item.get());
    }
    mButtonGroup->addButton(mCustomItem.get());

    connect(mButtonGroup.get(),
            qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked),
            this,
            &ColorWidget::onActivated);
}

void ColorWidget::setColor(const OptionalColor& color) {
    mColor = color;
    setIcon(createIcon(mColor, iconSize()));
    colorChanged(mColor);
}

void ColorWidget::onAboutToShowMenu() {
    mCustomItem->resetIcon();
    if (!mColor.has_value()) {
        setCurrentItem(mNoneItem.get());
        return;
    }
    if (mColor.value().isAuto()) {
        setCurrentItem(mAutoItem.get());
        return;
    }
    auto it = std::find_if(mPredefinedColorItems.begin(),
                           mPredefinedColorItems.end(),
                           [this](const auto& item) { return item->color() == mColor; });
    if (it != mPredefinedColorItems.end()) {
        setCurrentItem(it->get());
        return;
    }
    mCustomItem->setColor(mColor);
    setCurrentItem(mCustomItem.get());
}

void ColorWidget::onActivated(QAbstractButton* button) {
    auto* item = static_cast<ColorItem*>(button);
    OptionalColor color;
    if (item == mCustomItem.get()) {
        QColorDialog dialog(this);
        if (mColor.has_value()) {
            auto color = mColor.value();
            if (!color.isAuto()) {
                dialog.setCurrentColor(color.toColor({}));
            }
        }
        if (!dialog.exec()) {
            return;
        }
        color = Color(dialog.currentColor());
    } else {
        color = item->color();
    }
    setColor(color);
    menu()->hide();
}

void ColorWidget::setCurrentItem(ColorItem* item) {
    item->setChecked(true);
}

OptionalColor ColorWidget::color() const {
    return mColor;
}
