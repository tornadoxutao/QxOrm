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

#include <QxPrecompiled.h>

#include <QxDao/IxDao_Helper.h>

#include <QxMemLeak/mem_leak.h>

#define QX_DAO_ERR_INTERNAL                  "[QxOrm] 'qx::dao' internal error"
#define QX_DAO_ERR_UNKNOWN_ERROR             "[QxOrm] 'qx::dao' unknown error"
#define QX_DAO_ERR_NO_CONNECTION             "[QxOrm] error retrieving valid sql connection to database"
#define QX_DAO_ERR_OPEN_CONNECTION           "[QxOrm] unable to open connection to database"
#define QX_DAO_ERR_BUILD_SQL_QUERY           "[QxOrm] error building sql query : %s"
#define QX_DAO_ERR_EXECUTE_SQL_QUERY         "[QxOrm] execute sql query failed : %s"
#define QX_DAO_ERR_NO_DATA                   "[QxOrm] sql query returns no data"
#define QX_DAO_ERR_NO_QUERY_BUILDER          "[QxOrm] unable to construct sql query builder"
#define QX_DAO_ERR_NO_ELEMENT_IN_CONTAINER   "[QxOrm] no element in container"
#define QX_DAO_ERR_INVALID_PRIMARY_KEY       "[QxOrm] invalid primary key"
#define QX_DAO_ERR_INVALID_SQL_RELATION      "[QxOrm] invalid sql relation"

