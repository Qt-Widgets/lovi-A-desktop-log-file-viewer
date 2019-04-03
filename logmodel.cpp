#include "logmodel.h"

#include "lineprovider.h"
#include "logformat.h"

#include <QColor>
#include <QDebug>

using std::optional;

LogModel::LogModel(const LineProvider* lineProvider, QObject* parent)
        : QAbstractTableModel(parent), mLineProvider(lineProvider) {
    mEmptyLogFormat = LogFormat::createEmpty();
    setLogFormat(mEmptyLogFormat.get());
    connect(mLineProvider, &LineProvider::lineCountChanged, this, &LogModel::onLineCountChanged);
}

int LogModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mLineProvider->lineCount();
}

int LogModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return mColumns.count();
}

QVariant LogModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= mLineProvider->lineCount()) {
        return {};
    }
    auto it = mLogLineCache.find(row);
    LogLine logLine;
    if (it == mLogLineCache.end()) {
        const QString& line = mLineProvider->lineAt(row);
        logLine = processLine(line);
        mLogLineCache[row] = logLine;
        if (!logLine.isValid()) {
            qWarning() << "Line" << row + 1 << "does not match:" << line;
        }
    } else {
        logLine = it.value();
    }
    if (!logLine.isValid()) {
        const QString& line = mLineProvider->lineAt(row);
        return role == Qt::DisplayRole && index.column() == mColumns.count() - 1 ? QVariant(line)
                                                                                 : QVariant();
    }
    const auto& cell = logLine.cells.at(index.column());
    switch (role) {
    case Qt::BackgroundColorRole:
        return cell.bgColor.isValid() ? QVariant(cell.bgColor) : QVariant();
    case Qt::TextColorRole:
        return cell.fgColor.isValid() ? QVariant(cell.fgColor) : QVariant();
    case Qt::DisplayRole:
        return cell.text;
    };
    return {};
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Vertical) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        return mColumns.at(section);
    }
    return {};
}

QStringList LogModel::columns() const {
    return mColumns;
}

void LogModel::setLogFormat(const LogFormat* logFormat) {
    beginResetModel();
    if (logFormat) {
        mLogFormat = logFormat;
    } else {
        mLogFormat = mEmptyLogFormat.get();
    }
    mColumns = mLogFormat->parser.namedCaptureGroups();
    mColumns.removeFirst();
    mLogLineCache.clear();
    endResetModel();
}

LogLine LogModel::processLine(const QString& line) const {
    auto match = mLogFormat->parser.match(line);
    if (!match.hasMatch()) {
        return {};
    }
    LogLine logLine;
    int count = mColumns.count();

    logLine.cells.resize(count);
    for (int column = 0; column < count; ++column) {
        LogCell& cell = logLine.cells[column];
        cell.text = match.captured(column + 1).trimmed();
        applyHighlights(&cell, column);
    }
    return logLine;
}

static void applyColor(QColor* out, const optional<HighlightColor>& color, const QString& text) {
    if (!color.has_value()) {
        return;
    }
    *out = color.value().toColor(text);
}

void LogModel::applyHighlights(LogCell* cell, int column) const {
    for (const Highlight& highlight : mLogFormat->highlights) {
        if (highlight.condition->column() == column) {
            if (highlight.condition->eval(cell->text)) {
                applyColor(&cell->bgColor, highlight.bgColor, cell->text);
                applyColor(&cell->fgColor, highlight.fgColor, cell->text);
                return;
            }
        }
    }
}

void LogModel::onLineCountChanged(int newCount, int oldCount) {
    if (newCount <= oldCount) {
        // Full refresh
        beginResetModel();
        mLogLineCache.clear();
        endResetModel();
        return;
    }
    // Assume append
    beginInsertRows({}, oldCount, newCount - 1);
    endInsertRows();
}
