#include<bits/stdc++.h>
using namespace std;
class leftedgerouting{
private:
    int num_pin;
    int num_net;
    vector<int> net_num;
    vector<int> track_num;
    vector<int> top;
    vector<int> bottom;
    vector<vector<int>> track;
    vector<int> out_1;
    vector<int> out_2;
public:
    leftedgerouting(){};
    ~leftedgerouting(){};
    void read_file(string filename);
    bool isHead(int i, vector<vector<int>> &vcg);
    unordered_map<int, pair<int, int>> Sort(map<int, pair<int, int>> &interval);
    void routing();
    void outputfile(string outputfile);
};