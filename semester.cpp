#include <iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
#include<conio.h>   // for getch()
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
    vector<int>posts;   // store post id
    vector<string>followers;
    vector<string>following;

};
vector<User> users;
vector<Post>posts;
int postCounter=1;
// utility functions to split by delimiter string
vector<string> split(const string &src , const string &delimiter){
    vector<string> out;
    string s =src;
    size_t pos=0;
     while((pos=s.find(delimiter))!=string::npos){
        out.push_back(s.substr(0,pos));
        s.erase(0, pos+ delimiter.length());
     }
     out.push_back(s);
     return out;
}
// function that research user by username
int finduser(const string &uname){
    for ( int i=0;i<(int)users.size();i++){
        if(users[i].username==uname)
        return i;

    }
    return -1;
}
// function to save the all data into the file system
void savedata(){
  // users :
  ofstream f("users.txt");
if(!f){
    cerr<< " error opening users.txt for writing: \n";
    return;
}
 for (auto &u : users){
    f<<u.username<<"|"
     <<u.password<<"|"
     <<u.bio<<"|";
     // save user's post ids
     for ( int pid: u.posts){
        f<<pid<<",";
     }
     f<<"|";
     // save followers
     for ( const string &fol: u.followers){
        f<<fol<<",";
     }
     f<<"|";
     // save following
     for ( const string &fol: u.following){
        f<<fol<<",";
     }
     f<<"\n";
 }
 f.close();
 // posts 
 ofstream p("posts.txt");
    if(!p){
        cerr<<" error opening posts.txt for writing \n";
        return ;
    }
    for (auto &po : posts){
        p<<po.id<<"|"
         <<po.author<<"|"
         <<po.content<<"|"
         <<po.likes<<"|";
        // save comments separated by "~~"
        for ( const string &c: po.comments)
            p<<c<<"~~";
        p<<"\n";
    }
    p.close();
}
// load all data fron the files

void loadData() {
    users.clear();
    posts.clear();
    postCounter = 1;

    ifstream f("users.txt");
    if (f) {
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            vector<string> parts = split(line, "|");
            // Expect 6 fields: username, password, bio, posts, followers, following
            while (parts.size() < 6) parts.push_back("");
            User u;
            u.username = parts[0];
            u.password = parts[1];
            u.bio      = parts[2];

            // posts
            if (!parts[3].empty()) {
                vector<string> pids = split(parts[3], ",");
                for (auto &x : pids)
                    if (!x.empty())
                        u.posts.push_back(stoi(x));
            }

            // followers
            if (!parts[4].empty()) {
                vector<string> fol = split(parts[4], ",");
                for (auto &x : fol)
                    if (!x.empty())
                        u.followers.push_back(x);
            }

            // following
            if (!parts[5].empty()) {
                vector<string> fol = split(parts[5], ",");
                for (auto &x : fol)
                    if (!x.empty())
                        u.following.push_back(x);
            }

            users.push_back(u);
        }
        f.close();
    }

    ifstream p("posts.txt");
    if (p) {
        string line;
        while (getline(p, line)) {
            if (line.empty()) continue;
            vector<string> parts = split(line, "|");
            // Expect 5 fields: id, author, content, likes, commentsCombined
            while (parts.size() < 5) parts.push_back("");
            Post po;
            try {
                po.id = stoi(parts[0]);
            } catch (...) { continue; }
            po.author  = parts[1];
            po.content = parts[2];
            try {
                po.likes = stoi(parts[3]);
            } catch (...) { po.likes = 0; }

            // comments separated by "~~"
            if (!parts[4].empty()) {
                vector<string> cms = split(parts[4], "~~");
                for (auto &c : cms)
                    if (!c.empty())
                        po.comments.push_back(c);
            }

            posts.push_back(po);
            if (po.id >= postCounter) postCounter = po.id + 1;
        }
        p.close();
    }
}


