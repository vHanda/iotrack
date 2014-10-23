/*
 * Copyright (C) 2014  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <QTreeView>
#include <QFileInfo>
#include <QSortFilterProxyModel>

#include "functionsmodel.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("filename"), QStringLiteral("file to visualize"));
    parser.addHelpOption();
    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        parser.showHelp(1);
    }

    QString fileName = args.first();

    QTreeView* view = new QTreeView(0);

    FunctionsModel* model = new FunctionsModel(view);
    model->setFilePath(QFileInfo(fileName).absoluteFilePath());

    QSortFilterProxyModel* sortModel = new QSortFilterProxyModel(view);
    sortModel->setSourceModel(model);

    view->setModel(sortModel);
    view->setSortingEnabled(true);
    view->show();

    return app.exec();
}
