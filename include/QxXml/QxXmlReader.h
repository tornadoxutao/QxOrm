/****************************************************************************
**
** Copyright (C) 2010 QxOrm France and/or its subsidiary(-ies)
** Contact: QxOrm France Information (contact@qxorm.com)
**
** This file is part of the QxOrm library
**
** Commercial Usage
** Licensees holding valid QxOrm Commercial licenses may use this file in
** accordance with the QxOrm Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and QxOrm France
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file 'license.gpl3.txt' included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html
**
** If you are unsure which license is appropriate for your use, please
** contact the support department at support@qxorm.com
**
****************************************************************************/

#ifndef _QX_XML_READER_H_
#define _QX_XML_READER_H_

#ifdef _MSC_VER
#pragma once
#endif

#include <QtCore/qhash.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdatastream.h>

#include <QtXml/QXmlStreamReader>

#include "../../include/QxXml/QxXmlWriter.h"

namespace qx {

class QX_DLL_EXPORT QxXmlReader : public QXmlStreamReader
{

public:

   typedef boost::shared_ptr<QByteArray> type_byte_arr_ptr;
   typedef QHash<QString, type_byte_arr_ptr> type_hash_bin_data;
   typedef QHashIterator<QString, type_byte_arr_ptr> type_hash_bin_data_itr;

protected:

   type_hash_bin_data m_mapBinaryData;    // Collection of binary data associated with xml (<=> attached files)

public:

   QxXmlReader() : QXmlStreamReader() { ; }
   QxXmlReader(const QString & data) : QXmlStreamReader(data) { ; }
   virtual ~QxXmlReader() { ; }

   void addBinaryData(const type_hash_bin_data & other, bool bClear);
   void addBinaryData(const QString & sKey, type_byte_arr_ptr pData);
   void removeBinaryData(const QString & sKey);
   void removeAllBinaryData();

   bool isStartBinaryData() const;
   type_byte_arr_ptr readBinaryData();

};

} // namespace qx

QX_DLL_EXPORT QDataStream & operator >> (QDataStream & stream, qx::QxXmlReader & xmlReader);

#endif // _QX_XML_READER_H_