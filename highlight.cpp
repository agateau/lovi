#include "highlight.h"

HighlightColor::HighlightColor(const QString& text) {
    if (text == "auto") {
        mIsAuto = true;
        return;
    }
    mColor = text;
}

static std::vector<QColor> generateAutoColorVector() {
    std::vector<QColor> colors;
    for (qreal hue = 0; hue < 1; hue += 1.0 / 20) {
        colors.push_back(QColor::fromHsvF(hue, 0.5, 0.9));
    }
    return colors;
}

QColor HighlightColor::toColor(const QString& matchingText) const {
    if (!mIsAuto) {
        return mColor;
    }
    static std::vector<QColor> autoColors = generateAutoColorVector();
    int sum = std::accumulate(
        matchingText.begin(), matchingText.end(), 0, [](int currentSum, const QChar& ch) {
            return currentSum + ch.toLatin1();
        });
    return autoColors.at(sum % autoColors.size());
}
