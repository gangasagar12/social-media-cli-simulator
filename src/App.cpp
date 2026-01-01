#include "App.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <functional>
#include <ctime>

// Platform-specific includes for password masking
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

string hashPassword(const string &password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

void App::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

vector<string> split(const string &s, const string &delim) {
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

string getCurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

App::App() { loadAll(); }

App::~App() { saveAll(); }

void App::saveAll() {
    vector<string> u, p;
    for (const auto &x : users) u.push_back(x.serialize());
    for (const auto &x : posts) p.push_back(x.serialize());

    FileHandler::saveBinary(USERS_FILE, u);
    FileHandler::saveBinary(POSTS_FILE, p);
}

void App::loadAll() {
    users.clear(); posts.clear(); postCounter = 1;
    
    // Load users
    auto userLines = FileHandler::loadBinary(USERS_FILE);
    for (const auto &ln : userLines) {
        users.push_back(User::parse(ln));
    }

    // Load posts
    auto postLines = FileHandler::loadBinary(POSTS_FILE);
    for (const auto &ln : postLines) {
        Post p = Post::parse(ln);
        posts.push_back(p);
        if (p.getId() >= postCounter) postCounter = p.getId() + 1;
    }
}

void App::registerUser() {
    clearScreen();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string uname;
    cout << "\tEnter username: ";
    getline(cin, uname);

    if (uname.empty()) {
        cout << "\tUsername cannot be empty.\n"; 
        return; 
    }
    if (findUserIndex(uname) != -1) {
        cout << "\tUsername already exists!\n";
        return; 
    }

    cout << "\n\tEnter password: ";
    string pass = maskPassword();
    cout << "\n\tEnter bio: ";
    string bio; getline(cin, bio);
    users.emplace_back(uname, pass, bio);
    saveAll();

    cout << "\n\tRegistration successful!\n";
    cout << "\n\tPress Enter to continue...";
    cin.get();
    clearScreen();
}

int App::loginUser() {
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

    users[idx].setLastLoginNow();
    saveAll();
    cout << "\tLogin successful. Welcome, " << uname << "!\n";
    cout << "\tPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    clearScreen();
    return idx;
}

void App::createPost(int uid) {
    clearScreen();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string content;
    cout << "\tEnter post content: ";
    getline(cin, content);
    if (content.empty()) {
        cout << "\tPost content cannot be empty.\n";
        return; 
    }
    posts.emplace_back(postCounter++, users[uid].getUsername(), content);
    users[uid].addPostId(posts.back().getId());
    saveAll();
    cout << "\tPost created successfully.\n";
    cout << "\tPress Enter to continue...";
    cin.get();
    clearScreen();
}

int App::findUserPostByPosition(int uid, int position) const {
    string currentUser = users[uid].getUsername();
    vector<int> userPostIndices;
    
    for (int i = 0; i < (int)posts.size(); ++i) {
        if (posts[i].getAuthor() == currentUser) {
            userPostIndices.push_back(i);
        }
    }
    
    reverse(userPostIndices.begin(), userPostIndices.end());
    
    if (position < 1 || position > (int)userPostIndices.size()) {
        return -1;
    }
    
    return userPostIndices[position - 1];
}

void App::displayUserPosts(int uid) const {
    string currentUser = users[uid].getUsername();
    vector<int> userPostIndices;
    
    for (int i = 0; i < (int)posts.size(); ++i) {
        if (posts[i].getAuthor() == currentUser) {
            userPostIndices.push_back(i);
        }
    }
    
    if (userPostIndices.empty()) {
        cout << "\tYou have no posts.\n";
        return;
    }
    
    reverse(userPostIndices.begin(), userPostIndices.end());
    
    cout << "\n\t=== Your Posts ===\n";
    for (int i = 0; i < (int)userPostIndices.size(); ++i) {
        int postIdx = userPostIndices[i];
        cout << "\tPost " << (i + 1) << ":\n";
        posts[postIdx].printSummary();
    }
}

void App::displayAllPosts() const {
    if (posts.empty()) {
        cout << "\n\tNo posts available.\n";
        return;
    }
    
    cout << "\n\t======= Feed ======\n";
    for (auto it = posts.rbegin(); it != posts.rend(); ++it)
        it->printSummary();
    cout << "\n\tTotal posts: " << posts.size() << "\n";
}

void App::displayAllPostsForInteraction() const {
    if (posts.empty()) {
        cout << "\tNo posts available.\n";
        return;
    }
    
    cout << "\n\t=== All Posts ===\n";
    int position = 1;
    for (auto it = posts.rbegin(); it != posts.rend(); ++it, ++position) {
        cout << "\n\tPost " << position << ":\n";
        it->printSummary();
    }
}

int App::findPostByPosition(int position) const {
    if (position < 1 || position > posts.size()) {
        return -1;
    }
    return posts.size() - position;
}

void App::editPost(int uid) {
    clearScreen();
    displayUserPosts(uid);
    string currentUser = users[uid].getUsername();
    vector<int> userPostIndices;
    
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
    posts[pidx].setContent(nc);
    saveAll();
    cout << "\tPost " << postNum << " updated.\n";
    cout << "\tPress Enter to continue...";
    cin.get();
    clearScreen();
}

void App::deletePost(int uid) {
    clearScreen();
    displayUserPosts(uid);
    
    string currentUser = users[uid].getUsername();
    vector<int> userPostIndices;
    
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

void App::showFeed() {
    clearScreen();
    displayAllPosts();
    cout << "\tPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    cin.get();
    clearScreen();
}

void App::likeOrComment(int uid) {
    if (posts.empty()) {
        cout << "\n\tNo posts available.\n";
        return;
    }
    clearScreen();
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

void App::followUnfollow(int uid) {
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
    
    bool isFollowing = find(following.begin(), following.end(), target) != following.end();
    
    if (isFollowing) {
        users[uid].removeFollowing(target);
        users[idx].removeFollower(currentUser);
        saveAll();
        cout << "\n\tYou have unfollowed " << target << "\n";
    } else {
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

void App::showFollowersFollowing(int uid) {
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

void App::viewProfileAny(int uid) {
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
        int postCount = 0;
        for (auto it = posts.rbegin(); it != posts.rend(); ++it) {
            if (it->getAuthor() == target) {
                it->printSummary();
                postCount++;
                if (postCount >= 5) break;
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
        int postCount = 0;
        for (auto it = posts.rbegin(); it != posts.rend(); ++it) {
            if (it->getAuthor() == users[uid].getUsername()) {
                it->printSummary();
                postCount++;
                if (postCount >= 5) break;
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

void App::searchUsers() {
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

int App::findUserIndex(const string &uname) const {
    for (int i = 0; i < (int)users.size(); ++i)
        if (users[i].getUsername() == uname)
            return i;
    return -1;
}

int App::findPostIndex(int pid) const {
    for (int i = 0; i < (int)posts.size(); ++i)
        if (posts[i].getId() == pid)
            return i;
    return -1;
}

void App::userDashboard(int uid) {
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

string App::maskPassword() {
    string pass;
#ifdef _WIN32
    int ch;
    while ((ch = _getch()), ch != 13) {
        if (ch == 8 || ch == 127) {
            if (!pass.empty()) {
                pass.pop_back(); 
                cout << "\b \b"; 
            }
        } 
        else if (ch == 0 || ch == 224) { _getch(); }
        else { 
            pass.push_back((char)ch); 
            cout << '*'; 
        }
    }
    cout << "\n";
#else  
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