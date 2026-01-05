#include "Post.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
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

Post::Post() : id(0), likes(0) {}

Post::Post(int i, const string &a, const string &c)
    : id(i), author(a), content(c), likes(0) {
    created_ts = getCurrentTimestamp();
    last_edit_ts = "";
}

int Post::getId() const { return id; }
string Post::getAuthor() const { return author; }
string Post::getContent() const { return content; }
int Post::getLikes() const { return likes; }
const vector<pair<string, string>> &Post::getComments() const { return comments; }
string Post::getCreatedTS() const { return created_ts; }

void Post::setContent(const string &c) { 
    content = c; 
    last_edit_ts = getCurrentTimestamp();
}

void Post::addLike() { ++likes; }

void Post::addComment(const string &c) { 
    comments.push_back({getCurrentTimestamp(), c});
}

string Post::serialize() const {
    string out = to_string(id) + "," +
            escape(author) + "," +
            escape(content) + "," +
            to_string(likes) + "," +
            escape(created_ts) + "," +
            escape(last_edit_ts) + ",";
    
    string comments_str;
    for (size_t i = 0; i < comments.size(); ++i) {
        if (i > 0) comments_str += "~~";
        comments_str += comments[i].first + "|" + escapeComment(comments[i].second);
    }
    out += escape(comments_str);
    return out;
}

Post Post::parse(const string &line) {
    Post p;
    auto parts = parseLine(line);
    while (parts.size() < 7) parts.push_back("");
    try {
        p.id = stoi(parts[0]);
    } catch (...) { 
        p.id = 0; 
    }

    p.author = unescape(parts[1]);
    p.content = unescape(parts[2]);
    try {
        p.likes = stoi(parts[3]);
    } catch (...) { 
        p.likes = 0; 
    }
    p.created_ts = unescape(parts[4]);
    p.last_edit_ts = unescape(parts[5]);
    
    // Parse comments
    if (!parts[6].empty()) {
        string comments_part = unescape(parts[6]);
        auto cvec = split(comments_part, "~~");
        for (const auto &c : cvec) {
            if (c.empty()) continue;
            size_t pos = c.find('|');
            if (pos != string::npos) {
                string ts = c.substr(0, pos);
                string text = unescapeComment(c.substr(pos + 1));
                p.comments.push_back({ts, text});
            } else {
                p.comments.push_back({"", unescapeComment(c)});
            }
        }
    }
    return p;
}

void Post::printSummary() const {
    const size_t maxPreview = 140;
    string preview = content.size() <= maxPreview ? content : content.substr(0, maxPreview - 3) + "...";
    
    cout << "------------------------------------------------------------\n";
    cout << author << "  -  " << created_ts << "\n";
    cout << preview << "\n\n";
    cout << "Likes: " << likes << "    Comments: " << comments.size() << "\n";
    cout << "------------------------------------------------------------\n";
}

void Post::printFull() const {
    cout << "============================================================\n";
    cout << author << "  -  " << created_ts << "\n\n";
    cout << content << "\n\n";
    cout << "Likes: " << likes << "    Comments: " << comments.size() << "\n";
    cout << "Created: " << created_ts;
    if (!last_edit_ts.empty()) {
        cout << "    Edited: " << last_edit_ts;
    }
    cout << "\n";

    if (!comments.empty()) {
        cout << "\nComments:\n";
        for (const auto &c : comments) {
            if (!c.first.empty()) {
                cout << " [" << c.first << "] ";
            }
            cout << c.second << "\n";
        }
    }
    cout << "============================================================\n";
}

string Post::escape(const string &s) {
    bool needsQuotes = false;
    for (char c : s) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') {
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

string Post::escapeComment(const string &s) {
    string out;
    for (char c : s) {
        if (c == '|') {
            out += "\\p";
        } else if (c == '~') {
            out += "\\t";
        } else if (c == '\\') {
            out += "\\\\";
        } else {
            out.push_back(c);
        }
    }
    return out;
}

string Post::unescapeComment(const string &s) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            if (s[i+1] == 'p') {
                out.push_back('|');
                i++;
            } else if (s[i+1] == 't') {
                out.push_back('~');
                i++;
            } else if (s[i+1] == '\\') {
                out.push_back('\\');
                i++;
            } else {
                out.push_back(s[i]);
            }
        } else {
            out.push_back(s[i]);
        }
    }
    return out;
}

string Post::unescape(const string &s) {
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

vector<string> Post::parseLine(const string &line) {
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