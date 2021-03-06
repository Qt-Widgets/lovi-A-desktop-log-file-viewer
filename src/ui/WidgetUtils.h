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
#ifndef WIDGETUTILS_H
#define WIDGETUTILS_H

#include <QWidget>

#include <memory>

class QLineEdit;
class QString;

namespace WidgetUtils {

/**
 * Add an help icon to the right part of a line edit, clicking on it
 * shows message as a tooltip
 */
void addLineEditHelpIcon(QLineEdit* lineEdit, const QString& message);

/**
 * Instantiate a Qt Designer ui class and call setupUi() on it.
 * Makes it possible to pass its members to the constructor of other members.
 */
template <class T> std::unique_ptr<T> initUi(QWidget* parent) {
    auto ui = std::make_unique<T>();
    ui->setupUi(parent);
    return ui;
}

} // namespace WidgetUtils

#endif // WIDGETUTILS_H
