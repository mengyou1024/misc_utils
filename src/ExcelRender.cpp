#include "ExcelRender.hpp"

#if __has_include("xlsxdocument.h") && __has_include("QtCore")

    #include <QLoggingCategory>
    #include <QRegularExpression>
    #include <QVariant>

static Q_LOGGING_CATEGORY(TAG, "Render.Excel");

namespace Yo::File::Render::Excel {
    bool Render(const QString &_template, const QString &output, const QVariantMap &data) {
        QXlsx::Document doc(_template);
        if (!doc.load()) {
            qWarning(TAG) << "open template:" << _template << "error!";
            return false;
        }
        int cols = doc.dimension().columnCount();
        int rows = doc.dimension().rowCount();
        for (int r = 1; r <= rows; r++) {
            for (int c = 1; c <= cols; c++) {
                static QRegularExpression reg(R"(\$\{(.+)\})");
                auto                      val = doc.read(r, c);
                if (val.isValid()) {
                    auto res = reg.match(val.toString());
                    if (res.hasCaptured(1)) {
                        auto rawStr      = res.captured(1);
                        auto reolacedStr = val.toString().replace(QString("${%1}").arg(rawStr), data.value(res.captured(1)).toString());
                        doc.write(r, c, reolacedStr);
                        qDebug(TAG) << res.captured(1) << "---->" << data.value(res.captured(1));
                    }
                }
            }
        }
        if (!doc.saveAs(output)) {
            qDebug(TAG) << "save file:" << output << "error!";
            return false;
        }
        return true;
    }
} // namespace Yo::File::Render::Excel
#endif
