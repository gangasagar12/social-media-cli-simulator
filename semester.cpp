#include <iostream>   // for input output operations
#include <fstream>   // for file operations
#include <vector>   // for dynamic array
#include <string>
#include <algorithm>            // for algorithms like find remove
#include <limits>        // for numeric limit
#include <ctime>
#include <functional>   //  for the hash password


//  platform - specific includes for password masking

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
    static void saveLines(const string &filename, const vector<string> &lines) {  // save vector of string to file
        ofstream fout(filename);
        if (!fout) {
            cerr << "Error saving file: " << filename << "\n";
            return;
        }
        for (const auto &ln : lines) fout << ln << "\n";
    }

    static vector<string> loadLines(const string &filename) {   // load lines from file into vector
        vector<string> lines;  // vector to store lines
        ifstream fin(filename);  // open file for reading
        if (!fin) 
        return lines;   // return empty vector if file doesnt exits
       
        string line;  // read file line by line
        while (getline(fin, line))
            if (!line.empty()) lines.push_back(line);  // skip empty lines
        return lines;
    }
};


class Post {
    int id;
    string author;
    string content;
    int likes;
    vector<pair<string, string>> comments; // {timestamp, comment}
    string created_ts;    // creation timestamps
    string last_edit_ts;

public:
    Post() : id(0), likes(0) {}

    Post(int i, const string &a, const string &c)
        : id(i), author(a), content(c), likes(0) {
        created_ts = getCurrentTimestamp();
    }
//  accessosrs
    int getId() const {
         return id; 
        }
    
    string getAuthor() const {
         return author; 
        }
    string getContent() const {
         return content; 
        }
    int getLikes() const {
         return likes; 
        }
    
    const vector<pair<string, string>> &getComments() const {
         return comments; 
        }
    string getCreatedTS() const {
         return created_ts; 
        }

    void setContent(const string &c) { 
        content = c; 
        last_edit_ts = getCurrentTimestamp();  // update edit timestap
    }
    void addLike()
         { ++likes;   //  increment like count
             }

    void addComment(const string &c) { 
        comments.push_back({getCurrentTimestamp(), c});   // add comment with timestamp
    }

    string serialize() const {  // convert post object to string for file storage
        string out = to_string(id) + "|" +
                escapePipes(author) + "|" +
                escapePipes(content) + "|" +
                to_string(likes) + "|" +
                escapePipes(created_ts) + "|" +
                escapePipes(last_edit_ts) + "|";

        for (const auto &comment : comments) {
            out += escapePipes(comment.first) + "~!~" + escapePipes(comment.second) + "~~";
        }
        return out;
    }

    static Post parse(const string &line) {  
        Post p;         // create default post object
        auto parts = split(line, "|");   // split by pipe delimeter
        while (parts.size() < 7) parts.push_back("");   // ensure at least 7 parts
        try {
             p.id = stoi(parts[0]); } 
             catch (...) 
             { p.id = 0; }

        p.author = unescapePipes(parts[1]);
        p.content = unescapePipes(parts[2]);
        try {
             p.likes = stoi(parts[3]); } 
             catch (...) 
             { p.likes = 0; }
        p.created_ts = unescapePipes(parts[4]);
        p.last_edit_ts = unescapePipes(parts[5]);
        if (!parts[6].empty()) {

            vector<string> cvec = split(parts[6], "~~");
            for (const auto &c : cvec) {

                if (c.empty()) continue;
                auto tp = split(c, "~!~");  // split timestamp and comment
                if (tp.size() == 2)

                    p.comments.push_back({unescapePipes(tp[0]), unescapePipes(tp[1])});
                else if (tp.size() == 1)
                    p.comments.push_back({"", unescapePipes(tp[0])});
            }
        }
        return p;
    }

    void printSummary() const {  // display post summary
        cout << "Post ID: " << id << " | Author: " << author << " | Likes: " << likes
             << " | Comments: " << comments.size()
             << " | Created: " << created_ts << " | Last edit: " << last_edit_ts << "\n";
    }

