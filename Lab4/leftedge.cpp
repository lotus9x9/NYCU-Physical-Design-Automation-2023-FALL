#include"leftedge.h"
bool interval_comp(pair<int, pair<int, int>> &lhs, pair<int, pair<int, int>> &rhs){
    if (lhs.second.first > rhs.second.first){
        return true;
    }
    else if (lhs.second.first==rhs.second.first){
        if (lhs.second.second > rhs.second.second) return true;
        else return false;
    }
    else return false;
}
void leftedgerouting::read_file(string filename){
    num_pin=0;
    num_net=0;
    net_num.clear();
    track_num.clear();
    top.clear();
    bottom.clear();
    track.clear();
    out_1.clear();
    out_2.clear();
    ifstream inputfile(filename);
    string line;
    bool reading=false;
    while (getline(inputfile, line)){
        stringstream ss(line);
        string buf;
        ss >> buf;
        if (buf[0]=='T' || (buf[0]=='B'))
            continue;
        else if (reading==false)
        {
            top.push_back(stoi(buf));
            while ((ss >> buf))
            {
                top.push_back(stoi(buf));
                if (stoi(buf) > num_net) num_net=stoi(buf);
            }
            reading=true;
        }
        else
        {
            bottom.push_back(stoi(buf));
            while ((ss >> buf))
            {
                bottom.push_back(stoi(buf));
                if (stoi(buf) > num_net) num_net=stoi(buf);
            }
        }
    }
    num_pin=top.size();
}
bool leftedgerouting::isHead(int i, vector<vector<int>> &vcg)
{ 
    if (vcg[i][i]==-1) return false;
    for (int j=1; j <= num_net; j++){
        if (vcg[j][i] > 0 && j != i) return false;
    }
    return true;
}
unordered_map<int, pair<int, int>> leftedgerouting::Sort(map<int, pair<int, int>> &interval)
{
    vector<pair<int, pair<int, int>>> tmp;
    for (auto &elem : interval){
        tmp.emplace_back(elem);
    }
    sort(tmp.begin(),tmp.end(),interval_comp);
    unordered_map<int, pair<int, int>> intervalS;
    for (auto &elem : tmp){
        intervalS[elem.first]=elem.second;
    }
    return intervalS;
}
void leftedgerouting::routing(){
    vector<vector<int>> vcg(num_pin, vector<int>(num_pin, 0));
    map<int, pair<int, int>> interval; 
    int tmp=num_pin;
    int col=1;
    int index=0;
    while (tmp--){
        int ix, iy;
        ix=top[index];
        iy=bottom[index];
        index++;
        if (interval.count(ix)){
            if (col > interval[ix].second) interval[ix].second=col;
        }
        else if (ix != 0) interval[ix]=make_pair(col, 0);
        if (interval.count(iy)){
            if (col > interval[iy].second) interval[iy].second=col;
        }
        else if (iy != 0) interval[iy]=make_pair(col, 0);
        col++;
        if (ix==0 || iy==0)
            continue;
        vcg[ix][iy]=1;
    }
    auto intervalS=Sort(interval);
    int watermark=0;
    while (!intervalS.empty()){
        if (track.size() > num_pin)
            break;
        vector<int> nets;
        nets.reserve(100);
        for (auto &elem : intervalS){
            if (elem.second.first <= watermark)
                continue;
            if (!isHead(elem.first, vcg))
                continue;
            for (auto &elem : vcg[elem.first])
                elem=-1;
            nets.emplace_back(elem.first);
            watermark=elem.second.second;
            net_num.push_back(elem.first);
            out_1.push_back(elem.second.first - 1);
            out_2.push_back(elem.second.second - 1);
        }
        track.emplace_back(nets);
        for (auto &elem : nets)
            intervalS.erase(elem);
        watermark=0;
    }
    track_num.push_back(0);
    for (int j=1; j <= net_num.size(); j++){
        for (int a=0; a < track.size(); a++){
            for (int b=0; b < track[a].size(); b++){
                if (j==track[a][b]){
                    track_num.push_back(a + 1);
                    break;
                }
            }
        }
    }
}
void leftedgerouting::outputfile(string outputfile){
    ofstream output(outputfile);
    output << "Channel density: " << track.size() << endl;
    for (int i=1; i <= net_num.size(); i++){
        for (int j=0; j < net_num.size(); j++){
            if (i==net_num[j]){
                int tt=(track.size() - track_num[i]) + 1;
                output << "Net " << net_num[j] << endl;
                output << "C" << tt << " " << out_1[j] << " " << out_2[j] <<endl;
                break;
            }
        }
    }
}