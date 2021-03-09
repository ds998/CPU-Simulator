#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include "Except.h"
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>

extern chrono::time_point<chrono::system_clock> start;
extern chrono::time_point<chrono::system_clock> timeend;

struct Program;
struct Register{
    int val;

    void init(){
        val=0;
    }

    int getVal(){
        return val;
    }

    int getVal(bool l_h){
        int c=0;
        if(l_h){
            c=(val>>8 & 0x00ff);
        }
        else{
           c=(val & 0x00ff);
        }
        return c;
    }

    void setVal(int v){
        val=v;
    }

    void setVal(int v, bool l_h){
        if(l_h){
            int s=v;
            s<<=8;
            s|=(val & 0x00ff);
            val=s;
        }
        else{
            val&=0xff00;
            val|=(v & 0x00ff);
        }
    }
};

struct PSW:public Register{
    void setInterrupt(bool s){
        if(s){
            val|=0x8000;
        }
        else{
            val &= 0x7fff;
        }
    }

    bool getInterrupt(){
        if((short)(0x8000 & val)!=0){
            return true;
        }
        return false;
    }

    void setTerminal(bool s){
        if(s){
            val|=0x4000;
        }
        else{
            val &= 0xbfff;
        }
    }

    bool getTerminal(){
        if((short)(0x4000 & val)!=0){
            return true;
        }
        return false;
    }

    void setTimer(bool s){
        if(s){
            val|=0x2000;
        }
        else{
            val &= 0xdfff;
        }
    }

    bool getTimer(){
        if((short)(0x2000 & val)!=0){
            return true;
        }
        return false;
    }

    void setNFlag(bool s){
        if(s){
            val|=0x0008;
        }
        else{
            val &= 0xfff7;
        }
    }

    bool getNFlag(){
        if(((short)0x0008 & val)!=0){
            return true;
        }
        return false;
    }

    void setCFlag(bool s){
        if(s){
            val|=0x0004;
        }
        else{
            val &= 0xfffb;
        }
    }

    bool getCFlag(){
        if((short)(0x0004 & val)!=0){
            return true;
        }
        return false;
    }

    void setOFlag(bool s){
        if(s){
            val|=0x0002;
        }
        else{
            val &= 0xfffd;
        }
    }

    bool getOFlag(){
        if((short)(0x0002 & val)!=0){
            return true;
        }
        return false;
    }

    void setZFlag(bool s){
        if(s){
            val|=0x0001;
        }
        else{
            val &= 0xfffe;
        }
    }

    bool getZFlag(){
        if((short)(0x0001 & val)!=0){
            return true;
        }
        return false;
    }
};

extern Register general_reg[8];

extern PSW psw_register;

const int sp=6;
const int pc=7;

struct Memory{
   char mem[0x10000];
   const int stack_begin=0xfefe;
   const int stack_size=0x2000;
   const int iv_begin=0x0000;
   const int iv_size=0x0100;

   void init(){
       for(int i=0;i<0x10000;i++){
           mem[i]=0x00;
       }
   }

   bool CheckIfAllowed(int address){
       return (address>=0x0100) && (address<=0xcefe);
   }

   bool CheckIfInStack(int address){
        return (address>0xcefe) && (address<=0xfefe);
   }

   void push(int val){
       general_reg[sp].val-=2;
       setVal(val,general_reg[sp].val,2);
   }

   void pushToStack(int val){
      if(CheckIfInStack(general_reg[sp].val-2)){
          push(val);
      }
      else throw StackException();
   }

   int popOffStack(){
      if(CheckIfInStack(general_reg[sp].val+2)){
          return pop();
      }
      else throw StackException();
   }

   int pop(){
       int p=getVal(general_reg[sp].val,2);
       general_reg[sp].val+=2;
       return p;
   }

   int getVal(int address,int b_size=1){
       int val=0;
       int add=address;
       for(int i=b_size;i>0;i--){
           val<<=8;
           val|=(mem[add+i-1] & 0xff);
           
           
       }
       return val;
   }

   void setVal(int val,int address,int b_size=1){
       int v=val;
       int add=address;
       for(int i=0;i<b_size;i++){
           mem[add]=(v & 0xff);
           v>>=8;
           add++;
       }
   }


};

