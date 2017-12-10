#ifndef ENUM_SQL_INSERT_STMT_H
#define ENUM_SQL_INSERT_STMT_H

#include "auto_stmt.h"
#include "common/enum_misc.h"
#include "containers/enum_vector.h"
#include "sqlite/sqlite3.h"
#include "enum_sql_table.h"

namespace enum_sql
{
  template<typename Enum, typename = use_if_enum<Enum>>
  class Insert_stmt : public Auto_stmt
  {
  private:
    using self = Insert_stmt<Enum>;
    using base = Auto_stmt;

    std::vector<Enum> m_cols;
    enum_vector<Enum, int> m_enum_to_int;

    const Table<Enum> *m_table;
    sql_insert_policy m_policy = sql_insert_policy::replace;
  public:
    explicit Insert_stmt (const std::vector<Enum> &cols, const Table<Enum> *table, Auto_sql *db = nullptr)
      : Auto_stmt ()
    {
      set_cols (cols);
      set_table (table);
      set_stmt_string (construct_string_stmt ());
      set_db (db);
    }

    void set_cols (const std::vector<Enum> &cols)
    {
      m_cols = cols;

      int size = static_cast<int> (cols.size ());
      for (int i = 0; i < size; i++)
        m_enum_to_int[cols[i]] = i + 1;
    }

    void set_table (const Table<Enum> *table)
    {
      m_table = table;
    }

    void set_policy (sql_insert_policy policy)
    {
      m_policy = policy;
    }

    bool bind_value (Enum e, const std::vector<uint8_t> &bytes)
    {
      int p = pos (e);
      m_status = sqlite3_bind_blob (m_stmt, p, static_cast<void *> (bytes.data ()), static_cast<int> (bytes.size ()), SQLITE_TRANSIENT);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

    bool bind_value (Enum e, int value)
    {
      int p = pos (e);
      m_status = sqlite3_bind_int (m_stmt, p, value);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

    bool bind_value (Enum e, int64_t value)
    {
      int p = pos (e);
      m_status = sqlite3_bind_int64 (m_stmt, p, value);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

    bool bind_value (Enum e, double value)
    {
      int p = pos (e);
      m_status = sqlite3_bind_double (m_stmt, p, value);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

    bool bind_value (Enum e, const std::string &text)
    {
      int p = pos (e);
      m_status = sqlite3_bind_text (m_stmt, p, text.c_str (), text.length (), SQLITE_TRANSIENT);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

    bool bind_value (Enum e, const  std::nullptr_t)
    {
      int p = pos (e);
      m_status = sqlite3_bind_null (m_stmt, p);
      if (m_status == SQLITE_OK)
        return true;
      else
        return false;
    }

  private:
    int pos (Enum e) const
    {
      return m_enum_to_int[e];
    }

    std::string construct_string_stmt () const
    {
      if (!m_table)
        return "";

      std::string stmt = "INSERT OR ";
      stmt.append (enum_to_string (m_policy));
      stmt.append (" INTO ");
      stmt.append (m_table->full_table_name ());
      stmt.append (" (");

      std::string q_marks = "(";
      bool is_first = true;
      for (auto col : m_cols)
        {
          if (!is_first)
            {
              stmt.append (", ");
              q_marks.append (", ");
            }
          stmt.append (m_table->column_name (col));
          q_marks.append ("?");
          is_first = false;
        }
      stmt.append (") VALUES ");
      q_marks.append (")");
      stmt.append (q_marks);

      return stmt;
    }
  };
}
#endif // ENUM_SQL_INSERT_STMT_H
