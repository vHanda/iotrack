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

#include "functionsmodel.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

FunctionsModel::FunctionsModel(QObject* parent): QAbstractListModel(parent)
{
}

void FunctionsModel::setFilePath(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        qDebug() << "File could not be opened";
        return;
    }

    QTextStream input(&file);

    QMap<int, int> m_openFiles;
    QMap<int, QString> m_strings;
    while (!input.atEnd()) {
        QString line = input.readLine();
        // qDebug() << line;

        char command;
        QTextStream stream(&line);
        stream >> command;

        switch (command) {
            case 'o': {
                int fd;
                int sid;

                stream >> fd >> sid;
                m_openFiles.insert(fd, sid);
                break;
            }

            case 'c': {
                int fd;
                stream >> fd;
                m_openFiles.remove(fd);
                break;
            }

            case 'r': {
                int ret;
                int fd;
                int size;

                stream >> ret >> fd >> size;
                QString fileName = m_strings.value(m_openFiles.value(fd));

                QString btLine = input.readLine();
                QTextStream btStream(&btLine, QIODevice::ReadOnly);

                while (!btStream.atEnd()) {
                    QString fid;
                    btStream >> fid;
                    if (fid.isEmpty()) {
                        break;
                    }

                    QString function = m_strings.value(fid.toInt());
                    m_data[function][fileName].rBytes += size;
                }
                break;
            }
            case 'w': {
                int ret;
                int fd;
                int size;

                stream >> ret >> fd >> size;
                QString fileName = m_strings.value(m_openFiles.value(fd));

                QString btLine = input.readLine();
                QTextStream btStream(&btLine, QIODevice::ReadOnly);

                while (!btStream.atEnd()) {
                    QString fid;
                    btStream >> fid;
                    if (fid.isEmpty()) {
                        break;
                    }

                    QString function = m_strings.value(fid.toInt());
                    m_data[function][fileName].wBytes += size;
                }
                break;
            }
            case 's': {
                int id;
                QString str;

                stream >> id;
                str = stream.readAll().trimmed();
                m_strings.insert(id, str);
                break;
            }

            default:
                qDebug() << "Got line" << line;
                qDebug() << "Not a valid iotrace file";
                exit(1);
        }
    }

    beginResetModel();
    m_functions = m_data.keys();
    endResetModel();
}

QVariant FunctionsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= m_functions.size()) {
        return QVariant();
    }

    QString function = m_functions.at(index.row());
    const QMap<QString, IOData> data = m_data.value(function);

    int totalRead = 0;
    int totalWrite = 0;
    for (auto it = data.begin(); it != data.end(); it++) {
        totalRead += it.value().rBytes;
        totalWrite += it.value().wBytes;
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    int c = index.column();
    switch (c) {
        case 0:
            return totalRead;

        case 1:
            return totalWrite;

        case 2: {
            QTextStream s(&function, QIODevice::ReadOnly);
            int ip;
            s >> ip;
            return s.readAll();
        }
    }

    return QVariant();
}

int FunctionsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_functions.size();
}

int FunctionsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return 3;
}

QVariant FunctionsModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section < 0 || section > 2) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
        case 0:
            return QStringLiteral("Read");
        case 1:
            return QStringLiteral("Write");
        case 2:
            return QStringLiteral("Function");
    }

    return QVariant();
}