//  main function  
void  registeruser(){
    User u;
    cout<<" enter the username :";
    cin>>u.username;

    if(finduser(u.username)!=-1){
        cout<<" username have been already taken . try again.\n";
        return ;
        
    }
    cout<<" enter the password : ";
    cin>>u.password;
    cin.ignore();
    cout<<"enter your bio : ";
    getline(cin, u.bio);
    users.push_back(u);
    cout<<" registration successful .\n";
}
// for the user login
int userlogin(){
    string name,pass;
    cout<<"enter your name: ";
    cin>>name;
    cout<<" enter  your password;";
    cin>>pass;
    int idx=finduser(name);
    if(idx!=-1 && users[idx].password==pass){
        cout<<" ;login sucessfully in your accoutn: \n";
        return idx;

    }
    cout<<" invilid username or password . please try again.\n";
    return -1;
}
//  function of the ceate post 
void createpost( int uid ){
    Post p;
    p.id=postCounter++;
    p.author=users[uid].username;
    cin.ignore();
    cout<<" enter the content of your post: ";
    getline(cin,p.content);
    p.likes=0;
    posts.push_back(p);
    users[uid].posts.push_back(p.id);
    cout<<" post created sucessfully.\n";
    cout<<" post id: "<<p.id<<"\n";



}\

// function  for the view posts of the account
void viewposts(){
    cout<<" \n News feed: \n";
    for(auto it =posts.rbegin(); it!=posts.rend(); ++it){
        cout<<" Post ID: "<<it-> id<<" by "<< it-> author<<"\n";
        cout<< it-> content <<"\n";
        cout<<" Likes: "<< it-> likes << " | Comments: "<< it-> comments.size()
        <<"\n-----------------------\n";
    }
}
// function for the like and commant of the posst
void likecomment(){
    int pid;
    cout<<" enter the post id to like / comment: ";
    cin>>pid;
    for( auto &p: posts){
        if(p.id==pid){
            int ch;
            cout<<" 1. like\n";
            cout<<" 2. comment\n";
            cout<<" enter your choice: ";
            cin>>ch;
            if (ch==1){
                p.likes++;
                cout<<" likes: ";

            }
            else{
                string c;
                cin.ignore();
                cout<<" enter your comment : ";
                getline(cin,c);
                p.comments.push_back(c);
                cout<<" comment added sucessfully: \n";
            }
            return ;
        }

    }
    cout<<" post not found : \n";
}
void followunfollow(int uid){
    string uname;
    cout<<" enter the userame to follow / unfollow: ";
    cin>>uname;
    int idx= finduser(uname);
    if (idx==-1){
        cout<<" user not found: \n";
        return ;

    }
    auto &flist=users[uid].following;
    if(find(flist.begin(),flist.end(),uname)!=flist.end()){
        flist.erase(remove(flist.begin(),flist.end(),uname),flist.end());
        cout<< " you  have been  unfollowed: "<<uname<<"\n";

    }
    else{
        flist.push_back(uname);
        cout<<" you have been followed : "<<uname<<"\n";

    }
}
//  view profile 
void viewprofile(int uid){
    cout<<" PROFILE : ";
    cout<<" username: "<<users[uid].username<<"\n";
    cout<<" Bio : "<<users[uid].bio<<"\n";
    cout<< " posts: \n";
    for (int pid: users[uid].posts){
        for(auto &p : posts){
            if(p.id==pid){
                cout<<" "<<p.id<<": "<<p.content<<"\n";
                
            }
        }
    }
}

// function for the search user 
void searchuser(){
    string key;

    cout<<" enter the username to serach: ";
    cin>>key;
    cout<<" mathching users: \n";
    for( auto &u: users){
        if(u.username.find(key)!=string::npos){
            cout<<" -"<<u.username<<"\n";

        }
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
            case 1: createpost(uid);
            break;
            case 2: viewposts();
            break;
            case 3: likecomment();
            break;
            case 4: followunfollow(uid);
            break;
            default: cout<<" invalid choice";
            break;

            
        }
    } while (choice != 0);
    
}
//  body part of the main function
int main(){
    loadData();
    int choice;
    do {
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
            int uid= userlogin();
            if(uid!=-1){
                dashboard(uid);
                
            }
            else if (choice==3){
                savedata();
                cout<<" exiting the program tra again ans again.\n";
                exit(0);
            }
            else{
                cout<<" invilid choice please try againn.\n";
            }
        }
    } while (choice !=3);
    return 0;
}
