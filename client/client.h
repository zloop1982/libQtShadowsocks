/*
 * client.h - header file of Client class
 *
 * Copyright (C) 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtShadowsocks>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(bool _debug, QObject *parent = 0);
    bool readConfig(const QString &);
    QString getMethod() const;

public slots:
    bool start(bool _server = false);

private:
    bool debug;
    QSS::Controller *lc;
    QSS::Profile profile;

    bool cipherTest();

private slots:
    void logHandler(const QString &);
};

#endif // CLIENT_H
