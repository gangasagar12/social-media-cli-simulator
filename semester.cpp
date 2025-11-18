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
    user u;
    size_t p1 =line.find("|");
    size_t p2= line.find("|",P1+1);    
      u.username = line.substr(0, p1);
        u.password = line.substr(p1+1, p2-p1-1);
        u.bio = line.substr(p2+1);
        users.push_back(u);
    }
    f.close();
    ifstream p("posts.txt");
    while (getline(p, line)) {
        Post po;
        size_t p1 = line.find("|");
        size_t p2 = line.find("|", p1+1);
        size_t p3 = line.find("|", p2+1);
          po.id = stoi(line.substr(0, p1));
        po.author = line.substr(p1+1, p2-p1-1);
        po.content = line.substr(p2+1, p3-p2-1);
        po.likes = stoi(line.substr(p3+1));
        posts.push_back(po);
        if (po.id >= postCounter) postCounter = po.id + 1;

    }
    f.close();


    
}
//  main function  
void  registeruser(){
    user u;
    cout<<" enter the username :";
    cin>>.username;

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
    post p;
    p.id=postcounter++;
    p.author=users[uid].username;
    cin.ignore();
    cout<<" enter the content of your post: ";
    getline(cin,p.content);
    p.likes=0;
    posts.push_back(p);
    users[uid].posts.push_back(p.id);
    cout<<" post created sucessfully.\n";
    cout<<" post id: "<<p.id<<"\n";



}

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
    cout<<" Bio : ";users[uid].bio<<"\n";
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