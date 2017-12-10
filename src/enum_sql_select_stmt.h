#ifndef ENUM_SQL_SELECT_STMT_H
#define ENUM_SQL_SELECT_STMT_H

#include "containers/enum_vector.h"
#include "auto_stmt.h"
#include "sqlite/sqlite3.h"
#include "enum_sql_table.h"

#include <boost/any.hpp>

namespace enum_sql
{

  using boost::any;
  using boost::any_cast;

  enum class step_status
  {
    ROW,
    DONE,
    FAIL,
    COUNT
  };

  template<typename Enum, typename = use_if_enum<Enum>>
  class Select_stmt : public Auto_stmt
  {
  private:
    using self = Select_stmt<Enum>;
    using base = Auto_stmt;

    std::vector<Enum> m_cols;
    enum_vector<Enum, int> m_enum_to_int;

    std::string m_where_predicate;
    std::string m_order_by_predicate;

    const Table<Enum> *m_table = nullptr;

    step_status m_step_status = step_status::FAIL;
  public:
    Select_stmt () = default;
    ~Select_stmt () = default;

    Select_stmt (Select_stmt<Enum> &&rhs)
      : Auto_stmt (std::move (rhs))
    {
      m_cols = std::move (rhs.m_cols);
      m_enum_to_int = std::move (rhs.m_enum_to_int);
      m_table = rhs.m_table;
    }

    Select_stmt<Enum> &operator= (Select_stmt<Enum> &&rhs)
    {
      Auto_stmt::operator = (std::move (rhs));

      m_cols = std::move (rhs.m_cols);
      m_enum_to_int = std::move (rhs.m_enum_to_int);
      m_table = rhs.m_table;
    }

    Select_stmt (const std::vector<Enum> &cols, const Table<Enum> *table,
                 const std::string &where_predicate, const std::string &order_by_predicate,
                 Auto_sql *db = nullptr)
      : Auto_stmt ()
    {
      set_cols (cols);
      set_table (table);
      set_where_private (where_predicate);
      set_order_by_private (order_by_predicate);
      set_stmt_string (construct_string_stmt ());
      set_db (db);
    }

    void set_cols (const std::vector<Enum> &cols)
    {
      m_cols = cols;
      int size = static_cast<int> (cols.size ());
      for (int i = 0; i < size; i++)
        m_enum_to_int[cols[i]] = i;
    }

    void set_table (const Table<Enum> *table)
    {
      m_table = table;
    }

    bool execute () override
    {
      if (step_status::FAIL != step ())
        return true;
      else
        return false;
    }

    step_status step ()
    {
      if (!is_prepared ())
        return step_status::FAIL;

      m_status = sqlite3_step (m_stmt);

      m_step_status = sqlite_to_enum_step_status (m_status);

      return m_step_status;
    }

    step_status prepare_step ()
    {
      if (!is_prepared ())
        prepare ();

      return step ();
    }

    any column (Enum e)
    {
      auto type = m_table->column_type (e);
      switch (type)
        {
        case sql_data_type::BLOB:
          {
            auto p = pos (e);
            auto size = sqlite3_column_bytes (m_stmt, p);
            const void *data_c = sqlite3_column_blob (m_stmt, p);
            std::vector<uint8_t> data_cpp (size);
            for (int i = 0; i < size; i++)
              data_cpp[i] = static_cast<const uint8_t *> (data_c)[i];
            return any (data_cpp);
          }
        case sql_data_type::TEXT:
          {
            auto p = pos (e);
            auto size = sqlite3_column_bytes (m_stmt, p);
            const unsigned char *data_c = sqlite3_column_text (m_stmt, p);
            std::string data_cpp;
            for (int i = 0; i < size; i++)
              data_cpp.append (1, static_cast<char> (data_c[i]));
            return any (data_cpp);
          }
        case sql_data_type::REAL:
          {
            auto p = pos (e);
            auto val = sqlite3_column_double (m_stmt, p);
            return any (val);
          }
        case sql_data_type::INTEGER:
          {
            auto p = pos (e);
            auto val = sqlite3_column_int (m_stmt, p);
            return any (val);
          }
        case sql_data_type::Null:
          return any ();
        case sql_data_type::COUNT:
          DEBUG_PAUSE ("Shouldn't happen");
          return any ();
        }
      return any ();
    }

    step_status get_step_status () const
    {
      return m_step_status;
    }

    void set_where_predicate (const std::string &where_pr)
    {
      m_where_predicate = where_pr;
      set_stmt_string (construct_string_stmt ());
    }

    void set_order_by_predicate (const std::string &order_by)
    {
      m_order_by_predicate = order_by;
      set_stmt_string (construct_string_stmt ());
    }

  private:
    static step_status sqlite_to_enum_step_status (int status)
    {
      switch (status)
        {
        case SQLITE_DONE:
          return step_status::DONE;
        case SQLITE_ROW:
          return step_status::ROW;
        case SQLITE_OK:
          DEBUG_PAUSE ("Shouldn't happen");
          return step_status::DONE;
        }
      return step_status::FAIL;
    }

    int pos (Enum e) const
    {
      return m_enum_to_int[e];
    }

    std::string construct_string_stmt () const
    {
      if (!m_table)
        return "";

      std::string cols_to_select;

      bool is_first = true;
      for (Enum e : m_cols)
        {
          if (!is_first)
            {
              cols_to_select.append (", ");
            }
          cols_to_select.append (m_table->column_name (e));
          is_first = false;
        }
      std::string stmt = "SELECT ";
      stmt.append (cols_to_select);
      stmt.append (" ");
      stmt.append ("FROM ");
      stmt.append (m_table->full_table_name ());

      if (!m_where_predicate.empty ())
        {
          stmt.append (" WHERE ");
          stmt.append (m_where_predicate);
        }

      if (!m_order_by_predicate.empty ())
        {
          stmt.append (" ORDER BY ");
          stmt.append (m_order_by_predicate);
        }

      return stmt;
    }

    void set_where_private (const std::string &where_pr)
    {
      m_where_predicate = where_pr;
    }

    void set_order_by_private (const std::string &order_by)
    {
      m_order_by_predicate = order_by;
    }
  };

}
#endif // ENUM_SQL_SELECT_STMT_H
