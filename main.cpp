#include <iostream>
#include <vector>
#include<string>
#include<unordered_map>
using namespace std;

class User {
public:
    int id;
    string username;
    string password;
    string firstName, lastName;
    int age;
    string locality; 
    vector<string> hobbies;
    string schoolName;

    User() {}
    User(int id, string uname, string f, string l, string s, string loc, int age, vector<string> h) : id(id), username(uname), firstName(f), lastName(l), schoolName(s), locality(loc), age(age), hobbies(h) {}
    void displayProfile()
    {
        cout << "\n--- User Profile ---\n";
        cout << "username: " << username << "\n";
        cout << "Name: " << firstName << " " << lastName << "\n";
        cout << "Age: " << age << "\n";
        cout << "School/College: " << schoolName << "\n";
        cout << "Locality: " << locality << "\n";

        cout << "Hobbies: ";
        for (int i = 0; i < hobbies.size(); ++i)
        {
            cout << hobbies[i];
            if (i != hobbies.size() - 1)
                cout << ", ";
        }
        cout << "\n--------------------\n";
    }

    
};

class SocialNetwork
{
private:
    unordered_map<int, User> users;             // userId → User
    unordered_map<int, vector<pair<int, int>>> friends;          // userId → [(friendId, weight)]
    unordered_map<string, int> usernameToId;        // username → userId

public:
    SocialNetwork(){};
    void registerUser(User A);      //Take details → create User object → add to users + file
    int loginUser(const string& userName);         //Check if username exists, then load user session

    void loadUsers();               //Read users.csv → fill users & usernameToId
    void loadFriends();             //Read friends.csv → fill friends adjacency list

    void viewProfile(int userID);
    int getUserIdByUsername(const string &uname);
    
    void displayCurrentFriends(int userId);     //Print friend list from friends[user.id]
    vector<int> suggestedFriends(int userId);       //Use mutuals, similarity score, etc. to suggest
    void addFriend(User& A,User& B);                 //Add new connection to both users’ friend lists
    void interact(User A, User B);          //Increase friendship weight between two IDs

    
    void saveUsers();               //after new registration or profile update
    void saveFriends();             //after adding friends or increasing interaction weights


    int countMutualFriends(int userA, int userB);       //Compare adjacency lists
};


int main(){
    SocialNetwork sn;
    sn.loadUsers();
    sn.loadFriends();

    //start
    cout<<"Welcome to Your Personalised Friend Reccomendation System"<<endl;

    //choice for login
    int choice;
    
    //dashboard
    while(true){
        cout<<"1.Register New User"<<endl;
        cout<<"2.Login"<<endl;
        cout<<"3.Exit"<<endl;

        cout<<"Enter choice"<<endl;
        cin>>choice;
        cin.ignore();
        if(choice==1){
            //register new user

        }
        else if(choice==2){
            //Login to existing account

        }
        else if(choice==3){
            //exit
        }

        else cout<<"invalid option. Try again"<<endl;

    }

    return 0;
}

void SocialNetwork::loadUsers(){
    ifstream file("users.csv");
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return ;
    }
    string line;
    getline(file, line);
    while(getline(file,line)){
        if(line.empty())    continue;

        stringstream ss(line);
        string idStr, username, password, fName, lName, college, locality, ageStr, hobbiesStr;
        getline(ss, idStr, ',');
        getline(ss,username,',');
        getline(ss,password,',');
        getline(ss,fName,',');
        getline(ss,lName,',');
        getline(ss,college,',');
        getline(ss,locality,',');
        getline(ss,ageStr,',');
        getline(ss, hobbiesStr);
        int id,age;
        try{
            id = stoi(idStr);
            age = stoi(ageStr);
        }
        catch (...) {
            cout<< "Invalid Line\n";
            continue; 
        } 

        //split hobbies by comma
        vector<string>hobbies;
        string hobby;
        stringstream hss(hobbiesStr);
        while(getline(hss,hobby,',')){
            if (!hobby.empty())
            hobbies.push_back(hobby);
        }
        User u(id,username,fName,lName,college,locality,age,hobbies);
        u.password = password;
        users[id] = u;
        usernameToId[username] = id;

        
    }
    file.close();
    cout << " Loaded " << users.size() << " users successfully.\n";
}

