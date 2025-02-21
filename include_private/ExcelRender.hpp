#pragma once

#if __has_include("xlsxdocument.h") && __has_include("QtCore")
    #include <QtCore>
    #include <xlsxdocument.h>

namespace Yo::File::Render::Excel {
    bool Render(const QString &_template, const QString &output, const QVariantMap &data, const QMap<QString, QImage> &images);
} // namespace Yo::File::Render::Excel
#endif
