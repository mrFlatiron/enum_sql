#ifndef ENUM_SQL_TABLE_H
#define ENUM_SQL_TABLE_H

#include "containers/enum_bitset.h"
#include "containers/enum_vector.h"
#include "enum_sql_typedefs.h"

namespace enum_sql
{
  enum class sql_insert_policy
  {
    replace,
    rollback,
    ignore,
    abort,
    fail,
    COUNT
  };



  std::string enum_to_string (sql_insert_policy e)
  {
    switch (e)
      {
      case sql_insert_policy::replace:
        return "REPLACE";
      case sql_insert_policy::rollback:
        return "ROLLBACK";
      case sql_insert_policy::ignore:
        return "IGNORE";
      case sql_insert_policy::fail:
        return "FAIL";
      case sql_insert_policy::abort:
        return "ABORT";
      case sql_insert_policy::COUNT:
        ENUM_SQL_DEBUG_PAUSE ("Shouldn't happen");
        return "";
      }
    return "";
  }

  template<typename Enum, typename = use_if_enum<Enum>>
  class Table
  {
  private:

    enum_vector<Enum, std::string> m_col_names;
    enum_vector<Enum, sql_data_type> m_col_types;
    enum_vector<Enum, std::string> m_table_constr;
    std::string m_table_name;
    std::string m_schema_name;
    enum_bitset<Enum> m_enums_in_use;
  public:
    Table ()
    {
      m_enums_in_use.set (true);
    }

    ~Table () = default;

    sql_data_type column_type (Enum e)  const
    {
      return m_col_types[e];
    }

    void set_column_type (Enum e, sql_data_type type)
    {
      m_col_types[e] = type;
    }

    std::string column_name (Enum e)    const
    {
      return m_col_names[e];
    }

    void set_column_name (Enum e, std::string name)
    {
      m_col_names[e] = name;
    }

    std::string table_name ()           const
    {
      return m_table_name;
    }

    void set_table_name (std::string name)
    {
      m_table_name = name;
    }


    std::string table_constr (Enum e)   const
    {
      return m_table_constr[e];
    }

    void set_table_constr (Enum e, const std::string &constr)
    {
      m_table_constr[e] = constr;
    }

    std::string schema_name ()          const { return m_schema_name; }

    void set_schema_name (std::string name)
    {
      m_schema_name = name;
    }

    bool is_skipped (Enum e)            const { return !m_enums_in_use[e]; }

    void set_skipped (Enum e, bool val = true)
    {
      m_enums_in_use[e] = !val;
    }

    std::string full_table_name () const
    {
      std::string stmt;
      stmt.append (schema_name ());
      if (!schema_name ().empty ())
        stmt.append (".");
      stmt.append (table_name ());
      return stmt;
    }

    std::string full_column_def (Enum e) const
    {
      std::string def;
      def.append (column_name (e));
      def.append (" ");
      def.append (enum_to_string (column_type (e)));
      if (!table_constr (e).empty ())
        def.append (" ");
      def.append (table_constr (e));

      return def;
    }
  };
}

#endif // ENUM_SQL_TABLE_H
