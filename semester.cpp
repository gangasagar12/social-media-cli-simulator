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
            
        }
    }
}