    void printFull() const {   // function for display full post details with comments
        cout << "Post ID: " << id << " | Author: " << author
             << "\n" << content << "\n";
        cout << "Likes: " << likes << " | Comments: " << comments.size() 
             << "\nCreated: " << created_ts << " | Last edit: " << last_edit_ts << "\n";

        if (!comments.empty()) {  // display cmments if any exits
            cout << "Comments:\n";
            for (const auto &c : comments)
                cout << " [" << c.first << "] - " << c.second << "\n";
        }
    }

private:  // escape pipe character to avoid conflict with delimeter
    static string escapePipes(const string &s) {
        string out;
        for (char ch : s) out += (ch == '|') ? "&#124;" : string(1, ch);
        return out;
    }
    //  restore escaped pipe charcaters
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


class Person {
protected:
    string username, bio;

public:
    Person() = default;   // default constructor
    Person(const string &u, const string &b) : username(u), bio(b) {}
    virtual ~Person() = default;  // virtual destructor for polymorphism
    
    virtual void showProfile() const {  //  virtual function for displaying profile , can be overridden
        cout << " username: " << username << "\n";
        cout << " bio: " << bio << "\n";
    }
    string getUsername() const { return username; }
    string getBio() const { return bio; }
};

string hashPassword(const string &password);

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
    //  check if provided password matches stored hash
    bool checkPassword(const string &p) const { 
        return password == hashPassword(p); 
    }
    const vector<int> &getPosts() const {
         return posts; 
        }
    const vector<string> &getFollowers() const {
         return followers;
        }
    const vector<string> &getFollowing() const {
         return following; 
        }
    string getCreatedTS() const {
         return created_ts; 
        }
    string getLastLoginTS() const {
         return last_login_ts; 
        }
//  update last login time to current
    void setLastLoginNow() {
         last_login_ts = getCurrentTimestamp(); 
        }
    void addPostId(int pid) {
         posts.push_back(pid); 
        }
    void removePostId(int pid) {
         posts.erase(remove(posts.begin(), posts.end(), pid), posts.end()); 
        }
    void addFollower(const string &f) {
        if (find(followers.begin(), followers.end(), f) == followers.end()) followers.push_back(f); 
    }
    void removeFollower(const string &f) {
        followers.erase(remove(followers.begin(), followers.end(), f), followers.end()); 
    }
    void addFollowing(const string &f) {        // manage follwing list ( avoid duplicats)
        if (find(following.begin(), following.end(), f) == following.end()) following.push_back(f); 
    }
    void removeFollowing(const string &f){
         following.erase(remove(following.begin(), following.end(), f), following.end()); 
    }
    //  update bio
    void setBio(const string &b) {
         bio = b; 
        }

        // override showprofile to display user-specific information
    void showProfile() const override {  
        cout << " username: " << username << "\n";
        cout << " bio: " << bio << "\n";
        cout << " followers: " << followers.size() << " | following: " 
        << following.size() << "\n";

        cout << " posts: " << posts.size() << "\n";
        cout << " joined: " << created_ts << "\n";
        if (!last_login_ts.empty())
            cout << " last login: " << last_login_ts << "\n";
    }

    string serialize() const {
        string out = escapePipes(username) + "|" +
         escapePipes(password) + "|" + 
         escapePipes(bio) + "|";
                                    //  serialize post id as comman seperated list
        for (int pid : posts) out += to_string(pid) + ",";
        out += "|"; 
                        //  serialize followers as comman seperated list
        for (const auto &f : followers) out += escapePipes(f) + ",";
        out += "|";  
            //  serialize followers as comman seperate list
        for (const auto &f : following) out += escapePipes(f) + ",";

        out += "|" + escapePipes(created_ts) + "|" + escapePipes(last_login_ts);
        return out;
    }

