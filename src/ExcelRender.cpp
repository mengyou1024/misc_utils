#include "ExcelRender.hpp"

#if __has_include("xlsxdocument.h") && __has_include("QtCore")

    #include <QDebug>
    #include <QLoggingCategory>
    #include <QRegularExpression>
    #include <QVariant>

static Q_LOGGING_CATEGORY(TAG, "Render.Excel");

namespace Yo::File::Render::Excel {
    bool Render(const QString &_template, const QString &output, const QVariantMap &data) {
        QXlsx::Document doc(_template);
        if (!doc.load()) {
            qCWarning(TAG) << "open template:" << _template << "error!";
            return false;
        }
        int cols = doc.dimension().columnCount();
        int rows = doc.dimension().rowCount();
        for (int r = 1; r <= rows; r++) {
            for (int c = 1; c <= cols; c++) {
                static QRegularExpression reg(R"(\$\{(.+?)\})");
                auto                      val          = doc.read(r, c);
                auto                      iterator     = reg.globalMatch(val.toString());
                QString                   relocatedStr = val.toString();
                while (iterator.hasNext()) {
                    auto match = iterator.next();
                    if (!match.captured(1).isEmpty()) {
                        auto rawStr  = match.captured(1);
                        relocatedStr = relocatedStr.replace(QString("${%1}").arg(rawStr), data.value(match.captured(1)).toString());
                        qCDebug(TAG) << match.captured(1) << "---->" << data.value(match.captured(1));
                    }
                }
                if (relocatedStr != val.toString()) {
                    doc.write(r, c, relocatedStr);
                }
            }
        }
        if (!doc.saveAs(output)) {
            qCWarning(TAG).noquote() << "save file:" << output << "error!";
            return false;
        }
        return true;
    }
} // namespace Yo::File::Render::Excel
#endif