void SocialNetwork::loadFriends(){
   ifstream file("friends.csv"); 
   if(!file.is_open()){
        cout<<"couldn't open friends.csv";
        return;
   }
   string line;
   getline(file,line);  // skip first line

   while(getline(file,line)){
        if(line.empty())    continue;

        stringstream ss(line);
        string userIdStr, fIdStr, weightStr;
        getline(ss,userIdStr,',');
        getline(ss,fIdStr,',');
        getline(ss,weightStr);

        try {
            int userId = stoi(userIdStr);
            int fId = stoi(fIdStr);
            int weight = stoi(weightStr);

            // add connection
            friends[userId].push_back({fId, weight});
            friends[fId].push_back({userId, weight}); // only if CSV is one-directional
        }
        catch (...) {
            cout << " invalid line: " << line << endl;
        }
    }
   file.close();
}

void SocialNetwork::saveUsers(){
    ofstream file("users.csv");
    if(!file.is_open()){
        cout<<"Error saving users!\n";
        return;
    }
    file<<"id,username,password,firstName,lastName,college,locality,age,hobbies\n";

    for(auto &entry : users){
        const User &u = entry.second;

        file<<u.id<<","
            <<u.username<<","
            <<u.password<<","
            <<u.firstName<<","
            <<u.lastName<<","
            <<u.schoolName<<","
            <<u.locality<<","
            <<u.age<<",";
            
        for(size_t i=0;i<u.hobbies.size();i++){
            file<<u.hobbies[i];
            if(i!=u.hobbies.size()-1)   file<<",";
        }
        file<<"\n";
    }
    file.close();
    cout<<"Users saved succesfully";
}

void SocialNetwork::saveFriends(){
    ofstream file("friends.csv");
    if(!file.is_open()){
        cout<<"Error saving friends!\n";
        return;
    }
    file<<"user_id,friend_id,weight\n";
    
    //to avoid duplicates
    set<pair<int,int>> writtenPairs;
    for(auto &entry : friends){
        int userId = entry.first;
        for(auto &pair : entry.second){
            int friendId = pair.first;
            int weight = pair.second;

            if(writtenPairs.count({friendId,userId})==0){
                file<<userId<<","<<friendId<<","<<weight<<"\n";
                writtenPairs.insert({userId,friendId});
            }
        }
    }
    file.close();
    cout<<"friend saved succesfully.\n";
}

void SocialNetwork::displayCurrentFriends(int userId){

    if(!friends.count(userId)||friends[userId].empty()){
        cout<<"You have no friends yet.\n";
        return;
    }

    cout<<"\n--Friend List--\n";
    int count = 1;
    for(auto &pair : friends[userId]){
        int friendId = pair.first;
        int weight = pair.second;
        if(!users.count(friendId))  continue;

        User &f = users[friendId];
        cout<<count<<"."<<f.username    
            <<" (" << f.firstName << " " << f.lastName << ") "
            << " | Age: " << f.age
            << " | City: " << f.locality
            << " | Friendship weight: " << weight<< "\n";
            
        count++;
    }
    cout<<"----------------\n";
}

void SocialNetwork::addFriend(User& A, User& B){
    int userId = A.id;
    int friendId = B.id;

    for(auto &pair : friends[userId]){
        if(pair.first == friendId){
            cout<<"Already friends.\n";
            return;
        }
    }

    friends[userId].push_back({friendId,1});
    friends[friendId].push_back({userId,1});
    
    cout<<"You and "<<B.username<<" are now friends!\n";
}
