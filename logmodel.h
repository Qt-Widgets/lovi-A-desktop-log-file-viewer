#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QColor>

class Config;

struct LogCell {
    QString text;
    QColor bgColor;
    QColor fgColor;
};

struct LogLine {
    std::vector<LogCell> cells;

    bool isValid() const {
        return !cells.empty();
    }
};

class LogModel : public QAbstractTableModel {
public:
    LogModel(const Config& config, const QStringList& lines);

    int rowCount(const QModelIndex& parent = {}) const override;

    int columnCount(const QModelIndex& parent = {}) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QStringList columns() const;

private:
    const Config& mConfig;
    const QStringList mLines;
    QStringList mColumns;
    mutable QHash<int, LogLine> mLogLineCache;

    LogLine processLine(const QString& line) const;

    void applyHighlights(LogCell* logCell, int column) const;
};


#endif // LOGMODEL_H
