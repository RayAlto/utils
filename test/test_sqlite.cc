#include <iostream>

#include "rautils/db/sqlite.h"
#include "rautils/misc/status.h"

using rayalto::utils::db::Sqlite;
using rayalto::utils::misc::Status;

void shit_happens(const Status& status) {
    if (status) {
        std::cout << static_cast<std::string>(status) << std::endl;
        std::exit(1);
    }
}

int main(int argc, char const* argv[]) {
    Sqlite database;
    Status status;
    database.connect("./test/test.db", status);
    shit_happens(status);
    Sqlite::Cursor cursor = database.cursor();
    cursor.prepare("SELECT * FROM Friends WHERE Sex IS ?;", status);
    shit_happens(status);
    cursor.bind(1, "F", status);
    shit_happens(status);
    cursor.execute(status);
    shit_happens(status);
    std::cout << "Total column: " << cursor.column_count() << std::endl;
    while (cursor.has_next_row()) {
        std::cout << *cursor.column(0).integer() << ". "
                  << *cursor.column(1).text() << "(" << *cursor.column(2).text()
                  << ")" << std::endl;
        cursor.advance(status);
        shit_happens(status);
    }
    return 0;
}
