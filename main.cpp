#include <iostream>
#include <vector>
#include<string>
#include<unordered_map>
#include<sstream>
#include<unordered_set>
#include<queue>
#include<algorithm>
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

double SocialNetwork::computeSimilarity(const User &A, const User &B){
    double score = 0.0;

    //same college
    if(A.schoolName==B.schoolName){
        score+=3.0;
    }

    //same locality
    if(B.locality==A.locality){
        score+=3.0;
    }

    //age diff
    int ageDiff = abs(A.age-B.age);
    if(ageDiff<=2)  score+=1.0;
    else if (ageDiff<=5)    score+=0.5;
    else if(ageDiff<=10)    score+= 0.25;
    
    //common hobbies
    int common =0;
    set<string> uniqueHobbies;

    for(const string &h : A.hobbies)    uniqueHobbies.insert(h);
    for(const string &h : B.hobbies){
        if(uniqueHobbies.count(h))   common++;
        else uniqueHobbies.insert(h);
    }
    if(!uniqueHobbies.empty()){
        double hobbyScore = (double)common/uniqueHobbies.size();
        score+=hobbyScore*2;
    }

    return score;

//functions
vector<int> SocialNetwork::suggestedFriends(int userId){
    vector<int> result;
    if(!users.count(userId)){
        cout<<"Invalid userID\n";
        return result;  
    }
    //exclude yourself and your current friends in the traversal
    unordered_set<int> firstLevel;
    firstLevel.insert(userId);
    for(auto& p : friends[userId]){
        firstLevel.insert(p.first);
    }

    unordered_map<int ,double> scoreMap;

    //Bfs upto 2 levels
    queue<pair<int,int>> q;
    unordered_set<int> visited;
    q.push({userId,0});
    visited.insert(userId);
    bool foundGraphFriends = false;
    while(!q.empty()){
        pair<int,int> p = q.front();
        int curr = p.first;
        int dist = p.second;
        q.pop();
        if(dist==2) continue;
        for(auto & neigh : friends[curr]){
            int neighId = neigh.first;
            int weight = neigh.second;

            if(!visited.count(neighId)){
                visited.insert(neighId);
                q.push({neighId,dist+1});
            }

            if(firstLevel.count(neighId))   continue;

            foundGraphFriends = true;

            //graph score
            double graphScore = 0;
            int mutual = countMutualFriends(userId,neighId);
            graphScore += mutual*2.0;
            graphScore += weight*0.1;

            double sim = computeSimilarity(users[userId],users[neighId]);

            scoreMap[neighId] += sim + graphScore;


        }

    }
    if(!foundGraphFriends){
        for(auto& entry : users){
            int otherId = entry.first;
            if(otherId==userId) continue;
            double sim = computeSimilarity(users[userId],users[otherId]);
            scoreMap[otherId] = sim;

        }
    }


    //sort by score
    vector<pair<double,int>> ranked;
    for(auto& entry : scoreMap){
        ranked.push_back({entry.second,entry.first});

    }

    sort(ranked.begin(),ranked.end(),[&](auto &a, auto &b){
        return a.first>b.first;
    });

    int limit = min(10,(int)ranked.size());
    for(int i=0;i<limit;i++){
        result.push_back(ranked[i].second);
    }

    return result;
};
void SocialNetwork::registerUser(User A){
    if(usernameToId.count(A.username)){
        cout<<"Username '"<<A.username<<"' already exists. Try another.\n ";
        return;
    }

    A.id = getNextUserId();

    users[A.id] = A;

    usernameToId[A.username] = A.id;

    cout<<"User registered succesfully with ID: "<<A.id<<"\n";
}
int SocialNetwork::getNextUserId(){
        if(users.empty()) return 1;
        int maxId = 0;
        for(const auto &entry : users){
            if(entry.first>maxId)   maxId = entry.first;

        }
        return maxId + 1;
    }
int SocialNetwork::loginUser(const string &usernme){
    if(!usernameToId.count(usernme)){
        cout<<"Username not found\n";
        return -1;
    }

    int userId = usernameToId[usernme];
    User &u = users[userId];

    //ask for password
    cin.ignore();
    string inputPassword;

    cout<<"Enter password: ";

    getline(cin,inputPassword);
    
    if(inputPassword==u.password){
        cout<<"Login successful\n";
        return userId;

    }
    else{
        cout<<"Incorrect Password\n";
        return -1;
    }
}
int SocialNetwork::getUserIdByUsername(const string &uname) {
        if (usernameToId.count(uname)) return usernameToId[uname];
        return -1;
    }
void SocialNetwork::viewProfile(int userID){
        users[userID].displayProfile();
}
User& SocialNetwork::getUser(int id) {
        return users[id];
}
int SocialNetwork::countMutualFriends(int userA, int userB){
    if(!friends.count(userA)||!friends.count(userB)){
        return 0;
    }
    
    unordered_set<int> friendsOfA;
    for(auto &p:friends[userA]){
        friendsOfA.insert(p.first);
    }

    int mutualCount = 0;

    for(auto &p : friends[userB]){
        if(friendsOfA.count(p.first)){
            mutualCount++;
        }
    }
    return mutualCount;

}
void SocialNetwork::interact(User A, User B){
    int userId = A.id;
    int friendId = B.id;

    bool found = false;
    
    for(auto &pair: friends[userId]){
        if(pair.first == friendId){
            pair.second += 2;
            found  = true;
            break;
        }
    }

    for(auto &pair : friends[friendId]){
        if(pair.first == userId){
            pair.second += 1;
            break;
        }
    }

    if(!found){
        cout << "You and " << B.username << " are not friends yet. Add them first.\n";
    }
    else {
        cout <<"You interacted with " << B.username << ". Friendship weight increased.\n";
    }
}
