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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QPointer>

#include <memory>

class Config;
class LogFormatDialog;
class LogFormatStore;
class LogFormat;
class MainController;

class QTreeView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(Config* config, LogFormatStore* store, QWidget* parent = nullptr);
    ~MainWindow();

    void setLogFormat(LogFormat* logFormat);
    void loadLog(const QString& filePath);

private:
    void setupUi();
    void setupActions();
    void onRowsInserted();
    void onSelectionChanged();
    void reloadLogFormat();
    void showOpenLogDialog();
    void showLogFormatDialog();
    void copySelectedLines();
    void fillRecentFilesMenu();

    std::unique_ptr<MainController> mController;

    QAction* const mOpenLogAction;
    QAction* const mSelectLogFormatAction;
    QAction* const mAutoScrollAction;
    QAction* const mCopyLinesAction;

    QMenu* const mRecentFilesMenu;
    QToolBar* const mToolBar;
    QTreeView* const mTreeView;
    QPointer<LogFormatDialog> mLogFormatDialog;
};

#endif // MAINWINDOW_H