extern Memory memory;
struct Operand{
    int way_of_address;
    int reg=-1;
    int reg_l_h=-1;
    int b_value=0;
    int byte_size=0;
    int pc_val=-1;

    void ReadWayOfAddr(char byte){
        char b=byte;
        way_of_address=((b & 0xe0)>>5);
        if(way_of_address==1 || way_of_address==2 || way_of_address==3){
            reg=((b & 0x1e)>>1);
            if(way_of_address==1 && byte_size==1){
                reg_l_h=(b & 0x01);
            }
            if(reg==7){
                pc_val=general_reg[reg].val-1;
            }
        }
    }

    void setBValue(int val){
        switch(way_of_address){
            case 0:{
                b_value=val;
            }
            break;
            case 3:{
                b_value=val;
            }
            break;
            case 4:{
                b_value=val;
            }
            break;
            default:
               throw OperandException();
            
            break;
        }
    }

    int getValue(int instr_byte_size){
        switch(way_of_address){
            case 0:{
                if(instr_byte_size==2){
                    return b_value;
                }
                else{
                    return (b_value & 0x00ff);
                }
                
            }
            break;
            case 1:{
                if(instr_byte_size==2){
                    if(reg==15){
                        return psw_register.val;
                    }
                    else{
                        if(reg==7){
                            return pc_val;
                        }
                        return general_reg[reg].val;
                    }
                }
                else{
                    if(reg_l_h==1){
                        if(reg==15){
                            return psw_register.getVal(true);
                        }
                        else{
                            if(reg==7){
                                int xef=general_reg[reg].getVal();
                                general_reg[reg].setVal(pc_val);
                                int mof=general_reg[reg].getVal(true);
                                general_reg[reg].setVal(xef);
                                return mof;

                            }
                            return general_reg[reg].getVal(true);
                        }
                    }
                    else{
                        if(reg==15){
                            return psw_register.getVal(false);
                        }
                        else{
                            if(reg==7){
                                int xef=general_reg[reg].getVal();
                                general_reg[reg].setVal(pc_val);
                                int mof=general_reg[reg].getVal(false);
                                general_reg[reg].setVal(xef);
                                return mof;

                            }
                            return general_reg[reg].getVal(false);
                        }
                    }
                }
            }
            break;
            case 2:{
                if(instr_byte_size==2){
                    if(reg==15){
                        return memory.getVal(psw_register.getVal(),2);
                    }
                    else{
                        if(reg==7){
                            return memory.getVal(pc_val,2);
                        }
                        return memory.getVal(general_reg[reg].getVal(),2);
                    }
                }
                else{
                    if(reg==15){
                        return memory.getVal(psw_register.getVal(),1);
                    }
                    else{
                        if(reg==7){
                            return memory.getVal(pc_val,1);
                        }
                        return memory.getVal(general_reg[reg].getVal(),1);
                    }
                }
            }
            break;
            case 3:{
                if(instr_byte_size==2){
                    if(reg==15){
                        return memory.getVal(psw_register.getVal()+b_value,2);
                    }
                    else{
                        if(reg==7){
                            return memory.getVal(pc_val+b_value,2);
                        }
                        return memory.getVal(general_reg[reg].getVal()+b_value,2);
                    }
                }
                else{
                    if(reg==15){
                        return memory.getVal(psw_register.getVal()+b_value,1);
                    }
                    else{
                        if(reg==7){
                            return memory.getVal(pc_val+b_value,1);
                        }
                        return memory.getVal(general_reg[reg].getVal()+b_value,1);
                    }
                }
                
            }
            break;
            case 4:{
                if(instr_byte_size==2){
                    return memory.getVal(b_value,2);
                }
                else return memory.getVal(b_value,1);
            }
            break;
            default:
               throw OperandException();
            
            break;
        }
    }

