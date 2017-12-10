#include "auto_sql.h"
#include "common/enum_misc.h"

namespace enum_sql
{
  Auto_sql::~Auto_sql ()
  {
    close ();
  }

  Auto_sql::Auto_sql(Auto_sql &&rhs)
  {
    m_handle = rhs.m_handle;
    m_filename = std::move (rhs.m_filename);
    m_is_opened = rhs.m_is_opened;
    m_last_error = rhs.m_last_error;

    rhs.m_handle = nullptr;
  }

  Auto_sql &Auto_sql::operator = (Auto_sql &&rhs)
  {
    m_handle = rhs.m_handle;
    m_filename = std::move (rhs.m_filename);
    m_is_opened = rhs.m_is_opened;
    m_last_error = rhs.m_last_error;

    rhs.m_handle = nullptr;

    return *this;
  }

  Auto_sql::Auto_sql (std::string filename, Auto_sql::SOP policy)
  {
    open (filename, policy);
  }

  bool Auto_sql::open (std::string filename, Auto_sql::SOP policy)
  {
    if (!close ())
      return false;

    m_filename = filename;
    int flag_create = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    switch (policy)
      {
      case SOP::create_if_not_exist:
        m_last_error = sqlite3_open_v2 (m_filename.c_str (), &m_handle, flag_create, nullptr);
        break;
      case SOP::fail_if_not_exist:
        m_last_error = sqlite3_open_v2 (m_filename.c_str (), &m_handle, SQLITE_OPEN_READWRITE, nullptr);
        break;
      case SOP::COUNT:
        DEBUG_PAUSE ("Shouldn't happen");
        return false;
      }
    if (m_handle && m_last_error == SQLITE_OK)
      m_is_opened = true;
    else
      m_is_opened = false;

    return m_is_opened;
  }

  bool Auto_sql::close ()
  {
    if (!is_open ())
      return true;

    m_last_error = sqlite3_close_v2 (m_handle);

    if (SQLITE_OK == m_last_error)
      {
        m_handle = nullptr;
        m_is_opened = false;
        return true;
      }

    return false;
  }

  bool Auto_sql::is_open () const
  {
    return m_handle && m_is_opened;
  }

  sql_data_type Auto_sql::column_datatype (std::string table, std::string column) const
  {
    if (!is_open ())
      {
        DEBUG_PAUSE ("Not open");
        return sql_data_type::Null;
      }
    char const *buf;
    if (SQLITE_OK != sqlite3_table_column_metadata (m_handle, NULL, table.c_str (), column.c_str (),
                                                    &buf, NULL, NULL, NULL, NULL))
      {
        DEBUG_PAUSE ("Error");
        return sql_data_type::Null;
      }

    return string_to_enum<sql_data_type> (buf);
  }

  Auto_sql::operator sqlite3 *() const
  {
    return m_handle;
  }
}
