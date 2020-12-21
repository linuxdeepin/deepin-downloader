/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file func.h
 *
 * @brief 方法类
 *
 * @date 2020-08-28 09:50
 *
 * Author: bulongwei  <bulongwei@uniontech.com>
 *
 * Maintainer: bulongwei  <bulongwei@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FUNC_H
#define FUNC_H

#include <QObject>

class Func : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 网络是否联通
     * @return true为不通 false为通
     */
    static bool isNetConnect();

    /**
     * @brief http是否联通
     * @return true为不通 false为通
     */
    static bool isHTTPConnect();

    /**
     * @brief 设置Mimeapps文件中的值，如果key不存在，新建key。
     * @param key 配置中的key，如果key不存在新建key
     * @param value key对应值。
     * @return 是否设置成功
     */
    static bool setMimeappsValue(QString key, QString value = "");


private:
};

#endif // FUNC_H
