#ifndef AUTO_SQL_STMT_H
#define AUTO_SQL_STMT_H

#include "auto_sql.h"

struct sqlite3_stmt;

namespace enum_sql
{

  class Auto_stmt
  {
  protected:
    sqlite3_stmt *m_stmt = nullptr;
    Auto_sql *m_db;
    std::string m_stmt_string;
    int m_status = 0;
    bool m_is_prepared = false;
  public:
    Auto_stmt () = default;
    virtual ~Auto_stmt ();

    Auto_stmt (Auto_sql *db, const std::string &stmt_string);

    Auto_stmt (const Auto_stmt &rhs) = delete;
    Auto_stmt &operator= (const Auto_stmt &rhs) = delete;

    Auto_stmt (Auto_stmt &&rhs);
    Auto_stmt &operator= (Auto_stmt &&rhs);

    void set_db (Auto_sql *db);
    void set_stmt_string (const std::string &stmt);

    bool prepare ();

    bool is_prepared () const;

    int status () const;

    virtual bool execute ();

    bool prepare_execute();

    sqlite3_stmt *get () const;

    std::string stmt_string () const;

    bool finalize ();
  protected:
    bool need_finalize () const;
  };
}
#endif // AUTO_SQL_STMT_H
