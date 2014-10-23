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

#ifndef FUNCTIONSMODEL_H
#define FUNCTIONSMODEL_H

#include <QAbstractListModel>
#include <QHash>

class FunctionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit FunctionsModel(QObject* parent = 0);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent) const;

    void setFilePath(const QString& filePath);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    struct IOData {
        int rBytes;
        int wBytes;
    };

    // Maps functionName -> (fileName, IOData)
    QHash<QString, QMap<QString, IOData> > m_data;
    QStringList m_functions;
};

#endif // FUNCTIONSMODEL_H