namespace qx {
namespace dao {
namespace detail {

IxDao_Helper::IxDao_Helper() : m_lDataCount(0), m_bTransaction(false), m_bQuiet(false), m_bTraceQuery(true), m_bTraceRecord(false), m_bCartesianProduct(false), m_pDataMemberX(NULL), m_pDataId(NULL) { ; }

IxDao_Helper::~IxDao_Helper() { terminate(); }

void IxDao_Helper::updateQueryBuilder() { ; }

bool IxDao_Helper::isValid() const { return (! m_error.isValid() && m_pQueryBuilder); }

bool IxDao_Helper::hasFeature(QSqlDriver::DriverFeature ft) const { return (m_database.driver() ? m_database.driver()->hasFeature(ft) : false); }

QSqlDatabase & IxDao_Helper::database() { return m_database; }

const QSqlDatabase & IxDao_Helper::database() const { return m_database; }

QSqlQuery & IxDao_Helper::query() { return m_query; }

const QSqlQuery & IxDao_Helper::query() const { return m_query; }

QSqlError & IxDao_Helper::error() { return m_error; }

const QSqlError & IxDao_Helper::error() const { return m_error; }

qx::QxSqlQuery & IxDao_Helper::qxQuery() { return m_qxQuery; }

const qx::QxSqlQuery & IxDao_Helper::qxQuery() const { return m_qxQuery; }

qx::IxDataMemberX * IxDao_Helper::getDataMemberX() const { return m_pDataMemberX; }

long IxDao_Helper::getDataCount() const { return m_lDataCount; }

qx::IxDataMember * IxDao_Helper::getDataId() const { return m_pDataId; }

qx::IxDataMember * IxDao_Helper::nextData(long & l) const { return (m_pQueryBuilder ? m_pQueryBuilder->nextData(l) : NULL); }

QString IxDao_Helper::sql() const { return (m_pQueryBuilder ? m_pQueryBuilder->getSqlQuery() : ""); }

IxDao_Helper::type_lst_relation * IxDao_Helper::getSqlRelationX() const { return m_pSqlRelationX.get(); }

bool IxDao_Helper::getCartesianProduct() const { return m_bCartesianProduct; }

QStringList IxDao_Helper::getSqlColumns() const { return m_lstColumns; }

void IxDao_Helper::setSqlColumns(const QStringList & lst) { m_lstColumns = lst; }

void IxDao_Helper::updateError(const QSqlError & error) { m_error = error; }

void IxDao_Helper::quiet() { m_bQuiet = true; }

bool IxDao_Helper::exec() { return (m_qxQuery.isEmpty() ? this->query().exec(this->builder().getSqlQuery()) : this->query().exec()); }

qx::IxSqlQueryBuilder & IxDao_Helper::builder()
{
   qAssert(m_pQueryBuilder);
   return (* m_pQueryBuilder);
}

const qx::IxSqlQueryBuilder & IxDao_Helper::builder() const
{
   qAssert(m_pQueryBuilder);
   return (* m_pQueryBuilder);
}

QSqlError IxDao_Helper::errFailed()
{
   qDebug(QX_DAO_ERR_EXECUTE_SQL_QUERY, qPrintable(sql()));
   m_error = m_query.lastError();
   return m_error;
}

QSqlError IxDao_Helper::errEmpty()
{
   qDebug(QX_DAO_ERR_BUILD_SQL_QUERY, qPrintable(sql()));
   m_error = m_query.lastError();
   return m_error;
}

QSqlError IxDao_Helper::errNoData()
{
   updateError(QX_DAO_ERR_NO_DATA);
   return m_error;
}

QSqlError IxDao_Helper::errInvalidId()
{
   updateError(QX_DAO_ERR_INVALID_PRIMARY_KEY);
   return m_error;
}

QSqlError IxDao_Helper::errInvalidRelation()
{
   updateError(QX_DAO_ERR_INVALID_SQL_RELATION);
   return m_error;
}

bool IxDao_Helper::transaction()
{
   if (isValid() && hasFeature(QSqlDriver::Transactions))
   { m_bTransaction = m_database.transaction(); }
   return m_bTransaction;
}

bool IxDao_Helper::nextRecord()
{
   if (! m_query.next()) { return false; }
   if (m_bTraceRecord) { dumpRecord(); }
   return true;
}

bool IxDao_Helper::updateSqlRelationX(const QStringList & relation)
{
   m_bCartesianProduct = false;
   bool bError = false; QString sHashRelation;
   m_pSqlRelationX.reset(new IxDao_Helper::type_lst_relation());
   qx::IxSqlRelationX * pAllRelation = this->builder().getLstRelation();
   if ((relation.count() == 1) && (relation.at(0) == "*")) { sHashRelation = "*"; (* m_pSqlRelationX) = (* pAllRelation); }
   else if (relation.count() == pAllRelation->count()) { sHashRelation = "*"; (* m_pSqlRelationX) = (* pAllRelation); }
   else { Q_FOREACH(QString sKey, relation) { if (pAllRelation->exist(sKey)) { sHashRelation += (sKey + "|"); m_pSqlRelationX->insert(sKey, pAllRelation->getByKey(sKey)); } else { bError = true; } } }
   if (bError || (m_pSqlRelationX->count() <= 0)) { m_pSqlRelationX.reset(NULL); }
   if (m_pSqlRelationX) { _foreach(qx::IxSqlRelation * p, (* m_pSqlRelationX)) { m_bCartesianProduct = (m_bCartesianProduct || p->getCartesianProduct()); } }
   if (! bError && m_pQueryBuilder) { m_pQueryBuilder->setHashRelation(sHashRelation); m_pQueryBuilder->setCartesianProduct(m_bCartesianProduct); }
   return (! bError && (m_pSqlRelationX.get() != NULL));
}

void IxDao_Helper::dumpRecord() const
{
   if (! m_query.isValid()) { return; }
   QString sDump; QVariant v;
   QSqlRecord record = m_query.record();
   int iCount = record.count();
   if (iCount <= 0) { return; }
   for (int i = 0; i < iCount; ++i)
   { v = record.value(i); sDump += (v.isNull() ? QString("NULL") : v.toString()) + QString("|"); }
   sDump = sDump.left(sDump.count() - 1); // Remove last "|"
   qDebug("[QxOrm] dump sql record : %s", qPrintable(sDump));
}

void IxDao_Helper::addQuery(const qx::QxSqlQuery & query, bool bResolve)
{
   m_qxQuery = query;
   if (m_qxQuery.isEmpty()) { return; }
   QString sql = this->builder().getSqlQuery();
   QString sqlToAdd = m_qxQuery.query().trimmed();
   bool bAddSqlCondition = false;
   if (sqlToAdd.left(6).contains("WHERE ", Qt::CaseInsensitive)) { sqlToAdd = sqlToAdd.right(sqlToAdd.size() - 6); bAddSqlCondition = true; }
   else if (sqlToAdd.left(4).contains("AND ", Qt::CaseInsensitive)) { sqlToAdd = sqlToAdd.right(sqlToAdd.size() - 4); bAddSqlCondition = true; }
   sql += (bAddSqlCondition ? qx::IxSqlQueryBuilder::addSqlCondition(sql) : QString(" ")) + sqlToAdd;
   if (m_qxQuery.isDistinct() && sql.left(7).contains("SELECT ", Qt::CaseInsensitive))
   { sql = "SELECT DISTINCT " + sql.right(sql.size() - 7); }
   m_qxQuery.postProcess(sql);
   this->builder().setSqlQuery(sql);
   if (bResolve) { this->query().prepare(sql); m_qxQuery.resolve(this->query()); }
}

QSqlError IxDao_Helper::updateError(const QString & sError)
{
   m_error = QSqlError((QX_DAO_ERR_INTERNAL + QString(" <") + m_context + QString(">") + (sql().isEmpty() ? QString("") : (QString(" : ") + sql()))), sError, QSqlError::UnknownError);
   return m_error;
}

void IxDao_Helper::init(QSqlDatabase * pDatabase, const QString & sContext)
{
   m_time.start();
   m_context = sContext;
   m_bTraceQuery = qx::QxSqlDatabase::getSingleton()->getTraceSqlQuery();
   m_bTraceRecord = qx::QxSqlDatabase::getSingleton()->getTraceSqlRecord();
   qAssert(! m_context.isEmpty());

   m_database = (pDatabase ? (* pDatabase) : qx::QxSqlDatabase::getDatabase());
   if (! m_database.isValid()) { updateError(QX_DAO_ERR_NO_CONNECTION); return; }
   if (! m_database.isOpen() && ! m_database.open()) { updateError(QX_DAO_ERR_OPEN_CONNECTION); return; }

   updateQueryBuilder();
   if (! m_pQueryBuilder) { updateError(QX_DAO_ERR_NO_QUERY_BUILDER); return; }

   m_pQueryBuilder->init();
   m_query = QSqlQuery(m_database);
   m_query.setForwardOnly(true);
   m_pDataMemberX = (m_pQueryBuilder ? m_pQueryBuilder->getDataMemberX() : NULL);
   m_lDataCount = (m_pQueryBuilder ? m_pQueryBuilder->getDataCount() : 0);
   m_pDataId = (m_pQueryBuilder ? m_pQueryBuilder->getDataId() : NULL);
}

void IxDao_Helper::terminate()
{
   if (! isValid())
   {
      if (m_bTransaction) { m_database.rollback(); }
      if (! m_bQuiet) { qDebug("%s", qPrintable(m_error.driverText())); qDebug("%s", qPrintable(m_error.databaseText())); }
   }
   else if (m_pQueryBuilder)
   {
      if (m_bTransaction) { m_database.commit(); }
      if (! m_bQuiet && m_bTraceQuery) { m_pQueryBuilder->displaySqlQuery(m_time.elapsed()); }
   }
   else
   {
      if (m_bTransaction) { m_database.rollback(); }
      if (! m_bQuiet) { qDebug("%s", QX_DAO_ERR_UNKNOWN_ERROR); qAssert(false); }
   }

   m_bTransaction = false;
}

} // namespace detail
} // namespace dao
} // namespace qx