    void setValue(int instr_byte_size,int value){
        switch(way_of_address){
            case 1:{
                if(instr_byte_size==2){
                    if(reg==15){
                        psw_register.setVal(value);
                    }
                    else{
                        general_reg[reg].setVal(value);
                    }
                }
                else{
                    if(reg==15){
                        psw_register.setVal(value,reg_l_h);
                    }
                    else{
                        general_reg[reg].setVal(value,reg_l_h);
                    }
                }
            }
            break;
            case 2:{
                if(instr_byte_size==2){
                    if(reg==15){
                        memory.setVal(value,psw_register.getVal(),2);
                    }
                    else{
                        if(reg==7){
                            memory.setVal(value,pc_val,2);
                        }
                        else memory.setVal(value,general_reg[reg].getVal(),2);
                    }
                }
                else{
                    if(reg==15){
                        memory.setVal(value,psw_register.getVal());
                    }
                    else{
                        if(reg==7){
                            memory.setVal(value,pc_val);
                        }
                        else memory.setVal(value,general_reg[reg].getVal());
                    }
                }
            }
            break;
            case 3:{
                if(instr_byte_size==2){
                    if(reg==15){
                        memory.setVal(value,psw_register.getVal()+b_value,2);
                    }
                    else{
                        if(reg==7){
                            memory.setVal(value,pc_val+b_value,2);
                        }
                        else memory.setVal(value,general_reg[reg].getVal()+b_value,2);
                    }
                }
                else{
                    if(reg==15){
                        memory.setVal(value,psw_register.getVal()+b_value);
                    }
                    else{
                        if(reg==7){
                            memory.setVal(value,pc_val+b_value);
                        }
                        else memory.setVal(value,general_reg[reg].getVal()+b_value);
                    }
                }
                
            }
            break;
            case 4:{
                if(instr_byte_size==2){
                    memory.setVal(value,b_value,2);
                }
                else {
                    memory.setVal(value,b_value,1);

                }
            }
            break;
            default:
               throw OperandException();
            
            break;
        }
    }



};
struct CPU{
    char irq;
    int counter=0;
    int intr_counter=0;
    int instruction;
    int byte_size;
    Operand src_operand,dst_operand;
    void init(){
        irq=0x00;
        instruction=-1;
    }
    void ReadInstr();
    void ExecInstr();
    void HandleInter();
    void setInter(int s){
        if(s<0 || s>7){
            cout<<"Pogresan argument za prekid!"<<endl;
            exit(-1);
        }
        char c=0x01;
        c<<=s;
        irq|=c;

    }
};

extern CPU cpu;

struct Terminal{
    const int data_out=0xff00;
    const int data_in=0xff02;

    void term_func(){
            int s=memory.getVal(data_out,2);
            char c=(char)s;
            if(c!=0){
                cout<<c<<" ";
            }
            memory.setVal(0,data_out,2);
            char buf[1];
            if(read(STDIN_FILENO,buf,1)>0){
                memory.setVal(buf[0],data_in,2);
                cpu.setInter(3);
            }
            
        
    }

};
extern Terminal terminal;

struct Timer{
    const int cfg_timer=0xff10;

    void time_func(chrono::milliseconds xef){
        int x=500;
        if(chrono::milliseconds(x)>xef){
            return;
        }
        start=chrono::system_clock::now();
        int s=memory.getVal(cfg_timer,2);
        s=(s & 0x0007);
        switch(s){
            case 0:{
                x=500;
            }
            break;
            case 1:{
                x=1000;
            }
            break;
            case 2:{
                x=1500;
            }
            break;
            case 3:{
                x=2000;
            }
            break;
            case 4:{
                x=5000;
            }
            break;
            case 5:{
                x=10000;
            }
            break;
            case 6:{
                x=30000;
            }
            break;
            case 7:{
                x=60000;
            }
            break;
        }
        cpu.setInter(2);
            
        
    }
};
extern Timer timer;




class Emulator{
public:
    Emulator(){
        for(int i=0;i<8;i++){
            general_reg[i].init();
            
        }
        psw_register.init();
        memory.init();
        cpu.init();

        struct termios term;
        int fd=STDIN_FILENO;
        if(tcgetattr(fd,&term)!=0){
            cout<<"Terminal get error"<<endl;
            exit(-1);
        }
        else{
            term.c_lflag &= ~ICANON;
            term.c_cc[VMIN]=0;
            term.c_cc[VTIME]=0;
            if(tcsetattr(fd,TCSANOW ,&term)!=0){
                cout<<"Terminal get error"<<endl;
                exit(-1);
            }
        }
        
    }

    void load(Program program);
    void runEmulation();
    
    
};

#endif