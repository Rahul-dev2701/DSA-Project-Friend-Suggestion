#ifndef SOCIAL_NETWORK_H
#define SOCIAL_NETWORK_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <unordered_set>
#include <queue>
#include <set>
#include <fstream>
#include <algorithm>
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
    User(int id, string uname, string f, string l, string s, string loc, int age, vector<string> h);
    void displayProfile();
};

class SocialNetwork
{
private:
    unordered_map<int, User> users;             // userId → User
    unordered_map<int, vector<pair<int, int>>> friends;          // userId → [(friendId, weight)]
    unordered_map<string, int> usernameToId;        // username → userId

public:
    SocialNetwork();
    void registerUser(User A);      //Take details → create User object → add to users + file
    int loginUser(const string& userName);         //Check if username exists, then load user session

    void loadUsers();               //Read users.csv → fill users & usernameToId
    void loadFriends();             //Read friends.csv → fill friends adjacency list

    void viewProfile(int userID);
    User& getUser(int id);              //get user address from userId
    int getUserIdByUsername(const string &uname);
    
    void displayCurrentFriends(int userId);     //Print friend list from friends[user.id]
    vector<int> suggestedFriends(int userId);       //Use mutuals, similarity score, etc. to suggest
    void addFriend(User& A,User& B);                 //Add new connection to both users' friend lists
    void interact(User A, User B);          //Increase friendship weight between two IDs
    int getNextUserId();                    //get next available userId
    bool deleteUser(int userId);

    void saveUsers();               //after new registration or profile update
    void saveFriends();             //after adding friends or increasing interaction weights

    int countMutualFriends(int userA, int userB);       //Compare adjacency lists
    double computeSimilarity(const User &u1, const User &u2);       //Calculate based on school, locality, age, hobbies, etc.
};

#endif

