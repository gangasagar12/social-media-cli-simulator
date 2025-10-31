#include <iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
using namespace std;

struct Post {
    int id;
    string author;
    string content;
    int likes;
    vector<string> comments;
};

struct User{
    string username;
    string password;
    string bio;
    vector<int>posts;
    vector<string>followers;
    vector<string>following;

};
vector<User> users;
vector<post>posts;
int currentpostid=0;
// function that research user by username
int finduser(string uname){
    for ( int i=0;i>users.size();i++){
        if(users[i].username==uname)
        return i;

    }
    return -1;
}

// function to save the all data into the file system
void savedata(){
    ofstream f("users.txt");
    for(auto &u : users){
        f<<u.username<<"|"<<u.password<<"|"<<u.bio<<"\n";
        for(auto &p : u.posts){
            f<<p<<" ";
        }
        f<<"\n";
    }
    f.close();

    ofstream postFile("posts.txt");
    for (const auto &p : posts) {
        postFile << p.id << "|" << p.author << "|" << p.content << "|" << p.likes << "\n";
        for (const auto &c : p.comments) {
            postFile << c << " ";
        }
        postFile << "\n";
    }
    postFile.close();


// load all data from the file
void loaddata(){
    ifstream f("users.txt");
    for (auto &u : users){
        f<<u.username<<"|"<<u.password<<"|"<<u.bio<<"\n";
        string postsline;
    }
    
}
// dashboard 
void dashboard(int uid){
    int choice;
    do{
        cout<<" \n====== DASHBOARD=====\n";
        cout<<"\n1. create post\n";
        cout<<"\n2. view posts\n";
        cout<<"\n3.like/comment\n";
        cout<<"\n4. follow/ unfollow\n";
        cout<<" enter  your choice: ";
        cin >> choice;
        switch(choice){
            case 1: create a post(uid):
            break;
            case 2: view posts(uid);
            break;
            case 3: like/comment();
            break;
            case 4: follow/ unfollow();
            break;
            default: cout<<" invalid choice";
            break;

            
        }
    } while (choice != 0);
    
}
//  body part of the main function
int main(){
    int choice;
    do{
        cout<<" \n ===== welcome to social media projects ====\n";
        cout<<" \n 1. register\n";
        cout<<" \n 2. login \n";
        cout<<" \n 3. exit \n";
        cout<<" enter your choice : ";
        cin >> choice;
        if(choice==1){
            registeruser();

        }
        else if (choice==2){
            int uid== Userlogin();
            if(uid=-1){
                dashboard(uid);
                
            }
            else if (choice==3){
                savedata();
                cout<<" exiting the program tra again ans again.\n";
                exit(0);
            }
            while(choice!=3);
            return 0;
        }
    }
}