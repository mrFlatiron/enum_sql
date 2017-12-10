TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    src/auto_sql.cpp \
    src/auto_sql_stmt.cpp \
    sqlite/sqlite3.c

HEADERS += \
    sqlite/sqlite3.h \
    sqlite/sqlite3ext.h \
    src/auto_sql.h \
    src/enum_sql.h \
    src/enum_sql_index.h \
    src/enum_sql_insert_stmt.h \
    src/enum_sql_select_stmt.h \
    src/enum_sql_stmt_generator.h \
    src/enum_sql_table.h \
    src/enum_sql_typedefs.h \
    src/common/enum_helper.h \
    src/common/enum_misc.h \
    src/common/enum_range.h \
    src/containers/enum_bitset.h \
    src/containers/enum_vector.h \
    src/common/debug_utils.h \
    src/auto_stmt.h

LIBS += -lpthread -ldl
