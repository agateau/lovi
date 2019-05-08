#include "ColorUtils.h"

#include <KColorUtils>

#include <QFile>

#include <iostream>

using std::cerr;
using std::optional;

static constexpr char HEADER[] = R"(
<html>
<head>
<style>
tr.worst td {
    border: 1px solid red;
}
td { padding: 0.5em }
</style>
</head>
<body>
<table>
<tr>
    <th>Luma</th>
    <th>Generated BG</th>
    <th>Generated FG</th>
    <th>Contrast</th>
)";
static constexpr char COLOR_LINE[] = R"(
<tr class="%4">
    <td>%5</td>
    <td style='color: %1; background-color: %2'>%1 on %2</td>
    <td style='color: %2; background-color: %1'>%2 on %1</td>
    <td style='text-align: right'>%3</td>
)";
static constexpr char FOOTER[] = R"(
</table>
</body>
</html>
)";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " <palette.gpl>\n";
        return 1;
    }
    optional<Palette> maybePalette = ColorUtils::loadGimpPalette(argv[1]);
    if (!maybePalette.has_value()) {
        cerr << "Failed to load palette\n";
        return 2;
    }
    auto palette = maybePalette.value();

    QFile file("palette.html");
    if (!file.open(QIODevice::WriteOnly)) {
        cerr << "Failed to open " << file.fileName().toStdString() << '\n';
        return 3;
    }
    qreal worstContrast = 1000;
    int worstRow = -1;
    int row = 0;
    for (const QColor& color1 : palette.colors) {
        QColor color2 = ColorUtils::getContrastedColor(color1);
        qreal contrast = KColorUtils::contrastRatio(color1, color2);
        if (contrast < worstContrast) {
            worstContrast = std::min(contrast, worstContrast);
            worstRow = row;
        }
        ++row;
    }
    file.write(HEADER);
    row = 0;
    for (const QColor& color1 : palette.colors) {
        QColor color2 = ColorUtils::getContrastedColor(color1);
        qreal contrast = KColorUtils::contrastRatio(color1, color2);
        qreal luma = KColorUtils::luma(color1);
        auto line = QString(COLOR_LINE)
                        .arg(color1.name())
                        .arg(color2.name())
                        .arg(QString::number(contrast, 'f', 1))
                        .arg(row == worstRow ? "worst" : "")
                        .arg(QString::number(luma, 'f', 2));
        file.write(line.toUtf8());
        ++row;
    }
    file.write(FOOTER);
}
