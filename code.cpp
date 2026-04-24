#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

struct User {
    string username;
    string password;
    string name;
    string mail;
    int privilege = 0;
    bool exists = false;
    bool online = false;
};

static const int MAX_USERS = 50000;
User usersArr[MAX_USERS];
int userCount = 0;

int findUser(const string &u){
    for(int i=0;i<userCount;i++) if(usersArr[i].exists && usersArr[i].username==u) return i;
    return -1;
}

static inline bool getToken(istream &in, string &tok){
    if(!(in>>tok)) return false;
    return true;
}

struct ModParams {
    bool has_p=false, has_n=false, has_m=false, has_g=false;
    string p,n,m; int g=0;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string cmd;
    while (cin>>cmd){
        if(cmd=="add_user"){
            // params: -c -u -p -n -m -g ; first user special
            string par, cur, u, p, n, m; int g=-1;
            // read rest of line tokens until newline; but easy way: read entire line then parse
            // However we already consumed cmd. We'll parse remaining tokens from the same line by peeking until newline not trivial.
            // We'll instead read until next command or until we processed all '-' keys within the line. For robustness, parse next tokens until we see a token that doesn't start with '-'? But values may not start with '-'. We'll assume well-formed: each key followed by value.
            // We'll parse using getline on rest of line and then istringstream.
            string rest; getline(cin, rest);
            istringstream iss(rest);
            while(iss>>par){
                if(par=="-c"){ iss>>cur; }
                else if(par=="-u"){ iss>>u; }
                else if(par=="-p"){ iss>>p; }
                else if(par=="-n"){ iss>>n; }
                else if(par=="-m"){ iss>>m; }
                else if(par=="-g"){ string gs; iss>>gs; g = atoi(gs.c_str()); }
            }
            // check unique username
            if(findUser(u)!=-1){ cout<<-1<<"\n"; continue; }
            if(userCount>=MAX_USERS){ cout<<-1<<"\n"; continue; }
            if(userCount==0){
                // first user
                usersArr[userCount].username=u;
                usersArr[userCount].password=p;
                usersArr[userCount].name=n;
                usersArr[userCount].mail=m;
                usersArr[userCount].privilege=10;
                usersArr[userCount].exists=true;
                usersArr[userCount].online=false;
                userCount++;
                cout<<0<<"\n";
            }else{
                int ci = findUser(cur);
                if(ci==-1 || !usersArr[ci].online){ cout<<-1<<"\n"; continue; }
                if(g<0 || g>=usersArr[ci].privilege){ cout<<-1<<"\n"; continue; }
                usersArr[userCount].username=u;
                usersArr[userCount].password=p;
                usersArr[userCount].name=n;
                usersArr[userCount].mail=m;
                usersArr[userCount].privilege=g;
                usersArr[userCount].exists=true;
                usersArr[userCount].online=false;
                userCount++;
                cout<<0<<"\n";
            }
        }else if(cmd=="login"){
            string rest; getline(cin, rest);
            string par,u,p; istringstream iss(rest);
            while(iss>>par){
                if(par=="-u") iss>>u; else if(par=="-p") iss>>p;
            }
            int idx = findUser(u);
            if(idx==-1 || usersArr[idx].password!=p || usersArr[idx].online){ cout<<-1<<"\n"; }
            else { usersArr[idx].online=true; cout<<0<<"\n"; }
        }else if(cmd=="logout"){
            string rest; getline(cin, rest);
            string par,u; istringstream iss(rest);
            while(iss>>par){ if(par=="-u") iss>>u; }
            int idx = findUser(u);
            if(idx==-1 || !usersArr[idx].online){ cout<<-1<<"\n"; }
            else { usersArr[idx].online=false; cout<<0<<"\n"; }
        }else if(cmd=="query_profile"){
            string rest; getline(cin, rest);
            string par,c,u; istringstream iss(rest);
            while(iss>>par){ if(par=="-c") iss>>c; else if(par=="-u") iss>>u; }
            int ci=findUser(c), ui=findUser(u);
            if(ci==-1 || !usersArr[ci].online || ui==-1){ cout<<-1<<"\n"; continue; }
            if(usersArr[ci].privilege>usersArr[ui].privilege || c==u){
                cout<<usersArr[ui].username<<' '<<usersArr[ui].name<<' '<<usersArr[ui].mail<<' '<<usersArr[ui].privilege<<"\n";
            }else{
                cout<<-1<<"\n";
            }
        }else if(cmd=="modify_profile"){
            string rest; getline(cin, rest);
            string par,c,u; ModParams mp;
            istringstream iss(rest);
            while(iss>>par){
                if(par=="-c") iss>>c; else if(par=="-u") iss>>u; 
                else if(par=="-p"){ iss>>mp.p; mp.has_p=true; }
                else if(par=="-n"){ iss>>mp.n; mp.has_n=true; }
                else if(par=="-m"){ iss>>mp.m; mp.has_m=true; }
                else if(par=="-g"){ string gs; iss>>gs; mp.g=atoi(gs.c_str()); mp.has_g=true; }
            }
            int ci=findUser(c), ui=findUser(u);
            if(ci==-1 || !usersArr[ci].online || ui==-1){ cout<<-1<<"\n"; continue; }
            bool perm = (usersArr[ci].privilege>usersArr[ui].privilege) || (c==u);
            if(!perm){ cout<<-1<<"\n"; continue; }
            if(mp.has_g){
                if(!(usersArr[ci].privilege>mp.g)){ cout<<-1<<"\n"; continue; }
                usersArr[ui].privilege = mp.g;
            }
            if(mp.has_p) usersArr[ui].password=mp.p;
            if(mp.has_n) usersArr[ui].name=mp.n;
            if(mp.has_m) usersArr[ui].mail=mp.m;
            cout<<usersArr[ui].username<<' '<<usersArr[ui].name<<' '<<usersArr[ui].mail<<' '<<usersArr[ui].privilege<<"\n";
        }else if(cmd=="clean"){
            // clear all data
            for(int i=0;i<userCount;i++){ usersArr[i]=User(); }
            userCount=0;
            cout<<0<<"\n";
        }else if(cmd=="exit"){
            cout<<"bye\n";
            break;
        }else{
            // consume rest of line and ignore
            string rest; getline(cin, rest);
            cout<<-1<<"\n";
        }
    }
    return 0;
}
