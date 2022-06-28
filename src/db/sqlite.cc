#include "rautils/db/sqlite.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "rautils/misc/status.h"

namespace rayalto {
namespace utils {
namespace db {

/* ===== Sqlite::Impl ===== */

class Sqlite::SqliteImpl {
public:
    friend class Sqlite;

public:
    SqliteImpl() = default;
    SqliteImpl(const SqliteImpl&) = delete;
    SqliteImpl(SqliteImpl&& old) noexcept;
    SqliteImpl& operator=(const SqliteImpl&) = delete;
    SqliteImpl& operator=(SqliteImpl&& old) noexcept;

    virtual ~SqliteImpl();

    void connect(const std::string& uri, misc::Status& status);
    void connect(std::string&& uri, misc::Status& status);
    void connect(misc::Status& status);

    const std::string& uri() const;

protected:
    sqlite3* sqlite3_ = nullptr;

    std::string uri_;

protected:
    bool check_sqlite_ok(const int& return_code, misc::Status& status);
};

Sqlite::SqliteImpl::SqliteImpl(SqliteImpl&& old) noexcept :
    sqlite3_(old.sqlite3_) {
    old.sqlite3_ = nullptr;
}

Sqlite::SqliteImpl& Sqlite::SqliteImpl::operator=(
    SqliteImpl&& sqlite_impl) noexcept {
    if (this == &sqlite_impl) {
        return *this;
    }
    sqlite3_ = sqlite_impl.sqlite3_;
    sqlite_impl.sqlite3_ = nullptr;
    return *this;
}

Sqlite::SqliteImpl::~SqliteImpl() {
    if (sqlite3_ != nullptr) {
        sqlite3_close_v2(sqlite3_);
        sqlite3_ = nullptr;
    }
}

void Sqlite::SqliteImpl::connect(const std::string& uri, misc::Status& status) {
    uri_ = uri;
    connect(status);
}

void Sqlite::SqliteImpl::connect(std::string&& uri, misc::Status& status) {
    uri_ = std::move(uri);
    connect(status);
}

void Sqlite::SqliteImpl::connect(misc::Status& status) {
    check_sqlite_ok(sqlite3_open(uri_.c_str(), &sqlite3_), status);
}

const std::string& Sqlite::SqliteImpl::uri() const {
    return uri_;
}

bool Sqlite::SqliteImpl::check_sqlite_ok(const int& return_code,
                                         misc::Status& status) {
    if (return_code == SQLITE_OK) {
        return true;
    }
    status.error = true;
    status.message = std::make_unique<std::string>(sqlite3_errmsg(sqlite3_));
    sqlite3_close_v2(sqlite3_);
    sqlite3_ = nullptr;
    return false;
}

/* ===== Sqlite::Cursor::Impl ===== */

class Sqlite::Cursor::CursorImpl {
public:
    friend class Sqlite;

public:
    CursorImpl() = delete;
    CursorImpl(const CursorImpl&) = delete;
    CursorImpl(CursorImpl&& old) noexcept;
    CursorImpl& operator=(const CursorImpl&) = delete;
    CursorImpl& operator=(CursorImpl&& old) noexcept;

    CursorImpl(sqlite3* sqlite3_ptr);

    virtual ~CursorImpl();

    void execute(const std::string& statement, misc::Status& status);
    void execute(std::string&& statement, misc::Status& status);
    void execute(misc::Status& status);

    void prepare(const std::string& statement, misc::Status& status);
    void prepare(std::string&& statement, misc::Status& status);
    bool prepare(misc::Status& status);

    const std::string& statement() const;

    // bind blob
    void bind(const int& index,
              const std::vector<unsigned char>& blob,
              misc::Status& status);
    // bind real
    void bind(const int& index, const double& real, misc::Status& status);
    // bind integer
    void bind(const int& index, const int& integer, misc::Status& status);
    void bind(const int& index,
              const std::int64_t& integer,
              misc::Status& status);
    // bind null
    void bind(const int& index, const std::nullptr_t&, misc::Status& status);
    // bind text
    void bind(const int& index, const std::string& text, misc::Status& status);

    bool has_next_row();
    int column_count();
    void advance(misc::Status& status);
    Column column(const int& index);

protected:
    sqlite3_stmt* sqlite3_stmt_ = nullptr;
    sqlite3* sqlite3_ = nullptr;

