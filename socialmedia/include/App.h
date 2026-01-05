#ifndef APP_H
#define APP_H

#include "User.h"
#include "Post.h"
#include "FileHandler.h"
#include <vector>

class App {
    std::vector<User> users;
    std::vector<Post> posts;
    int postCounter = 1;
    const std::string USERS_FILE = "users.bin";
    const std::string POSTS_FILE = "posts.bin";
    
public:
    App();
    ~App();
    
    void saveAll();
    void loadAll();
    
    void registerUser();
    int loginUser();
    
    void createPost(int uid);
    int findUserPostByPosition(int uid, int position) const;
    void displayUserPosts(int uid) const;
    void displayAllPosts() const;
    void displayAllPostsForInteraction() const;
    int findPostByPosition(int position) const;
    void editPost(int uid);
    void deletePost(int uid);
    void showFeed();
    void likeOrComment(int uid);
    void followUnfollow(int uid);
    void showFollowersFollowing(int uid);
    void viewProfileAny(int uid);
    void searchUsers();
    
    int findUserIndex(const std::string &uname) const;
    int findPostIndex(int pid) const;
    
    void userDashboard(int uid);
    
    static std::string maskPassword();
    
private:
    static std::string hashPassword(const std::string &password);
    static void clearScreen();
};

#endif // APP_H