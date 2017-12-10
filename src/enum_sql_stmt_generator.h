#ifndef ENUM_SQL_STMT_GENERATOR_H
#define ENUM_SQL_STMT_GENERATOR_H

#include "enum_sql_insert_stmt.h"
#include "enum_sql_select_stmt.h"
#include "enum_sql_table.h"


namespace enum_sql
{
  class Stmt_generator
  {
  public:
    template<typename Enum, typename = use_if_enum<Enum>>
    static Auto_stmt create_stmt (const Table<Enum> &table, Auto_sql *db = nullptr)
    {
      std::string stmt = "CREATE TABLE ";
      stmt.append (table.full_table_name ());
      stmt.append (" (");

      bool is_first = true;
      for (auto e : enum_range<Enum> ())
        {
          if (table.is_skipped (e))
            continue;

          if (!is_first)
            {
              stmt.append (", ");
            }
          stmt.append (table.full_column_def (e));
          is_first = false;
        }

      stmt.append (")");

      Auto_stmt retval;
      retval.set_stmt_string (stmt);

      retval.set_db (db);
      return retval;
    }

    template<typename Enum, typename = use_if_enum<Enum>>
    static Auto_stmt drop_stmt (const Table<Enum> &table, Auto_sql *db = nullptr)
    {
      std::string stmt = "DROP TABLE ";
      stmt.append (table.full_table_name ());

      Auto_stmt retval;
      retval.set_stmt_string (stmt);
      retval.set_db (db);

      return retval;
    }

    template<typename Enum, typename = use_if_enum<Enum>>
    static Auto_stmt sync_pragma_stmt (const Table<Enum> &table, bool is_on, Auto_sql *db = nullptr)
    {
      std::string stmt = "PRAGMA ";
      stmt.append (table.schema_name ());
      if (!table.schema_name ().empty ())
        stmt.append (".");
      stmt.append ("synchronous ");
      stmt.append ("= ");
      if (is_on)
        stmt.append ("NORMAL");
      else
        stmt.append ("OFF");

      Auto_stmt retval;
      retval.set_stmt_string (stmt);
      retval.set_db (db);

      return retval;
    }

    template<typename Enum, typename = use_if_enum<Enum>>
    static Insert_stmt<Enum> insert_stmt (const Table<Enum> &table, const std::vector<Enum> &cols, Auto_sql *db = nullptr)
    {
      return Insert_stmt<Enum> (cols, &table, db);
    }

    template<typename Enum, typename = use_if_enum<Enum>>
    static Select_stmt<Enum> select_stmt (const Table<Enum> &table, const std::vector<Enum> &cols,
                                            std::string where_pred = "",
                                            std::string order_by = "",
                                            Auto_sql *db = nullptr)
    {
      return Select_stmt<Enum> (cols, &table, where_pred, order_by, db);
    }

    static Auto_stmt generic_stmt (const std::string &stmt_string, Auto_sql *db = nullptr)
    {
      Auto_stmt retval;

      retval.set_stmt_string (stmt_string);
      retval.set_db (db);

      return retval;
    }
  };
}


#endif // ENUM_SQL_STMT_GENERATOR_H
