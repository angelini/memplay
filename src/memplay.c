#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "dbg.h"
#include "database.h"

Database *Database_create_sample()
{
    ColumnDef *first_name = ColumnDef_new("first_name", STR);
    ColumnDef *last_name = ColumnDef_new("last_name", STR);
    ColumnDef *age = ColumnDef_new("age", INT);

    const ColumnDef **person_cols = malloc(sizeof(ColumnDef *) * 3);
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

Record *parse_line(char *line, const RecordDef *def, int def_idx)
{
    char *tok;
    int col_idx = 0;

    const void **values = malloc(sizeof(void *) * def->size);
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

    if (def_idx >= db->def_size) die("unknown def index: %d", def_idx);

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
