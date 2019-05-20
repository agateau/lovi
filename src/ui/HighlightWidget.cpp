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
#include "HighlightWidget.h"

#include "ConditionLineEditChecker.h"
#include "LogFormat.h"
#include "WidgetUtils.h"
#include "ui_HighlightWidget.h"

using std::unique_ptr;

static constexpr char HIGHLIGHT_SYNTAX_HELP[] = QT_TRANSLATE_NOOP(
    "HighlightWidget",
    "<qt>The syntax for an highlight condition is:\n"
    "\n"
    "<pre>[COLUMN] [OPERATOR] [CRITERIA]</pre>\n"
    "\n"
    "<p><b>COLUMN</b>: The name of a column defined in the log parser.</p>"
    "<p><b>OPERATOR</b>: One of:<ul>"
    "  <li>= or ==: exact match</li>"
    "  <li>contains: the cell must contain the \"criteria\" string</li>"
    "  <li>~: the cell must match the \"criteria\" regular expression</li>"
    "</ul></p>"
    "<p><b>CRITERIA</b>: The string to match. If it contains spaces, surround it with "
    "<b>\"</b>. To use a literal <b>\"</b>, escape it by prefixing it with another <b>\"</b>.</p>"
    "</qt>");

// Helper function to init `ui` and create all its widgets so that mLineEditChecker can be called
// with a valid QLineEdit instance
static unique_ptr<Ui::HighlightWidget> initUi(QWidget* parent) {
    auto ui = std::make_unique<Ui::HighlightWidget>();
    ui->setupUi(parent);
    return ui;
}

HighlightWidget::HighlightWidget(QWidget* parent)
        : QWidget(parent)
        , ui(initUi(this))
        , mLineEditChecker(std::make_unique<ConditionLineEditChecker>(ui->conditionLineEdit)) {
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setupUi();
}

HighlightWidget::~HighlightWidget() {
}

void HighlightWidget::setHighlight(Highlight* highlight) {
    if (mHighlight == highlight) {
        return;
    }
    mHighlight = highlight;
    if (!mHighlight) {
        setEnabled(false);
        ui->conditionLineEdit->clear();
        return;
    }
    setEnabled(true);

    mLineEditChecker->setLogFormat(highlight->logFormat());
    ui->conditionLineEdit->setText(highlight->conditionDefinition());
    ui->bgColorWidget->setColor(highlight->bgColor());
    ui->fgColorWidget->setColor(highlight->fgColor());
    ui->scopeComboBox->setCurrentIndex(static_cast<int>(highlight->scope()));
}

Highlight* HighlightWidget::highlight() const {
    return mHighlight;
}

QLineEdit* HighlightWidget::lineEdit() const {
    return ui->conditionLineEdit;
}

static void setLabelIcon(QLabel* label, const QString& name) {
    QIcon icon = QIcon::fromTheme(name);
    label->setPixmap(icon.pixmap(32));
    label->setFixedSize(label->pixmap()->size());
}

void HighlightWidget::setupUi() {
    setEnabled(false);
    layout()->setMargin(0);

    setLabelIcon(ui->bgColorLabel, "color-fill");
    setLabelIcon(ui->fgColorLabel, "format-text-color");

    connect(ui->conditionLineEdit, &QLineEdit::editingFinished, this, [this] {
        mHighlight->setConditionDefinition(ui->conditionLineEdit->text());
    });

    WidgetUtils::addLineEditHelpIcon(ui->conditionLineEdit, tr(HIGHLIGHT_SYNTAX_HELP));

    connect(ui->bgColorWidget,
            &ColorWidget::colorChanged,
            this,
            [this](const OptionalColor& color) { mHighlight->setBgColor(color); });

    connect(ui->fgColorWidget,
            &ColorWidget::colorChanged,
            this,
            [this](const OptionalColor& color) { mHighlight->setFgColor(color); });

    connect(ui->scopeComboBox, qOverload<int>(&QComboBox::activated), this, [this](int index) {
        auto scope = static_cast<Highlight::Scope>(index);
        mHighlight->setScope(scope);
    });
}
