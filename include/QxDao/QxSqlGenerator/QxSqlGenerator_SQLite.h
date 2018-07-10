/****************************************************************************
**
** http://www.qxorm.com/
** http://sourceforge.net/projects/qxorm/
** Original file by Lionel Marty
**
** This file is part of the QxOrm library
**
** This software is provided 'as-is', without any express or implied
** warranty. In no event will the authors be held liable for any
** damages arising from the use of this software.
**
** GNU Lesser General Public License Usage
** This file must be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file 'license.lgpl.txt' included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact :
** contact@qxorm.com
**
****************************************************************************/

#ifndef _QX_SQL_GENERATOR_SQLITE_H_
#define _QX_SQL_GENERATOR_SQLITE_H_

#ifdef _MSC_VER
#pragma once
#endif

/*!
 * \file QxSqlGenerator_SQLite.h
 * \author Lionel Marty
 * \ingroup QxDao
 * \brief SQL generator for SQLite database
 */

#include <QxDao/QxSqlGenerator/QxSqlGenerator_Standard.h>

namespace qx {
namespace dao {
namespace detail {

/*!
 * \ingroup QxDao
 * \brief qx::dao::detail::QxSqlGenerator_SQLite : SQL generator for SQLite database
 */
class QX_DLL_EXPORT QxSqlGenerator_SQLite : public QxSqlGenerator_Standard
{

public:

   QxSqlGenerator_SQLite();
   virtual ~QxSqlGenerator_SQLite();

};

typedef boost::shared_ptr<QxSqlGenerator_SQLite> QxSqlGenerator_SQLite_ptr;

} // namespace detail
} // namespace dao
} // namespace qx

#endif // _QX_SQL_GENERATOR_SQLITE_H_
