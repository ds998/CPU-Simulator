#include "Emulator.h"
#include "Linker.h"

chrono::time_point<chrono::system_clock> start;
chrono::time_point<chrono::system_clock> timeend;
Register general_reg[8];
PSW psw_register;
Memory memory;
CPU cpu;
Terminal terminal;
Timer timer;

void CPU::ReadInstr(){
    instruction = memory.getVal(general_reg[pc].getVal());
    general_reg[pc].val++;
    counter=1;
    byte_size=((instruction & 0x04)>>2)+1;
    instruction>>=3;
    src_operand.byte_size=byte_size;
    dst_operand.byte_size=byte_size;
    try{
        switch(instruction){
        case 0:case 1:case 2:{
            
        }
        break;
        case 3: case 4: case 5: case 6: case 7: case 8:case 10:{
            int dst_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            dst_operand.ReadWayOfAddr(dst_desc & 0xff);
            switch(dst_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(dst_operand.way_of_address==2){
                        dst_operand.byte_size=0;
                    }
                }
                break;
                case 3:case 4:{
                    dst_operand.byte_size=2;
                    int vx=0;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
            }
        }
        break;
        case 9:{
            int src_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            src_operand.ReadWayOfAddr(src_desc & 0xff);
            switch(src_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(src_operand.way_of_address==2){
                        src_operand.byte_size=0;
                    }
                }
                break;
                case 3:case 4:{
                    int vx=0;
                    src_operand.byte_size=2;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
                break;
            }
        }
        break;
        case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:case 20:case 21:case 22:case 23:{
            int src_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            src_operand.ReadWayOfAddr(src_desc & 0xff);
            switch(src_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(src_operand.way_of_address==2){
                        src_operand.byte_size=0;
                    }
                }
                break;
                case 3:case 4:{
                    int vx=0;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
                break;
            }


            int dst_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            dst_operand.ReadWayOfAddr(dst_desc & 0xff);
            switch(dst_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(dst_operand.way_of_address==2){
                        dst_operand.byte_size=0;
                    }
                }
                break;
                case 3:case 4:{
                    int vx=0;
                    dst_operand.byte_size=2;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
            }


        }
        break;
        case 24:{
            int dst_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            dst_operand.ReadWayOfAddr(dst_desc & 0xff);
            switch(dst_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(dst_operand.way_of_address==2){
                        dst_operand.byte_size=0;
                    }

                }
                break;
                case 3:case 4:{
                    int vx=0;
                    dst_operand.byte_size=2;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    dst_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
            }

            int src_desc = memory.getVal(general_reg[pc].getVal());
            general_reg[pc].val++;
            counter++;
            src_operand.ReadWayOfAddr(src_desc & 0xff);
            switch(src_operand.way_of_address){
                case 0:{
                    int vx=0;
                    for(int i=0;i<byte_size;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                case 1: case 2:{
                    if(src_operand.way_of_address==2){
                        src_operand.byte_size=0;
                    }
                }
                break;
                case 3:case 4:{
                    int vx=0;
                    src_operand.byte_size=2;
                    for(int i=0;i<2;i++){
                        vx|=(memory.getVal(general_reg[pc].getVal())<<(8*i));
                        general_reg[pc].val++;
                        counter++;
                    }
                    src_operand.setBValue(vx);
                }
                break;
                default:{
                    throw OperandException();
                }
                break;
            }
        }
        break;
        default:{
            throw OpcodeException();
        }

       
    }

    }
    catch(OperandException &e){
        cout<<e.what()<<endl;
        throw InstrErrorException();
    }
    catch(OpcodeException &e){
        cout<<e.what()<<endl;
        throw InstrErrorException();
    }
    
}

void CPU::ExecInstr(){
    try{
        switch(instruction){
        case 0:{
            exit(0);
        }
        break;
        case 1:{
            int s=memory.popOffStack();
            psw_register.setVal(s);
            s=memory.popOffStack();
            general_reg[pc].setVal(s);
        }
        break;
        case 2:{
            int s=memory.popOffStack();
            general_reg[pc].setVal(s);
        }
        break;
        case 3:{
            memory.pushToStack(general_reg[pc].getVal());
            memory.pushToStack(psw_register.getVal());
            psw_register.setInterrupt(true);
            int s=dst_operand.getValue(byte_size);
            s%=8;
            s*=2;
            general_reg[pc].setVal(memory.getVal(s));
        }
        break;
        case 4:{
            memory.pushToStack(general_reg[pc].getVal());
            int s=dst_operand.getValue(byte_size);
            if(dst_operand.way_of_address<2){
                general_reg[pc].setVal(s);
            }
            else general_reg[pc].setVal(memory.getVal(s));
        }
        break;
        case 5:{
            int s=dst_operand.getValue(byte_size);
            if(dst_operand.way_of_address<2){
                general_reg[pc].setVal(s);
            }
            else general_reg[pc].setVal(memory.getVal(s));
        }
        break;
        case 6:{
            if(psw_register.getZFlag()){
                int s=dst_operand.getValue(byte_size);
                if(dst_operand.way_of_address<2){
                    general_reg[pc].setVal(s);
                }
                else general_reg[pc].setVal(memory.getVal(s));
            }
        }
        break;
        case 7:{
            if(!psw_register.getZFlag()){
                int s=dst_operand.getValue(byte_size);
                if(dst_operand.way_of_address<2){
                    general_reg[pc].setVal(s);
                }
                else general_reg[pc].setVal(memory.getVal(s));
            }
        }
        break;
        case 8:{
            if(!psw_register.getZFlag() && psw_register.getNFlag()==psw_register.getOFlag()){
                int s=dst_operand.getValue(byte_size);
                if(dst_operand.way_of_address<2){
                    general_reg[pc].setVal(s);
                }
                else general_reg[pc].setVal(memory.getVal(s));
            }
        }
        break;
        case 9:{
            memory.pushToStack(src_operand.getValue(byte_size));
        }
        break;
        case 10:{
            int s=memory.popOffStack();
            dst_operand.setValue(byte_size,s);
        }
        break;
        case 11:{
            int temp=dst_operand.getValue(byte_size);
            dst_operand.setValue(byte_size,src_operand.getValue(byte_size));
            src_operand.setValue(byte_size,temp);
        }
        break;
        case 12:{
            int s=src_operand.getValue(byte_size);
            dst_operand.setValue(byte_size,s);
            psw_register.setZFlag(dst_operand.getValue(byte_size)==0);
            int ch=dst_operand.getValue(byte_size);
            if(byte_size==1){
                psw_register.setNFlag((char)(ch&0xff)<0);
            }
            else{
                psw_register.setNFlag(ch<0);
            }
        }
        break;
        case 13:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y+x;
            if(byte_size==1){
                psw_register.setCFlag(res>0x7f);
                psw_register.setOFlag(res<0xff || res>0x7f);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setCFlag(res>0x7fff);
                psw_register.setOFlag(res<0xffff || res>0x7fff);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,s+d);
        }
        break;
        case 14:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y-x;
            if(byte_size==1){
                psw_register.setCFlag(res>0x7f);
                psw_register.setOFlag(res<0xff || res>0x7f);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setCFlag(res>0x7fff);
                psw_register.setOFlag(res<0xffff || res>0x7fff);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d-s);
        }
        break;
        case 15:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y*x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d*s);
        }
        break;
        case 16:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y/x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d/s);
        }
        break;
        case 17:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y-x;
            if(byte_size==1){
                psw_register.setCFlag(res>0x7f);
                psw_register.setOFlag(res<0xff || res>0x7f);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setCFlag(res>0x7fff);
                psw_register.setOFlag(res<0xffff || res>0x7fff);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            
        }
        break;
        case 18:{
            int s=src_operand.getValue(byte_size);
            int res=~s;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,~s);
        }
        break;
        case 19:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y&x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d&s);
        }
        break;
        case 20:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y|x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d|s);
        }
        break;
        case 21:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y^x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,d^s);
        }
        break;
        case 22:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y&x;
            if(byte_size==1){
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
        }
        break;
        case 23:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y<<x;
            if(byte_size==1){
                psw_register.setCFlag(res&0x10);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setCFlag(res&0x1000);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,y<<x);
            
        }
        break;
        case 24:{
            int s=src_operand.getValue(byte_size);
            int d=dst_operand.getValue(byte_size);
            int x=s;
            int y=d;
            int res=y>>x;
            if(byte_size==1){
                psw_register.setCFlag(res & 0x01);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xff) <0);
            }
            else{
                psw_register.setCFlag(res & 0x0001);
                psw_register.setZFlag(res==0);
                psw_register.setNFlag((char)(res & 0xffff) <0);
            }
            dst_operand.setValue(byte_size,y>>x);
            
        }
        break;
        }
    }
    catch(StackException &e){
        cout<<e.what()<<endl;
        throw InstrErrorException();
    }
    
    dst_operand.b_value=0;
    src_operand.b_value=0;
    dst_operand.reg_l_h=-1;
    src_operand.reg_l_h=-1;
    dst_operand.byte_size=0;
    src_operand.byte_size=0;
    dst_operand.reg=-1;
    src_operand.reg=-1;
    src_operand.pc_val=-1;
    dst_operand.pc_val=-1;
}

