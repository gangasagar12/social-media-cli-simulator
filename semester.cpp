#include <iostream>   // for input output operations
#include <fstream>   // for file operations
#include <vector>   // for dynamic array
#include <string>
#include <algorithm>            // for algorithms like find remove
#include <limits>        // for numeric limit
#include <ctime>
#include <functional>   // for the hash password


// platform - specific includes for password masking

#ifdef _WIN32
#include <conio.h>   
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

string hashPassword(const string &password) {  // password hashing function
    hash<string> hasher;
    return to_string(hasher(password)); // compute hash and convert to string
}

void clearScreen() {   // function for clear screen 
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

vector<string> split(const string &s, const string &delim) {
    vector<string> out;   // store result substring
    if (s.empty()) {   // handle empty string case
        out.push_back("");
        return out;
    }

    size_t start = 0, pos;   // starts positiona and found position

    while ((pos = s.find(delim, start)) != string::npos) {  // finds all occurance of delimmeter
        out.push_back(s.substr(start, pos - start));
        start = pos + delim.size();
    }

    out.push_back(s.substr(start));   // add the last substring after last delimeter
    return out;
}

string getCurrentTimestamp() {  // for the function current date time
    time_t now = time(nullptr);
    char buf[32];   // before the store formatted time
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);   
}

class FileHandler {
public:
    static void saveBinary(const string &filename, const vector<string> &lines) {
        ofstream fout(filename, ios::binary);
        if (!fout) {
            cerr << "Error saving binary file: " << filename << "\n";
            return;
        }
        
        // Write number of lines
        size_t numLines = lines.size();
        fout.write(reinterpret_cast<const char*>(&numLines), sizeof(numLines));
        
        // Write each line
        for (const auto &ln : lines) {
            size_t length = ln.size();
            fout.write(reinterpret_cast<const char*>(&length), sizeof(length));
            fout.write(ln.c_str(), length);
        }
    }

