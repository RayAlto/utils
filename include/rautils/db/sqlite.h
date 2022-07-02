#ifndef RA_UTILS_RAUTILS_DB_SQLITE_H_
#define RA_UTILS_RAUTILS_DB_SQLITE_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "rautils/misc/status.h"

namespace rayalto::utils::db {

class Sqlite {
public:
    class Cursor;
    class Column;

    Sqlite();
    Sqlite(const Sqlite&) = delete;
    Sqlite(Sqlite&&) noexcept = default;
    Sqlite& operator=(const Sqlite&) = delete;
    Sqlite& operator=(Sqlite&&) noexcept = default;

    virtual ~Sqlite();

    Sqlite& connect(const std::string& uri, misc::Status& status);
    Sqlite& connect(std::string&& uri, misc::Status& status);

    [[nodiscard]] const std::string& uri() const;

    Cursor cursor();

protected:
    class SqliteImpl;
    std::unique_ptr<SqliteImpl> impl_;
};

class Sqlite::Cursor {
public:
    friend class Sqlite;

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

    [[nodiscard]] const std::string& statement() const;

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

    explicit Cursor(CursorImpl&& impl);
};

class Sqlite::Column {
public:
    friend class Sqlite::Cursor::CursorImpl;

    enum class Type : std::uint8_t {
        NULLTYPE,
        INTEGER,
        INTEGER64,
        REAL,
        TEXT,
        BLOB
    };

    [[nodiscard]] constexpr const char* type_str() const {
        return type_ == Type::NULLTYPE    ? "NULL"
               : type_ == Type::INTEGER   ? "INTEGER"
               : type_ == Type::INTEGER64 ? "INTEGER"
               : type_ == Type::REAL      ? "REAL"
               : type_ == Type::TEXT      ? "TEXT"
               : type_ == Type::BLOB      ? "BLOB"
                                          : "?";
    }

    Column() = delete;
    Column(const Column&) = delete;
    Column(Column&&) = default;
    Column& operator=(const Column&) = delete;
    Column& operator=(Column&&) = default;

    virtual ~Column() = default;

    [[nodiscard]] const Type& type() const;

    const std::unique_ptr<int>& integer();
    const std::unique_ptr<std::int64_t>& integer64();
    const std::unique_ptr<double>& real();
    const std::unique_ptr<std::string>& text();
    const std::unique_ptr<std::vector<unsigned char>>& blob();

    explicit operator int();
    explicit operator std::int64_t();
    explicit operator double();
    explicit operator std::string();
    explicit operator std::vector<unsigned char>();

protected:
    explicit Column(const int& data);
    explicit Column(const std::int64_t& data);
    explicit Column(const double& data);
    explicit Column(const std::string& data);
    explicit Column(std::string&& data);
    explicit Column(const std::vector<unsigned char>& data);
    explicit Column(std::vector<unsigned char>&& data);
    explicit Column(const std::nullptr_t&);

    Type type_;
    std::variant<std::unique_ptr<int>,
                 std::unique_ptr<std::int64_t>,
                 std::unique_ptr<double>,
                 std::unique_ptr<std::string>,
                 std::unique_ptr<std::vector<unsigned char>>>
        data_;
};

} // namespace rayalto::utils::db

#endif // RA_UTILS_RAUTILS_DB_SQLITE_H_
