#include "SocialNetwork.h"

// User class methods
User::User(int id, string uname, string f, string l, string s, string loc, int age, vector<string> h) 
    : id(id), username(uname), firstName(f), lastName(l), schoolName(s), locality(loc), age(age), hobbies(h) {}

void User::displayProfile()
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

// SocialNetwork class methods
SocialNetwork::SocialNetwork() {}

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
}

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
}

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

bool SocialNetwork::deleteUser(int userId){
    if(!users.count(userId)){
        return false;
    }
    
    User &u = users[userId];
    string username = u.username;
    
    // Remove user from users map
    users.erase(userId);
    
    // Remove username from usernameToId map
    usernameToId.erase(username);
    
    //  remove user from all their friends' friend lists
    if(friends.count(userId)){
        for(auto &pair : friends[userId]){
            int friendId = pair.first;
            // Remove this user from friend's friend list
            if(friends.count(friendId)){
                auto &friendList = friends[friendId];
                friendList.erase(
                    remove_if(friendList.begin(), friendList.end(),
                    [userId](const std::pair<int, int> &p) { return p.first == userId; }),
                    friendList.end()
                );
            }
        }
        friends.erase(userId);
    }
    
    // Also check all other users' friend lists and remove this user if present
    for(auto &entry : friends){
        auto &friendList = entry.second;
        friendList.erase(
            remove_if(friendList.begin(), friendList.end(),
            [userId](const std::pair<int, int> &p) { return p.first == userId; }),
            friendList.end()
        );
    }
    
    return true;
}