    std::string statement_;
    bool has_next_row_;

protected:
    bool check_sqlite_ok(const int& return_code, misc::Status& status);
    bool check_sqlite_row(const int& return_code, misc::Status& status);
    bool check_sqlite_etc(const int& return_code, misc::Status& status);
};

Sqlite::Cursor::CursorImpl::CursorImpl(CursorImpl&& old) noexcept :
    sqlite3_(old.sqlite3_), sqlite3_stmt_(old.sqlite3_stmt_) {
    old.sqlite3_ = nullptr;
    old.sqlite3_stmt_ = nullptr;
}

Sqlite::Cursor::CursorImpl& Sqlite::Cursor::CursorImpl::operator=(
    CursorImpl&& cursor_impl) noexcept {
    if (this == &cursor_impl) {
        return *this;
    }
    sqlite3_ = cursor_impl.sqlite3_;
    sqlite3_stmt_ = cursor_impl.sqlite3_stmt_;
    cursor_impl.sqlite3_ = nullptr;
    cursor_impl.sqlite3_stmt_ = nullptr;
    return *this;
}

Sqlite::Cursor::CursorImpl::CursorImpl(sqlite3* sqlite3_ptr) :
    sqlite3_(sqlite3_ptr) {}

Sqlite::Cursor::CursorImpl::~CursorImpl() {
    if (sqlite3_stmt_ != nullptr) {
        sqlite3_finalize(sqlite3_stmt_);
        sqlite3_stmt_ = nullptr;
    }
}

void Sqlite::Cursor::CursorImpl::execute(const std::string& statement,
                                         misc::Status& status) {
    statement_ = statement;
    execute(status);
}

void Sqlite::Cursor::CursorImpl::execute(std::string&& statement,
                                         misc::Status& status) {
    statement_ = std::move(statement);
    execute(status);
}

void Sqlite::Cursor::CursorImpl::execute(misc::Status& status) {
    if (!prepare(status)) {
        return;
    }
    if (!check_sqlite_row(sqlite3_step(sqlite3_stmt_), status)) {
        return;
    }
    has_next_row_ = true;
}

void Sqlite::Cursor::CursorImpl::prepare(const std::string& statement,
                                         misc::Status& status) {
    statement_ = statement;
    prepare(status);
}

void Sqlite::Cursor::CursorImpl::prepare(std::string&& statement,
                                         misc::Status& status) {
    statement_ = std::move(statement);
    prepare(status);
}

bool Sqlite::Cursor::CursorImpl::prepare(misc::Status& status) {
    if (sqlite3_stmt_ != nullptr) {
        return true;
    }
    if (!check_sqlite_ok(sqlite3_prepare_v2(sqlite3_,
                                            statement_.c_str(),
                                            statement_.length(),
                                            &sqlite3_stmt_,
                                            nullptr),
                         status)) {
        return false;
    }
    return true;
}

const std::string& Sqlite::Cursor::CursorImpl::statement() const {
    return statement_;
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const std::vector<unsigned char>& blob,
                                      misc::Status& status) {
    check_sqlite_etc(
        sqlite3_bind_blob(
            sqlite3_stmt_, index, blob.data(), blob.size(), SQLITE_TRANSIENT),
        status);
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const double& real,
                                      misc::Status& status) {
    check_sqlite_etc(sqlite3_bind_double(sqlite3_stmt_, index, real), status);
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const int& integer,
                                      misc::Status& status) {
    check_sqlite_etc(sqlite3_bind_int(sqlite3_stmt_, index, integer), status);
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const std::int64_t& integer,
                                      misc::Status& status) {
    check_sqlite_etc(sqlite3_bind_int64(sqlite3_stmt_, index, integer), status);
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const std::nullptr_t&,
                                      misc::Status& status) {
    check_sqlite_etc(sqlite3_bind_null(sqlite3_stmt_, index), status);
}

void Sqlite::Cursor::CursorImpl::bind(const int& index,
                                      const std::string& text,
                                      misc::Status& status) {
    check_sqlite_etc(sqlite3_bind_text(sqlite3_stmt_,
                                       index,
                                       text.c_str(),
                                       text.length(),
                                       SQLITE_TRANSIENT),
                     status);
}

bool Sqlite::Cursor::CursorImpl::has_next_row() {
    return has_next_row_;
}

int Sqlite::Cursor::CursorImpl::column_count() {
    return sqlite3_column_count(sqlite3_stmt_);
}

void Sqlite::Cursor::CursorImpl::advance(misc::Status& status) {
    if (!check_sqlite_row(sqlite3_step(sqlite3_stmt_), status)) {
        has_next_row_ = false;
        return;
    }
    has_next_row_ = true;
}

Sqlite::Column Sqlite::Cursor::CursorImpl::column(const int& index) {
    switch (sqlite3_column_type(sqlite3_stmt_, index)) {
    case SQLITE_NULL: // ...
        return {nullptr};
        break;
    case SQLITE_INTEGER: {
        std::int64_t data_64 = sqlite3_column_int64(sqlite3_stmt_, index);
        int data = static_cast<int>(data_64);
        if (data == data_64) {
            // is 32-bit integer
            return {data};
        }
        else {
            // is 64-bit integer
            return {data_64};
        }
    } break;
    case SQLITE_FLOAT:
        return {sqlite3_column_double(sqlite3_stmt_, index)};
        break;
    case SQLITE_TEXT:
        return {std::string(reinterpret_cast<const char*>(
                                sqlite3_column_text(sqlite3_stmt_, index)),
                            sqlite3_column_bytes(sqlite3_stmt_, index))};
        break;
    case SQLITE_BLOB: {
        const unsigned char* data = reinterpret_cast<const unsigned char*>(
            sqlite3_column_blob(sqlite3_stmt_, index));
        if (data == nullptr) {
            return {std::vector<unsigned char>()};
        }
        else {
            return {std::vector<unsigned char>(
                data, data + sqlite3_column_bytes(sqlite3_stmt_, index))};
        }
    }; break;
    default: break;
    }
    return {nullptr};
}

bool Sqlite::Cursor::CursorImpl::check_sqlite_ok(const int& return_code,
                                                 misc::Status& status) {
    if (return_code == SQLITE_OK) {
        return true;
    }
    status.error = true;
    status.message = std::make_unique<std::string>(sqlite3_errmsg(sqlite3_));
    sqlite3_finalize(sqlite3_stmt_);
    sqlite3_stmt_ = nullptr;
    return false;
}

bool Sqlite::Cursor::CursorImpl::check_sqlite_row(const int& return_code,
                                                  misc::Status& status) {
    if (return_code == SQLITE_ROW) {
        return true;
    }
    if (return_code == SQLITE_DONE) {
        return false;
    }
    status.error = true;
    status.message = std::make_unique<std::string>(
        "SQLite error(" + std::to_string(return_code) + "): ");
    switch (return_code) {
    case SQLITE_BUSY:
        status.message->append(
            "The database engine was unable to acquire the database locks it needs to do the job");
        break;
    case SQLITE_DONE:
        status.message->append(
            "The cursor has finished executing successfully");
        break;
    case SQLITE_ERROR: status.message->append(sqlite3_errmsg(sqlite3_)); break;
    case SQLITE_MISUSE:
        status.message->append("This routine was called inappropriately");
        break;
    default:
        status.message->append("See https://www.sqlite.org/rescode.html");
        break;
    }
    return false;
}

bool Sqlite::Cursor::CursorImpl::check_sqlite_etc(const int& return_code,
                                                  misc::Status& status) {
    if (return_code == SQLITE_OK) {
        return true;
    }
    status.error = true;
    status.message = std::make_unique<std::string>(
        "See https://www.sqlite.org/rescode.html");
    return false;
}

/* ===== Sqlite ===== */

Sqlite::Sqlite() : impl_(std::make_unique<SqliteImpl>()) {}

Sqlite::~Sqlite() = default;

Sqlite& Sqlite::connect(const std::string& uri, misc::Status& status) {
    impl_->connect(uri, status);
    return *this;
}

Sqlite& Sqlite::connect(std::string&& uri, misc::Status& status) {
    impl_->connect(std::move(uri), status);
    return *this;
}

const std::string& Sqlite::uri() const {
    return impl_->uri();
}

Sqlite::Cursor Sqlite::cursor() {
    return {impl_->sqlite3_};
}

/* ===== Sqlite::Cursor ===== */

Sqlite::Cursor::~Cursor() = default;

Sqlite::Cursor& Sqlite::Cursor::execute(const std::string& statement,
                                        misc::Status& status) {
    impl_->execute(statement, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::execute(std::string&& statement,
                                        misc::Status& status) {
    impl_->execute(std::move(statement), status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::execute(misc::Status& status) {
    impl_->execute(status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::prepare(const std::string& statement,
                                        misc::Status& status) {
    impl_->prepare(statement, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::prepare(std::string&& statement,
                                        misc::Status& status) {
    impl_->prepare(std::move(statement), status);
    return *this;
}

const std::string& Sqlite::Cursor::statement() const {
    return impl_->statement();
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const std::vector<unsigned char>& blob,
                                     misc::Status& status) {
    impl_->bind(index, blob, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const double& real,
                                     misc::Status& status) {
    impl_->bind(index, real, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const int& integer,
                                     misc::Status& status) {
    impl_->bind(index, integer, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const std::int64_t& integer,
                                     misc::Status& status) {
    impl_->bind(index, integer, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const std::nullptr_t&,
                                     misc::Status& status) {
    impl_->bind(index, nullptr, status);
    return *this;
}

Sqlite::Cursor& Sqlite::Cursor::bind(const int& index,
                                     const std::string& text,
                                     misc::Status& status) {
    impl_->bind(index, text, status);
    return *this;
}

bool Sqlite::Cursor::has_next_row() {
    return impl_->has_next_row();
}

int Sqlite::Cursor::column_count() {
    return impl_->column_count();
}

Sqlite::Cursor& Sqlite::Cursor::advance(misc::Status& status) {
    impl_->advance(status);
    return *this;
}

Sqlite::Column Sqlite::Cursor::column(const int& index) {
    return impl_->column(index);
}

Sqlite::Cursor::Cursor(CursorImpl&& impl) :
    impl_(std::make_unique<CursorImpl>(std::move(impl))) {}

/* ===== Sqlite::Column ===== */

const Sqlite::Column::Type& Sqlite::Column::type() const {
    return type_;
}

const std::unique_ptr<int>& Sqlite::Column::integer() {
    return std::get<std::unique_ptr<int>>(data_);
}

const std::unique_ptr<std::int64_t>& Sqlite::Column::integer64() {
    return std::get<std::unique_ptr<std::int64_t>>(data_);
}

const std::unique_ptr<double>& Sqlite::Column::real() {
    return std::get<std::unique_ptr<double>>(data_);
}

const std::unique_ptr<std::string>& Sqlite::Column::text() {
    return std::get<std::unique_ptr<std::string>>(data_);
}

const std::unique_ptr<std::vector<unsigned char>>& Sqlite::Column::blob() {
    return std::get<std::unique_ptr<std::vector<unsigned char>>>(data_);
}

Sqlite::Column::operator int() {
    const std::unique_ptr<int>& data = std::get<std::unique_ptr<int>>(data_);
    return (data == nullptr ? int {} : *data);
}

Sqlite::Column::operator std::int64_t() {
    const std::unique_ptr<std::int64_t>& data =
        std::get<std::unique_ptr<std::int64_t>>(data_);
    return (data == nullptr ? std::int64_t {} : *data);
}

Sqlite::Column::operator double() {
    const std::unique_ptr<double>& data =
        std::get<std::unique_ptr<double>>(data_);
    return (data == nullptr ? double {} : *data);
}

Sqlite::Column::operator std::string() {
    const std::unique_ptr<std::string>& data =
        std::get<std::unique_ptr<std::string>>(data_);
    return (data == nullptr ? std::string {} : *data);
}

Sqlite::Column::operator std::vector<unsigned char>() {
    const std::unique_ptr<std::vector<unsigned char>>& data =
        std::get<std::unique_ptr<std::vector<unsigned char>>>(data_);
    return (data == nullptr ? std::vector<unsigned char> {} : *data);
}

Sqlite::Column::Column(const int& data) :
    type_(Type::INTEGER), data_(std::make_unique<int>(data)) {}

Sqlite::Column::Column(int&& data) :
    type_(Type::INTEGER), data_(std::make_unique<int>(std::move(data))) {}

Sqlite::Column::Column(const std::int64_t& data) :
    type_(Type::INTEGER64), data_(std::make_unique<std::int64_t>(data)) {}

Sqlite::Column::Column(std::int64_t&& data) :
    type_(Type::INTEGER64),
    data_(std::make_unique<std::int64_t>(std::move(data))) {}

Sqlite::Column::Column(const double& data) :
    type_(Type::REAL), data_(std::make_unique<double>(data)) {}

Sqlite::Column::Column(double&& data) :
    type_(Type::REAL), data_(std::make_unique<double>(std::move(data))) {}

Sqlite::Column::Column(const std::string& data) :
    type_(Type::TEXT), data_(std::make_unique<std::string>(data)) {}

Sqlite::Column::Column(std::string&& data) :
    type_(Type::TEXT), data_(std::make_unique<std::string>(std::move(data))) {}

Sqlite::Column::Column(const std::vector<unsigned char>& data) :
    type_(Type::BLOB),
    data_(std::make_unique<std::vector<unsigned char>>(data)) {}

Sqlite::Column::Column(std::vector<unsigned char>&& data) :
    type_(Type::BLOB),
    data_(std::make_unique<std::vector<unsigned char>>(std::move(data))) {}

Sqlite::Column::Column(const std::nullptr_t&) : type_(Type::NULLTYPE) {}

Sqlite::Column::Column(std::nullptr_t&&) : type_(Type::NULLTYPE) {}

} // namespace db
} // namespace utils
} // namespace rayalto
