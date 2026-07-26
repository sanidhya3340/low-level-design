#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using Clock = chrono::system_clock;
using TimePoint = chrono::system_clock::time_point;

class User {
protected:
    string id;
    string name;

public:
    User(string id, string name) : id(move(id)), name(move(name)) {}
    virtual ~User() = default;

    const string& getId() const { return id; }
    const string& getName() const { return name; }
    virtual int getBorrowDurationDays() const = 0;
    virtual string getType() const = 0;
};

class Student : public User {
public:
    Student(string id, string name) : User(move(id), move(name)) {}
    int getBorrowDurationDays() const override { return 7; }
    string getType() const override { return "Student"; }
};

class Faculty : public User {
public:
    Faculty(string id, string name) : User(move(id), move(name)) {}
    int getBorrowDurationDays() const override { return 14; }
    string getType() const override { return "Faculty"; }
};

class Book {
private:
    string title;
    string author;
    string isbn;
    bool available;

public:
    Book(string title, string author, string isbn)
        : title(move(title)), author(move(author)), isbn(move(isbn)), available(true) {}

    const string& getTitle() const { return title; }
    const string& getAuthor() const { return author; }
    const string& getISBN() const { return isbn; }
    bool isAvailable() const { return available; }
    void setAvailable(bool value) { available = value; }
};

class Loan {
private:
    Book* book;
    User* user;
    TimePoint issuedAt;
    TimePoint dueDate;

public:
    Loan(Book& book, User& user, TimePoint issuedAt, TimePoint dueDate)
        : book(&book), user(&user), issuedAt(issuedAt), dueDate(dueDate) {}

    Book& getBook() const { return *book; }
    User& getUser() const { return *user; }
    TimePoint getDueDate() const { return dueDate; }

    bool isOverdue(TimePoint now = Clock::now()) const {
        return now > dueDate;
    }
};

class Library {
private:
    unordered_map<string, Book> books;
    vector<unique_ptr<User>> users;
    vector<Loan> activeLoans;

public:
    void addBook(const Book& book) {
        books[book.getISBN()] = book;
    }

    void removeBook(const string& isbn) {
        books.erase(isbn);
    }

    void addUser(unique_ptr<User> user) {
        users.push_back(move(user));
    }

    vector<Book> searchByTitle(const string& query) const {
        vector<Book> results;
        for (const auto& [isbn, book] : books) {
            if (book.getTitle().find(query) != string::npos) {
                results.push_back(book);
            }
        }
        return results;
    }

    vector<Book> searchByAuthor(const string& query) const {
        vector<Book> results;
        for (const auto& [isbn, book] : books) {
            if (book.getAuthor().find(query) != string::npos) {
                results.push_back(book);
            }
        }
        return results;
    }

    vector<Book> searchByISBN(const string& query) const {
        vector<Book> results;
        auto it = books.find(query);
        if (it != books.end()) {
            results.push_back(it->second);
        }
        return results;
    }

    bool borrowBook(User& user, const string& isbn) {
        if (auto* book = findBook(isbn)) {
            if (!book->isAvailable()) {
                cout << "Book is already borrowed.\n";
                return false;
            }

            auto now = Clock::now();
            auto dueDate = now + chrono::hours(24 * user.getBorrowDurationDays());
            activeLoans.emplace_back(*book, user, now, dueDate);
            book->setAvailable(false);
            cout << user.getName() << " borrowed '" << book->getTitle() << "'.\n";
            return true;
        }

        throw runtime_error("Book not found");
    }

    bool returnBook(User& user, const string& isbn) {
        auto it = find_if(activeLoans.begin(), activeLoans.end(), [&](const Loan& loan) {
            return loan.getBook().getISBN() == isbn && loan.getUser().getId() == user.getId();
        });

        if (it == activeLoans.end()) {
            cout << "No active loan found for this user and book.\n";
            return false;
        }

        Book& book = it->getBook();
        book.setAvailable(true);
        activeLoans.erase(it);
        cout << user.getName() << " returned '" << book.getTitle() << "'.\n";
        return true;
    }

    vector<Loan> getOverdueLoans() const {
        vector<Loan> overdue;
        auto now = Clock::now();
        for (const auto& loan : activeLoans) {
            if (loan.isOverdue(now)) {
                overdue.push_back(loan);
            }
        }
        return overdue;
    }

private:
    Book* findBook(const string& isbn) {
        auto it = books.find(isbn);
        return it == books.end() ? nullptr : &it->second;
    }
};

string formatTimePoint(const TimePoint& tp) {
    time_t time = Clock::to_time_t(tp);
    return ctime(&time);
}

int main() {
    Library library;

    library.addBook(Book("Clean Code", "Robert C. Martin", "9780132350884"));
    library.addBook(Book("Design Patterns", "Gang of Four", "9780201633610"));

    auto alice = make_unique<Student>("S1", "Alice");
    auto drSmith = make_unique<Faculty>("F1", "Dr. Smith");
    auto bob = make_unique<Student>("S2", "Bob");

    User* aliceUser = alice.get();
    User* facultyUser = drSmith.get();
    User* bobUser = bob.get();

    library.addUser(move(alice));
    library.addUser(move(drSmith));
    library.addUser(move(bob));

    library.borrowBook(*aliceUser, "9780132350884");
    library.borrowBook(*facultyUser, "9780132350884");
    library.returnBook(*aliceUser, "9780132350884");
    library.borrowBook(*bobUser, "9780132350884");

    cout << "\nSearch by title 'Design':\n";
    for (const auto& book : library.searchByTitle("Design")) {
        cout << book.getTitle() << " by " << book.getAuthor() << "\n";
    }

    return 0;
}