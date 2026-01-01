#ifndef USER_H
#define USER_H

#include "Person.h"
#include <string>
#include <vector>

class User : public Person {
    std::string password;
    std::vector<int> posts;
    std::vector<std::string> followers, following;
    std::string created_ts, last_login_ts;

public:
    User() = default;
    User(const std::string &uname, const std::string &pass, const std::string &b);
    
    bool checkPassword(const std::string &p) const;
    const std::vector<int> &getPosts() const;
    const std::vector<std::string> &getFollowers() const;
    const std::vector<std::string> &getFollowing() const;
    std::string getCreatedTS() const;
    std::string getLastLoginTS() const;
    
    void setLastLoginNow();
    void addPostId(int pid);
    void removePostId(int pid);
    void addFollower(const std::string &f);
    void removeFollower(const std::string &f);
    void addFollowing(const std::string &f);
    void removeFollowing(const std::string &f);
    void setBio(const std::string &b);
    
    void showProfile() const override;
    
    std::string serialize() const;
    static User parse(const std::string &line);

private:
    static std::string escapeField(const std::string &s);
    static std::string unescapeField(const std::string &s);
    static std::vector<std::string> parseLine(const std::string &line);
};

#endif // USER_H