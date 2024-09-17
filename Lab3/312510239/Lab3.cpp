#include <bits/stdc++.h>
using namespace std;
#include <chrono>
using namespace std::chrono;

uint64_t timeSinceEpochMillisec() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
struct mos
{
    string name;
    string gate;
    string source;
    string drain;
    bool type;
    bool drainisconnected;
    bool sourceisconnected;
    bool direction;
};

class Placement{

private:
    double PMOS_width;
    double NMOS_width;
    double vertical;
    vector<mos> moslist;
    vector<mos> pmoslist;
    vector<mos> nmoslist;
    unordered_map<string, pair<bool, bool>> pin_state;
    double bestcost;
    vector<int>init_seq;
    set<int>dummy_position;
    vector<int>dummy_sequence;
    vector<string>pmos_pin;
    vector<string>nmos_pin;
    unordered_map<string, set<int>> locationofpin;
public:
    Placement(){};
    ~Placement(){};
    void inputfile(string filename){
        ifstream in(filename);
        mos tmpmos;
        char temp_char;
        string tmp;
        getline(in, tmp);
        while(in>>temp_char) {
            in>>tmp;
            if(tmp=="ENDS") break;
            tmpmos.name=tmp;
            in>>tmpmos.drain>>tmpmos.gate>>tmpmos.source;
            in>>tmp>>tmp;
            if(tmp=="pmos_rvt") {
                tmpmos.type=1;
                in>>tmp;
                tmp.erase(0,2);
                PMOS_width=stod(tmp);
                pin_state[tmpmos.source].first=true;
                pin_state[tmpmos.drain].first=true;
            }
            else {
                tmpmos.type=0;
                in>>tmp;
                tmp.erase(0,2);
                NMOS_width=stod(tmp);
                pin_state[tmpmos.source].second=true;
                pin_state[tmpmos.drain].second=true;
            }
            in>>tmp>>tmp;
            tmpmos.drainisconnected=false;
            tmpmos.sourceisconnected=false;
            tmpmos.direction=false;
            moslist.push_back(tmpmos);
        }
        vertical=PMOS_width*0.5+NMOS_width*0.5+27;
    }
    void seperatemos(){
        mos dummy;
        dummy.name="Dummy";
        dummy.drain.clear();
        dummy.gate.clear();
        dummy.source.clear();
        dummy.type=0;
        dummy.direction=0;
        dummy.drainisconnected=true;
        dummy.sourceisconnected=true;
        pmoslist.push_back(dummy); 
        nmoslist.push_back(dummy);
        vector<bool>visited(moslist.size(),false);
        for (int i=0; i < moslist.size(); i++)
        {
            if (visited[i]) continue;
            int tmptype;
            tmptype=moslist[i].type;
            string tmpgate;
            tmpgate=moslist[i].gate;
            if (tmptype) pmoslist.push_back(moslist[i]);
            else nmoslist.push_back(moslist[i]);
            visited[i]=true;
            for (int j=0; j< moslist.size(); j++){
                if (visited[j]) continue;
                if (moslist[j].gate==tmpgate){
                    if (!tmptype&&moslist[j].type){
                        pmoslist.push_back(moslist[j]);
                        visited[j]=true;
                        break;
                    }
                    else if (tmptype&&!moslist[j].type)
                    {
                        nmoslist.push_back(moslist[j]);
                        visited[j]=true;
                        break;
                    }    
                }
            }
        }
    }
    void initial_Sequence(){
        for (int i=1; i < moslist.size()/2; i++)
        {
            init_seq.push_back(i);
        }
        // for (int i=0; i < 50; i++)
        // {
        //     int a=rand()%init_seq.size();
        //     int b=rand()%init_seq.size();
        //     swap(init_seq[a],init_seq[b]);
        // }
        
    }
    set<int> finddummyposition(){
        set<int>dummy_position;
        vector<int>pmos_dummy_position;
        for(int i=0; i < init_seq.size()-1; i++) {
            if(!pmoslist[init_seq[i]].drainisconnected&&pmoslist[init_seq[i]].drain==pmoslist[init_seq[i+1]].drain) {
                pmoslist[init_seq[i]].drainisconnected=1;
                pmoslist[init_seq[i]].direction=1;
                pmoslist[init_seq[i+1]].drainisconnected=1;
                pmoslist[init_seq[i+1]].direction=0;
            }
            else if(!pmoslist[init_seq[i]].drainisconnected&&pmoslist[init_seq[i]].drain==pmoslist[init_seq[i+1]].source) {
                pmoslist[init_seq[i]].drainisconnected=1;
                pmoslist[init_seq[i]].direction=1;
                pmoslist[init_seq[i+1]].sourceisconnected=1;
                pmoslist[init_seq[i+1]].direction=1;
            }
            else if(!pmoslist[init_seq[i]].sourceisconnected&&pmoslist[init_seq[i]].source==pmoslist[init_seq[i+1]].source) {
                pmoslist[init_seq[i]].sourceisconnected=1;
                pmoslist[init_seq[i]].direction=0;
                pmoslist[init_seq[i+1]].sourceisconnected=1;
                pmoslist[init_seq[i+1]].direction=1;
            }
            else if(!pmoslist[init_seq[i]].sourceisconnected&&pmoslist[init_seq[i]].source==pmoslist[init_seq[i+1]].drain) {
                pmoslist[init_seq[i]].sourceisconnected=1;
                pmoslist[init_seq[i]].direction=0;
                pmoslist[init_seq[i+1]].drainisconnected=1;
                pmoslist[init_seq[i+1]].direction=0;
            }
            else {
                pmos_dummy_position.push_back(i);
            }
        }
        for (int i=0; i < pmos_dummy_position.size(); i++){
            dummy_position.insert(pmos_dummy_position[i]);
        }
        vector<int>nmos_dummy_position;
        for(int i=0; i < init_seq.size()-1; i++) {
            if(!nmoslist[init_seq[i]].drainisconnected&&nmoslist[init_seq[i]].drain==nmoslist[init_seq[i+1]].drain) {
                nmoslist[init_seq[i]].drainisconnected=1;
                nmoslist[init_seq[i]].direction=1;
                nmoslist[init_seq[i+1]].drainisconnected=1;
                nmoslist[init_seq[i+1]].direction=0;
            }
            else if(!nmoslist[init_seq[i]].drainisconnected&&nmoslist[init_seq[i]].drain==nmoslist[init_seq[i+1]].source) {
                nmoslist[init_seq[i]].drainisconnected=1;
                nmoslist[init_seq[i]].direction=1;
                nmoslist[init_seq[i+1]].sourceisconnected=1;
                nmoslist[init_seq[i+1]].direction=1;
            }
            else if(!nmoslist[init_seq[i]].sourceisconnected&&nmoslist[init_seq[i]].source==nmoslist[init_seq[i+1]].source) {
                nmoslist[init_seq[i]].sourceisconnected=1;
                nmoslist[init_seq[i]].direction=0;
                nmoslist[init_seq[i+1]].sourceisconnected=1;
                nmoslist[init_seq[i+1]].direction=1;
            }
            else if(!nmoslist[init_seq[i]].sourceisconnected&&nmoslist[init_seq[i]].source==nmoslist[init_seq[i+1]].drain) {
                nmoslist[init_seq[i]].sourceisconnected=1;
                nmoslist[init_seq[i]].direction=0;
                nmoslist[init_seq[i+1]].drainisconnected=1;
                nmoslist[init_seq[i+1]].direction=0;
            }
            else {
                nmos_dummy_position.push_back(i);
            }
        }
        for (int i=0; i < nmos_dummy_position.size(); i++){
            dummy_position.insert(nmos_dummy_position[i]);
        }
        return dummy_position;
    }
    vector<int> finddummysequence() {
        vector<int> dummy_seq;
        int i=0;
        for(auto it=dummy_position.begin(); it != dummy_position.end(); it++) {
            for(; i <= (*it); i++) {
                dummy_seq.push_back(init_seq[i]);
            }
            dummy_seq.push_back(0);
        }
        for(; i < init_seq.size(); i++){
            dummy_seq.push_back(init_seq[i]);
        }
        return dummy_seq;
    }
    void get_sequence_and_pin() {
        dummy_position=finddummyposition();
        dummy_sequence=finddummysequence();
        dummy_sequence.push_back(0);
        for(int i=0; i < dummy_sequence.size()-1; i++) {
            if(dummy_sequence[i]==0) {
                for(int i=0; i < 3; i++) {
                    pmos_pin.push_back("Dummy");
                    nmos_pin.push_back("Dummy");
                }
                continue;
            }
            if(pmoslist[dummy_sequence[i]].direction) {
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].source);
                locationofpin[pmoslist[dummy_sequence[i]].source].insert(pmos_pin.size()-1);
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            else {
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].drain);
                locationofpin[pmoslist[dummy_sequence[i]].drain].insert(pmos_pin.size()-1);
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            if(nmoslist[dummy_sequence[i]].direction) {
                nmos_pin.push_back(nmoslist[dummy_sequence[i]].source);
                locationofpin[nmoslist[dummy_sequence[i]].source].insert(nmos_pin.size()-1);
                nmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            else {
                nmos_pin.push_back(nmoslist[dummy_sequence[i]].drain);
                locationofpin[nmoslist[dummy_sequence[i]].drain].insert(nmos_pin.size()-1);
                nmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            if(dummy_sequence[i+1]==0) {
                if(pmoslist[dummy_sequence[i]].direction==0) {
                    pmos_pin.push_back(pmoslist[dummy_sequence[i]].source);
                    locationofpin[pmoslist[dummy_sequence[i]].source].insert(pmos_pin.size()-1);
                }
                else {
                    pmos_pin.push_back(pmoslist[dummy_sequence[i]].drain);
                    locationofpin[pmoslist[dummy_sequence[i]].drain].insert(pmos_pin.size()-1);
                }

                if(nmoslist[dummy_sequence[i]].direction==0) {
                    nmos_pin.push_back(nmoslist[dummy_sequence[i]].source);
                    locationofpin[nmoslist[dummy_sequence[i]].source].insert(nmos_pin.size()-1);
                }
                else {
                    nmos_pin.push_back(nmoslist[dummy_sequence[i]].drain);
                    locationofpin[nmoslist[dummy_sequence[i]].drain].insert(nmos_pin.size()-1);
                }
            }
        }
        dummy_sequence.pop_back();

    }
    double cal_cost() {
        double cost=0;
        get_sequence_and_pin();
        int left, right;
        for (auto it=pin_state.begin(); it != pin_state.end(); it++) {
            double x=0;
            double y=0;
            left=*(locationofpin[(*it).first].begin());
            right=*(--(locationofpin[(*it).first].end()));
            
            if((*it).second.first&&(*it).second.second) {
                y=vertical;
            }
            else {
                if(left==right) continue;
            }
            if(left != 0&&right != nmos_pin.size()-1) x=((right-left)/2)*20+((right-left)/2-1)*34+0.5*34+0.5*34;
            else if(left==0&&right==nmos_pin.size()-1) x=((right-left)/2)*20+((right-left)/2-1)*34+0.5*25+0.5*25;
            else x=((right-left)/2)*20+((right-left)/2-1)*34+0.5*34+0.5*25;
            int newcost=x+y;
            cost+=newcost;
        }
        return cost;
    }
    void findbestcost(){
        bestcost=cal_cost();
    }
    void outputfile(string filename){
        ofstream out(filename);
        out<<bestcost<<endl;
        for(int i=0; i < dummy_sequence.size(); i++) {
            out<<pmoslist[dummy_sequence[i]].name<<" ";
        }
        out<<endl;
        for(int i=0; i < pmos_pin.size(); i++) {
            if(pmos_pin[i]=="Dummy")i=i+2;
            out<<pmos_pin[i]<<" ";
        }
        out<<endl;
        for(int i=0; i < dummy_sequence.size(); i++) {
            out<<nmoslist[dummy_sequence[i]].name<<" ";
        }
        out<<endl;
        for(int i=0; i < nmos_pin.size(); i++) {
            if(nmos_pin[i]=="Dummy")i=i+2;
            out<<nmos_pin[i]<<" ";
        }
    }
};

int main(int argc, char* argv[]){ 
    srand(time(NULL));
    Placement s;
    s.inputfile(argv[1]);
    s.initial_Sequence();
    s.seperatemos();
    s.findbestcost();
    s.outputfile(argv[2]);
    return 0;
}