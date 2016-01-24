#include "dbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define DEFAULT_COL_SIZE 256
#define DEFAULT_STR_COL_SIZE DEFAULT_COL_SIZE * 64

typedef enum col_t { BOOL, INT, STR } col_t;

typedef struct ColumnDef {
    char *key;
    col_t type;
} ColumnDef;

typedef struct RecordDef {
    char *name;
    ColumnDef **column_ds;
    int size;
} RecordDef;

typedef struct Record {
    int def_idx;
    void **values;
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

ColumnDef *ColumnDef_new(char *key, col_t type)
{
    ColumnDef *col = malloc(sizeof(ColumnDef));
    assert(col != NULL);

    col->key = key;
    col->type = type;

    return col;
}

RecordDef *RecordDef_new(char *name, ColumnDef **column_ds, int size)
{
    RecordDef *def = malloc(sizeof(RecordDef));
    assert(def != NULL);

    def->name = name;
    def->column_ds = column_ds;
    def->size = size;

    return def;
}

Record *Record_new(int def_idx, void **values)
{
    Record *record = malloc(sizeof(Record));
    assert(record != NULL);

    record->def_idx = def_idx;
    record->values = values;

    return record;
}

Database *Database_new(RecordDef **record_ds, int size)
{
    Database *db = malloc(sizeof(Database));
    assert(db != NULL);

    db->record_ds = record_ds;
    db->def_size = size;

    db->bool_size = 0;
    db->int_size = 0;
    db->str_size = 0;

    for (int i = 0; i < size; i++) {
        RecordDef *record_d = record_ds[i];

        for (int j = 0; j < record_d->size; j++) {
            ColumnDef *column_d = record_d->column_ds[j];

            switch (column_d->type) {
            case BOOL: db->bool_size += 1; break;
            case INT: db->int_size += 1; break;
            case STR: db->str_size += 1; break;
            }
        }
    }

    db->bool_idxs = malloc(sizeof(int) * db->bool_size);
    db->bool_columns = malloc(sizeof(bool *) * db->bool_size);
    assert(db->bool_idxs != NULL); assert(db->bool_columns != NULL);

    db->int_idxs = malloc(sizeof(int) * db->int_size);
    db->int_columns = malloc(sizeof(int *) * db->int_size);
    assert(db->int_idxs != NULL); assert(db->int_columns != NULL);

    db->str_idxs = malloc(sizeof(int) * db->str_size);
    db->str_columns = malloc(sizeof(char *) * db->str_size);
    assert(db->str_idxs != NULL); assert(db->str_columns != NULL);

    for (int i = 0; i < db->bool_size; i++) {
        db->bool_idxs[i] = 0;
        db->bool_columns[i] = malloc(sizeof(bool) * DEFAULT_COL_SIZE);
        assert(db->bool_columns[i] != NULL);
    }

    for (int i = 0; i < db->int_size; i++) {
        db->int_idxs[i] = 0;
        db->int_columns[i] = malloc(sizeof(int) * DEFAULT_COL_SIZE);
        assert(db->int_columns[i] != NULL);
    }


    for (int i = 0; i < db->str_size; i++) {
        db->str_idxs[i] = 0;
        db->str_columns[i] = malloc(sizeof(char) * DEFAULT_STR_COL_SIZE);
        assert(db->str_columns[i] != NULL);
    }

    return db;
}

void Database_push(Database *db, Record *record)
{
    int def_idx = record->def_idx;
    RecordDef *def = db->record_ds[def_idx];

    int bool_cur = 0, int_cur = 0, str_cur = 0;

    for (int i = 0; i < def->size; i++) {
        ColumnDef *column_d = def->column_ds[i];

        if (column_d->type == BOOL) {
            int bool_idx = db->bool_idxs[bool_cur];
            bool val = (bool) record->values[i];
            db->bool_columns[bool_cur][bool_idx] = val;
            db->bool_idxs[bool_cur] += 1;
            bool_cur += 1;

        } else if (column_d->type == INT) {
            int int_idx = db->int_idxs[int_cur];
            int val = (int) record->values[i];
            db->int_columns[int_cur][int_idx] = val;
            db->int_idxs[int_cur] += 1;
            int_cur += 1;

        } else {
            int str_idx = db->str_idxs[str_cur];
            char *val = (char*) record->values[i];
            strcpy(&db->str_columns[str_cur][str_idx], val);
            db->str_idxs[str_cur] += strlen(val);
            str_cur += 1;
        }
    }

    db->size += 1;
}

Database *Database_create_sample()
{
    ColumnDef *first_name = ColumnDef_new("first_name", STR);
    ColumnDef *last_name = ColumnDef_new("last_name", STR);
    ColumnDef *age = ColumnDef_new("age", INT);

    ColumnDef **person_cols = malloc(sizeof(ColumnDef *) * 3);
    assert(person_cols != NULL);
    person_cols[0] = first_name;
    person_cols[1] = last_name;
    person_cols[2] = age;
    RecordDef *person = RecordDef_new("person", person_cols, 3);

    RecordDef **record_ds = malloc(sizeof(RecordDef *));
    assert(record_ds != NULL);
    record_ds[0] = person;
    Database *db = Database_new(record_ds, 1);

    return db;
}

Record *parse_line(char *line, RecordDef *def, int def_idx)
{
    char *tok;
    int col_idx = 0;

    void **values = malloc(sizeof(void *) * def->size);
    assert(values != NULL);

    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
        void *value;

        switch (def->column_ds[col_idx]->type) {
        case BOOL:
            value = (void *) (long) atoi(tok);
            break;
        case INT:
            value = (void *) (long) atoi(tok);
            break;
        case STR:
            value = strdup(tok);
            break;
        }

        values[col_idx] = value;
        col_idx += 1;
    }

    return Record_new(def_idx, values);
}

int main(int argc, char **argv)
{
    Database *db = Database_create_sample();

    if (argc < 3) die("usage: %s <csvfile> <defindex>", argv[0]);

    char *csv_file = argv[1];
    int def_idx = atoi(argv[2]);

    FILE* stream = fopen(csv_file, "r");

    char line[1024];
    while (fgets(line, 1024, stream)) {
        char *tmp = strdup(line);
        Record *row = parse_line(tmp, db->record_ds[def_idx], def_idx);
        free(tmp);

        Database_push(db, row);
    }

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

    return 0;
}
