#ifndef ENUM_SQL_TYPEDEFS_H
#define ENUM_SQL_TYPEDEFS_H

#include <string>
#include "common/debug_utils.h"

namespace enum_sql
{
  enum class sql_data_type
  {
    INTEGER,
    REAL,
    TEXT,
    BLOB,
    Null,
    COUNT
  };

  static inline std::string enum_to_string (sql_data_type t)
  {
    switch (t)
      {
      case sql_data_type::Null:
        return "NULL";
      case sql_data_type::INTEGER:
        return "INTEGER";
      case sql_data_type::REAL:
        return "REAL";
      case sql_data_type::TEXT:
        return "TEXT";
      case sql_data_type::BLOB:
        return "BLOB";

      case sql_data_type::COUNT:
        DEBUG_PAUSE("Shouldn't happen");
        return "";
      }
    return "";
  }
}
#endif // ENUM_SQL_TYPEDEFS_H
