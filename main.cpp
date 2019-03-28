#include "config.h"
#include "logmodel.h"

#include <QAbstractListModel>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

#include <iostream>

using std::optional;
using std::cout;

void dumpModel(LogModel* model) {
    int rowCount = model->rowCount();
    int columnCount = model->columns().count();
    cout << "<html><body>\n";
    cout << "<table>\n";
    for (int row = 0; row < rowCount; ++row) {
        auto idx = model->index(row);
        QString bgColor = idx.data(LogModel::BackgroundColorRole).toString();
        cout << "<tr";
        if (!bgColor.isEmpty()) {
            cout << " style='background-color:" << bgColor.toStdString() << "'";
        }
        cout << ">";

        for (int column = 0; column < columnCount; ++column) {
            QString value = idx.data(column).toString();
            cout << "<td>" << value.toStdString() << "</td>";
        }
        cout << "</tr>\n";
    }
    cout << "</table>\n";
    cout << "</body></html>\n";
}

optional<QByteArray> readFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << fileName;
        return {};
    }
    return file.readAll();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString configFileName = argv[1];
    QString logFileName = argv[2];

    auto json = readFile(configFileName);
    if (!json.has_value()) {
        return 1;
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << configFileName << ":" << error.errorString();
        return 1;
    }

    auto config = Config::fromJsonDocument(doc);
    if (!config) {
        qWarning() << "Failed to parse" << configFileName;
        return 1;
    }

    QStringList lines;
    {
        auto logContent = readFile(logFileName);
        if (!logContent.has_value()) {
            return 1;
        }
        lines = QString::fromUtf8(logContent.value()).split('\n');
    }

    LogModel model(config.value(), lines);

    dumpModel(&model);

    return 0;
}