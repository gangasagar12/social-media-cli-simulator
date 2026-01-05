#include "Person.h"
#include <iostream>

using namespace std;

Person::Person(const string &u, const string &b) : username(u), bio(b) {}

void Person::showProfile() const {
    cout << "Username: " << username << "\n";
    cout << "Bio: " << bio << "\n";
}

string Person::getUsername() const { return username; }
string Person::getBio() const { return bio; }