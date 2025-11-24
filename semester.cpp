#include <iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
#include<conio.h>   // for getch()
#include<sstream> // for the string stream
#include<limits> // for numeric_limits
using namespace std;

vector<string> split(const string &s, const string &delim) {
    vector<string> out;
    if (s.empty()) 
    { out.push_back(""); 
        return out; 
    }
    size_t start = 0, pos;
    while ((pos = s.find(delim, start)) != string::npos) {  // find next occurrence
        out.push_back(s.substr(start, pos - start));  // extract substring
        start = pos + delim.size();
    }
    out.push_back(s.substr(start)); // add remaining part
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
        fout.close();
    }

    static vector<string> loadLines(const string &filename) {
        vector<string> lines;
        ifstream fin(filename);
        if (!fin) return lines;
        string line;
        while (getline(fin, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        fin.close();
        return lines;
    }
};


class Post {
    private: 
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
// function to password masking  using getch function
string maskpassword(){
    string pass;
    char ch;  // to store each character
    while(true){
        ch=getch(); // get charcater without echoing
        if(ch==13){
            cout<<"\n";
            break;
        }
        else if(ch==8){
            if(!pass.empty()){
                pass.pop_back();
                cout<<"\b \b"; // erase last char from console
            }
        }
        else if (ch==0 || ch==224){
            getch(); // ignore special keys
        }
        else{
            pass.push_back(ch);
            cout<<"*"; // print asterisk for each char
        }
    }
    return pass;
}

//  main function  
void registerUser() {
    User u;
    cout << "Enter username: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline leftover
    getline(cin, u.username);
    if (u.username.empty()) {
        cout << "Username cannot be empty.\n";
        return;
    }
    if (finduser(u.username) != -1) {
        cout << "Username already exists!\n";
        return;
    }

    cout << "Enter password: ";
    u.password = maskpassword();

    cout << "Enter bio: ";
    getline(cin, u.bio);

    users.push_back(u);
    savedata(); // persist immediately
    cout << "Registration successful!\n";
}
// for the user login
int userlogin(){
    string name,pass;
    cout<<"enter your name: ";
    cin.clear(); // clear any previous error flags
    cin>>name;
    cout<<" enter  your password;";
    cin>>pass;
 
    pass=maskpassword();  // get masked pasword
    // search all users for username and password match
    for ( int i=0;i<users.size();i++){
        if(users[i].username==name && users[i].password==pass){
            cout<<" login successful. \n";
            return i;
        }
    }
    cout<<" invalid username or password. please try again.\n";
    return -1;
}
//  function of the ceate post 
void createpost( int uid ){
    Post p;
    p.id=postCounter++;
    p.author=users[uid].username;
    cin.clear(); // clear input buffer
    cin.ignore(); // flush newline leftover
    cout<<" enter the content of your post: ";
    getline(cin,p.content);
    p.likes=0;
    if (p.content.empty()){
        cout<<" post content cannot be empty. \n";
        return ;
    }
    posts.push_back(p);
    users[uid].posts.push_back(p.id);
    savedata(); // for the saving of data
    cout<<" post created sucessfully.\n";
    cout<<" post id: "<<p.id<<"\n";



}

// function  for the view posts of the account
void viewposts(){
    cout<<" \n News feed: \n";
    if(posts.empty()){
        cout<<" no posts available. \n";
        return ;
    }
    for(auto it =posts.rbegin(); it!=posts.rend(); ++it){
        cout<<" Post ID: "<<it-> id<<" by "<< it-> author<<"\n";
        cout<< it-> content <<"\n";
        cout<<" Likes: "<< it-> likes << " | Comments: "<< it-> comments.size()
        <<"\n-----------------------\n";
        if(!it->comments.empty()){
            cout<<" Comments: \n";
            for ( const string &c: it-> comments){
                cout<<" - "<< c <<"\n";
            }
        }
    }
}
// function for the like and commant of the posst
void likeComment() {
    cout << "Enter Post ID: ";
    int pid;
    if (!(cin >> pid)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input.\n";
        return;
    }
    for (auto &p : posts) {
        if (p.id == pid) {
            int ch;
            cout << "1. Like\n";
            cout << "2. Comment\n";
            cout << " Enter choice: ";
            cin >> ch;
            if (! ch) {
                 cin.clear();
                  cin.ignore();
                   cout << "Invalid.\n"; 
                   return; 
                }
            if (ch == 1) {
                p.likes++;
                savedata();
                cout << "Liked!\n";
            } else if (ch == 2) {
                cin.ignore(); // clear input buffer   
                string c;
                cout << "Enter comment: ";
                getline(cin, c);
                if (!c.empty()) {
                    p.comments.push_back(c);
                    savedata();
                    cout << "Comment added!\n";
                } else {
                    cout << "Comment empty. Aborted.\n";
                }
            } else {
                cout << "Invalid choice.\n";
            }
            return;
        }
    }
    cout << "Post not found!\n";
}

void followunfollow(int uid){
    string uname;
    cout<<" enter the userame to follow / unfollow: ";
    cin>>uname;
    cin.clear();
    cin.ignore(); // clear input buffer  
    if(uname.empty()){
        cout<<" invalid username. \n";
        return ;
    }
    int idx= finduser(uname);
    if (idx==-1){
        cout<<" user not found: \n";
        return ;

    }
    if(uid==idx){
        cout<<" you cannot follow / unfollow yourself. \n";
        return ;
    }
    auto &flist=users[uid].following;
    auto &targetfollowers= users[idx].followers;
    auto it = find(targetfollowers.begin(), targetfollowers.end(), users[uid].username);
    if(find(flist.begin(),flist.end(),uname)!=flist.end()){
        flist.erase(remove(flist.begin(),flist.end(),uname),flist.end());
        cout<< " you  have been  unfollowed: "<<uname<<"\n";

    }
    else{
        flist.push_back(uname);
         targetfollowers.push_back(users[uid].username);
         savedata();
        cout<<" you have been followed : "<<uname<<"\n";

    }
}
//  view profile 
void viewprofile(int uid){
    cout<<" PROFILE : ";
    cout<<" username: "<<users[uid].username<<"\n";
    cout<<" Bio : "<<users[uid].bio<<"\n";
    cout<<" Followers: "<< users[uid].followers.size()<<" | Following: "<< users[uid].following.size()<<"\n";
    cout<< " posts: \n";
    if (users[uid].posts.empty()){
        cout<<" no posts available. \n";
        return ;
    }
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
    if(key.empty()){
        cout<<" invalid username. \n";
        return ;
    }
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
            case 3: likeComment();
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
            registerUser();

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
