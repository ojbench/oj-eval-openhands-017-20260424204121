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

// Train data structures and utilities
struct Train {
    string id;
    int stationNum = 0;
    int seatNum = 0;
    string stations[105];
    int prices[105]; // size stationNum-1
    int travel[105]; // size stationNum-1
    int stopover[105]; // size stationNum-2
    int startH=0, startM=0;
    int saleFromM=6, saleFromD=1, saleToM=8, saleToD=31;
    char type='G';
    bool exists=false;
    bool released=false;
};

static const int MAX_TRAINS = 2000;
Train trainsArr[MAX_TRAINS];
int trainCount = 0;

int findTrain(const string &tid){
    for(int i=0;i<trainCount;i++) if(trainsArr[i].exists && trainsArr[i].id==tid) return i;
    return -1;
}

int monthBaseDays(int m){ // 6->0, 7->30, 8->61
    if(m==6) return 0; if(m==7) return 30; return 61;
}

bool parseDate(const string &s, int &m, int &d){
    if(s.size()!=5 || s[2]!='-') return false;
    m = atoi(s.substr(0,2).c_str());
    d = atoi(s.substr(3,2).c_str());
    return true;
}

bool parseTime(const string &s, int &h, int &mi){
    if(s.size()!=5 || s[2] != ':') return false;
    h = atoi(s.substr(0,2).c_str());
    mi = atoi(s.substr(3,2).c_str());
    return true;
}

void addMinutesToDate(int baseM, int baseD, int baseH, int baseMi, long long addMin, int &outM, int &outD, int &outH, int &outMi){
    long long baseDayIndex = monthBaseDays(baseM) + (baseD - 1);
    long long total = baseDayIndex*1440LL + baseH*60 + baseMi + addMin;
    if(total<0) total = 0; // clamp
    long long dayIndex = total / 1440LL;
    int minuteOfDay = (int)(total % 1440LL);
    outH = minuteOfDay/60; outMi = minuteOfDay%60;
    // map back to month/day
    if(dayIndex < 30){ outM=6; outD=(int)dayIndex+1; }
    else if(dayIndex < 61){ outM=7; outD=(int)(dayIndex-30)+1; }
    else { outM=8; outD=(int)(dayIndex-61)+1; }
}