void CPU::HandleInter(){
    if(psw_register.getInterrupt()) return;

    char c=0x01;
    int x=0;

    while(c!=0 && c<= (0x01<<(intr_counter-1))){
        if(c & irq){
            if(((c==0x04 && psw_register.getTimer()) || (c==0x08 && psw_register.getTerminal())))return;
            irq&=~c;
            try{
                memory.pushToStack(general_reg[pc].getVal());
                memory.pushToStack(psw_register.getVal());
            }
            catch(StackException &s){
                cout<<s.what()<<endl;
                cout<<"Greska u prekidu!"<<endl;
                exit(-1);
            }
            psw_register.setInterrupt(true);
            general_reg[pc].setVal(memory.getVal(2*x,2));
            break;
        }
        c<<=1;
        x++;
    }
}

void Emulator::load(Program program){
    int start_address=-1;
    for(int i=0;i<(int)program.end_instructions.size();i++){
        End_Instruction ei=program.end_instructions[i];
        if(ei.section!="iv_table"){
           if(ei.address<0x0100 || ei.address>=0xdefe){
               cout<<"Los plasman sekcije!"<<endl;
               exit(-1);
           }
        }
        else{
            if(ei.instructions.size()>14 || ei.instructions.size()%2!=0){
                cout<<"Losa iv_tabela!"<<endl;
                exit(-1);
            }
            cpu.intr_counter=ei.instructions.size()/2;
        }

        for(int i=0;i<(int)ei.instructions.size();i++){
            memory.setVal(ei.instructions[i],ei.address+i,1);
        }

        if(start_address==-1 && ei.section!="bss" && ei.section!="iv_table"){
            start_address=ei.address;
        }
    }

    if(cpu.intr_counter==0){
       cpu.intr_counter=4;

       memory.setVal(0x64,0xff14,1);
       memory.setVal(0x00,0xff15,1);
       memory.setVal(start_address & 0xff,0xff16,1);
       memory.setVal((start_address>>8) & 0xff,0xff17,1);
       memory.setVal(0x80,0xff18,1);
       memory.setVal(0xfc,0xff19,1);
       memory.setVal(0xfe,0xff1a,1);
       memory.setVal(0x08,0xff1b,1);

       memory.setVal(0x00,0xff1c,1);


       memory.setVal(0x6c,0xff1d,1);
       memory.setVal(0x00,0xff1e,1);
       memory.setVal(0x01,0xff1f,1);
       memory.setVal(0x00,0xff20,1);
       memory.setVal(0x80,0xff21,1);
       memory.setVal(0x30,0xff22,1);
       memory.setVal(0xff,0xff23,1);
       memory.setVal(0x08,0xff24,1);

       memory.setVal(0x64,0xff25,1);
       memory.setVal(0x80,0xff26,1);
       memory.setVal(0x02,0xff27,1);
       memory.setVal(0xff,0xff28,1);
       memory.setVal(0x80,0xff29,1);
       memory.setVal(0x00,0xff2a,1);
       memory.setVal(0xff,0xff2b,1);
       memory.setVal(0x08,0xff2c,1);


       memory.setVal(0xff14,0x0000,2);
       memory.setVal(0xff1c,0x0002,2);
       memory.setVal(0xff1d,0x0004,2);
       memory.setVal(0xff25,0x0006,2);
       
    }

    general_reg[sp].val=0xfefe;
}

void Emulator::runEmulation(){
    memory.pushToStack(psw_register.getVal());
    memory.pushToStack(general_reg[pc].getVal());
    general_reg[pc].setVal(memory.getVal(0x0000,2));
    start=chrono::system_clock::now();
    while(true){
        try{
            cpu.ReadInstr();
            cpu.ExecInstr();
        }
        catch(InstrErrorException &e){
            cout<<e.what()<<endl;
            general_reg[pc].setVal(general_reg[pc].getVal()-cpu.counter);
            cpu.setInter(1);
        }


        cpu.HandleInter();
        timeend=chrono::system_clock::now();
        timer.time_func(chrono::milliseconds(chrono::duration_cast<chrono::milliseconds>(timeend-start)));
        terminal.term_func();

    }

}