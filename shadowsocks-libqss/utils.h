/*
 * Copyright (C) 2015-2016 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>
#include <QStringList>

struct Utils
{
    //test data encrypt/decrypt speed. print result to terminal
    static void testSpeed(const std::string &method, uint32_t data_size_mb);
    static void testSpeed(uint32_t data_size_mb);//test all methods

    /**
     * @brief messageHandler handles Qt messages/logs
     * @param type
     * @param context
     * @param msg
     */
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    enum class LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    static LogLevel logLevel;
};

#endif // UTILS_H