bool inSaleRange(const Train &t, int qm, int qd){
    int from = monthBaseDays(t.saleFromM)+(t.saleFromD-1);
    int to   = monthBaseDays(t.saleToM)+(t.saleToD-1);
    int q    = monthBaseDays(qm)+(qd-1);
    return q>=from && q<=to;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string cmd;
    while (cin>>cmd){
        if(cmd=="add_user"){
            string par, cur, u, p, n, m; int g=-1;
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
            if(findUser(u)!=-1){ cout<<-1<<"\n"; continue; }
            if(userCount>=MAX_USERS){ cout<<-1<<"\n"; continue; }
            if(userCount==0){
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
        }else if(cmd=="add_train"){
            string rest; getline(cin, rest);
            string par; Train t; t.exists=false;
            string s_stations, s_prices, s_travel, s_stop, s_sale, s_time, s_type;
            while(true){
                istringstream iss(rest);
                string tmp; bool any=false;
                while(iss>>par){ any=true;
                    if(par=="-i"){ iss>>t.id; }
                    else if(par=="-n"){ string ns; iss>>ns; t.stationNum=atoi(ns.c_str()); }
                    else if(par=="-m"){ string ms; iss>>ms; t.seatNum=atoi(ms.c_str()); }
                    else if(par=="-s"){ iss>>s_stations; }
                    else if(par=="-p"){ iss>>s_prices; }
                    else if(par=="-x"){ iss>>s_time; }
                    else if(par=="-t"){ iss>>s_travel; }
                    else if(par=="-o"){ iss>>s_stop; }
                    else if(par=="-d"){ iss>>s_sale; }
                    else if(par=="-y"){ iss>>s_type; }
                }
                break;
            }
            if(findTrain(t.id)!=-1){ cout<<-1<<"\n"; continue; }
            if(trainCount>=MAX_TRAINS || t.stationNum<2 || t.stationNum>100){ cout<<-1<<"\n"; continue; }
            // parse time
            if(!parseTime(s_time, t.startH, t.startM)){ cout<<-1<<"\n"; continue; }
            // parse type
            t.type = s_type.empty()? 'G' : s_type[0];
            // parse sale
            {
                size_t pos = s_sale.find('|');
                if(pos==string::npos){ cout<<-1<<"\n"; continue; }
                string a = s_sale.substr(0,pos), b = s_sale.substr(pos+1);
                if(!parseDate(a, t.saleFromM, t.saleFromD) || !parseDate(b, t.saleToM, t.saleToD)){ cout<<-1<<"\n"; continue; }
            }
            // parse stations
            int cnt=0;{
                size_t start=0; while(true){ size_t pos = s_stations.find('|', start); string token = s_stations.substr(start, pos==string::npos? string::npos : pos-start); if(token.size()) t.stations[cnt++]=token; else t.stations[cnt++]=""; if(pos==string::npos) break; start=pos+1; if(cnt>104) break; }
            }
            if(cnt!=t.stationNum){ cout<<-1<<"\n"; continue; }
            // parse prices
            int cntp=0;{
                size_t start=0; while(true){ size_t pos = s_prices.find('|', start); string token = s_prices.substr(start, pos==string::npos? string::npos : pos-start); t.prices[cntp++]=atoi(token.c_str()); if(pos==string::npos) break; start=pos+1; if(cntp>104) break; }
            }
            if(cntp!=t.stationNum-1){ cout<<-1<<"\n"; continue; }
            // parse travel
            int cntt=0;{
                size_t start=0; while(true){ size_t pos = s_travel.find('|', start); string token = s_travel.substr(start, pos==string::npos? string::npos : pos-start); t.travel[cntt++]=atoi(token.c_str()); if(pos==string::npos) break; start=pos+1; if(cntt>104) break; }
            }
            if(cntt!=t.stationNum-1){ cout<<-1<<"\n"; continue; }
            // parse stopover
            int cnto=0; if(s_stop=="_") { cnto=0; }
            else {
                size_t start=0; while(true){ size_t pos = s_stop.find('|', start); string token = s_stop.substr(start, pos==string::npos? string::npos : pos-start); if(token.size()) t.stopover[cnto++]=atoi(token.c_str()); else t.stopover[cnto++]=0; if(pos==string::npos) break; start=pos+1; if(cnto>104) break; }
            }
            if(t.stationNum==2){ /* ok even if cnto==0 */ }
            else if(cnto!=t.stationNum-2){ cout<<-1<<"\n"; continue; }
            t.exists=true; t.released=false;
            trainsArr[trainCount++] = t;
            cout<<0<<"\n";
        }else if(cmd=="release_train"){
            string rest; getline(cin, rest); string par, tid; istringstream iss(rest); while(iss>>par){ if(par=="-i") iss>>tid; }
            int ti = findTrain(tid);
            if(ti==-1 || trainsArr[ti].released){ cout<<-1<<"\n"; }
            else { trainsArr[ti].released=true; cout<<0<<"\n"; }
        }else if(cmd=="query_train"){
            string rest; getline(cin, rest); string par, tid, sd; istringstream iss(rest); while(iss>>par){ if(par=="-i") iss>>tid; else if(par=="-d") iss>>sd; }
            int ti = findTrain(tid);
            int qm, qd; if(!parseDate(sd, qm, qd)){ cout<<-1<<"\n"; continue; }
            if(ti==-1 || !inSaleRange(trainsArr[ti], qm, qd)){ cout<<-1<<"\n"; continue; }
            Train &t = trainsArr[ti];
            cout<<t.id<<' '<<t.type<<"\n";
            long long accPrice=0; long long minutes=0;
            for(int i=0;i<t.stationNum;i++){
                int arrM=0,arrD=0,arrH=0,arrMi=0, depM=0,depD=0,depH=0,depMi=0;
                if(i==0){
                    // arrival x; departure base time
                    addMinutesToDate(qm, qd, t.startH, t.startM, 0, depM,depD,depH,depMi);
                    cout<<t.stations[i]<<" xx-xx xx:xx -> ";
                    cout<< (depM<10?"0":"")<<depM<<'-'<<(depD<10?"0":"")<<depD<<' '<<(depH<10?"0":"")<<depH<<':'<<(depMi<10?"0":"")<<depMi<<' ';
                    cout<<0<<' '<<t.seatNum<<"\n";
                }else if(i==t.stationNum-1){
                    // arrival after last travel
                    minutes += t.travel[i-1];
                    addMinutesToDate(qm, qd, t.startH, t.startM, minutes, arrM,arrD,arrH,arrMi);
                    cout<<t.stations[i]<<' '<<(arrM<10?"0":"")<<arrM<<'-'<<(arrD<10?"0":"")<<arrD<<' '<<(arrH<10?"0":"")<<arrH<<':'<<(arrMi<10?"0":"")<<arrMi<<" -> xx-xx xx:xx ";
                    cout<<accPrice<<" x\n";
                }else{
                    minutes += t.travel[i-1];
                    addMinutesToDate(qm, qd, t.startH, t.startM, minutes, arrM,arrD,arrH,arrMi);
                    minutes += t.stopover[i-1];
                    addMinutesToDate(qm, qd, t.startH, t.startM, minutes, depM,depD,depH,depMi);
                    cout<<t.stations[i]<<' '<<(arrM<10?"0":"")<<arrM<<'-'<<(arrD<10?"0":"")<<arrD<<' '<<(arrH<10?"0":"")<<arrH<<':'<<(arrMi<10?"0":"")<<arrMi<<" -> ";
                    cout<<(depM<10?"0":"")<<depM<<'-'<<(depD<10?"0":"")<<depD<<' '<<(depH<10?"0":"")<<depH<<':'<<(depMi<10?"0":"")<<depMi<<' ';
                    cout<<accPrice<<' '<<t.seatNum<<"\n";
                }
                if(i<t.stationNum-1) accPrice += t.prices[i];
            }
        }else if(cmd=="delete_train"){
            string rest; getline(cin, rest); string par, tid; istringstream iss(rest); while(iss>>par){ if(par=="-i") iss>>tid; }
            int ti = findTrain(tid);
            if(ti==-1 || trainsArr[ti].released==true){ cout<<-1<<"\n"; }
            else { trainsArr[ti].exists=false; cout<<0<<"\n"; }
        }else if(cmd=="query_ticket"){
            string rest; getline(cin, rest);
            cout<<0<<"\n";
        }else if(cmd=="query_transfer"){
            string rest; getline(cin, rest);
            cout<<0<<"\n";
        }else if(cmd=="clean"){
            for(int i=0;i<userCount;i++){ usersArr[i]=User(); }
            userCount=0;
            for(int i=0;i<trainCount;i++){ trainsArr[i]=Train(); }
            trainCount=0;
            cout<<0<<"\n";
        }else if(cmd=="exit"){
            cout<<"bye\n";
            break;
        }else{
            string rest; getline(cin, rest);
            cout<<-1<<"\n";
        }
    }
    return 0;
}
