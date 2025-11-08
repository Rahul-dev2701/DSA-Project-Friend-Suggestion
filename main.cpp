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

