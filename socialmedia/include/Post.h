#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include <utility>

class Post {
    int id;
    std::string author;
    std::string content;
    int likes;
    std::vector<std::pair<std::string, std::string>> comments; // {timestamp, comment}
    std::string created_ts;
    std::string last_edit_ts;

public:
    Post();
    Post(int i, const std::string &a, const std::string &c);
    
    // Accessors
    int getId() const;
    std::string getAuthor() const;
    std::string getContent() const;
    int getLikes() const;
    const std::vector<std::pair<std::string, std::string>> &getComments() const;
    std::string getCreatedTS() const;
    
    void setContent(const std::string &c);
    void addLike();
    void addComment(const std::string &c);
    
    std::string serialize() const;
    static Post parse(const std::string &line);
    
    void printSummary() const;
    void printFull() const;

private:
    static std::string escape(const std::string &s);
    static std::string escapeComment(const std::string &s);
    static std::string unescapeComment(const std::string &s);
    static std::string unescape(const std::string &s);
    static std::vector<std::string> parseLine(const std::string &line);
};

#endif // POST_H