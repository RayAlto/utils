#ifndef RA_UTILS_RAUTILS_DB_SQLITE_H_
#define RA_UTILS_RAUTILS_DB_SQLITE_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "rautils/misc/status.h"

namespace rayalto {
namespace utils {
namespace db {

class Sqlite {
public:
    class Cursor;
    class Column;

public:
    Sqlite();
    Sqlite(const Sqlite&) = delete;
    Sqlite(Sqlite&&) noexcept = default;
    Sqlite& operator=(const Sqlite&) = delete;
    Sqlite& operator=(Sqlite&&) noexcept = default;

    virtual ~Sqlite();

    Sqlite& connect(const std::string& uri, misc::Status& status);
    Sqlite& connect(std::string&& uri, misc::Status& status);

    const std::string& uri() const;

    Cursor cursor();

protected:
    class SqliteImpl;
    std::unique_ptr<SqliteImpl> impl_;
};

class Sqlite::Cursor {
public:
    friend class Sqlite;

public:
    Cursor(const Cursor&) = delete;
    Cursor(Cursor&&) noexcept = default;
    Cursor& operator=(const Cursor&) = delete;
    Cursor& operator=(Cursor&&) noexcept = default;

    virtual ~Cursor();

    Cursor& execute(const std::string& statement, misc::Status& status);
    Cursor& execute(std::string&& statement, misc::Status& status);
    Cursor& execute(misc::Status& status);

    Cursor& prepare(const std::string& statement, misc::Status& status);
    Cursor& prepare(std::string&& statement, misc::Status& status);

    const std::string& statement() const;

    // bind blob
    Cursor& bind(const int& index,
                 const std::vector<unsigned char>& blob,
                 misc::Status& status);
    // bind real
    Cursor& bind(const int& index, const double& real, misc::Status& status);
    // bind integer
    Cursor& bind(const int& index, const int& integer, misc::Status& status);
    Cursor& bind(const int& index,
                 const std::int64_t& integer,
                 misc::Status& status);
    // bind null
    Cursor& bind(const int& index, const std::nullptr_t&, misc::Status& status);
    // bind text
    Cursor& bind(const int& index,
                 const std::string& text,
                 misc::Status& status);

    bool has_next_row();
    int column_count();
    Cursor& advance(misc::Status& status);
    Column column(const int& index);

protected:
    class CursorImpl;
    std::unique_ptr<CursorImpl> impl_;

protected:
    Cursor(CursorImpl&& impl);
};

class Sqlite::Column {
public:
    friend class Sqlite::Cursor::CursorImpl;

public:
    enum class Type : std::uint8_t {
        NULLTYPE,
        INTEGER,
        INTEGER64,
        REAL,
        TEXT,
        BLOB
    };

    constexpr const char* type_str() const {
        return type_ == Type::NULLTYPE    ? "NULL"
               : type_ == Type::INTEGER   ? "INTEGER"
               : type_ == Type::INTEGER64 ? "INTEGER"
               : type_ == Type::REAL      ? "REAL"
               : type_ == Type::TEXT      ? "TEXT"
               : type_ == Type::BLOB      ? "BLOB"
                                          : "?";
    }

public:
    Column() = delete;
    Column(const Column&) = delete;
    Column(Column&&) = default;
    Column& operator=(const Column&) = delete;
    Column& operator=(Column&&) = default;

    const Type& type() const;

    const std::unique_ptr<int>& integer();
    const std::unique_ptr<std::int64_t>& integer64();
    const std::unique_ptr<double>& real();
    const std::unique_ptr<std::string>& text();
    const std::unique_ptr<std::vector<unsigned char>>& blob();

    operator int();
    operator std::int64_t();
    operator double();
    operator std::string();
    operator std::vector<unsigned char>();

protected:
    Column(const int& data);
    Column(int&& data);
    Column(const std::int64_t& data);
    Column(std::int64_t&& data);
    Column(const double& data);
    Column(double&& data);
    Column(const std::string& data);
    Column(std::string&& data);
    Column(const std::vector<unsigned char>& data);
    Column(std::vector<unsigned char>&& data);
    Column(const std::nullptr_t&);
    Column(std::nullptr_t&&);

protected:
    Type type_;
    std::variant<std::unique_ptr<int>,
                 std::unique_ptr<std::int64_t>,
                 std::unique_ptr<double>,
                 std::unique_ptr<std::string>,
                 std::unique_ptr<std::vector<unsigned char>>>
        data_;
};

} // namespace db
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_DB_SQLITE_H_
