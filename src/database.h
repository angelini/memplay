#ifndef _Database_h
#define _Database_h

#include <stdbool.h>

typedef enum col_t { BOOL, INT, STR } col_t;

typedef struct ColumnDef {
    const char *key;
    col_t type;
} ColumnDef;

typedef struct RecordDef {
    const char *name;
    const ColumnDef **column_ds;
    int size;
} RecordDef;

typedef struct Record {
    int def_idx;
    const void **values;
} Record;

typedef struct Database {
    RecordDef **record_ds;
    int def_size;
    int size;

    int bool_size;
    int *bool_idxs;
    bool **bool_columns;

    int int_size;
    int *int_idxs;
    int **int_columns;

    int str_size;
    int *str_idxs;
    char **str_columns;
} Database;

ColumnDef *ColumnDef_new(const char *key, col_t type);

RecordDef *RecordDef_new(const char *name, const ColumnDef **column_ds, int size);

Record *Record_new(int def_idx, const void **values);

Database *Database_new(RecordDef **record_ds, int size);

void Database_push(Database *db, const Record *record);

#endif
