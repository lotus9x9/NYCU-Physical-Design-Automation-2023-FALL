#include <bits/stdc++.h>
using namespace std;

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
    int totalmos;
    double PMOS_width;
    double NMOS_width;
    vector<mos> moslist;
    vector<mos> pmoslist;
    vector<mos> nmoslist;
    unordered_map<string, pair<bool, bool>> pin_state;
    double bestcost;
    vector<int>SA_sequence;
    set<int>dummy_position;
    vector<int>dummy_sequence;
    vector<string>pmos_pin;
    vector<string>nmos_pin;
    vector<int> best_dummy;
    vector<string> best_pmos;
    vector<string> best_nmos;
    unordered_map<string, set<int>> pin_loc;
public:
    Placement(){};
    ~Placement(){};
    void inputfile(string filename){
        ifstream in(filename);
        mos tmpmos;
        char temp_char;
        string temp_str;
        getline(in, temp_str);
        while(in >> temp_char) {
            in >> temp_str;
            if(temp_str == "ENDS") break;
            tmpmos.name = temp_str;
            in >> temp_str;
            tmpmos.drain = temp_str;
            in >> temp_str;
            tmpmos.gate = temp_str;
            in >> temp_str;
            tmpmos.source = temp_str;
            in >> temp_str >> temp_str;
            if(temp_str == "pmos_rvt") {
                tmpmos.type = 1;
                in >> temp_char >> temp_char >> temp_str;
                PMOS_width = stod(temp_str);
                pin_state[tmpmos.source].first = true;
                pin_state[tmpmos.drain].first = true;
            }
            else {
                tmpmos.type = 0;
                in >> temp_char >> temp_char >> temp_str;
                NMOS_width = stod(temp_str);
                pin_state[tmpmos.source].second = true;
                pin_state[tmpmos.drain].second = true;
            }
            in >> temp_char >> temp_str >> temp_str;
            tmpmos.drainisconnected = tmpmos.sourceisconnected = tmpmos.direction = 0;        
            moslist.push_back(tmpmos);
        }
        totalmos = moslist.size() / 2;
    }
    void seperatemos(){
        mos dummy;
        dummy.name="Dummy";
        dummy.drain = dummy.gate = dummy.source = "";
        dummy.type = 0;
        dummy.direction = 0;
        dummy.drainisconnected = dummy.sourceisconnected = 1;
        pmoslist.push_back(dummy); 
        nmoslist.push_back(dummy);
        vector<bool>visited(moslist.size(),false);
        for (int i = 0; i < moslist.size(); i++)
        {
            if (visited[i]) continue;
            int tmptype;
            tmptype=moslist[i].type;
            string tmpgate;
            tmpgate=moslist[i].gate;
            if (tmptype) pmoslist.push_back(moslist[i]);
            else nmoslist.push_back(moslist[i]);
            visited[i]=true;
            for (int j = 0; j< moslist.size(); j++){
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
    void initial_SA_Sequence(){
        for (int i = 1; i < totalmos; i++)
        {
            SA_sequence.push_back(i);
        }
    }
    set<int> finddummyposition(){
        set<int>dummy_position;
        vector<int>pmos_dummy_position;
        for(int i = 0; i < SA_sequence.size()-1; i++) {
            if(pmoslist[SA_sequence[i]].drain == pmoslist[SA_sequence[i+1]].drain && !pmoslist[SA_sequence[i]].drainisconnected) {
                pmoslist[SA_sequence[i]].drainisconnected = 1;
                pmoslist[SA_sequence[i]].direction = 1;
                pmoslist[SA_sequence[i+1]].drainisconnected = 1;
                pmoslist[SA_sequence[i+1]].direction = 0;
            }
            else if(pmoslist[SA_sequence[i]].drain == pmoslist[SA_sequence[i+1]].source && !pmoslist[SA_sequence[i]].drainisconnected) {
                pmoslist[SA_sequence[i]].drainisconnected = 1;
                pmoslist[SA_sequence[i]].direction = 1;
                pmoslist[SA_sequence[i+1]].sourceisconnected = 1;
                pmoslist[SA_sequence[i+1]].direction = 1;
            }
            else if(pmoslist[SA_sequence[i]].source == pmoslist[SA_sequence[i+1]].source && !pmoslist[SA_sequence[i]].sourceisconnected) {
                pmoslist[SA_sequence[i]].sourceisconnected = 1;
                pmoslist[SA_sequence[i]].direction = 0;
                pmoslist[SA_sequence[i+1]].sourceisconnected = 1;
                pmoslist[SA_sequence[i+1]].direction = 1;
            }
            else if(pmoslist[SA_sequence[i]].source == pmoslist[SA_sequence[i+1]].drain && !pmoslist[SA_sequence[i]].sourceisconnected) {
                pmoslist[SA_sequence[i]].sourceisconnected = 1;
                pmoslist[SA_sequence[i]].direction = 0;
                pmoslist[SA_sequence[i+1]].drainisconnected = 1;
                pmoslist[SA_sequence[i+1]].direction = 0;
            }
            else {
                pmos_dummy_position.push_back(i);
            }
        }
        vector<int>nmos_dummy_position;
        for(int i = 0; i < SA_sequence.size()-1; i++) {
            if(nmoslist[SA_sequence[i]].drain == nmoslist[SA_sequence[i+1]].drain && !nmoslist[SA_sequence[i]].drainisconnected) {
                nmoslist[SA_sequence[i]].drainisconnected = 1;
                nmoslist[SA_sequence[i]].direction = 1;
                nmoslist[SA_sequence[i+1]].drainisconnected = 1;
                nmoslist[SA_sequence[i+1]].direction = 0;
            }
            else if(nmoslist[SA_sequence[i]].drain == nmoslist[SA_sequence[i+1]].source && !nmoslist[SA_sequence[i]].drainisconnected) {
                nmoslist[SA_sequence[i]].drainisconnected = 1;
                nmoslist[SA_sequence[i]].direction = 1;
                nmoslist[SA_sequence[i+1]].sourceisconnected = 1;
                nmoslist[SA_sequence[i+1]].direction = 1;
            }
            else if(nmoslist[SA_sequence[i]].source == nmoslist[SA_sequence[i+1]].source && !nmoslist[SA_sequence[i]].sourceisconnected) {
                nmoslist[SA_sequence[i]].sourceisconnected = 1;
                nmoslist[SA_sequence[i]].direction = 0;
                nmoslist[SA_sequence[i+1]].sourceisconnected = 1;
                nmoslist[SA_sequence[i+1]].direction = 1;
            }
            else if(nmoslist[SA_sequence[i]].source == nmoslist[SA_sequence[i+1]].drain && !nmoslist[SA_sequence[i]].sourceisconnected) {
                nmoslist[SA_sequence[i]].sourceisconnected = 1;
                nmoslist[SA_sequence[i]].direction = 0;
                nmoslist[SA_sequence[i+1]].drainisconnected = 1;
                nmoslist[SA_sequence[i+1]].direction = 0;
            }
            else {
                nmos_dummy_position.push_back(i);
            }
        }
        for (int i = 0; i < pmos_dummy_position.size(); i++){
            dummy_position.insert(pmos_dummy_position[i]);
        }
        for (int i = 0; i < nmos_dummy_position.size(); i++){
            dummy_position.insert(nmos_dummy_position[i]);
        }
        return dummy_position;
    }
    vector<int> finddummysequence() {
        vector<int> seq_dummy;
        int i = 0;
        for(auto it = dummy_position.begin(); it != dummy_position.end(); it++) {
            for(; i <= (*it); i++) 
                seq_dummy.push_back(SA_sequence[i]);
            seq_dummy.push_back(0);
        }
        for(; i < SA_sequence.size(); i++) 
            seq_dummy.push_back(SA_sequence[i]);
        
        return seq_dummy;
    }
    void clearall(){
        pmos_pin.clear();
        nmos_pin.clear();
        pin_loc.clear();
        for (int i = 1; i < pmoslist.size(); i++)
        {
            pmoslist[i].drainisconnected=false;
            pmoslist[i].sourceisconnected=false;
            nmoslist[i].drainisconnected=false;
            nmoslist[i].sourceisconnected=false;
        }
    }
    void get_sequence_and_pin() {
        dummy_position=finddummyposition();
        dummy_sequence=finddummysequence();
        dummy_sequence.push_back(0);
        for(int i = 0; i < dummy_sequence.size()-1; i++) {
            if(dummy_sequence[i] == 0) {
                for(int i = 0; i < 3; i++) {
                    pmos_pin.push_back("Dummy");
                    nmos_pin.push_back("Dummy");
                }
                continue;
            }

            if(pmoslist[dummy_sequence[i]].direction == 0) {
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].drain);
                pin_loc[pmoslist[dummy_sequence[i]].drain].insert(pmos_pin.size()-1);

                pmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            else {
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].source);
                pin_loc[pmoslist[dummy_sequence[i]].source].insert(pmos_pin.size()-1);
                
                pmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }

            if(nmoslist[dummy_sequence[i]].direction == 0) {
                nmos_pin.push_back(nmoslist[dummy_sequence[i]].drain);
                pin_loc[nmoslist[dummy_sequence[i]].drain].insert(nmos_pin.size()-1);
                
                nmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }
            else {
                nmos_pin.push_back(nmoslist[dummy_sequence[i]].source);
                pin_loc[nmoslist[dummy_sequence[i]].source].insert(nmos_pin.size()-1);
                
                nmos_pin.push_back(pmoslist[dummy_sequence[i]].gate);
            }

            if(dummy_sequence[i+1] == 0) {
                if(pmoslist[dummy_sequence[i]].direction == 0) {
                    pmos_pin.push_back(pmoslist[dummy_sequence[i]].source);
                    pin_loc[pmoslist[dummy_sequence[i]].source].insert(pmos_pin.size()-1);
                }
                else {
                    pmos_pin.push_back(pmoslist[dummy_sequence[i]].drain);
                    pin_loc[pmoslist[dummy_sequence[i]].drain].insert(pmos_pin.size()-1);
                }

                if(nmoslist[dummy_sequence[i]].direction == 0) {
                    nmos_pin.push_back(nmoslist[dummy_sequence[i]].source);
                    pin_loc[nmoslist[dummy_sequence[i]].source].insert(nmos_pin.size()-1);
                }
                else {
                    nmos_pin.push_back(nmoslist[dummy_sequence[i]].drain);
                    pin_loc[nmoslist[dummy_sequence[i]].drain].insert(nmos_pin.size()-1);
                }
            }
        }
        dummy_sequence.pop_back();

    }
    double cal_cost() {
        double cost=0;
        get_sequence_and_pin();
        int left, right;
        const double verti = PMOS_width * 0.5 + NMOS_width * 0.5 + 27;
        for (auto it = pin_state.begin(); it != pin_state.end(); it++) {
            double x = 0;
            double y = 0;
            left = *(pin_loc[(*it).first].begin());
            right = *(--(pin_loc[(*it).first].end()));
            
            if((*it).second.first == true && (*it).second.second == true) {
                y = verti;
            }
            else {
                if(left == right) continue;
            }
            if(left != 0 && right != nmos_pin.size()-1) x = ((right-left)/2)*20 + ((right-left)/2-1)*34 + 0.5*34 + 0.5*34;
            else if(left == 0 && right == nmos_pin.size()-1) x = ((right-left)/2)*20 + ((right-left)/2-1)*34 + 0.5*25 + 0.5*25;
            else x = ((right-left)/2)*20 + ((right-left)/2-1)*34 + 0.5*34 + 0.5*25;
            

            cost += x + y;
        }
        return cost;
    }
    void m1(int *a,int *b,bool ctrl){
        if (ctrl){
            *a=rand()%SA_sequence.size();
            *b=rand()%SA_sequence.size();
            while (*a==*b){
                *b=rand()%SA_sequence.size();
            }
        }
        swap(SA_sequence[(*a)],SA_sequence[(*b)]);
    }
    bool canmove(double T,double cost){
        if (cost<=0) return true;
        double x = (double) rand()/(RAND_MAX + 1.0);
        if(x < exp(-(cost/T)))return true;
        return false;
    }
    void findbestcost(){
        vector<int>best_sequence;
        bestcost=cal_cost();
        best_sequence=SA_sequence;
        best_dummy=dummy_sequence;
        best_pmos=pmos_pin;
        best_nmos=nmos_pin;
    }
    void simulated_annealing(){
        double T,T0;
        double cur_cost,new_cost,delta_cost;
        int MP=0,uphill=0;
        int N;
        vector<int>best_sequence;
        bestcost=cal_cost();
        best_sequence=SA_sequence;
        best_dummy=dummy_sequence;
        best_pmos=pmos_pin;
        best_nmos=nmos_pin;
        T0=50000;
        T=T0;
        int x=totalmos*pin_loc.size();
        N=10*sqrt(x);
        cout<<N<<endl;
        clearall();
        while (T>0.01){
            while (MP<2*N&&uphill<N){
                int i,j;
                cur_cost=cal_cost();
                clearall();

                m1(&i,&j,true);
                new_cost=cal_cost();
                delta_cost=(new_cost-cur_cost)/(totalmos*pin_loc.size());
                MP++;
                if (canmove(T,delta_cost)){
                    if (delta_cost>0)uphill++;
                    else if(new_cost<bestcost){
                        bestcost=new_cost;
                        best_dummy=dummy_sequence;
                        best_pmos=pmos_pin;
                        best_nmos=nmos_pin;
                    }
                }
                else m1(&i,&j,false);
                clearall();
            }
            MP=0;
            uphill=0;
            T = 0.85*T;
            cout<<T<<"   newcost="<<new_cost<<"  bestcost="<<bestcost<<endl;
        }
    }
    void outputfile(string filename){
        ofstream out(filename);
        out<<bestcost<<endl;
        for(int i = 0; i < best_dummy.size(); i++) {
            out << pmoslist[best_dummy[i]].name << " ";
        }
        out << endl;
        for(int i = 0; i < best_pmos.size(); i++) {
            if(best_pmos[i] == "Dummy")
                i+=2;
            out << best_pmos[i] << " ";
        }
        out << endl;

        for(int i = 0; i < best_dummy.size(); i++) {
            out << nmoslist[best_dummy[i]].name << " ";
        }
        out << endl;
        for(int i = 0; i < best_nmos.size(); i++) {
            if(best_nmos[i] == "Dummy")
                i+=2;
            out << best_nmos[i] << " ";
        }
    }
};


int main(){
    srand(time(NULL));
    Placement s;
    s.inputfile("input.txt");
    s.initial_SA_Sequence();
    s.seperatemos();
    s.findbestcost();
    // s.simulated_annealing();
    s.outputfile("output.txt");
}