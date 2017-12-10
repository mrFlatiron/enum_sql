#ifndef ENUM_SQL_INDEX_H
#define ENUM_SQL_INDEX_H

#include "enum_sql_table.h"

namespace enum_sql
{
  template<typename Enum, typename = use_if_enum<Enum>>
  class Index
  {
    const Table<Enum> *m_table = nullptr;
    std::vector<Enum> m_indexed_columns;
    std::string m_index_name;
    bool m_if_not_exist_kw = false;
    bool m_unique_kw = false;
  public:
    Index () = default;
    virtual ~Index () = default;

    explicit Index (const Table<Enum> *table)
    {
      m_table = table;
    }

    bool is_valid () const {return m_table && !m_index_name.empty () && m_indexed_columns.size ();}
  protected:
    virtual std::vector<Enum> indexed_columns () const {return m_indexed_columns;}
    virtual std::string index_name () const {return m_index_name;}

    virtual bool if_not_exist () const {return m_if_not_exist_kw;}
    virtual bool unique () const {return m_unique_kw;}
  public:
    void set_indexed_columns (const std::vector<Enum> &cols)
    {
      m_indexed_columns = cols;
    }

    void set_index_name (std::string name)
    {
      m_index_name = name;
    }

    void set_table (const Table<Enum> *table)
    {
      m_table = table;
    }

    void set_if_not_exist (bool val)
    {
      m_if_not_exist_kw = val;
    }

    void set_unique (bool val)
    {
      m_unique_kw = val;
    }

    std::string as_string () const
    {
      if (!is_valid ())
        return "";

      std::string stmt = "CREATE ";
      if (unique ())
        stmt.append ("UNIQUE ");

      stmt.append ("INDEX ");

      if (if_not_exist ())
        stmt.append ("IF NOT EXIST ");

      stmt.append (m_table->schema_name ());
      if (!m_table->schema_name ().empty ())
        stmt.append (".");

      stmt.append (index_name ());
      stmt.append (" ON ");
      stmt.append (m_table->table_name ());
      stmt.append (" (");

      bool is_first = true;

      for (auto col : indexed_columns ())
        {
          if (!is_first)
            stmt.append (", ");

          stmt.append (m_table->column_name (col));
          is_first = false;
        }

      stmt.append (") ");

      return stmt;
    }

    operator std::string () const
    {
      return as_string ();
    }
  };
}
#endif // ENUM_SQL_INDEX_H
