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
    public:
    Post(){
        id=0;
        likes=0;
    }
    Post(int i, string a, string c){
        id=i;
        author=a;
        content=c;
        likes=0;
    }
    // getters
    int getid() const { 
        return id; 
    }
    string getauthor() const { 
        return author; 
    }
    string getcontent() const { 
        return content; 
    }
    int getlikes() const {
        return likes;
    }
    const vector<string>& getcomments() const {
        return comments;
    }
    // operations
    void setcontent(const string &c){
        content=c;
    }
    void addlike(){
        likes++;
    }
    void addcomment(const string &c){
        comments.push_back(c);
    }
    string serialize() const {
        // id|author|content|likes|comment1~~comment2~~
        string out = to_string(id) + "|" + author + "|" + content + "|" + to_string(likes) + "|";
        for (const auto &c : comments) out += c + "~~";
        return out;
    }

    
    static Post parse(const string &line) {
         Post p;
        // Expect 5 parts separated by '|'
        vector<string> parts = split(line, "|");
        while (parts.size() < 5) 
        parts.push_back("");
        try { p.id = stoi(parts[0]);
        } 
        catch(...) 
        { p.id = 0;
         }
        p.author = parts[1];
        p.content = parts[2];
        try { p.likes = stoi(parts[3]); }
         catch(...) 
         { p.likes = 0; }
        if (!parts[4].empty()) {
            vector<string> cms = split(parts[4], "~~");
            for (auto &c : cms) {
                if (!c.empty()) p.comments.push_back(c);
            }
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
        for (char ch: s) {
            if (ch == '|') out += "&#124;";
            else out += ch;
        }
        return out;
    }
    static string escape(const string &s) {
        string out=s;
        size_t pos=0;
        for (char ch: s) {
            if (ch == '~') out += "&#126;";
            else out += ch;
        }
        return out;
    }

};

class person{
    protected:

    string username;
    string bio;
 public:
      person(){}  // default constructor
      person(const string &uname, const string &b){
        username=uname;
        bio=b;
      }
      virtual ~person(){} // virtual destructor
    virtual void showProfile(){
        cout<<" username: "<< username <<"\n";
        cout<<" bio: "<< bio <<"\n";
    }
    string getusername() const {
        return username;
    }
    string getbio() const { 
        return bio;
    }
};

class user: public person{
    private:
    string password;
    vector<int> posts; // store post IDs
    vector<string> followers;
    vector<string> following;
public:
    user(){}
    user(const string &uname, const string &pass, const string &b)  //
  {
    username=uname;
    password=pass;
    bio=b;
  }
    // getters
    bool checkpassword(const string &p) const {
        return password==p;
    }
    const vector<int>& getposts() const {
        return posts;
    }
    const vector<string>& getfollowers() const {
        return followers;
    }
    const vector<string>& getfollowing() const {
        return following;
    }

    ~user(){}  // destructor
    // operations
    void addpost(int pid){
        posts.push_back(pid);
    }
    void addfollower(const string &fol){
        followers.push_back(fol);
    }
    void addfollowing(const string &fol){
        following.push_back(fol);
    }
    void removepost(int pid){
        posts.erase(remove(posts.begin(),posts.end(),pid),posts.end());
    }
    void removefollower(const string &fol){
        followers.erase(remove(followers.begin(),followers.end(),fol),followers.end());
    }
    void removefollowing(const string &fol){
        following.erase(remove(following.begin(),following.end(),fol),following.end());
    }
    void setbio(const string &b){
        bio=b;
    }
    void showprofile() override {
        cout<<" username: "<< username <<"\n";
        cout<<" bio: "<< bio <<"\n";
        cout<<" followers: "<< followers.size() << " | following: "<< following.size() <<"\n";  
        cout<<" posts: \n";
        if (posts.empty()){
            cout<<" no posts available. \n";
            return ;
        }
    }
    //  serialization
    string serialize() const {
        
        string out = username + "|" + password + "|" + bio + "|";
        for (int pid : posts)
         out += to_string(pid) + ",";
        out += "|";
        for (const string &fol : followers)
         out += fol + ",";
        out += "|";
        for (const string &fol : following) 
        out += fol + ",";
        return out;
    }

    static user parse( const string &line){
        vector<string> parts = split(line, "|");
        while (parts.size() < 6)
            parts.push_back("");
            user u;
            u.username = parts[0];
            u.password = parts[1];
            u.bio      = parts[2];

            // posts
            if(!parts[3].empty()){
                vector<string> pids = split(parts[3],",");
                for ( const auto &x: pids)
                 if(!x.empty())
                  u.posts.push_back(stoi(x));
            }
            // followers
            if(!parts[4].empty()){

            }
            return u;

    }
    
private:
     static string cape(const string &s) {
        string out;
        for (char ch : s) {
            if (ch == '|') out += "&#124;";
            else out += ch;
        }
        return out;
    }
    static string scapeTilde(const string &s) {
        string out;
        for (char ch : s) {
            if (ch == '~') out += "&#126;";
            else out += ch;
        }
        return out;
    }
class app{
    private:
        vector<user>users;
        vector<Post>posts;
        int postcounter=1;
    
        const string USERS_FILE="users.txt";
        const string POSTS_FILE="posts.txt";
        public:
        app(){
            loaddata();
        }
        ~app(){
            saveall();
        }

        void saveall(){
            vector<string>ulines;
            for ( const auto &u: users){
                ulines.push_back(u.serialize());
            }
            FileHandler::saveLines(USERS_FILE, ulines);
            //  save posts
            vector<string>plines;
            for ( const auto &p: posts){
                plines.push_back(p.serialize());
            }
            FileHandler::saveLines(POSTS_FILE, plines);
        }
        //  load data
        void loadall(){
            users.clear();   // clear existing data
            posts.clear();  // clear existing data
            postcounter=1;  // reset post counter
            vector<string>ulines= FileHandler::loadLines(USERS_FILE);
            for(auto &ln: ulines){
                user u= user::parse(ln);
                users.push_back(u);
            
            }
            // loads posts
            vector<string>plines= FileHandler::loadLines(POSTS_FILE);
            for(const auto &ln: plines){
                Post p= Post::parse(ln);
                posts.push_back(p);
                if (p.getid()>= postcounter)
                postcounter=p.getid()+1;
            }
        }
        // function for register user
    void registeruser(){
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string uname;
        cout<<" enter username: ";
        getline(cin, uname);
        if(uname.empty()){
            cout<<" username cannot be empty. \n";
            return ;
        }
        if(finduser(uname)!=-1){
            cout<<" username already exists! \n";
            return ;
        }
        cout<<" enter password: ";
        string pass= maskpassword(); // password masking
        cout<<" enter bio: ";
        string bio;
        getline(cin, bio);
        users.emplace_back(uname, pass, bio);
        saveall();
        cout<<" registration successful! \n";

    }
    int loginuser(){
        string uname;
        cout<<" enter username: ";
        cin>>uname;
        cout<<" enter password: ";
        string pass= maskpassword();
        int idx=finduser(uname);
        if(idx==-1){
            cout<<" username not found. \n";
            return -1;

    }
        if(!users[idx].checkpassword(pass)){
            cout<<" wrong password. \n";
            return -1;
        }
        cout<<" login successful. welcome, "<< uname <<"! \n";
        return idx;
    }
};

// for the creating the password


