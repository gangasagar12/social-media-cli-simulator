#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
protected:
    std::string username, bio;

public:
    Person() = default;
    Person(const std::string &u, const std::string &b);
    virtual ~Person() = default;
    
    virtual void showProfile() const;
    std::string getUsername() const;
    std::string getBio() const;
};

#endif // PERSON_H