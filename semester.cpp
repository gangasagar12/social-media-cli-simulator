#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <limits>

#ifdef _WIN32
  #include <conio.h> // _getch()
#else
  #include <termios.h>
  #include <unistd.h>
#endif

using namespace std;

// --- Utilities ---
vector<string> split(const string &s, const string &delim) {
    vector<string> out;
    if (s.empty())
     {
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

class FileHandler {
public:
    static void saveLines(const string &filename, const vector<string> &lines) {
        ofstream fout(filename);
        if (!fout) {
            cerr << "Error saving file: " << filename << "\n";
            return;
        }
        for (const auto &ln : lines) fout << ln << "\n";
    }

    static vector<string> loadLines(const string &filename) {
        vector<string> lines;
        ifstream fin(filename);
        if (!fin) return lines;
        string line;
        while (getline(fin, line)) {
            // keep empty lines out
            if (!line.empty()) lines.push_back(line);
        }
        return lines;
    }
};

// --- Post ---
class Post {
    int id;
    string author;
    string content;
    int likes;
    vector<string> comments;

public:
    Post(){
        id=0;
        likes=0;
    }
    Post(int i, const string &a, const string &c)
    {
        id=i;
        author=a;
        content=c;
        likes=0;
    }

    int getId() const { 
        return id;
     }
    string getAuthor() const
     { 
        return author;
     }
    string getContent() const 
    { 
        return content; 
    }
    int getLikes() const 
    {
         return likes;
         }
    const vector<string>& getComments() const 
    { 
        return comments; 
    }

    void setContent(const string &c)
     {
         content = c; 
        }
    void addLike()
     { 
        ++likes;
     }
    void addComment(const string &c)
     { 
        comments.push_back(c);
     }

    string serialize() const {
        // Format: id|author|content|likes|c1~~c2~~
        string out = to_string(id) + "|" + escapePipes(author) + "|" + escapePipes(content) + "|" + to_string(likes) + "|";
        for (const auto &c : comments) out += escapePipes(c) + "~~";
        return out;
    }

    static Post parse(const string &line) {
        Post p;
        vector<string> parts = split(line, "|");
        while (parts.size() < 5) 
        parts.push_back("");
        try 
        { p.id = stoi(parts[0]); }
         catch(...)
          {
             p.id = 0;
         }
        p.author = unescapePipes(parts[1]);
        p.content = unescapePipes(parts[2]);
        try { p.likes = stoi(parts[3]); 
        }
         catch(...) { p.likes = 0;
         }
        if (!parts[4].empty()) {
            vector<string> cms = split(parts[4], "~~");
            for (auto &c : cms) if (!c.empty()) p.comments.push_back(unescapePipes(c));
        }
        return p;
    }

    void printSummary() const {
        cout << "Post ID: " << id << " | Author: " << author << " | Likes: " << likes << " | Comments: " << comments.size() << "\n";
    }

    void printFull() const {
        cout << "Post ID: " << id << " | Author: " << author << "\n";
        cout << content << "\n";
        cout << "Likes: " << likes << " | Comments: " << comments.size() << "\n";
        if (!comments.empty()) {
            cout << "Comments:\n";
            for (const auto &c : comments) cout << " - " << c << "\n";
        }
    }

private:
    static string escapePipes(const string &s) {
        string out;
        for (char ch : s) {
            if (ch == '|') out += "&#124;";
            else out += ch;
        }
        return out;
    }
    static string unescapePipes(const string &s) {
        // very small unescape for the marker used above
        string out = s;
        size_t pos = 0;
        while ((pos = out.find("&#124;", pos)) != string::npos) {
            out.replace(pos, 6, "|");
            pos += 1;
        }
        return out;
    }
};

// --- Person / User ---
class Person {
protected:
    string username;
    string bio;
public:
    Person() = default;
    Person(const string &u, const string &b) {
        username=u;
        bio=b;
    }

    virtual ~Person() = default;
    virtual void showProfile() {
        cout << " username: " << username << "\n";
        cout << " bio: " << bio << "\n";
    }
    string getUsername() const { 
        return username; 
    }
    string getBio() const {
         return bio; 
        }
};

class User : public Person {
    string password;
    vector<int> posts;
    vector<string> followers;
    vector<string> following;
public:
    User() = default;
    User(const string &uname, const string &pass, const string &b) {
        username = uname;
         password = pass;
          bio = b;
    }

    bool checkPassword(const string &p) const {
         return password == p;
         }
    const vector<int>& getPosts() const { 
        return posts;
     }
    const vector<string>& getFollowers() const { 
        return followers; 
    }
    const vector<string>& getFollowing() const { 
        return following;
     }

    void addPostId(int pid) 
    {
         posts.push_back(pid); 
        }
    void removePostId(int pid) 
    { 
        posts.erase(remove(posts.begin(), posts.end(), pid), 
        posts.end());
     }

    void addFollower(const string &f)
     { if 
        (find(followers.begin(), followers.end(), f) == followers.end()) followers.push_back(f); 
    }
    void removeFollower(const string &f)
     { 
        followers.erase(remove(followers.begin(), followers.end(), f), followers.end()); 
    }

    void addFollowing(const string &f)
     {
         if (find(following.begin(), following.end(), f) == following.end()) following.push_back(f); 
    }
    void removeFollowing(const string &f) 
    { 
        following.erase(remove(following.begin(), following.end(), f), following.end()); 
    }

    void setBio(const string &b) 
    { bio = b; 
    }

    void showProfile() override {
        cout << " username: " << username << "\n";
        cout << " bio: " << bio << "\n";
        cout << " followers: " << followers.size() << " | following: " << following.size() << "\n";
        cout << " posts: " << posts.size() << "\n";
    }

    string serialize() const {
        // username|password|bio|p1,p2,|f1,f2,|g1,g2,
        string out = escapePipes(username) + "|" + escapePipes(password) + "|" + escapePipes(bio) + "|";
        for (int pid : posts) out += to_string(pid) + ",";
        out += "|";
        for (const auto &f : followers) out += escapePipes(f) + ",";
        out += "|";
        for (const auto &f : following) out += escapePipes(f) + ",";
        return out;
    }

    static User parse(const string &line) {
        vector<string> parts = split(line, "|");
        while (parts.size() < 6) parts.push_back("");
        User u;
        u.username = unescapePipes(parts.size() > 0 ? parts[0] : "");
        u.password = unescapePipes(parts.size() > 1 ? parts[1] : "");
        u.bio = unescapePipes(parts.size() > 2 ? parts[2] : "");

        if (parts.size() > 3 && !parts[3].empty()) {
            vector<string> pids = split(parts[3], ",");
            for (const auto &x : pids) if (!x.empty()) {
                try { 
                    u.posts.push_back(stoi(x));
                 }
                     catch(...) {}
            }
        }
        if (parts.size() > 4 && !parts[4].empty()) {
            vector<string> fls = split(parts[4], ",");
            for (const auto &x : fls) if (!x.empty()) u.followers.push_back(unescapePipes(x));
        }
        if (parts.size() > 5 && !parts[5].empty()) {
            vector<string> flw = split(parts[5], ",");
            for (const auto &x : flw) if (!x.empty()) u.following.push_back(unescapePipes(x));
        }
        return u;
    }

private:
    static string escapePipes(const string &s) {
        string out;
        for (char ch : s) {
            if (ch == '|') out += "&#124;";
            else out += ch;
        }
        return out;
    }
    static string unescapePipes(const string &s) {
        string out = s;
        size_t pos = 0;
        while ((pos = out.find("&#124;", pos)) != string::npos) {
            out.replace(pos, 6, "|");
            pos += 1;
        }
        return out;
    }
};

// --- App ---
class App {
    vector<User> users;
    vector<Post> posts;
    int postCounter = 1;

    const string USERS_FILE = "users.txt";
    const string POSTS_FILE = "posts.txt";

public:
    App()
     { 
        loadAll();
     }
    ~App()
     { 
        saveAll(); 
    }

    void saveAll() {
        vector<string> ulines;
        for (const auto &u : users) ulines.push_back(u.serialize());
        FileHandler::saveLines(USERS_FILE, ulines);

        vector<string> plines;
        for (const auto &p : posts) plines.push_back(p.serialize());
        FileHandler::saveLines(POSTS_FILE, plines);
    }

    void loadAll() {
        users.clear(); posts.clear(); postCounter = 1;
        vector<string> ulines = FileHandler::loadLines(USERS_FILE);
        for (auto &ln : ulines) users.push_back(User::parse(ln));
        vector<string> plines = FileHandler::loadLines(POSTS_FILE);
        for (auto &ln : plines) {
            Post p = Post::parse(ln);
            posts.push_back(p);
            if (p.getId() >= postCounter) postCounter = p.getId() + 1;
        }
    }

    void registerUser() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string uname;
        cout << "Enter username: ";
        getline(cin, uname);
        if (uname.empty()) 
        { cout << "Username cannot be empty.\n";
             return;
             }
        if (findUserIndex(uname) != -1) 
        { 
            cout << "Username already exists!\n";
             return;
             }
        cout << "Enter password: ";
        string pass = maskPassword();
        cout << "Enter bio: ";
        string bio;
         getline(cin, bio);
        users.emplace_back(uname, pass, bio);
        saveAll();
        cout << "Registration successful!\n";
    }

    int loginUser() {
        string uname;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        string pass = maskPassword();
        int idx = findUserIndex(uname);
        if (idx == -1) 
        {
             cout << "Username not found.\n"; 
             return -1;
             }
        if (!users[idx].checkPassword(pass)) 
        { cout << "Wrong password.\n"; 
            return -1;
         }
        cout << "Login successful. Welcome, " << uname << "!\n";
        return idx;
    }

    void createPost(int uid) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string content;
        cout << "Enter post content: ";
        getline(cin, content);
        if (content.empty())
        {
             cout << "Post content cannot be empty.\n";
              return; 
            }
        Post p(postCounter++, users[uid].getUsername(), content);
        posts.push_back(p);
        users[uid].addPostId(p.getId());
        saveAll();
        cout << "Post created successfully with ID: " << p.getId() << "\n";
    }

    void editPost(int uid) {
        cout << "Enter post ID to edit: ";
        int pid; if (!(cin >> pid))
        { cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n"; 
        return;
     }
        int pidx = findPostIndex(pid);
        if (pidx == -1) 
        { cout << "Post not found.\n"; 
            return; 
        }
        if (posts[pidx].getAuthor() != users[uid].getUsername())
        { cout << "You can't edit other's post.\n"; 
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Current content:\n" << posts[pidx].getContent() << "\n";
        cout << "Enter new content: ";
        string nc;
         getline(cin, nc);
        if (nc.empty()) 
        { cout << "Content empty. Aborted.\n";
            return; 
        }
        posts[pidx].setContent(nc);
        saveAll();
        cout << "Post updated.\n";
    }

    void deletePost(int uid) {
        cout << "Enter post ID to delete: ";
        int pid; if (!(cin >> pid)) 
        { cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input.\n"; 
            return; 
        }
        int pidx = findPostIndex(pid);
        if (pidx == -1) 
        { cout << "Post not found.\n"; 
            return;
         }
        if (posts[pidx].getAuthor() != users[uid].getUsername())
         { 
            cout << "You can't delete other's post.\n"; 
            return; 
        }
        posts.erase(posts.begin() + pidx);
        users[uid].removePostId(pid);
        saveAll();
        cout << "Post deleted.\n";
    }

    void showNewsFeed() {
        if (posts.empty())
         {
             cout << "No posts yet.\n"; 
            return; 
        }
        cout << " News Feed (Newest first) \n";
        for (auto it = posts.rbegin(); it != posts.rend(); ++it)
         it->printSummary();
    }

    void likeOrComment(int uid) {
         int pid; 
        cout << "Enter Post ID: ";
        cin>>pid;
        if (!pid) 
        {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
             cout << "Invalid input.\n"; 
             return;
             }
        int pidx = findPostIndex(pid);
        if (pidx == -1) 
        { 
            cout << "Post not found.\n"; 
            return; 
        }
        cout<<"1. like: \n";
        cout<<" 2. comments: \n";
        int choice; 
        cin>>choice;
        if (!choice)
        { cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
              cout << "Invalid input.\n";
               return; 
            }
        if (choice == 1) {
            posts[pidx].addLike();
            saveAll();
            cout << "Post liked.\n";
        } else if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter comment: ";
            string comment; 
            getline(cin, comment);
            if (comment.empty())
            { 
                cout << "Comment cannot be empty.\n";
                 return; 
                }
            posts[pidx].addComment(comment);
            saveAll();
            cout << "Comment added.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void followUnfollow(int uid) {
        cout << "Enter username to follow/unfollow: ";
        string target;
         cin >> target;
        if (target == users[uid].getUsername()) 
        { 
            cout << "Cannot follow yourself.\n";
             return;
            }
        int idx = findUserIndex(target);
        if (idx == -1)
        { 
            cout << "User not found.\n";
             return; 
            }
        auto &following = users[uid].getFollowing();
        if (find(users[uid].getFollowing().begin(), users[uid].getFollowing().end(), target) != users[uid].getFollowing().end()) {
            users[uid].removeFollowing(target);
            users[idx].removeFollower(users[uid].getUsername());
            saveAll();
            cout << "Unfollowed " << target << "\n";
        } else {
            users[uid].addFollowing(target);
            users[idx].addFollower(users[uid].getUsername());
            saveAll();
            cout << "Followed " << target << "\n";
        }
    }

    void showFollowersFollowing(int uid) {
        cout << "Followers:\n";
        for (const auto &f : users[uid].getFollowers()) cout << " - " << f << "\n";
        cout << "Following:\n";
        for (const auto &f : users[uid].getFollowing()) cout << " - " << f << "\n";
    }

    void viewProfileAny(int uid) {
        cout<<" 1. view another user: \n";
        cout<<" 2. view my profile :\n";
        int choice; 
        cin>>choice;
        if (!choice) 
        { 
            cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
             cout << "Invalid input.\n"; 
             return; 
            }
        if (choice == 1) {
            cout << "Enter username to view: ";
            string target; 
            cin >> target;
            int idx = findUserIndex(target);
            if (idx == -1) 
            { cout << "User not found.\n";
                 return;
                 }
            users[idx].showProfile();
            cout << "Posts:\n";
            for (int pid : users[idx].getPosts()) {
                int pidx = findPostIndex(pid);
                if (pidx != -1) posts[pidx].printSummary();
            }
        } else if (choice == 2) {
            users[uid].showProfile();
            cout << "Your posts:\n";
            for (int pid : users[uid].getPosts()) {
                int pidx = findPostIndex(pid);
                if (pidx != -1) posts[pidx].printSummary();
            }
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void searchUsers() {
        cout << "Enter search keyword: ";
        string key; cin >> key;
        cout << "Matching users:\n";
        for (const auto &u : users) {
            if (u.getUsername().find(key) != string::npos) cout << " - " << u.getUsername() << "\n";
        }
    }

    int findUserIndex(const string &uname) const {
        for (int i = 0; i < (int)users.size(); ++i) {
        if (users[i].getUsername() == uname) 
        return i;
        }
        return -1;
    }

    int findPostIndex(int pid) const {
        for (int i = 0; i < (int)posts.size(); ++i)
        {
             if (posts[i].getId() == pid) 
             return i;
        }
        return -1;
    }

    void userDashboard(int uid) {
        int ch;
        cin>>ch;
        do {
            cout << "\n===== User Dashboard =====\n";
            cout << "1. Create post\n";
            cout << "2. Edit post\n";
            cout << "3. Delete post\n";
            cout << "4. View news feed\n";
            cout << "5. Like/Comment on post\n";
            cout << "6. Follow/Unfollow user\n";
            cout << "7. Show followers/following\n";
            cout << "8. View profile (any)\n";
            cout << "9. Search users\n";
            cout << "0. Logout\n";
            cout << "Enter choice: ";
            if (!ch) 
            { cin.clear(); 
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
                case 4: showNewsFeed();
                 break;
                case 5: likeOrComment(uid);
                 break;
                case 6: followUnfollow(uid);
                 break;
                case 7: showFollowersFollowing(uid); 
                break;
                case 8: viewProfileAny(uid);
                 break;
                case 9: searchUsers(); break;
                case 0: cout << "Logging out...\n"; 
                break;
                default: cout << "Invalid choice. Try again.\n"; 
                break;
            }
        } while (ch != 0);
    }

    static string maskPassword() {
        string pass;
#ifdef _WIN32
        int ch;
        while (true) {
            ch = _getch();
            if (ch == 13)
             {
                 cout << "\n";
                  break; 
                } // Enter
            else if (ch == 8 || ch == 127) { // backspace
                if (!pass.empty()) {
                    pass.pop_back();
                    cout << "\b \b";
                }
            } else if (ch == 0 || ch == 224) {
                // special key, skip second code
                _getch();
            } else {
                pass.push_back((char)ch);
                cout << '*';
            }
        }
#else
        // Disable echo, read line, re-enable echo
        struct termios oldt, newt;
        if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
            newt = oldt;
            newt.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            getline(cin, pass);
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            cout << "\n";
        } else {
            // fallback
            getline(cin, pass);
        }
#endif
        return pass;
    }
};

// --- main ---
int main() {
    App socialApp;
    int choice;
    cin>>choice;
    do {
        cout << "\n==== Welcome to Social Media App ====\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        if (! choice) {
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
                if (uid != -1) 
                socialApp.userDashboard(uid);
                break;
            }
            case 3:
                cout << "Exiting... Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
                break;
        }
    } while (choice != 3);
    return 0;
}