    static User parse(const string &line) {
        auto parts = split(line, "|");
        while (parts.size() < 8) parts.push_back("");  // ensure 8 parts
        User u;

        u.username = unescapePipes(parts[0]);
        u.password = unescapePipes(parts[1]);
        u.bio = unescapePipes(parts[2]);
        //  parse post id
        if (!parts[3].empty()) {
            for (const auto &x : split(parts[3], ","))
                if (!x.empty()) try {
                     u.posts.push_back(stoi(x)); 
                    } 
                    catch (...) {}
        }

        if (!parts[4].empty()) {
            for (const auto &x : split(parts[4], ","))
                if (!x.empty()) u.followers.push_back(unescapePipes(x));
        }

        if (!parts[5].empty()) {
            for (const auto &x : split(parts[5], ","))
                if (!x.empty()) u.following.push_back(unescapePipes(x));
        }
        u.created_ts = unescapePipes(parts[6]);
        u.last_login_ts = unescapePipes(parts[7]);
        return u;
    }

private:
        //  some escape / unescape function as post
    static string escapePipes(const string &s) {
        string out;
        for (char ch : s) out += (ch == '|') ? "&#124;" : string(1, ch);
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

            // Manage the entire social media application
class App {
    vector<User> users;
    vector<Post> posts;
    int postCounter = 1;
    const string USERS_FILE = "users.txt";   // filr for user data
    const string POSTS_FILE = "posts.txt";      // file for post data
public:
    //  constructor load data from files
    App() { loadAll(); }
    // destrcutor load data to files
    ~App() { saveAll(); }

    void saveAll() {  // save all data to files
        vector<string> u, p;            // vector to store serialized data
        for (const auto &x : users) u.push_back(x.serialize());  // serialixed all user
        for (const auto &x : posts) p.push_back(x.serialize());  // serialize all posts

        FileHandler::saveLines(USERS_FILE, u);
        FileHandler::saveLines(POSTS_FILE, p);
    }

    void loadAll() {        // load all data from files
        users.clear(); posts.clear(); postCounter = 1;
        for (const auto &ln : FileHandler::loadLines(USERS_FILE)) 
        users.push_back(User::parse(ln));

        for (const auto &ln : FileHandler::loadLines(POSTS_FILE)) {
            Post p = Post::parse(ln);
            posts.push_back(p);
            //  update post counter to avoid id conflicts
            if (p.getId() >= postCounter) postCounter = p.getId() + 1;
        }
    }

    void registerUser() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // clean input buffer
        string uname;
        cout << "\n\tEnter username: ";
        getline(cin, uname);

        if (uname.empty()) {  // valid username
             cout << "\n\tUsername cannot be empty.\n"; 
             return; 
            }
        if (findUserIndex(uname) != -1) {   // check if username already exits
             cout << "\n\tUsername already exists!\n";
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
        string uname;
        cout << "\n\tEnter username: "; 
        cin >> uname;
        cout << "\n\tEnter password: ";
        string pass = maskPassword();
        int idx = findUserIndex(uname);

        if (idx == -1) {
             cout << "\n\tUsername not found.\n"; 
            return -1; 
        }
        if (!users[idx].checkPassword(pass)) {
             cout << "\n\tWrong password.\n";
            return -1; 
        }

        users[idx].setLastLoginNow();  // update last login time
        saveAll();  // save update timestamp
        cout << "\t\tLogin successful. Welcome, " << uname << "!\n";
        cout << "\t\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
         cin.get();
        clearScreen();
        return idx;
    }

    void createPost(int uid) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string content;
        cout << "\n\tEnter post content: ";
        getline(cin, content);
        if (content.empty()) {
            cout << "\n\tPost content cannot be empty.\n";
             return; 
            }
            //  create new post with unique id
        posts.emplace_back(postCounter++, users[uid].getUsername(), content);
        //  add post id to user post list
        users[uid].addPostId(posts.back().getId());
        saveAll();
        cout << "\n\tPost created successfully with ID: " << posts.back().getId() << "\n";
        cout << "\n\tPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void editPost(int uid) {
        cout << "\n\tEnter post ID to edit: ";
        int pid;
        if (!(cin >> pid)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
             cout << "\n\tInvalid input.\n";
            return; 
        }

        int pidx = findPostIndex(pid);  // find post  by id
        if (pidx == -1)
            { cout << "\n\tPost not found.\n"; 
            return; }
            //  check authorization ( only author can  edit)

        if (posts[pidx].getAuthor() != users[uid].getUsername()) {
             cout << "\n\tYou can't edit other's post.\n"; 
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n\tCurrent content:\n" << posts[pidx].getContent() << "\n";
        cout << "\n\tEnter new content: "; string nc; getline(cin, nc);
        if (nc.empty()) {
            cout << "\n\tContent empty. Aborted.\n"; 
            return; 
        }
        //  update post content
        posts[pidx].setContent(nc);
        saveAll();
        cout << "\n\tPost updated.\n";
        cout << "\n\tPress Enter to continue...";
        cin.get();
        clearScreen();
    }

    void deletePost(int uid) {
        cout << "\n\tEnter post ID to delete: ";
        int pid;
        if (!(cin >> pid)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n\tInvalid input.\n";
            return; 
        }
        // find post by id
        int pidx = findPostIndex(pid);
        if (pidx == -1) {
             cout << "\n\tPost not found.\n"; 
             return; 
            }
        if (posts[pidx].getAuthor() != users[uid].getUsername()) {
             cout << "\n\tYou can't delete other's post.\n"; 
            return; 
        
        }// remove the post from posts vector
        posts.erase(posts.begin() + pidx);
        users[uid].removePostId(pid);  // remove post id from users post list
        saveAll();
        cout << "\n\tPost deleted.\n";
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void showNewsFeed() {
        if (posts.empty()) {
             cout << "\n\tNo posts yet.\n"; 
            return; 
        }
        cout << "\n\t Feed (Newest first) \n";
                                                // display posts in reverse order 
        for (auto it = posts.rbegin(); it != posts.rend(); ++it)
         it->printSummary();
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void likeOrComment(int uid) {
        int pid;
        cout << "Enter Post ID: ";
        if (!(cin >> pid)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input.\n"; 
            return; 
        }
        int pidx = findPostIndex(pid);
        if (pidx == -1) {
            cout << "Post not found.\n"; 
            return; 
        }

        cout << "\n\t1.\n\t Like";
        cout<<"\n\t2. comment \n";
        cout<<"\n\tenter choice";
        int choice;
        if (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "\n\tInvalid input.\n"; 
            return; 
        }
        if (choice == 1) {
            posts[pidx].addLike();
            saveAll(); 
            cout << "Post liked.\n";

        } else if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n\tEnter comment: ";
            string comment; 
            getline(cin, comment);
            if (comment.empty()) {
                cout << "\n\tComment cannot be empty.\n"; 
                return; 
            }
            posts[pidx].addComment(comment); 
            saveAll(); 
            cout << "\n\tComment added.\n";
        } else {
            cout << "Invalid choice.\n";
        }

        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void followUnfollow(int uid) {
        cout << "\n\tEnter username to follow/unfollow: ";
        string target; 
        cin >> target;
        if (target == users[uid].getUsername()) {
            cout << "\n\tCannot follow yourself.\n"; 
            return; 
        }
        int idx = findUserIndex(target);
        if (idx == -1) {
            cout << "User not found.\n"; 
            return; 
        }
        // check if already following
        if (find(users[uid].getFollowing().begin(), 
            users[uid].getFollowing().end(), 
            target) != users[uid].getFollowing().end()) {  
                //  unfollow : remove from following and unfollowing list
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
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
    }

    void showFollowersFollowing(int uid) {
        cout << "\n\tFollowers:\n";
        for (const auto &f : users[uid].getFollowers())
         cout << " - " << f << "\n";
        cout << "\n\tFollowing:\n";
        for (const auto &f : users[uid].getFollowing())
         cout << " - " << f << "\n";
        cout << "\n\tPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cin.get();
        clearScreen();
    }

    void viewProfileAny(int uid) {
        cout << "\n\t1. View another user ";
        cout<< "\n\t2. view my profile \n";
        cout<<"\n\tenter choice:\n";
        int choice; if (!(cin >> choice)) {
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
            if (idx == -1) {
                cout << "User not found.\n"; 
                return; 
            }
            users[idx].showProfile();
            cout << "Posts:\n";
            for (int pid : users[idx].getPosts()) {
                int pidx = findPostIndex(pid);
                if (pidx != -1) posts[pidx].printSummary();
            }
        } 
        else if (choice == 2) {
            users[uid].showProfile();
            cout << "Your posts:\n";
            for (int pid : users[uid].getPosts()) {
                int pidx = findPostIndex(pid);
                if (pidx != -1) posts[pidx].printSummary();
            }
        } 
        else {
            cout << "Invalid choice.\n";
        }
        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
         cin.get();
        clearScreen();
    }

    void searchUsers() {
        cout << "\t\tEnter search keyword: ";
        string key; 
        cin >> key;
        cout << "Matching users:\n";
        for (const auto &u : users) 
            if (u.getUsername().find(key) != string::npos)
            cout << " - " << u.getUsername() << "\n";

        cout << "\n\tPress Enter to continue...";
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
            cout <<"\n\t1. Create post: \n";
                 cout<<"\n\t2. Edit post:\n";
                 cout<<"\n\t3. Delete post: \n";
                 cout<<"\n\t4. View feed : \n";
                 cout<<"\n\t5. like/ comment on post: \n";
                 cout<<"\n\t6. follow/ unfollow user: \n";
                 cout<<"\n\t7. show followers/ following: \n";
                 cout<<"\n\t8. view profile any: \n";
                 cout<<"\n\t9. search users: \n";
                 cout<<"\n\t0. logout: \n";
            cout << "\tEnter choice: ";
            if (!(cin >> ch)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                ch = -1;
            }
            switch (ch) {
                case 1: 
                createPost(uid);
                 break;
                case 2:
                 editPost(uid); 
                break;
                case 3:
                 deletePost(uid); 
                 break;
                case 4:
                 showNewsFeed(); 
                break;
                case 5:
                 likeOrComment(uid);
                  break;
                case 6: 
                followUnfollow(uid);
                 break;
                case 7:
                 showFollowersFollowing(uid);
                  break;
                case 8: 
                viewProfileAny(uid);
                 break;
                case 9:
                 searchUsers();
                  break;
                case 0:
                 cout << "Logging out...\n";
                  clearScreen(); 
                  break;
                default:
                 cout << "Invalid choice. Try again.\n";
                  break;
            }
        } while (ch != 0);
    }

    static string maskPassword() {
        string pass;
    #ifdef _WIN32
        int ch;
        while ((ch = _getch()), ch != 13) {   // 13 =enter key
            if (ch == 8 || ch == 127) {         // 8= backspace 127=delet
                if (!pass.empty())              // remove last character
                 { pass.pop_back(); 
                    cout << "\b \b"; }      // erase astrcick from screen
            } 
            else if (ch == 0 || ch == 224) { _getch(); }
            else { pass.push_back((char)ch); cout << '*'; }
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
        cout << "\n\t==== Welcome to SocialShell ====\n";
        cout<<"\n\t1.Register user: \n";
        cout<<"\n\t2.Login user: \n";
        cout<<"\n\t3. Exit: \n";
        cout << "\n\tEnter choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        switch (choice) {
            case 1: socialApp.registerUser(); break;
            case 2: {
                int uid = socialApp.loginUser();
                if (uid != -1) socialApp.userDashboard(uid);
                break;
            }
            case 3:
             cout << "Exiting... Goodbye!\n";
              break;
            default:
             cout << "\n\tInvalid choice. Try again.\n"; 
             break;
        }
    } while (choice != 3);
    return 0;
}