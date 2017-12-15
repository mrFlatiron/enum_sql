#include "auto_stmt.h"
#include "auto_sql.h"
#include "sqlite/sqlite3.h"

namespace enum_sql
{
  Auto_stmt::~Auto_stmt ()
  {
    finalize ();
  }

  Auto_stmt::Auto_stmt (Auto_sql *db, const std::string &stmt_string)
  {
    set_db (db);
    set_stmt_string (stmt_string);
    prepare ();
  }

  Auto_stmt::Auto_stmt (Auto_stmt &&rhs)
  {
    *this = std::move (rhs);
  }

  Auto_stmt &Auto_stmt::operator= (Auto_stmt &&rhs)
  {
    m_stmt = rhs.m_stmt;
    m_db = rhs.m_db;
    m_stmt_string = std::move (rhs.m_stmt_string);
    m_status = rhs.m_status;
    m_is_prepared = rhs.m_is_prepared;

    rhs.m_is_prepared = false;

    return *this;
  }

  void Auto_stmt::set_db (Auto_sql *db)
  {
    m_db = db;
  }

  void Auto_stmt::set_stmt_string (const std::string &stmt)
  {
    m_stmt_string = stmt;
  }

  bool Auto_stmt::prepare ()
  {
    if (!m_db)
      return false;

    if (!m_db->is_open ())
      {
        m_stmt = nullptr;
        m_is_prepared = false;
        m_status = SQLITE_ERROR;
        return false;
      }
    if (m_stmt_string.empty ())
      {
        ENUM_SQL_DEBUG_PAUSE ("Shouldn't be empty");
        m_status = SQLITE_ERROR;
        return false;
      }
    const char *tail;
    m_status = sqlite3_prepare_v2 (*m_db, m_stmt_string.c_str (), m_stmt_string.length (), &m_stmt, &tail);
    if (m_status == SQLITE_OK && m_stmt)
      m_is_prepared = true;
    else
      {
        m_is_prepared = false;
      }

    return m_is_prepared;
  }

  bool Auto_stmt::is_prepared () const
  {
    return m_is_prepared && m_db && m_db->is_open ();
  }

  int Auto_stmt::status () const
  {
    return m_status;
  }

  bool Auto_stmt::execute ()
  {
    if (!is_prepared ())
      return false;

    m_status = sqlite3_step (m_stmt);

    if (m_status == SQLITE_ROW
        || m_status == SQLITE_DONE)
      return true;

    return false;
  }

  bool Auto_stmt::prepare_execute ()
  {
    if (!is_prepared ())
      prepare ();

    return execute (); //derived execute
  }

  sqlite3_stmt *Auto_stmt::get () const
  {
    return m_stmt;
  }

  std::string Auto_stmt::stmt_string () const
  {
    return m_stmt_string;
  }

  bool Auto_stmt::finalize ()
  {
    if (need_finalize ())
      {
        m_status = sqlite3_finalize (m_stmt);

        if (m_status != SQLITE_OK)
          {
            ENUM_SQL_DEBUG_PAUSE ("Something went wrong");
            return false;
          }
        m_is_prepared = false;
        return true;
      }
    return true;
  }

  bool Auto_stmt::need_finalize () const
  {
    return m_is_prepared;
  }
}
