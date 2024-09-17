#include <bits/stdc++.h>
#include<unordered_set>
#include <chrono>
using namespace std::chrono;

uint64_t timeSinceEpochMillisec() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
using namespace std;

struct Net{
    vector<int>celllist;
    int cellinleft,cellinright;
};
struct Cell{
    vector<int>netlist;
    bool set;
    bool locked;
    int gain;
    int mincutsize;
    bool isupdate;
};
class graph
{
private:
    vector<Net>netarray;
    vector<Cell>cellarray;
    double balancefactor;
    int totalnets;
    int totalcells;
    int groupmaxsize;
    int groupminsize;
    int cutsize;
    int Pmax;
    int setleft;
    int mincutsize;
    int minsetleft;
    int cutsize1;
    int cutsize2;
    vector<bool>greatcutsize;
    vector<unordered_set<int>>bucketlistleft;
    vector<unordered_set<int>>bucketlistright;
public:
    graph(){
        balancefactor=0;
        totalnets=0;
        totalcells=0;
        groupmaxsize=0;
        groupminsize=0;
        cutsize=0;
        Pmax=0;
        setleft=0;
        cutsize1=INT32_MAX;
        cutsize2=INT32_MAX;
        mincutsize=INT32_MAX;
    }
    void Inputfile(string filename){
        ifstream inputfile(filename);
        string line;
        getline(inputfile,line);
        istringstream balance(line);
        balance>>balancefactor;
        while (getline(inputfile,line,';')){
            if (line[0]=='\n'||line[0]==' ')
            {
                line.erase(0,1);
            }
            istringstream iss(line);
            Net net;
            int cellname;
            string Netkeyword;
            string tmp;
            iss>>Netkeyword>>tmp;
            while (iss>>tmp)
            {
                tmp.erase(0,1);
                cellname=stoi(tmp)-1;
                bool found=0;
                for (int value :net.celllist)
                {
                    if (value==cellname)
                    {
                        found=1;
                        break;
                    }
                }
                if (!found)
                {
                    net.celllist.push_back(cellname);
                }
                if (cellname>=cellarray.size()){
                    cellarray.resize(cellname+1);
                    totalcells=cellname+1;
                }
                cellarray[cellname].netlist.push_back(totalnets);
            }
            netarray.push_back(net);
            totalnets++;
        }
        inputfile.close();
    }
    void initialgain(){
        mincutsize=INT32_MAX;
        cutsize=0;
        setleft=0;
        for (int i = 0; i < totalcells; i++)
        {
            if (!cellarray[i].set)
            {
                setleft++;
            }
        }
        for (int i = 0; i < totalnets; i++)
        {   
            netarray[i].cellinleft=0;
            netarray[i].cellinright=0;
            for (int j = 0; j < netarray[i].celllist.size(); j++)
            {
                if (cellarray[netarray[i].celllist[j]].set==0){
                    netarray[i].cellinleft++;
                }
                else
                {
                    netarray[i].cellinright++;
                }
            }
        }
        for (int i = 0; i < totalnets; i++)
        {
            if ((netarray[i].cellinleft!=0)&&(netarray[i].cellinright!=0))
            {
                cutsize++;
            }
        }
        
        for (int i = 0; i < totalcells; i++)
        {
            cellarray[i].gain=0;
            for (int j = 0; j < cellarray[i].netlist.size(); j++)
            {
                if ((cellarray[i].set==0)&&(netarray[cellarray[i].netlist[j]].cellinleft==1))
                {
                    cellarray[i].gain++;
                }
                else if ((cellarray[i].set==1)&&(netarray[cellarray[i].netlist[j]].cellinright==1))
                {
                    cellarray[i].gain++;
                }
                if ((cellarray[i].set==0)&&(netarray[cellarray[i].netlist[j]].cellinright==0))
                {
                    cellarray[i].gain--;
                }
                else if ((cellarray[i].set==1)&&(netarray[cellarray[i].netlist[j]].cellinleft==0))
                {
                    cellarray[i].gain--;
                }
            }
        }
    }
    void initialpart(){
        greatcutsize.resize(totalcells);
        groupminsize=(1-balancefactor)*cellarray.size()/2;
        groupmaxsize=(1+balancefactor)*cellarray.size()/2;
        int agroupsize=0;
        for (int i=0; i <groupmaxsize ; i++)
        {
            cellarray[i].set=0;
            setleft++;
        }
    }
    void initialbucketlist(){
        for (int i = 0; i < totalcells; i++)
        {
            if (cellarray[i].netlist.size()>=Pmax)
            {
                Pmax=cellarray[i].netlist.size();
            }
        }
        bucketlistleft.clear();
        bucketlistright.clear();
        bucketlistleft.resize(2*Pmax+1);
        bucketlistright.resize(2*Pmax+1);
        for (int i = 0; i < totalcells; i++)
        {
            if (cellarray[i].set==0)
            {
                bucketlistleft[cellarray[i].gain+Pmax].insert(i);
            }
            else bucketlistright[cellarray[i].gain+Pmax].insert(i);
        }
    }
    void updatebucketlist(int cellindex){
        if (!cellarray[cellindex].set)
        {
            auto tmp1=bucketlistleft[cellarray[cellindex].gain+Pmax].begin();
            bucketlistleft[cellarray[cellindex].gain+Pmax].erase(tmp1);
        }
        else {
            auto tmp1=bucketlistright[cellarray[cellindex].gain+Pmax].begin();
            bucketlistright[cellarray[cellindex].gain+Pmax].erase(tmp1);
        }
        for (int i = 0; i < cellarray[cellindex].netlist.size(); i++)
        {
            if (!cellarray[cellindex].set)
            {
                netarray[cellarray[cellindex].netlist[i]].cellinleft--;
                netarray[cellarray[cellindex].netlist[i]].cellinright++;
            }
            else {
                netarray[cellarray[cellindex].netlist[i]].cellinleft++;
                netarray[cellarray[cellindex].netlist[i]].cellinright--;
            }
        }
        cellarray[cellindex].set=!cellarray[cellindex].set;
        cellarray[cellindex].locked=1;
        cutsize=cutsize-cellarray[cellindex].gain;
        for (int i = 0; i <cellarray[cellindex].netlist.size(); i++)
        {
            for (int j = 0; j <netarray[cellarray[cellindex].netlist[i]].celllist.size(); j++)
            {  
                int curcell=netarray[cellarray[cellindex].netlist[i]].celllist[j];
                if ((!cellarray[curcell].isupdate)&&(cellarray[curcell].locked==false))
                {
                    int FS=0,TE=0;
                    if (!cellarray[curcell].set)
                    {
                        auto tmp=bucketlistleft[cellarray[curcell].gain+Pmax].find(curcell);
                        bucketlistleft[cellarray[curcell].gain+Pmax].erase(tmp);
                    }
                    else{
                        auto tmp=bucketlistright[cellarray[curcell].gain+Pmax].find(curcell);
                        bucketlistright[cellarray[curcell].gain+Pmax].erase(tmp);
                    }
                    for (int k =0 ; k<cellarray[curcell].netlist.size(); k++){
                        int curnet=cellarray[curcell].netlist[k];
                        if ((cellarray[curcell].set==0)&&(netarray[curnet].cellinleft==1))
                        {
                            FS++;
                        }
                        else if ((cellarray[curcell].set==1)&&(netarray[curnet].cellinright==1))
                        {
                            FS++;
                        }
                        if ((cellarray[curcell].set==0)&&(netarray[curnet].cellinright==0))
                        {
                            TE++;
                        }
                        else if ((cellarray[curcell].set==1)&&(netarray[curnet].cellinleft==0))
                        {
                            TE++;
                        }
                    }
                    cellarray[curcell].gain=FS-TE;
                    cellarray[curcell].isupdate=1;
                    if (!cellarray[curcell].set) bucketlistleft[cellarray[curcell].gain+Pmax].insert(curcell);
                    else bucketlistright[cellarray[curcell].gain+Pmax].insert(curcell);
                }
            }
        }
        for (int i = 0; i < totalcells; i++)
        {
            cellarray[i].isupdate=0;
        }
        
    }
    bool fm(){
        int maxgainleft=-1,maxgainright=-1;
        for (int i = bucketlistleft.size()-1; i>=0; i--)
        {
            if (!bucketlistleft[i].empty()){
                maxgainleft=*(bucketlistleft[i].begin());
                break;
            }    
        }
        for (int i = bucketlistright.size()-1; i >=0; i--)
        {
            if (!bucketlistright[i].empty()){ 
                maxgainright=*(bucketlistright[i].begin());
                break;
            }
        }
        if((maxgainleft==-1)&&(setleft==groupmaxsize)) return 1;
        else if((maxgainright==-1)&&(setleft==groupminsize)) return 1;
        else if ((maxgainleft==-1)&&(maxgainright==-1))return 1;
        else{
            if (setleft>=groupmaxsize)
            {
                updatebucketlist(maxgainleft);
                setleft--;
            }
            else if (setleft==groupminsize)
            {
                updatebucketlist(maxgainright);
                setleft++;
            }
            else if ((maxgainleft!=-1)&&(maxgainright==-1))
            {
                updatebucketlist(maxgainleft);
                setleft--;
            }
            else if ((maxgainleft==-1)&&(maxgainright!=-1))
            {
                updatebucketlist(maxgainright);
                setleft++;
            }
            else if (maxgainleft>=maxgainright)
            {
                updatebucketlist(maxgainleft);
                setleft--;
            }
            else if (maxgainright>maxgainleft)
            {
                updatebucketlist(maxgainright);
                setleft++;
            }
        }
        if (mincutsize>cutsize && setleft>=groupminsize && setleft <= groupmaxsize && (totalcells-setleft)>=groupminsize && (totalcells-setleft)<=groupmaxsize)
        {
            minsetleft=setleft;
            mincutsize=cutsize;
            for (int i = 0; i < totalcells; i++)
            {
                greatcutsize[i]=cellarray[i].set;
            }
        }
        return 0;
    }
    bool stopiteration(){
        cutsize2=cutsize1;
        cutsize1=mincutsize;
        if(cutsize1==cutsize2) return 1;
        else return 0;
    }
    void updatepartition(){
        for (int i = 0; i < totalcells; i++)
        {
            cellarray[i].set=greatcutsize[i];
            cellarray[i].locked=0;
        }
    }
    int getcutsize(){
        return cutsize;
    }
    void printoutput(string filename){
        ofstream output(filename);
        cutsize=0;
        for (int i = 0; i < totalnets; i++)
        {   
            netarray[i].cellinleft=0;
            netarray[i].cellinright=0;
            for (int j = 0; j < netarray[i].celllist.size(); j++)
            {
                if (cellarray[netarray[i].celllist[j]].set==0){
                    netarray[i].cellinleft++;
                }
                else
                {
                    netarray[i].cellinright++;
                }
            }
        }
        for (int i = 0; i < totalnets; i++)
        {
            if ((netarray[i].cellinleft!=0)&&(netarray[i].cellinright!=0))
            {
                cutsize++;
            }
        }
        output<<"Cutsize = "<<cutsize<<endl;
        output<<"G1 "<<minsetleft<<endl;
        for (int i = 0; i < totalcells; i++)
        {
            if (!greatcutsize[i])
            {
                output<<"c"<<i+1<<" ";
            }
        }
        output<<";"<<endl;
        output<<"G2 "<<totalcells-minsetleft<<endl;
        for (int j = 0; j < totalcells; j++)
        {
            if (greatcutsize[j])
            {
                output<<"c"<<j+1<<" ";
            }
        }
        output<<";"<<endl;
        output.close();
    }
};

int main(int argc,char *argv[]){
    uint64_t start = timeSinceEpochMillisec();
    graph g;
    g.Inputfile(argv[1]);
    g.initialpart();
    bool stop=false;
    int cnt=0;
    while(!stop&&cnt!=1){
        g.initialgain();
        g.initialbucketlist();
        bool flag2=false;
        while (!flag2)
        {
            flag2=g.fm();
        }
        stop=g.stopiteration();
        cnt++;
        g.updatepartition();
    }
    g.printoutput(argv[2]);
    uint64_t end = timeSinceEpochMillisec();
    cout << "Total time: " << end - start << " ms\n";
}
