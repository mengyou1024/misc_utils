#include "ExcelRender.hpp"

#if __has_include("xlsxdocument.h") && __has_include("QtCore")

    #include <QDebug>
    #include <QJsonDocument>
    #include <QJsonObject>
    #include <QLoggingCategory>
    #include <QRegularExpression>
    #include <QVariant>

static Q_LOGGING_CATEGORY(TAG, "Render.Excel");

namespace Yo::File::Render::Excel {
    bool Render(const QString &_template, const QString &output, const QVariantMap &data, const QMap<QString, QImage> &images) {
        auto output_dir = QFileInfo(output).absoluteDir();
        if (!output_dir.exists()) {
            output_dir.mkpath(QFileInfo(output).absolutePath());
        }

        QXlsx::Document doc(_template);
        if (!doc.load()) {
            qCWarning(TAG) << "open template:" << _template << "error!";
            return false;
        }
        int cols = doc.dimension().columnCount();
        int rows = doc.dimension().rowCount();
        // 字符串替换
        for (int r = 1; r <= rows; r++) {
            for (int c = 1; c <= cols; c++) {
                static QRegularExpression reg(R"(\$\{(.+?)\})");
                auto                      val              = doc.read(r, c);
                auto                      iterator         = reg.globalMatch(val.toString());
                QString                   relocated_string = val.toString();
                while (iterator.hasNext()) {
                    auto match = iterator.next();
                    if (!match.captured(1).isEmpty()) {
                        auto raw_string  = match.captured(1);
                        relocated_string = relocated_string.replace(QString("${%1}").arg(raw_string), data.value(match.captured(1)).toString());
                        qCDebug(TAG) << match.captured(1) << "---->" << data.value(match.captured(1));
                    }
                }
                if (relocated_string != val.toString()) {
                    doc.write(r, c, relocated_string);
                }
            }
        }
        // 图像插入
        for (int r = 1; r <= rows; r++) {
            for (int c = 1; c <= cols; c++) {
                static QRegularExpression reg(R"(\$<(.+?)>)");
                auto                      val                = doc.read(r, c);
                auto                      iterator           = reg.globalMatch(val.toString());
                QString                   relocated_string   = val.toString();
                QJsonDocument             image_replace_json = {};
                while (iterator.hasNext()) {
                    auto match         = iterator.next();
                    image_replace_json = {};
                    if (!match.captured(1).isEmpty()) {
                        auto raw_string = match.captured(1);
                        // 将原有字符串删除
                        relocated_string = relocated_string.replace(QString("$<%1>").arg(raw_string), "");
                        QJsonParseError err;
                        image_replace_json = QJsonDocument::fromJson(QString("{%1}").arg(raw_string).toUtf8(), &err);
                        if (image_replace_json.isNull()) {
                            qCWarning(TAG) << QString("{%1}").arg(raw_string) << "Err:" << err.errorString();
                        }
                    }
                }
                if (relocated_string != val.toString()) {
                    doc.write(r, c, relocated_string);

                    if (!image_replace_json.isObject()) {
                        // 如果字符串不能转化为JSON, 则下一次循环
                        continue;
                    }
                    auto image_replace_json_obj = image_replace_json.object();
                    if (!image_replace_json_obj.contains("name") || !images.contains(image_replace_json_obj.value("name").toString())) {
                        // 如果JSON中不包含图像名或者传入的图像列表不包含该文件名
                        qCWarning(TAG) << "can not find image name, available image name: " << images.keys();
                        continue;
                    }

                    std::optional<int> img_width  = std::nullopt;
                    std::optional<int> img_height = std::nullopt;
                    QImage             image      = images.value(image_replace_json_obj.value("name").toString());

                    if (image_replace_json_obj.contains("width")) {
                        img_width = image_replace_json_obj.value("width").toInt();
                    }

                    if (image_replace_json_obj.contains("height")) {
                        img_height = image_replace_json_obj.value("height").toInt();
                    }

                    if (img_width.has_value() && img_height.has_value()) {
                        doc.insertImage(r - 1, c - 1, image.scaled(QSize(img_width.value(), img_height.value()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                    } else if (img_width.has_value()) {
                        doc.insertImage(r - 1, c - 1, image.scaledToWidth(img_width.value(), Qt::SmoothTransformation));
                    } else if (img_height.has_value()) {
                        doc.insertImage(r - 1, c - 1, image.scaledToHeight(img_height.value(), Qt::SmoothTransformation));
                    } else {
                        doc.insertImage(r - 1, c - 1, image);
                    }
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
