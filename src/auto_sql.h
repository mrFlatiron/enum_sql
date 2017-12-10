#ifndef auto_sql_H
#define auto_sql_H


#include "sqlite/sqlite3.h"
#include "enum_sql_typedefs.h"

namespace enum_sql
{
  enum class sql_open_policy
  {
    create_if_not_exist,
    fail_if_not_exist,
    COUNT
  };

  class Auto_sql
  {
  private:
    using self = Auto_sql;
    using SOP = sql_open_policy;

    sqlite3 *m_handle = nullptr;
    std::string m_filename;
    bool m_is_opened = false;
    int m_last_error = SQLITE_OK;
  public:
    Auto_sql () = default;
    ~Auto_sql ();

    Auto_sql (const Auto_sql &rhs) = delete;
    Auto_sql &operator= (const Auto_sql &rhs) = delete;

    Auto_sql (Auto_sql &&rhs);
    Auto_sql &operator= (Auto_sql &&rhs);

    Auto_sql (std::string filename, SOP policy = SOP::create_if_not_exist);

    bool open (std::string filename, SOP policy = SOP::create_if_not_exist);

    bool close ();

    bool is_open () const;

    operator sqlite3 * () const;

    sql_data_type column_datatype (std::string table, std::string column) const;

  };

}
#endif // auto_sql_H
