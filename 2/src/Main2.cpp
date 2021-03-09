#include "Linker.h"
#include "Emulator.h"

int main(int argc,char ** argv){
    if(!(argc>1)){
        cout<<"Greska u argumentima"<<endl;
        return -1;
    }

    vector<string> args;
    for(int i=1;i<argc;i++){
        args.push_back(argv[i]);
    }
    Linker l;
    Program program=l.link(args);
    Emulator em;
    em.load(program);
    em.runEmulation();
    return 0;




    

    


}