#include "leftedge.cpp"


int main(int argc,char **argv){
    leftedgerouting s;
    s.read_file(argv[1]);
    s.routing();
    s.outputfile(argv[2]);
    return 0;
}