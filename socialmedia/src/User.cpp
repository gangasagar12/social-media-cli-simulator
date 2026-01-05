#include "User.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

// Helper function to split strings (defined in App originally, but we need it here)
static vector<string> split(const string &s, const string &delim) {
    vector<string> out;
    if (s.empty()) {
        out.push_back("");
        return out;
    }

    size_t start = 0, pos;

    while ((pos = s.find(delim, start)) != string::npos) {
        out.push_back(s.substr(start, pos - start));
        start = pos + delim.size();
    }

    out.push_back(s.substr(start));
    return out;
}

static string getCurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

static string hashPassword(const string &password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

User::User(const string &uname, const string &pass, const string &b)
    : Person(uname, b), password(hashPassword(pass)) {
    created_ts = getCurrentTimestamp();
    last_login_ts = "";
}

bool User::checkPassword(const string &p) const { 
    return password == hashPassword(p); 
}

const vector<int> &User::getPosts() const { return posts; }
const vector<string> &User::getFollowers() const { return followers; }
const vector<string> &User::getFollowing() const { return following; }
string User::getCreatedTS() const { return created_ts; }
string User::getLastLoginTS() const { return last_login_ts; }

void User::setLastLoginNow() { last_login_ts = getCurrentTimestamp(); }
void User::addPostId(int pid) { posts.push_back(pid); }
void User::removePostId(int pid) { 
    posts.erase(remove(posts.begin(), posts.end(), pid), posts.end()); 
}

void User::addFollower(const string &f) {
    if (find(followers.begin(), followers.end(), f) == followers.end()) 
        followers.push_back(f); 
}

void User::removeFollower(const string &f) {
    followers.erase(remove(followers.begin(), followers.end(), f), followers.end()); 
}

void User::addFollowing(const string &f) {
    if (find(following.begin(), following.end(), f) == following.end()) 
        following.push_back(f); 
}

void User::removeFollowing(const string &f) { 
    following.erase(remove(following.begin(), following.end(), f), following.end()); 
}

void User::setBio(const string &b) { bio = b; }

void User::showProfile() const {
    cout << "Username: " << username << "\n";
    cout << "Bio: " << bio << "\n";
    cout << "Followers: " << followers.size() 
         << " | Following: " << following.size() << "\n";
    cout << "Posts: " << posts.size() << "\n";
    cout << "Joined: " << created_ts << "\n";
    if (!last_login_ts.empty())
        cout << "Last login: " << last_login_ts << "\n";
}

string User::serialize() const {
    // Serialize posts as semicolon-separated list
    string posts_str;
    for (size_t i = 0; i < posts.size(); ++i) {
        if (i > 0) posts_str += ";";
        posts_str += to_string(posts[i]);
    }
    
    // Serialize followers as semicolon-separated list
    string followers_str;
    for (size_t i = 0; i < followers.size(); ++i) {
        if (i > 0) followers_str += ";";
        followers_str += escapeField(followers[i]);
    }
    
    // Serialize following as semicolon-separated list
    string following_str;
    for (size_t i = 0; i < following.size(); ++i) {
        if (i > 0) following_str += ";";
        following_str += escapeField(following[i]);
    }
    
    return escapeField(username) + "," +
           escapeField(password) + "," + 
           escapeField(bio) + "," +
           escapeField(posts_str) + "," +
           escapeField(followers_str) + "," +
           escapeField(following_str) + "," +
           escapeField(created_ts) + "," +
           escapeField(last_login_ts);
}

User User::parse(const string &line) {
    auto parts = parseLine(line);
    while (parts.size() < 8) parts.push_back("");
    User u;

    u.username = unescapeField(parts[0]);
    u.password = unescapeField(parts[1]);
    u.bio = unescapeField(parts[2]);
    
    // Parse posts
    if (!parts[3].empty()) {
        string posts_part = unescapeField(parts[3]);
        auto post_ids = split(posts_part, ";");
        for (const auto &x : post_ids) {
            if (!x.empty()) {
                try { 
                    u.posts.push_back(stoi(x)); 
                } catch (...) {}
            }
        }
    }

    // Parse followers
    if (!parts[4].empty()) {
        string followers_part = unescapeField(parts[4]);
        auto follower_list = split(followers_part, ";");
        for (const auto &x : follower_list) {
            if (!x.empty()) u.followers.push_back(unescapeField(x));
        }
    }

    // Parse following
    if (!parts[5].empty()) {
        string following_part = unescapeField(parts[5]);
        auto following_list = split(following_part, ";");
        for (const auto &x : following_list) {
            if (!x.empty()) u.following.push_back(unescapeField(x));
        }
    }
    
    u.created_ts = unescapeField(parts[6]);
    u.last_login_ts = unescapeField(parts[7]);
    return u;
}

string User::escapeField(const string &s) {
    bool needsQuotes = false;
    for (char c : s) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r' || c == ';') {
            needsQuotes = true;
            break;
        }
    }
    
    if (!needsQuotes) return s;
    
    string out;
    out.push_back('"');
    for (char c : s) {
        if (c == '"') {
            out.push_back('"');
            out.push_back('"');
        } else {
            out.push_back(c);
        }
    }
    out.push_back('"');
    return out;
}

string User::unescapeField(const string &s) {
    if (s.size() >= 2 && s[0] == '"' && s[s.size()-1] == '"') {
        string inner = s.substr(1, s.size() - 2);
        string result;
        for (size_t i = 0; i < inner.size(); i++) {
            if (inner[i] == '"' && i + 1 < inner.size() && inner[i+1] == '"') {
                result.push_back('"');
                i++;
            } else {
                result.push_back(inner[i]);
            }
        }
        return result;
    }
    return s;
}

vector<string> User::parseLine(const string &line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    current.push_back('"');
                    i++;
                } else {
                    inQuotes = false;
                }
            } else {
                current.push_back(c);
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(current);
                current.clear();
            } else {
                current.push_back(c);
            }
        }
    }
    fields.push_back(current);
    
    return fields;
}