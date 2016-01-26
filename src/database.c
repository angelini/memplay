#include "dbg.h"
#include "database.h"

#define DEFAULT_COL_SIZE 256
#define DEFAULT_STR_COL_SIZE DEFAULT_COL_SIZE * 64

ColumnDef *ColumnDef_new(char *key, col_t type)
{
    ColumnDef *col = calloc(1, sizeof(ColumnDef));
    check_mem(col);

    col->key = key;
    col->type = type;

    return col;

error:
    return NULL;
}

void ColumnDef_destroy(ColumnDef *column_d)
{
    if (column_d) {
        if (column_d->key) free(column_d->key);
        free(column_d);
    }
}

RecordDef *RecordDef_new(char *name, ColumnDef **column_ds, int size)
{
    RecordDef *def = calloc(1, sizeof(RecordDef));
    check_mem(def);

    def->name = name;
    def->column_ds = column_ds;
    def->size = size;

    return def;

error:
    return NULL;
}

void RecordDef_destroy(RecordDef *record_d)
{
    if (record_d) {
        if (record_d->name) free(record_d->name);

        if (record_d->column_ds) {
            for (int i = 0; i < record_d->size; i++) {
                ColumnDef_destroy(record_d->column_ds[i]);
            }
            free(record_d->column_ds);
        }

        free(record_d);
    }
}

Record *Record_new(int def_idx, char **values, int size)
{
    Record *record = calloc(1, sizeof(Record));
    check_mem(record);

    record->def_idx = def_idx;
    record->values = values;
    record->size = size;

    return record;

error:
    return NULL;
}

void Record_destroy(Record *record)
{
    if (record) {
        if (record->values) {
            for (int i = 0; i < record->size; i++) {
                free(record->values[i]);
            }
        }

        free(record);
    }
}

Database *Database_new(RecordDef **record_ds, int size)
{
    Database *db = calloc(1, sizeof(Database));
    check_mem(db);

    db->record_ds = record_ds;
    db->def_size = size;

    db->bool_size = 0;
    db->int_size = 0;
    db->str_size = 0;

    for (int i = 0; i < size; i++) {
        RecordDef *record_d = record_ds[i];

        for (int j = 0; j < record_d->size; j++) {
            const ColumnDef *column_d = record_d->column_ds[j];

            switch (column_d->type) {
            case BOOL: db->bool_size += 1; break;
            case INT: db->int_size += 1; break;
            case STR: db->str_size += 1; break;
            }
        }
    }

    db->bool_idxs = calloc(db->bool_size, sizeof(int));
    db->bool_columns = calloc(db->bool_size, sizeof(bool *));
    check_mem(db->bool_idxs); check_mem(db->bool_columns);

    db->int_idxs = calloc(db->int_size, sizeof(int));
    db->int_columns = calloc(db->int_size, sizeof(int *));
    check_mem(db->int_idxs); check_mem(db->int_columns);

    db->str_idxs = calloc(db->str_size, sizeof(int));
    db->str_columns = calloc(db->str_size, sizeof(char *));
    check_mem(db->str_idxs); check_mem(db->str_columns);

    for (int i = 0; i < db->bool_size; i++) {
        db->bool_idxs[i] = 0;
        db->bool_columns[i] = calloc(DEFAULT_COL_SIZE, sizeof(bool));
        check_mem(db->bool_columns[i]);
    }

    for (int i = 0; i < db->int_size; i++) {
        db->int_idxs[i] = 0;
        db->int_columns[i] = calloc(DEFAULT_COL_SIZE, sizeof(int));
        check_mem(db->int_columns[i]);
    }


    for (int i = 0; i < db->str_size; i++) {
        db->str_idxs[i] = 0;
        db->str_columns[i] = calloc(DEFAULT_STR_COL_SIZE, sizeof(char));
        check_mem(db->str_columns[i]);
    }

    return db;

error:
    if (db) Database_destroy(db);
    return NULL;
}

void Database_destroy(Database *db)
{
    if (db) {
        if (db->record_ds) {
            for (int i = 0; i < db->def_size; i++) {
                RecordDef_destroy(db->record_ds[i]);
            }
        }

        if (db->bool_idxs) free(db->bool_idxs);
        if (db->bool_columns) {
            for (int i = 0; i < db->bool_size; i++) {
                free(db->bool_columns[i]);
            }
            free(db->bool_columns);
        }

        if (db->int_idxs) free(db->int_idxs);
        if (db->int_columns) {
            for (int i = 0; i < db->int_size; i++) {
                free(db->int_columns[i]);
            }
            free(db->int_columns);
        }

        if (db->str_idxs) free(db->str_idxs);
        if (db->str_columns) {
            for (int i = 0; i < db->str_size; i++) {
                free(db->str_columns[i]);
            }
            free(db->str_columns);
        }

        free(db);
    }
}

void Database_push(Database *db, Record *record)
{
    int def_idx = record->def_idx;
    RecordDef *def = db->record_ds[def_idx];

    int bool_cur = 0, int_cur = 0, str_cur = 0;

    for (int i = 0; i < def->size; i++) {
        const ColumnDef *column_d = def->column_ds[i];

        if (column_d->type == BOOL) {
            int bool_idx = db->bool_idxs[bool_cur];
            bool val = (bool) atoi(record->values[i]);
            db->bool_columns[bool_cur][bool_idx] = val;
            db->bool_idxs[bool_cur] += 1;
            bool_cur += 1;

        } else if (column_d->type == INT) {
            int int_idx = db->int_idxs[int_cur];
            int val = atoi(record->values[i]);
            db->int_columns[int_cur][int_idx] = val;
            db->int_idxs[int_cur] += 1;
            int_cur += 1;

        } else {
            int str_idx = db->str_idxs[str_cur];
            char *val = strdup(record->values[i]);
            strcpy(&db->str_columns[str_cur][str_idx], val);
            db->str_idxs[str_cur] += strlen(val);
            str_cur += 1;
        }
    }

    db->size += 1;
}

void Database_print(Database *db)
{
    for (int i = 0; i < db->bool_size; i++) {
        for (int j = 0; j < db->bool_idxs[i]; j++) {
            debug("bools[%d][%d]: %d", i, j, db->bool_columns[i][j]);
        }
    }

    for (int i = 0; i < db->int_size; i++) {
        for (int j = 0; j < db->int_idxs[i]; j++) {
            debug("ints[%d][%d]: %d", i, j, db->int_columns[i][j]);
        }
    }

    for (int i = 0; i < db->str_size; i++) {
        for (int j = 0; j < db->str_idxs[i]; j++) {
            debug("strs[%d][%d]: %c", i, j, db->str_columns[i][j]);
        }
    }
}
