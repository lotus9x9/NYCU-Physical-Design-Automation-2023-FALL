#include <bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

uint64_t timeSinceEpochMillisec() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#define ALPHA 0.67
#define P 0.99
#define epsilon 0.00001
#define highratio 0.1
#define lowratio 0.78

struct InputInf{
    int width;
    int height;
    string name;
};

class Block{
public:
    Block(){
        x=0;
        y=0;
        w=0;
        h=0;
    };
    string outputName;
    string name;
    int x,y;
    int w,h;
    list<pair<int,int>> widthandheight;
    vector<Block*>child;
};
bool cmp(Block* a, Block* b){
    return stoi(a->name) < stoi(b->name);
}
class slicingtree{
public:
    slicingtree(): totalArea(0) {};
    ~slicingtree(){};
    
    int totalArea;
    double bestcost;
    double lowerbound,upperbound; 

    vector<InputInf> width_height;
    void inputfunc(string filename){
        ifstream input(filename);
        input >> lowerbound >> upperbound;
        input.ignore(2,'\n');
        string curLine;
        while(getline(input,curLine)){
            stringstream ss(curLine);
            InputInf inf;
            ss >> inf.name;
            ss >> inf.width >> inf.height;
            totalArea += inf.width * inf.height;
            width_height.push_back(inf);
        }
    }
    vector<string> initialString(){
        vector<string>s;
        s.push_back("1");
        s.push_back("2");
        s.push_back("V");
        for(int i = 3; i <= width_height.size(); i++){
            s.push_back(to_string(i));
            int x=rand()%2;
            if (x==0)
            {
                s.push_back("V");
            }
            else s.push_back("H");
        }
        return s;
    }
    vector<Block*> converttoblock(vector<string>const &a){
        vector<Block*>s;
        for (int i = 0; i < a.size(); i++)
        {
            if (a[i]!="H"&&a[i]!="V")
            {
                int tmpw=width_height[stoi(a[i])-1].width;
                int tmph=width_height[stoi(a[i])-1].height;
                
                Block* tmp=new Block;
                tmp->outputName = width_height[stoi(a[i])-1].name;
                tmp->name=a[i];
                int width ;
                int height;
                if (tmpw>tmph)
                {
                    tmp->widthandheight.push_back({tmph,tmpw});
                }else{
                    tmp->widthandheight.push_back({tmpw,tmph});
                }
                s.push_back(tmp);
            }
            else {
                Block* tmp2=new Block;
                tmp2->name=a[i];
                s.push_back(tmp2);
            }
        }
        return s;
    }
    void deleteNPE(vector<Block*> &npe){
        int len = npe.size();
        for(int i = 0; i < len; i++){
            delete npe[i];
        }
    }
    void stackBlock(Block* a, Block* b, Block* op){
        if(op->name == "H"){
            auto it1 = a->widthandheight.rbegin();
            auto it2 = b->widthandheight.rbegin();
            while(it1 != a->widthandheight.rend() && it2 != b->widthandheight.rend()){
                int wl=(*it1).first;
                int hl=(*it1).second;
                int wr=(*it2).first;
                int hr=(*it2).second;
                int width = max(wl,wr);
                int height = hl+hr;
                op->widthandheight.push_front({width,height});
                if(wl > wr) it1++;
                else if(wr > wl) it2++;
                else { 
                    it1++; 
                    it2++; 
                }
            }
        }
        else{ 
            auto it1 = a->widthandheight.begin();
            auto it2 = b->widthandheight.begin();
            while(it1 != a->widthandheight.end() && it2 != b->widthandheight.end()){
                int wl=(*it1).first;
                int hl=(*it1).second;
                int wr=(*it2).first;
                int hr=(*it2).second;
                int width = wl+wr;
                int height = max(hl,hr);
                op->widthandheight.push_front({width,height});
                if(hl > hr) it1++;
                else if(hr > hl) it2++;
                else { 
                    it1++; 
                    it2++; 
                }
            }
        }
        op->child.push_back(a);
        op->child.push_back(b);
    }
    double findans(vector<Block*> &a){
        stack<Block*>s;
        for (int i = 0; i < a.size(); i++)
        {
            if (a[i]->name!="H"&&a[i]->name!="V")
            {
                s.push(a[i]);
            }
            else {
                Block* tmp1;
                Block* tmp2;
                tmp1=s.top();
                s.pop();
                tmp2=s.top();
                s.pop();
                stackBlock(tmp1,tmp2,a[i]);
                s.push(a[i]);
            }
        }
        Block* c=s.top();
        double minCost = DBL_MAX;
        for (auto it=c->widthandheight.begin(); it != c->widthandheight.end(); it++)
        {   
            int area;
            int w=(*it).first;
            int h=(*it).second;
            area=w*h;
            int longer=max(w,h);
            int shorter=min(w,h);
            double R=(double)longer/(double)shorter;
            double penalty=(R>=lowerbound&&R<=upperbound)?0:5;
            double cost = ALPHA*(double)area/(double)totalArea+(1-ALPHA)*penalty;
            if(cost<minCost){
                minCost=cost;
                c->w=w;
                c->h=h;
            }
        }
        return minCost;
    }
    void M1(vector<string> &s){
        vector<int>tmp;
            for (int i = 0; i < s.size(); i++)
            {
                if (s[i]!="H"&&s[i]!="V")
                {
                    tmp.push_back(i);
                }
            }
            int a=rand()%(tmp.size()-1);
            swap(s[tmp[a]],s[tmp[a+1]]);
    }
    void M2(vector<string> &s){
        vector<int>tmp;
        for (int i = 0; i < s.size()-1; i++)
        {
            if ((s[i]!="H"&&s[i]!="V")&&(s[i+1]=="H"||s[i+1]=="V"))
            {
                tmp.push_back(i+1);
            }
        }
        int a=rand()%(tmp.size());
        for (int i = tmp[a]; i < s.size(); i++)
        {
            if (s[i]=="H"||s[i]=="V")
            {
                if (s[i]=="H")
                {
                    s[i]="V";
                }
                else s[i]="H";
            }
            else {
                break;
            }
        }
    }
    void M3(vector<string> &s){
        vector<int> pos;
        int len_s = s.size();
        for(int i = 0; i < len_s-1; i++){
            if(((s[i] != "H" && s[i] != "V") && (s[i+1] == "H" || s[i+1] == "V"))||((s[i] == "H" || s[i] == "V") && (s[i+1] != "H" && s[i+1] != "V"))){
                pos.push_back(i);
            }
        }
        while(!pos.empty()){
            int id = rand() % pos.size();
            id = pos[id];
            vector<string> tmp = s;
            swap(tmp[id],tmp[id+1]);
            if(ballot(tmp) && skew(tmp)){
                s = tmp;
                break;
            }
            else{
                pos.erase(find(pos.begin(),pos.end(),id));
            }
        }
    }
    bool ballot(vector<string>const &s){
        int a=0; int b=0;
        for(int i=0;i<s.size();i++){
            if(s[i]=="H"|| s[i]=="V"){ 
                a++;
            }
            if(s[i]!="H" && s[i]!="V"){ 
            b++;					
            }
            if(a>=b)return false; 
        }
        return true;
    }
    bool skew(vector<string>const &s){
        for (int i = 0; i < s.size()-1; i++)
        {
            if (s[i]=="H"||s[i]=="V")
            {
                if (s[i]==s[i+1])
                {
                    return false;
                }
            }
        }
        return true;
    }
    double initialT(vector<string> &s){
        double total=0;
        int n=0;
        for (int i = 0; i < 100; i++)
        {   
            vector<Block*>tmp1=converttoblock(s);
            double cost=findans(tmp1);
            int a=rand()%3;
            switch (a)
            {
            case 0:
                M1(s);
                break;
            case 1:
                M2(s);
                break;
            case 2:
                M3(s);
                break;
            }
            vector<Block*>tmp2=converttoblock(s);
            double newcost=findans(tmp2);
            if (newcost>cost)
            {
                total+=newcost-cost;
                n++;
            }
        }
        double T=abs((total/n)/log(P));
        return T;
    }
    vector<Block*> SA(vector<string> &s){
        int N = 15*(s.size());
        vector<string> best = s; 
        vector<string> tmp = s; 
        double T0 = initialT(tmp);
        int MT, uphill, reject;
        double T = T0;
        vector<Block*> best_npe = converttoblock(best);
        double bestCost = findans(best_npe);
        while(true){
            MT = 0;
            uphill = 0;
            reject = 0;
            vector<Block*> old_npe;
            double oldcost;
            while(true){
                if(old_npe.empty()){
                    old_npe = converttoblock(tmp);
                    oldcost = findans(old_npe);
                }
                int op = rand() % 3;
                switch (op){
                    case 0: M1(tmp); break;
                    case 1: M2(tmp); break;
                    case 2: M3(tmp); break;
                    default: break;
                }
                MT++;
                vector<Block*> new_npe = converttoblock(tmp);
                double newcost = findans(new_npe);
                double diff=newcost-oldcost;
                double R = (double)rand() / RAND_MAX;
                if(diff < 0 || R < exp(-1 * diff / T)){
                    if(old_npe != best_npe) deleteNPE(old_npe);
                    old_npe=new_npe;
                    oldcost=newcost;
                    s=tmp;
                    if(diff > 0) uphill++;
                    if(newcost < bestCost) {
                        best = s;
                        bestCost = newcost;
                        if(!best_npe.empty()) deleteNPE(best_npe);
                        best_npe = new_npe;
                        bestcost = newcost;
                    }
                }
                else{
                    tmp = s; 
                    deleteNPE(new_npe);
                    reject++;
                }
                
                if(uphill>N||MT>2*N) break;
            }
            T=(T > 0.05*T0)?highratio*T:lowratio*T;
            cout<<T<<endl;
            if((double)reject/(double)MT > 0.95||T<epsilon) break;
        }
        s = best;
        return best_npe;
    }
    void retrace(vector<Block*> &s){
        queue<Block*>a;
        a.push(s[s.size()-1]);
        while (!a.empty())
        {
            Block* currentblock=a.front();
            int currentw=currentblock->w;
            int currenth=currentblock->h;
            a.pop();

            Block *leftnode=currentblock->child[0];
            Block *rightnode=currentblock->child[1];
            for (auto i = leftnode->widthandheight.begin(); i!=leftnode->widthandheight.end(); i++)
            {
                int wl=(*i).first;
                int hl=(*i).second;
                for (auto j = rightnode->widthandheight.begin(); j != rightnode->widthandheight.end(); j++)
                {
                    int wr=(*j).first;
                    int hr=(*j).second;
                    if (currentblock->name=="H")
                    {
                        if (max(wl,wr)==currentw&&hl+hr==currenth)
                        {
                            leftnode->w=wl;
                            leftnode->h=hl;
                            rightnode->w=wr;
                            rightnode->h=hr;
                        }
                    }
                    else if (currentblock->name=="V")
                    {
                        if (wl+wr==currentw&&max(hl,hr)==currenth)
                        {
                            leftnode->w=wl;
                            leftnode->h=hl;
                            rightnode->w=wr;
                            rightnode->h=hr;
                        }
                    }
                }
            }
            leftnode->x=currentblock->x;
            leftnode->y=currentblock->y;
            if (currentblock->name=="V")
            {
                rightnode->x=leftnode->x+leftnode->w;
                rightnode->y=leftnode->y;
            }
            else {
                rightnode->x=leftnode->x;
                rightnode->y=leftnode->y+leftnode->h;
            }
            if (leftnode->name=="H"||leftnode->name=="V")
            {
                a.push(leftnode);
            }
            if (rightnode->name=="H"||rightnode->name=="V")
            {
                a.push(rightnode);
            }
        }
    }
    bool enditer(vector<Block*> const &npe){
        Block* topBlock = npe[npe.size()-1];
        int curWidth = topBlock->w;
        int curHeight = topBlock->h;
        double curAspectRatio = (double)curWidth/(double)curHeight;
        return (curAspectRatio <= upperbound) && (curAspectRatio >= lowerbound);
    }
    void printResult(vector<Block*> npe, string filename){
        ofstream output(filename);
        Block* finalFPInf = npe[npe.size()-1];
        output << "A = " << finalFPInf->w * finalFPInf->h <<endl;
        cout<<finalFPInf->w * finalFPInf->h<<endl;
        output << "R = " << (double)finalFPInf->w / (double)finalFPInf->h <<endl;
        vector<Block*> tmp;
        for(int i = 0; i < npe.size(); i++){
            if(npe[i]->name != "H" && npe[i]->name != "V") tmp.push_back(npe[i]);
        }
        sort(tmp.begin(), tmp.end(), cmp);
        for(int i = 0; i < tmp.size(); i++){
            output << tmp[i]->outputName << " " << tmp[i]->x << " " << tmp[i]->y;
            if(tmp[i]->w != width_height[i].width) output << " R";
            output << endl;
        }
        output.close();
    }
};

int main(int argc, char* argv[]){
    slicingtree st;
    vector<string> s;
    srand(time(NULL));
    uint64_t start = timeSinceEpochMillisec(); 
    st.inputfunc(argv[1]);
    s=st.initialString();
    vector<Block*> npe;
    int iterTimes = 0;
    bool flag=0;
    while(!flag){
        npe = st.SA(s);
        iterTimes++;
        cout << "Iteration: " << iterTimes;
        uint64_t end = timeSinceEpochMillisec();
        cout << "   current time: " << end - start << "ms\n";
        flag=st.enditer(npe);
    }
    st.retrace(npe);
    st.printResult(npe,argv[2]);
    
    for(int i = 0; i < s.size(); i++){
        cout << s[i];
    }
    cout << '\n';
    cout << st.bestcost << '\n';

    uint64_t end = timeSinceEpochMillisec();
    cout << "Total time: " << end - start << "ms\n";
    return 0;
}