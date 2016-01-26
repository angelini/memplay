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

    ColumnDef **person_cols = calloc(3, sizeof(ColumnDef *));
    check_mem(person_cols);
    person_cols[0] = first_name;
    person_cols[1] = last_name;
    person_cols[2] = age;
    RecordDef *person = RecordDef_new("person", person_cols, 3);

    RecordDef **record_ds = calloc(1, sizeof(RecordDef *));
    check_mem(record_ds);
    record_ds[0] = person;
    Database *db = Database_new(record_ds, 1);

    return db;

error:
    if (person_cols) {
        free(person_cols);
        if (record_ds) free(record_ds);
    }
    return NULL;
}

Record *parse_line(char *line, const RecordDef *def, int def_idx)
{
    char *tok;
    int col_idx = 0;

    char **values = calloc(def->size, sizeof(char *));
    check_mem(values);

    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
        values[col_idx] = strdup(tok);
        col_idx += 1;
    }

    return Record_new(def_idx, values, def->size);

error:
    if (values) free(values);
    return NULL;
}

int main(int argc, char **argv)
{
    Database *db = Database_create_sample();

    check(argc == 3, "usage: %s <csvfile> <defindex>", argv[0]);

    char *csv_file = argv[1];
    int def_idx = atoi(argv[2]);

    check(def_idx < db->def_size, "unknown def index: %d", def_idx);

    FILE* stream = fopen(csv_file, "r");
    char line[1024];

    while (fgets(line, 1024, stream)) {
        char *tmp = strdup(line);
        Record *record = parse_line(tmp, db->record_ds[def_idx], def_idx);
        free(tmp);

        Database_push(db, record);
        Record_destroy(record);
    }

    Database_print(db);
    Database_destroy(db);

    return 0;

error:
    return 1;
}
