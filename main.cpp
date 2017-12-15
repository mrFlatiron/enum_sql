#include <iostream>

enum class test_enum
{
  id,
  title,
  desc,
  TEST_END
};

namespace enum_sql
{
  constexpr test_enum enum_end (test_enum) {return test_enum::TEST_END;}
}

#include "src/enum_sql.h"




int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

    enum_sql::Auto_sql db ("Table");

    if (!db.is_open ())
      {
        ENUM_SQL_DEBUG_PAUSE ("Failed to open");
        return 0;
      }

    enum_sql::Table<test_enum> table;

    table.set_table_name ("one");

    table.set_column_name (test_enum::id, "id");
    table.set_column_type (test_enum::id, enum_sql::sql_data_type::INTEGER);

    table.set_column_name (test_enum::desc, "desc");
    table.set_column_type (test_enum::desc, enum_sql::sql_data_type::TEXT);

    table.set_column_name (test_enum::title, "title");
    table.set_column_type (test_enum::title, enum_sql::sql_data_type::TEXT);

    auto create = enum_sql::Stmt_generator::create_stmt (table, &db);
    auto drop = enum_sql::Stmt_generator::drop_stmt (table, &db);
    auto sync = enum_sql::Stmt_generator::sync_pragma_stmt (table, false, &db);
    auto insert = enum_sql::Stmt_generator::insert_stmt (table, {test_enum::id, test_enum::title}, &db);
    auto select = enum_sql::Stmt_generator::select_stmt (table, {test_enum::title, test_enum::id}, "", "", &db);
    auto generic = enum_sql::Stmt_generator::generic_stmt ("DROP TABLE one;", &db);

    drop.prepare_execute ();
    create.prepare_execute ();
    sync.prepare_execute ();

    insert.prepare ();
    insert.bind_value (test_enum::id, 1);
    insert.bind_value (test_enum::title, "hello world");
    insert.execute ();

    select.prepare ();
    select.step ();

    auto id = boost::any_cast<int> (select.column (test_enum::id));

    auto title = boost::any_cast<std::string> (select.column (test_enum::title));


    std::cout << select.stmt_string () << " " << id << " " << title << std::endl;

    select.finalize ();

    generic.prepare_execute ();

  return 0;
}
