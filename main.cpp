#include <iostream>
#include <vector>
#include<string>
#include<unordered_map>
#include<sstream>
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
    User& getUser(int id);              //get user address from userId
    int getUserIdByUsername(const string &uname);
    
    void displayCurrentFriends(int userId);     //Print friend list from friends[user.id]
    vector<int> suggestedFriends(int userId);       //Use mutuals, similarity score, etc. to suggest
    void addFriend(User& A,User& B);                 //Add new connection to both users’ friend lists
    void interact(User A, User B);          //Increase friendship weight between two IDs
    int getNextUserId();                    //get next available userId

    
    void saveUsers();               //after new registration or profile update
    void saveFriends();             //after adding friends or increasing interaction weights


    int countMutualFriends(int userA, int userB);       //Compare adjacency lists
    double computeSimilarity(const User &u1, const User &u2);       //Calculate based on school, locality, age, hobbies, etc.
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
            string userName, firstName, lastName, college, location, password; 
            int age;
            vector<string> hobbies;
            cout<<"Enter username: ";
            getline(cin,userName);
            if (sn.getUserIdByUsername(userName) != -1) {
                cout << " Username already exists. Try another.\n";
                continue;
            }
            cout<<"Enter password: ";
            getline(cin,password);
            cout<<"Enter first name: ";
            getline(cin,firstName);
            cout<<"Enter last name: ";
            getline(cin,lastName);
            cout<<"Enter your college: ";
            getline(cin,college);
            cout<<"Enter your city: ";
            getline(cin,location);
            cout << "Enter age: ";
            cin >> age;
            cin.ignore();
            cout << "Enter hobbies separated by commas: ";
            //input hobbies

            string hobbyInput;
            getline(cin, hobbyInput);
            stringstream ss(hobbyInput);
            string hobby;
            while (getline(ss, hobby, ',')) {
                hobbies.push_back(hobby);
            }

            int newId = sn.getNextUserId();
            User u(newId, userName, firstName, lastName, college, location, age, hobbies);
            sn.registerUser(u);
            u.password = password;
            cout<<"registration succesfull";
            sn.saveUsers();
            

        }
        else if(choice==2){
            //Login to existing account
            string userName;
            cout<<"Enter username: ";
            cin>>userName;
            int userId = sn.loginUser(userName);
            //check if userId exist
            if (userId == -1) {
                cout << "Username not found!\n";
                continue;
            }
            cout<<"Logged in successfully as "<<userName<<"\n";
            while(true){
                cout<<"User Menu: \n";
                cout<<"1. View Profile\n";
                cout<<"2. View friend list\n";
                cout<<"3. People you might want to be your friend\n";
                cout<<"4. Interact with a friend\n";
                cout<<"5. Logout\n";
                cout<<"Enter choice\n";
                int op;
                cin>>op;
                if(op==1){
                    sn.viewProfile(userId);
                }
                else if(op==2){
                    sn.displayCurrentFriends(userId);
                }
                else if(op==3){
                    vector<int> suggestions = sn.suggestedFriends(userId);
                    if (suggestions.empty()) {
                        cout << "No friend suggestions available.\n";
                    }else{
                        cout << "Friend Suggestions:\n";
                        for(size_t i=0;i<suggestions.size();i++){
                            cout<<i+1<<".\n";
                            sn.viewProfile(suggestions[i]);
                        }
                        cout << "\nEnter the numbers of users you want to add as friends (comma separated), or 0 to skip: ";
                        cin.ignore();
                        string input;
                        getline(cin, input); 
                        if(input!="0"){
                            size_t start = 0, end;
                            while ((end = input.find(',', start)) != string::npos) {
                                string token = input.substr(start, end - start);
                                if (!token.empty()) {
                                    int index = stoi(token) - 1;
                                    if (index >= 0 && index < (int)suggestions.size()) {
                                        int fid = suggestions[index];
                                        sn.addFriend(sn.getUser(userId), sn.getUser(fid));
                                        cout << sn.getUser(fid).username << " added as a friend!\n";
                                    }
                                }
                                start = end+1;
                            }
                            // handle the last (or only) number
                            if (start < input.size()) {
                                string token = input.substr(start);
                                if (!token.empty()) {
                                    int index = stoi(token) - 1;
                                    if (index >= 0 && index < (int)suggestions.size()) {
                                        int fid = suggestions[index];
                                        sn.addFriend(sn.getUser(userId), sn.getUser(fid));
                                        cout << sn.getUser(fid).username << " added as a friend!\n";
                                    }
                                }
                            }
                        }
                    }
                    sn.saveFriends();
                    
                }
                else if(op==4){
                   cout << "Enter friend's username to interact with: ";
                    string fname;
                    cin >> fname;

                    int friendId = sn.getUserIdByUsername(fname);
                    if (friendId == -1) {
                        cout << "No such user exists!\n";
                    } else {
                        sn.interact(sn.getUser(userId), sn.getUser(friendId));
                        sn.saveFriends();
                    }
                }
                else if(op==5){
                    cout << "Logging out...\n";
                    break;
                }
                else    cout<<"Invalid choice\n";
            }
            
        }
        else if(choice==3){
            //exit
            cout << "Exiting FriendBook...\n";
            sn.saveUsers();
            sn.saveFriends();
            break;
        }

        else cout<<"invalid option. Try again"<<endl;

    }

    return 0;
}


//functions
int SocialNetwork::getNextUserId(){}
void SocialNetwork::viewProfile(int userID){};
User& SocialNetwork::getUser(int id){};
int SocialNetwork::getUserIdByUsername(const string &uname){};
void SocialNetwork::loadUsers(){};
void SocialNetwork::loadFriends(){};
void SocialNetwork::saveUsers(){};
void SocialNetwork::saveFriends(){};
void SocialNetwork::registerUser(User A){};
int SocialNetwork::loginUser(const string &usernme){};
void SocialNetwork::displayCurrentFriends(int userId){};
void SocialNetwork::addFriend(User& A, User& B){};
void SocialNetwork::interact(User A, User B){};
int SocialNetwork::countMutualFriends(int userA, int userB){};
double SocialNetwork::computeSimilarity(const User &A, const User &B){};
vector<int> SocialNetwork::suggestedFriends(int userId){};
