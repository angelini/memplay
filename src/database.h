#ifndef _Database_h
#define _Database_h

#include <stdbool.h>

typedef enum col_t { BOOL, INT, STR } col_t;

typedef struct ColumnDef {
    char *key;
    col_t type;
} ColumnDef;

typedef struct RecordDef {
    char *name;
    int size;
    ColumnDef **column_ds;
} RecordDef;

typedef struct Record {
    int def_idx;
    int size;
    char **values;
} Record;

typedef struct Database {
    int def_size;
    RecordDef **record_ds;
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

ColumnDef *ColumnDef_new(char *key, col_t type);

void ColumnDef_destroy(ColumnDef *column_d);

RecordDef *RecordDef_new(char *name, ColumnDef **column_ds, int size);

void RecordDef_destroy(RecordDef *record_d);

Record *Record_new(int def_idx, char **values, int size);

void Record_destroy(Record *record);

Database *Database_new(RecordDef **record_ds, int size);

void Database_destroy(Database *db);

void Database_push(Database *db, Record *record);

void Database_print(Database *db);

#endif