    static vector<string> loadBinary(const string &filename) {
        vector<string> lines;
        ifstream fin(filename, ios::binary);
        
        if (!fin) {
            // Try to load from text file (for backward compatibility)
            return loadTextBackup(filename);
        }
        
        // Read number of lines
        size_t numLines = 0;
        fin.read(reinterpret_cast<char*>(&numLines), sizeof(numLines));
        
        // Check if we reached EOF or encountered an error
        if (!fin.good() && !fin.eof()) {
            cerr << "Error reading binary file: " << filename << "\n";
            return lines;
        }
        
        // Read each line
        for (size_t i = 0; i < numLines; ++i) {
            size_t length = 0;
            fin.read(reinterpret_cast<char*>(&length), sizeof(length));
            
            if (!fin.good()) {
                cerr << "Error reading line length from: " << filename << "\n";
                break;
            }
            
            string line(length, '\0');
            fin.read(&line[0], length);
            
            if (fin.good()) {
                lines.push_back(line);
            } else {
                cerr << "Error reading line content from: " << filename << "\n";
                break;
            }
        }
        
        return lines;
    }
    
private:
    static vector<string> loadTextBackup(const string &filename) {
        vector<string> lines;
        // Try to find .txt version for backward compatibility
        string txtFilename = filename;
        size_t dotPos = txtFilename.find_last_of('.');
        if (dotPos != string::npos) {
            txtFilename = txtFilename.substr(0, dotPos) + ".txt";
        }
        
        ifstream fin(txtFilename);
        if (!fin) {
            // If .txt doesn't exist, check .bin as text (last resort)
            fin.open(filename);
            if (!fin) return lines;
        }
        
        string line;
        while (getline(fin, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        
        // If we loaded from .txt, save as .bin for next time
        if (txtFilename != filename && !lines.empty()) {
            saveBinary(filename, lines);
        }
        
        return lines;
    }
};

class Post {
    int id;
    string author;
    string content;
    int likes;
    vector<pair<string, string>> comments; // {timestamp, comment}
    string created_ts;    // creation timestaps
    string last_edit_ts;

public:
    Post() : id(0), likes(0) {}

    Post(int i, const string &a, const string &c)
        : id(i), author(a), content(c), likes(0) {
        created_ts = getCurrentTimestamp();
        last_edit_ts = "";
    }
// accessosrs
    int getId() const {
         return id; 
        }
    string getAuthor() const {
         return author; }

    string getContent() const { return content; }
    int getLikes() const { return likes; }
    const vector<pair<string, string>> &getComments() const { return comments; }
    string getCreatedTS() const { return created_ts; }

    void setContent(const string &c) { 
        content = c; 
        last_edit_ts = getCurrentTimestamp();  // update edit timestap
    }
    void addLike() { ++likes; }  // increment like count

    void addComment(const string &c) { 
        comments.push_back({getCurrentTimestamp(), c});   // add comment with timestamp
    }

    string serialize() const {  // convert post object to string for file storage
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

    static Post parse(const string &line) {  
        Post p;         // create default post object
        auto parts = parseLine(line);   // parse CSV line
        while (parts.size() < 7) parts.push_back("");   // ensure at least 7 parts
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

    void printSummary() const {  // display post summary
        const size_t maxPreview = 140;
        string preview = content.size() <= maxPreview ? content : content.substr(0, maxPreview - 3) + "...";
        
        cout << "------------------------------------------------------------\n";
        cout << author << "  -  " << created_ts << "\n";
        cout << preview << "\n\n";
        cout << "Likes: " << likes << "    Comments: " << comments.size() << "\n";
        cout << "------------------------------------------------------------\n";
    }

    void printFull() const {   // function for display full post details with comments
        cout << "============================================================\n";
        cout << author << "  -  " << created_ts << "\n\n";
        cout << content << "\n\n";
        cout << "Likes: " << likes << "    Comments: " << comments.size() << "\n";
        cout << "Created: " << created_ts;
        if (!last_edit_ts.empty()) {
            cout << "    Edited: " << last_edit_ts;
        }
        cout << "\n";

        if (!comments.empty()) {  // display cmments if any exits
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

private:  
    static string escape(const string &s) {
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
    
    static string escapeComment(const string &s) {
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
    
    static string unescapeComment(const string &s) {
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
    
    static string unescape(const string &s) {
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
    
    static vector<string> parseLine(const string &line) {
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
};

class Person {
protected:
    string username, bio;

public:
    Person() = default;   // default constructor
    Person(const string &u, const string &b) : username(u), bio(b) {}
    virtual ~Person() = default;  // virtual destructor for polymorphism
    
    virtual void showProfile() const {  // virtual function for displaying profile , can be overridden
        cout << "Username: " << username << "\n";
        cout << "Bio: " << bio << "\n";
    }
    string getUsername() const { return username; }
    string getBio() const { return bio; }
};

class User : public Person {  // function that represnt the a social media user with extended functionalty
    string password;
    vector<int> posts;
    vector<string> followers, following;
    string created_ts, last_login_ts;

public:
    User() = default;
    User(const string &uname, const string &pass, const string &b)
     : Person(uname, b), password(pass) {
        created_ts = getCurrentTimestamp();
        last_login_ts = "";  // no login yet
    }
    // check if provided password matches stored hash
    bool checkPassword(const string &p) const { 
        return password == hashPassword(p); 
    }
    const vector<int> &getPosts() const { return posts; }
    const vector<string> &getFollowers() const { return followers; }
    const vector<string> &getFollowing() const { return following; }
    string getCreatedTS() const { return created_ts; }
    string getLastLoginTS() const { return last_login_ts; }
    
    // update last login time to current
    void setLastLoginNow() { last_login_ts = getCurrentTimestamp(); }
    void addPostId(int pid) { posts.push_back(pid); }
    void removePostId(int pid) { 
        posts.erase(remove(posts.begin(), posts.end(), pid), posts.end()); 
    }
    void addFollower(const string &f) {
        if (find(followers.begin(), followers.end(), f) == followers.end()) 
            followers.push_back(f); 
    }
    void removeFollower(const string &f) {
        followers.erase(remove(followers.begin(), followers.end(), f), followers.end()); 
    }
    void addFollowing(const string &f) {        // manage following list (avoid duplicates)
        if (find(following.begin(), following.end(), f) == following.end()) 
            following.push_back(f); 
    }
    void removeFollowing(const string &f) { 
        following.erase(remove(following.begin(), following.end(), f), following.end()); 
    }
    // update bio
    void setBio(const string &b) { bio = b; }

    // override showprofile to display user-specific information
    void showProfile() const override {  
        cout << "Username: " << username << "\n";
        cout << "Bio: " << bio << "\n";
        cout << "Followers: " << followers.size() 
             << " | Following: " << following.size() << "\n";
        cout << "Posts: " << posts.size() << "\n";
        cout << "Joined: " << created_ts << "\n";
        if (!last_login_ts.empty())
            cout << "Last login: " << last_login_ts << "\n";
    }

    string serialize() const {
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

    static User parse(const string &line) {
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

private:
    static string escapeField(const string &s) {
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
    
    static string unescapeField(const string &s) {
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

    static vector<string> parseLine(const string &line) {
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
};

// Manage the entire social media application
class App {
    vector<User> users;
    vector<Post> posts;
    int postCounter = 1;
    const string USERS_FILE = "users.bin";   // Changed from .txt to .bin
    const string POSTS_FILE = "posts.bin";   // Changed from .txt to .bin
    
public:
    // constructor load data from files
    App() { loadAll(); }
    // destructor load data to files
    ~App() { saveAll(); }

    void saveAll() {  // save all data to files
        vector<string> u, p;            // vector to store serialized data
        for (const auto &x : users) u.push_back(x.serialize());  // serialize all user
        for (const auto &x : posts) p.push_back(x.serialize());  // serialize all posts

        FileHandler::saveBinary(USERS_FILE, u);  // Changed to saveBinary
        FileHandler::saveBinary(POSTS_FILE, p);  // Changed to saveBinary
    }

    void loadAll() {        // load all data from files
        users.clear(); posts.clear(); postCounter = 1;
        
        // Load users
        auto userLines = FileHandler::loadBinary(USERS_FILE);  // Changed to loadBinary
        for (const auto &ln : userLines) {
            users.push_back(User::parse(ln));
        }

        // Load posts
        auto postLines = FileHandler::loadBinary(POSTS_FILE);  // Changed to loadBinary
        for (const auto &ln : postLines) {
            Post p = Post::parse(ln);
            posts.push_back(p);
            // update post counter to avoid id conflicts
            if (p.getId() >= postCounter) postCounter = p.getId() + 1;
        }
    }

    void registerUser() {
          clearScreen();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // clean input buffer
        string uname;
        cout << "\tEnter username: ";
        getline(cin, uname);

        if (uname.empty()) {  // valid username
            cout << "\tUsername cannot be empty.\n"; 
            return; 
        }
        if (findUserIndex(uname) != -1) {   // check if username already exits
            cout << "\tUsername already exists!\n";
            return; 

        }

        cout << "\n\tEnter password: ";  // get password with masking
        string pass = maskPassword();
        cout << "\n\tEnter bio: ";
        string bio; getline(cin, bio);
        users.emplace_back(uname, hashPassword(pass), bio);
        saveAll();

        cout << "\n\tRegistration successful!\n";
        cout << "\n\tPress Enter to continue...";
        cin.get();   // wait for user input
        clearScreen();   // clean screen
    }

    int loginUser() {
          clearScreen();
        string uname;
        cout << "\tEnter username: "; 
        cin >> uname;
        cout << "\tEnter password: ";
        string pass = maskPassword();
        int idx = findUserIndex(uname);

        if (idx == -1) {
            cout << "\tUsername not found.\n"; 
            return -1; 
        }
        if (!users[idx].checkPassword(pass)) {
            cout << "\tWrong password.\n";
                
            return -1; 
            
        }

        users[idx].setLastLoginNow();  // update last login time
        saveAll();  // save update timestamp
        cout << "\tLogin successful. Welcome, " << uname << "!\n";
        cout << "\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
        return idx;
    }

    void createPost(int uid) {
        clearScreen(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string content;
        cout << "\tEnter post content: ";
        getline(cin, content);
        if (content.empty()) {
            cout << "\tPost content cannot be empty.\n";
            return; 
        }
        // create new post with unique id
        posts.emplace_back(postCounter++, users[uid].getUsername(), content);
        // add post id to user post list
        users[uid].addPostId(posts.back().getId());
        saveAll();
        cout << "\tPost created successfully.\n";
        cout << "\tPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    // Helper function to find post by position in user's posts
    int findUserPostByPosition(int uid, int position) const {
        string currentUser = users[uid].getUsername();
        vector<int> userPostIndices;
        
        // Find all indices of posts by this user
        for (int i = 0; i < (int)posts.size(); ++i) {
            if (posts[i].getAuthor() == currentUser) {
                userPostIndices.push_back(i);
            }
        }
        
        // User's posts are stored in chronological order, but we want newest first
        // So reverse the order
        reverse(userPostIndices.begin(), userPostIndices.end());
        
        if (position < 1 || position > (int)userPostIndices.size()) {
            return -1;
        }
        
        // Return the actual index in posts vector
        return userPostIndices[position - 1];
    }

    // Helper function to show user's own posts
    void displayUserPosts(int uid) const {
        string currentUser = users[uid].getUsername();
        vector<int> userPostIndices;
        
        // Find all indices of posts by this user
        for (int i = 0; i < (int)posts.size(); ++i) {
            if (posts[i].getAuthor() == currentUser) {
                userPostIndices.push_back(i);
            }
        }
        
        if (userPostIndices.empty()) {
            cout << "\tYou have no posts.\n";
            return;
        }
        
        // Display newest first
        reverse(userPostIndices.begin(), userPostIndices.end());
        
        cout << "\n\t=== Your Posts ===\n";
        for (int i = 0; i < (int)userPostIndices.size(); ++i) {
            int postIdx = userPostIndices[i];
            cout << "\tPost " << (i + 1) << ":\n";
            posts[postIdx].printSummary();
        }
    }

    // Helper function to show all posts in feed (public)
    void displayAllPosts() const {
        if (posts.empty()) {
            cout << "\n\tNo posts available.\n";
            return;
        }
        
        cout << "\n\t======= Feed ======\n";
        // display posts in reverse order (newest first)
        for (auto it = posts.rbegin(); it != posts.rend(); ++it)
            it->printSummary();
        cout << "\n\tTotal posts: " << posts.size() << "\n";
    }

    void editPost(int uid) {
          clearScreen();
        // First show user's own posts
        displayUserPosts(uid);
        string currentUser = users[uid].getUsername();
        vector<int> userPostIndices;
        
        // Count user's posts
        for (int i = 0; i < (int)posts.size(); ++i) {
            if (posts[i].getAuthor() == currentUser) {
                userPostIndices.push_back(i);
            }
        }
        
        if (userPostIndices.empty()) {
            cout << "\tYou have no posts to edit.\n";
            cout << "\tPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            clearScreen();
            return;
        }
        
        int totalUserPosts = userPostIndices.size();
        cout << "\n\tEnter post number to edit (" << totalUserPosts << "): ";
        int postNum;
        if (!(cin >> postNum)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\tInvalid input.\n";
            return; 
        }

        int pidx = findUserPostByPosition(uid, postNum);
        if (pidx == -1) {
            cout << "\tInvalid post number. Please enter a number between 1 and " << totalUserPosts << ".\n"; 
            return; 
        }
        clearScreen();
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\tCurrent content of Post " << postNum << ":\n";
        posts[pidx].printFull();
        cout << "\tEnter new content: "; 
        string nc; 
        getline(cin, nc);
        if (nc.empty()) {
            cout << "\tContent empty. Aborted.\n"; 
            return; 
        }
        // update post content
        posts[pidx].setContent(nc);
        saveAll();
        cout << "\tPost " << postNum << " updated.\n";
        cout << "\tPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void deletePost(int uid) {
          clearScreen();
        // First show user's own posts
        displayUserPosts(uid);
        
        string currentUser = users[uid].getUsername();
        vector<int> userPostIndices;
        
        // Count user's posts
        for (int i = 0; i < (int)posts.size(); ++i) {
            if (posts[i].getAuthor() == currentUser) {
                userPostIndices.push_back(i);
            }
        }
        
        if (userPostIndices.empty()) {
            cout << "\tYou have no posts to delete.\n";
            cout << "\tPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            clearScreen();
            return;
        }
        
        int totalUserPosts = userPostIndices.size();
        cout << "\tEnter post number to delete (" << totalUserPosts << "): ";
        int postNum;
        if (!(cin >> postNum)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\tInvalid input.\n";
            return; 
        }

        int pidx = findUserPostByPosition(uid, postNum);
        if (pidx == -1) {
            cout << "\tInvalid post number. Please enter a number between 1 and " << totalUserPosts << ".\n"; 
            return; 
        }
        
        // Show which post is being deleted
        cout << "\tDeleting Post " << postNum << "\n";
        posts[pidx].printFull();
        int postId = posts[pidx].getId();
        posts.erase(posts.begin() + pidx);
        users[uid].removePostId(postId);
        saveAll();
         cout << "\n\tPost deleted successfully.\n";
    cout << "\tPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    clearScreen();
        
    }

    void showFeed() {
          clearScreen();
        displayAllPosts();
        cout << "\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    // Helper function to show all posts for like/comment (public feed)
    void displayAllPostsForInteraction() const {
        if (posts.empty()) {
            cout << "\tNo posts available.\n";
            return;
        }
        
        cout << "\n\t=== All Posts ===\n";
        // display posts in reverse order (newest first)
        int position = 1;
        for (auto it = posts.rbegin(); it != posts.rend(); ++it, ++position) {
            cout << "\n\tPost " << position << ":\n";
            it->printSummary();
        }
    }

    // Helper function to find post by position in all posts (public feed)
    int findPostByPosition(int position) const {
        if (position < 1 || position > posts.size()) {
            return -1;
        }
        // Posts are stored oldest first, but displayed newest first
        // So position 1 corresponds to index (posts.size() - 1)
        return posts.size() - position;
    }

    void likeOrComment(int uid) {
        if (posts.empty()) {
            cout << "\n\tNo posts available.\n";
            return;
        }
          clearScreen();
        // Show all posts for interaction
        displayAllPostsForInteraction();
        
        cout << "\n\tEnter post number ( " << posts.size() << "): ";
        int postNum;
        if (!(cin >> postNum)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input.\n"; 
            return; 
        }
        
        int pidx = findPostByPosition(postNum);
        if (pidx == -1) {
            cout << "\tInvalid post number. Please enter a number between 1 and " << posts.size() << ".\n"; 
            return; 
        }

        cout << "\tPost " << postNum << " selected:\n";
        posts[pidx].printFull();
        cout << "\t1. Like\n";
        cout << "\t2. Comment\n";
        cout << "\tEnter choice: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "\tInvalid input.\n"; 
            return; 
        }
        if (choice == 1) {
            posts[pidx].addLike();
            saveAll(); 
            cout << "\tPost " << postNum << " liked.\n";

        } else if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\tEnter comment: ";
            string comment; 
            getline(cin, comment);
            if (comment.empty()) {
                cout << "\tComment cannot be empty.\n"; 
                return; 
            }
            posts[pidx].addComment(comment); 
            saveAll(); 
            cout << "\tComment added to Post " << postNum << ".\n";
        } else {
            cout << "\tInvalid choice.\n";
        }

        cout << "\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void followUnfollow(int uid) {
          clearScreen();
        string target;
        cout << "\tEnter username to follow/unfollow: ";
        if (!(cin >> target)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\tInvalid input.\n";
            return;
        }
        
        if (target == users[uid].getUsername()) {
            cout << "\tCannot follow yourself.\n"; 
            return;
        }
        
        int idx = findUserIndex(target);
        if (idx == -1) {
            cout << "\tUser not found.\n"; 
            return; 
        }
        clearScreen();
        string currentUser = users[uid].getUsername();
        const auto& following = users[uid].getFollowing();
        
        // Check if already following
        bool isFollowing = find(following.begin(), following.end(), target) != following.end();
        
        if (isFollowing) {
            // Unfollow
            users[uid].removeFollowing(target);
            users[idx].removeFollower(currentUser);
            saveAll();
            cout << "\n\tYou have unfollowed " << target << "\n";
        } else {
            // Follow
            users[uid].addFollowing(target);
            users[idx].addFollower(currentUser);
            saveAll();
            cout << "\n\tYou are now following " << target << "\n";
        }
        
        cout << "\t" << target << "'s followers: " << users[idx].getFollowers().size() 
             << " | following: " << users[idx].getFollowing().size() << "\n";
        
        cout << "\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
    }

    void showFollowersFollowing(int uid) {
          clearScreen();
        cout << "\t===== Your Profile Summary =====\n";
        cout << "\tUsername: " << users[uid].getUsername() << "\n";
        cout << "\tFollowers: " << users[uid].getFollowers().size() 
             << " | Following: " << users[uid].getFollowing().size() << "\n";
        
        cout << "\n\tFollowers:\n";
        const auto& followers = users[uid].getFollowers();
        if (followers.empty()) {
            cout << "\t  No followers yet.\n";
        } else {
            for (const auto &f : followers)
                cout << "\t  - " << f << "\n";
        }
        
        cout << "\n\tFollowing:\n";
        const auto& following = users[uid].getFollowing();
        if (following.empty()) {
            cout << "\t  Not following anyone yet.\n";
        } else {
            for (const auto &f : following)
                cout << "\t  - " << f << "\n";
        }
        
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void viewProfileAny(int uid) {
          clearScreen();
        cout << "\t1. View another user\n";
        cout << "\t2. View my profile\n";
        cout << "\tEnter choice: ";
        int choice; 
        if (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input.\n"; 
            return; 
        }
        
        if (choice == 1) {
            cout << "\tEnter username to view: ";
            string target;
            cin >> target;
            int idx = findUserIndex(target);
            if (idx == -1) {
                cout << "\tUser not found.\n"; 
                return; 
            }
            
            cout << "\t=== " << target << "'s Profile ===\n";
            users[idx].showProfile();
            
            cout << "\t=== Recent Posts ===\n";
            // Show user's posts
            int postCount = 0;
            for (auto it = posts.rbegin(); it != posts.rend(); ++it) {
                if (it->getAuthor() == target) {
                    it->printSummary();
                    postCount++;
                    if (postCount >= 5) break; // Show only recent 5 posts
                }
            }
            if (postCount == 0) {
                cout << "\tNo posts yet.\n";
            }
        } 
        else if (choice == 2) {
            cout << "\n\t=== Your Profile ===\n";
            users[uid].showProfile();
            
            cout << "\n\t=== Your Recent Posts ===\n";
            // Show user's posts
            int postCount = 0;
            for (auto it = posts.rbegin(); it != posts.rend(); ++it) {
                if (it->getAuthor() == users[uid].getUsername()) {
                    it->printSummary();
                    postCount++;
                    if (postCount >= 5) break; // Show only recent 5 posts
                }
            }
            if (postCount == 0) {
                cout << "\tNo posts yet.\n";
            }
        } 
        else {
            cout << "\n\tInvalid choice.\n";
        }
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
    }

    void searchUsers() {
          clearScreen();
        cout << "\n\tEnter search keyword: ";
        string key; 
        cin >> key;
        
        cout << "\n\t=== Matching Users ===\n";
        vector<string> matches;
        for (const auto &u : users) {
            if (u.getUsername().find(key) != string::npos) {
                matches.push_back(u.getUsername());
            }
        }
        
        if (matches.empty()) {
            cout << "\tNo users found matching '" << key << "'\n";
        } else {
            for (const auto &username : matches) {
                cout << "\t- " << username << "\n";
            }
        }

        cout << "\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    int findUserIndex(const string &uname) const {
        for (int i = 0; i < (int)users.size(); ++i)
            if (users[i].getUsername() == uname)
                return i;
        return -1;      // not found
    }

    int findPostIndex(int pid) const {
        for (int i = 0; i < (int)posts.size(); ++i)
            if (posts[i].getId() == pid)
                return i;
        return -1;
    }

    void userDashboard(int uid) {
        int ch;
        do {
            cout << "\n\t===== User Dashboard =====\n";
            cout << "\tLogged in as: " << users[uid].getUsername() << "\n";
            cout << "\t===========================\n";
            cout << "\n\t1. Create post";
            cout << "\n\t2. Edit post";
            cout << "\n\t3. Delete post";
            cout << "\n\t4. View feed";
            cout << "\n\t5. Like/comment on post";
            cout << "\n\t6. Follow/unfollow user";
            cout << "\n\t7. Show followers/following";
            cout << "\n\t8. View profile";
            cout << "\n\t9. Search users";
            cout << "\n\t0. Logout";
            cout << "\n\t===========================\n";
            cout << "\tEnter choice: ";
            
            if (!(cin >> ch)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                ch = -1;
            }
            
            switch (ch) {
                case 1: createPost(uid); 
                    break;
                case 2: editPost(uid); 
                    break;
                case 3: deletePost(uid); 
                    break;
                case 4: showFeed(); 
                    break;
                case 5: likeOrComment(uid); 
                    break;
                case 6: followUnfollow(uid); 
                    break;
                case 7: showFollowersFollowing(uid); 
                    break;
                case 8: viewProfileAny(uid); 
                    break;
                case 9: searchUsers(); 
                    break;
                case 0: 
                    cout << "\tLogging out...\n";
                    clearScreen(); 
                    break;
                default:
                    cout << "\tInvalid choice. Try again.\n";
                    break;
            }
        } while (ch != 0);
    }

    static string maskPassword() {
        string pass;
#ifdef _WIN32
        int ch;
        while ((ch = _getch()), ch != 13) {   // 13 = enter key
            if (ch == 8 || ch == 127) {         // 8 = backspace 127 = delete
                if (!pass.empty()) {              // remove last character
                    pass.pop_back(); 
                    cout << "\b \b"; 
                }      // erase asterisk from screen
            } 
            else if (ch == 0 || ch == 224) { _getch(); }
            else { 
                pass.push_back((char)ch); 
                cout << '*'; 
            }
        }
        cout << "\n";
#else  
        // linux
        struct termios oldt, newt;
        if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
            newt = oldt;
            newt.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            getline(cin, pass);
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            cout << "\n";
        } else {
            getline(cin, pass);
        }
#endif
        return pass;
    }
};

// ===== MAIN =====
int main() {
    App socialApp;
    int choice;
    
    do {
        cout << "\n\t=================================\n";
        cout << "\t     Welcome to SocialShell\n";
        cout << "\t=================================\n";
        cout << "\n\t1. Register user";
        cout << "\n\t2. Login user";
        cout << "\n\t3. Exit";
        cout << "\n\t=================================\n";
        cout << "\n\tEnter choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        
        switch (choice) {
            case 1: 
                socialApp.registerUser(); 
                break;
            case 2: {
                int uid = socialApp.loginUser();
                if (uid != -1) socialApp.userDashboard(uid);
                break;
            }
            case 3:
                cout << "\n\tExiting... Goodbye!\n";
                break;
            default:
                cout << "\n\tInvalid choice. Try again.\n"; 
                break;
        }
    } while (choice != 3);
    
    return 0;
}