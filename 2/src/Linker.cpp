#include "Linker.h"
#include <regex>
#include <fstream>

bool compare_pair(pair<string,int> p1,pair<string,int> p2){
    return p1.second<p2.second;
}
Program Linker::link(vector<string> args){
    read_input(args);
    read_files();
    resize_sections();
    redefine_symbols();
    do_rel_blocks();
    combine_code();
    return program;
}

vector<string> Linker::split(const string &s, char delim) {
	vector<string> result;
	string new_s = "";
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] == delim) {
			if (new_s != "")result.push_back(new_s);
			new_s = "";
		}
		else new_s += s[i];
	}
	if (new_s != "")result.push_back(new_s);
	return result;
}

void Linker::read_input(vector<string>args){
    bool order=false;
    for(int i=0;i<(int)args.size();i++){
        if(regex_match(args[i],regex("^-place=([a-zA-Z_][a-zA-Z0-9_]*)@(0x[0-9abcdef]{1,4})$"))){
            if(order){
               cout<<"Pogresan redosled algoritama!"<<endl;
               exit(-1);
            }
           string s=args[i].substr(7);
           vector<string> sef=split(s,'@');
           pair<string,int> p;
           p.first=sef[0];
           p.second=stoi(sef[1],nullptr,16);
           for(int j=0;j<(int)sec_placement.size();j++){
                if(sec_placement[j].first==sef[0]){
                    cout<<"Sekcija vec navedena!"<<endl;
                    exit(-1);
                }
           }
           sec_placement.push_back(p);
           

        }
        else if(regex_match(args[i],regex("^([a-zA-Z_][a-zA-Z0-9_]*).txt$"))){
            order=true;
            filenames.push_back(args[i]);

        }
        else{
            cout<<"Los argument!"<<endl;
            exit(-1);
        }
    }
    if((int)sec_placement.size()>0){
        sort(sec_placement.begin(),sec_placement.end(), compare_pair);
    }


}

void Linker::read_files(){
    for(int i=0;i<(int)filenames.size();i++){
        ifstream ifs;
        ifs.open(filenames[i],ifstream::in);
        string s;
        File f;
        bool sym_tab=false;
        bool equ_tab=false;
        bool sec=false;
        bool code=false;
        bool rel=false;
        while(getline(ifs,s)){
           if(s=="tabela simbola"){
               sym_tab=true;
           }
           else if(s=="kraj tabele simbola"){
               sym_tab=false;
           }
           else if(s=="equ tabela"){
               equ_tab=true;
           }
           else if(s=="kraj equ tabele"){
               equ_tab=false;
           }
           else if(regex_match(s,regex("^([a-zA-Z_][a-zA-Z0-9_]*)$")) && s!="kod"){
               sec=true;
               SectionMaterial sm;
               sm.sec_name=s;
               f.sections.push_back(sm);
           }
           else if(s=="kod"){
               code=true;
           }
           else if(s=="kraj koda"){
               code=false;
           }
           else if(s=="rel.zapisi"){
               rel=true;
           }
           else if(s=="kraj rel.zapisa"){
               rel=false;
               sec=false;
           }
           else{
               if(sym_tab){
                   if(sec || code || rel || equ_tab){
                       cout<<"Lose formatiran fajl!"<<endl;
                       exit(-1);
                   }
                   vector<string> ss=split(s,' ');
                   Entry entry;
                   entry.name=ss[0];
                   entry.section=ss[1];
                   entry.val=stoi(ss[2]);
                   entry.glob_loc=stoi(ss[3]);
                   entry.size=stoi(ss[4]);
                   f.symbol_table.push_back(entry);

               }
               else if(equ_tab){
                   if(sec || code || rel || sym_tab){
                       cout<<"Lose formatiran fajl!"<<endl;
                       exit(-1);
                   }
                   vector<string> ss=split(s,' ');
                   Equ_Member em;
                   em.symbol=ss[0];
                   em.dependent_symbol=ss[1];
                   em.plus_minus=ss[2];
                   f.equ_table.push_back(em);

               }
               else if(sec){
                   if(sym_tab || equ_tab){
                       cout<<"Lose formatiran fajl!"<<endl;
                       exit(-1);
                   }

                   if(code){
                       if(rel){
                           cout<<"Lose formatiran fajl!"<<endl;
                           exit(-1);
                       }
                       vector<string> ss=split(s,' ');
                       for(int j=0;j<(int)ss.size();j++){
                           f.sections[(int)f.sections.size()-1].code.push_back(stoi(ss[j],nullptr,16));
                       }

                   }
                   else if(rel){
                       if(code){
                           cout<<"Lose formatiran fajl!"<<endl;
                           exit(-1);
                       }
                       vector<string> ss=split(s,' ');
                       RelBlock rb;
                       rb.address=stoi(ss[0]);
                       rb.type=stoi(ss[1]);
                       rb.table_num=stoi(ss[2]);
                       rb.byte_size=stoi(ss[3]);
                       f.sections[(int)f.sections.size()-1].rel.push_back(rb);

                   }
                   else{
                       cout<<"Lose formatiran fajl!"<<endl;
                       exit(-1);
                   }
                }
                else{
                    cout<<"Lose formatiran fajl!"<<endl;
                    exit(-1);
                }
           }
        }
        program.files.push_back(f);

        
    }
    /*for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            cout << dec << program.files[i].symbol_table[j].name << " " << program.files[i].symbol_table[j].section << " " << program.files[i].symbol_table[j].val << " " << program.files[i].symbol_table[j].glob_loc << " " << program.files[i].symbol_table[j].size << endl;
        }
        for(int j=0;j<(int)program.files[i].equ_table.size();j++){
            cout << dec << program.files[i].equ_table[j].first << " " << program.files[i].equ_table[j].second << endl;
        }
        for(int j=0;j<(int)program.files[i].sections.size();j++){
            for(int k=0;k<(int)program.files[i].sections[j].code.size();k++){
                cout<<hex<<(program.files[i].sections[j].code[k] & 0xff)<<" ";
            }
            cout<<endl;
            for(int k=0;k<(int)program.files[i].sections[j].rel.size();k++){
                cout<<dec<<program.files[i].sections[j].rel[k].address<<" "<<program.files[i].sections[j].rel[k].type<<" "<<program.files[i].sections[j].rel[k].table_num<<" "<<program.files[i].sections[j].rel[k].byte_size<<endl;
            }

        }
        cout<<endl;
        cout<<endl;
    }
    */
    
}

void Linker::resize_sections(){
   int first_c=-1;
   vector<string> passed_sections;
   for(int i=0;i<(int)sec_placement.size();i++){
       string sec_name=sec_placement[i].first;
       int num=sec_placement[i].second;
       passed_sections.push_back(sec_name);
       int counter=num;
       if(first_c!=-1 && first_c>=num){
           cout<<"Losa pozicija"<<endl;
            exit(-1);
       }
       for(int j=0;j<(int)program.files.size();j++){
           for(int k=0;k<(int)(program.files[j].symbol_table.size());k++){
                if(program.files[j].symbol_table[k].name==sec_name){
                    if(counter<0xf && sec_name!="iv_table"){
                        cout<<"Losa pozicija"<<endl;
                        exit(-1);
                    }
                    program.files[j].symbol_table[k].val=counter;
                    counter+=program.files[j].symbol_table[k].size;
                    if(counter>=0xdefe){
                        cout<<"Losa pozicija"<<endl;
                        exit(-1);
                    }
                    first_c=counter;

                }
           }      
       }
       Entry entry;
       entry.name=sec_name;
       entry.section=sec_name;
       entry.glob_loc=1;
       entry.val=num;
       entry.size=counter-num;
       program.exit_symbol_table.push_back(entry);

   }

   int counter=0;

   for(int i=0;i<(int)program.files.size();i++){
       for(int j=0;j<(int)program.files[i].sections.size();j++){
           string sec_name=program.files[i].sections[j].sec_name;
           int idx=-1;
           for(int k=0;k<(int)passed_sections.size();k++){
               if(passed_sections[k]==sec_name){
                   idx=k;
                   break;
               }
           }
           if(idx==-1){
                int num=0;
                counter=0;
                if((int)passed_sections.size()>0){
                    for(int k=0;k<(int)program.exit_symbol_table.size();k++){
                        counter=program.exit_symbol_table[k].val+program.exit_symbol_table[k].size;
                    }
                }
                else{
                    counter=0xf;
                }
                num=counter;
                for(int k=0;k<(int)program.files.size();k++){
                    for(int l=0;l<(int)(program.files[k].symbol_table.size());l++){
                        if(program.files[k].symbol_table[l].name==sec_name){
                            if((counter<0xf && sec_name!="iv_table")|| counter>=0xdefe){
                                cout<<"Losa pozicija"<<endl;
                                exit(-1);
                            }
                            program.files[k].symbol_table[l].val=counter;
                            counter+=program.files[k].symbol_table[l].size;
                            if(counter>=0xdefe){
                                cout<<"Losa pozicija"<<endl;
                                exit(-1);
                            }
                        }
                    }      
                }
                Entry entry;
                entry.name=sec_name;
                entry.section=sec_name;
                entry.glob_loc=1;
                entry.val=num;
                entry.size=counter-num;
                program.exit_symbol_table.push_back(entry);
                passed_sections.push_back(sec_name);


           }
       }
   }

   for(int i=0;i<(int)program.exit_symbol_table.size();i++){
        if(program.exit_symbol_table[i].name=="iv_table"){
            if(program.exit_symbol_table[i].val!=0x0000){
                cout<<"Tabela prekida moze samo na nultoj poziciji!"<<endl;
                exit(-1);
            }
        }
   }



   /*for(int i=0;i<(int)program.exit_symbol_table.size();i++){
       cout << dec << program.exit_symbol_table[i].name << " " << program.exit_symbol_table[i].section << " " << program.exit_symbol_table[i].val << " " << program.exit_symbol_table[i].glob_loc << " " << program.exit_symbol_table[i].size << endl;
   }
   cout<<endl;
   cout<<endl;
   for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            cout << dec << program.files[i].symbol_table[j].name << " " << program.files[i].symbol_table[j].section << " " << program.files[i].symbol_table[j].val << " " << program.files[i].symbol_table[j].glob_loc << " " << program.files[i].symbol_table[j].size << endl;
        }
   }*/



   


}

void Linker::redefine_symbols(){
    for(int i=0;i<(int)program.files.size();i++){
         for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            Entry en=program.files[i].symbol_table[j];
            if(en.name!=en.section && en.glob_loc==1 && en.section!="UNDEFINED" && en.section!="ABSOLUTE"){
                for(int k=0;k<(int)program.exit_symbol_table.size();k++){
                    if(program.exit_symbol_table[k].name==en.name){
                        cout<<"Dupla definicija simbola: "<<en.name<<endl;
                        exit(-1);
                    }
                }
                for(int k=0;k<(int)program.files[i].symbol_table.size();k++){
                    if(program.files[i].symbol_table[k].name==en.section){
                        en.val+=program.files[i].symbol_table[k].val;
                        program.exit_symbol_table.push_back(en);
                        break;
                    }
                }
            }
         }
    }

    for(int i=0;i<(int)program.files.size();i++){
         for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            Entry en=program.files[i].symbol_table[j];
            bool equ_found=false;
            for(int k=0;k<(int)program.files[i].equ_table.size();k++){
                if(en.name==program.files[i].equ_table[k].symbol){
                    equ_found=true;
                    break;
                }

            }
            if(en.section=="UNDEFINED" && !equ_found){
                bool found=false;
                for(int k=0;k<(int)program.exit_symbol_table.size();k++){
                    if(program.exit_symbol_table[k].name==en.name){
                        found=true;
                        break;
                    }
                }
                if(!found){
                    cout<<"Nedefinisan simbol: "<<en.name<<endl;
                    exit(-1);
                }
                
            }
         }
    }

    int equ_counter=0;
    for(int i=0;i<(int)program.files.size();i++){
         for(int j=0;j<(int)program.files[i].equ_table.size();j++){
            equ_counter++;
         }
    }
    int solved=-1;
    while(equ_counter>0){
        if(solved==0){
           cout<<"Neizracunljivi equ izrazi"<<endl;
           exit(-1);
        }
        solved=0;
        for(int i=0;i<(int)program.files.size();i++){
            vector<Equ_Member> remaining_pairs;
            for(int j=0;j<(int)program.files[i].equ_table.size();j++){
                Equ_Member p=program.files[i].equ_table[j];
                bool second_found=false;
                int idx=-1;
                int indicator=-1;
                for(int k=0;k<(int)program.exit_symbol_table.size();k++){
                    if(program.exit_symbol_table[k].name==p.dependent_symbol){
                        second_found=true;
                        idx=k;
                        indicator=0;
                        break;
                    }

                }
         
                if(!second_found){
                    for(int k=0;k<(int)program.exit_symbol_table.size();k++){
                        if(program.exit_symbol_table[k].name==p.dependent_symbol){
                            second_found=true;
                            idx=k;
                            indicator=1;
                            break;
                        }

                    }

                }

                if(!second_found){
                    remaining_pairs.push_back(p);
                    continue;
                }

                int val=-1;


                if(indicator==0){
                    val=program.exit_symbol_table[idx].val;

                }
                else{
                    val=program.files[i].symbol_table[idx].val;

                }

                for(int k=0;k<(int)program.files[i].symbol_table.size();k++){
                    if(p.symbol==program.files[i].symbol_table[k].name){
                        program.files[i].symbol_table[k].section="ABSOLUTE";
                        if(p.plus_minus=="plus"){
                            program.files[i].symbol_table[k].val+=val;
                        }
                        else{
                            program.files[i].symbol_table[k].val-=val;

                        }
                        if(program.files[i].symbol_table[k].glob_loc==1){
                            program.exit_symbol_table.push_back(program.files[i].symbol_table[k]);

                        }

                        break;




                    }
                }


                solved=1;
                equ_counter--;
            }
            program.files[i].equ_table=remaining_pairs;
        }

    }
    /*
   for(int i=0;i<(int)program.exit_symbol_table.size();i++){
       cout << dec << program.exit_symbol_table[i].name << " " << program.exit_symbol_table[i].section << " " << program.exit_symbol_table[i].val << " " << program.exit_symbol_table[i].glob_loc << " " << program.exit_symbol_table[i].size << endl;
   }
   cout<<endl;
   cout<<endl;
   for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            cout << dec << program.files[i].symbol_table[j].name << " " << program.files[i].symbol_table[j].section << " " << program.files[i].symbol_table[j].val << " " << program.files[i].symbol_table[j].glob_loc << " " << program.files[i].symbol_table[j].size << endl;
        }
        cout<<"|||||"<<endl;
        for(int j=0;j<(int)program.files[i].sections.size();j++){
            for(int k=0;k<(int)program.files[i].sections[j].code.size();k++){
                cout<<hex<<(program.files[i].sections[j].code[k] &0xff)<<" ";
            }
            cout<<endl;
            for(int k=0;k<(int)program.files[i].sections[j].rel.size();k++){
                cout<<dec<<program.files[i].sections[j].rel[k].address<<" "<<program.files[i].sections[j].rel[k].type<<" "<<program.files[i].sections[j].rel[k].table_num<<" "<<program.files[i].sections[j].rel[k].byte_size<<endl;
            }
        }
        cout<<"xxxxxx"<<endl;
        


   }
   */


     
}

void Linker::do_rel_blocks(){
    for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].sections.size();j++){
            for(int k=0;k<(int)program.files[i].sections[j].rel.size();k++){
                int val=0;
                if(program.files[i].sections[j].rel[k].byte_size==1){
                    val=(program.files[i].sections[j].code[program.files[i].sections[j].rel[k].address] & 0xff);
                }
                else{
                    val=(program.files[i].sections[j].code[program.files[i].sections[j].rel[k].address+1] & 0xff);
                    val<<=8;
                    val|=(program.files[i].sections[j].code[program.files[i].sections[j].rel[k].address] & 0xff);
                }
                for(int m=0;m<(int)program.exit_symbol_table.size();m++){
                    if(program.exit_symbol_table[m].name==program.files[i].symbol_table[program.files[i].sections[j].rel[k].table_num].name){
                        if(program.files[i].symbol_table[program.files[i].sections[j].rel[k].table_num].name==program.files[i].symbol_table[program.files[i].sections[j].rel[k].table_num].section){
                            val+=program.files[i].symbol_table[program.files[i].sections[j].rel[k].table_num].val;
                        }
                        else val+=program.exit_symbol_table[m].val;
                        int addr=program.files[i].sections[j].rel[k].address;
                        for(int n=0;n<program.files[i].sections[j].rel[k].byte_size;n++){
                            program.files[i].sections[j].code[addr]=(val & 0xff);
                            val>>=8;
                            addr++;
                        }
                        program.files[i].sections[j].rel[k].table_num=m;
                        for(int n=0;n<(int)program.files[i].symbol_table.size();n++){
                            if(program.files[i].symbol_table[n].name==program.files[i].sections[j].sec_name){
                                program.files[i].sections[j].rel[k].address+=program.files[i].symbol_table[n].val;
                                program.exit_rel_blocks.push_back(program.files[i].sections[j].rel[k]);
                                break;
                            }
                        }
                        break;
                    }
                }

                for(int m=0;m<(int)program.files[i].symbol_table.size();m++){
                    if(program.files[i].symbol_table[m].name==program.files[i].symbol_table[program.files[i].sections[j].rel[k].table_num].name && program.files[i].symbol_table[m].section=="ABSOLUTE"){
                        val+=program.files[i].symbol_table[m].val-val;
                        int addr=program.files[i].sections[j].rel[k].address;
                        for(int n=0;n<program.files[i].sections[j].rel[k].byte_size;n++){
                            program.files[i].sections[j].code[addr]=(val & 0xff);
                            val>>=8;
                            addr++;
                        }
                        break;


                    }



                }
            }
        }
    }
    
    /*
   for(int i=0;i<(int)program.exit_symbol_table.size();i++){
       cout << dec << program.exit_symbol_table[i].name << " " << program.exit_symbol_table[i].section << " " << program.exit_symbol_table[i].val << " " << program.exit_symbol_table[i].glob_loc << " " << program.exit_symbol_table[i].size << endl;
   }
   for(int i=0;i<(int)program.exit_rel_blocks.size();i++){
       cout << dec << program.exit_rel_blocks[i].address << " " << program.exit_rel_blocks[i].type << " " << program.exit_rel_blocks[i].table_num << " " << program.exit_rel_blocks[i].byte_size << endl;
   }
   cout<<endl;
   cout<<endl;
   for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            cout << dec << program.files[i].symbol_table[j].name << " " << program.files[i].symbol_table[j].section << " " << program.files[i].symbol_table[j].val << " " << program.files[i].symbol_table[j].glob_loc << " " << program.files[i].symbol_table[j].size << endl;
        }
        cout<<"|||||"<<endl;
        for(int j=0;j<(int)program.files[i].sections.size();j++){
            for(int k=0;k<(int)program.files[i].sections[j].code.size();k++){
                cout<<hex<<(program.files[i].sections[j].code[k] &0xff)<<" ";
            }
            cout<<endl;
            for(int k=0;k<(int)program.files[i].sections[j].rel.size();k++){
                cout<<dec<<program.files[i].sections[j].rel[k].address<<" "<<program.files[i].sections[j].rel[k].type<<" "<<program.files[i].sections[j].rel[k].table_num<<" "<<program.files[i].sections[j].rel[k].byte_size<<endl;
            }
        }
        cout<<"xxxxxx"<<endl;
   }
   */

}

void Linker::combine_code(){
    for(int i=0;i<(int)program.exit_symbol_table.size();i++){
        if(program.exit_symbol_table[i].size>0){
            End_Instruction ei;
            ei.address=program.exit_symbol_table[i].val;
            int counter=ei.address;
            ei.section=program.exit_symbol_table[i].name;
            for(int j=0;j<(int)program.files.size();j++){
                for(int k=0;k<(int)program.files[j].sections.size();k++){
                    if(program.files[j].sections[k].sec_name==ei.section){
                        for(int m=0;m<(int)program.files[j].sections[k].code.size();m++){
                            ei.instructions.push_back((program.files[j].sections[k].code[m] & 0xff));
                            counter++;
                        }
                        break;
                    }
                    
                }
            }
            program.end_instructions.push_back(ei);
        }
    }

    /*
    for(int i=0;i<(int)program.exit_symbol_table.size();i++){
       cout << dec << program.exit_symbol_table[i].name << " " << program.exit_symbol_table[i].section << " " << program.exit_symbol_table[i].val << " " << program.exit_symbol_table[i].glob_loc << " " << program.exit_symbol_table[i].size << endl;
   }
   for(int i=0;i<(int)program.exit_rel_blocks.size();i++){
       cout << dec << program.exit_rel_blocks[i].address << " " << program.exit_rel_blocks[i].type << " " << program.exit_rel_blocks[i].table_num << " " << program.exit_rel_blocks[i].byte_size << endl;
   }
   for(int i=0;i<(int)program.end_instructions.size();i++){
       cout<<dec<<program.end_instructions[i].section<<" "<<program.end_instructions[i].address<<" ";
       for(int j=0;j<(int)program.end_instructions[i].instructions.size();j++){
           cout<<hex<<(program.end_instructions[i].instructions[j] & 0xff)<<" ";
       }
       cout<<endl;
   }
   cout<<endl;
   cout<<endl;
   for(int i=0;i<(int)program.files.size();i++){
        for(int j=0;j<(int)program.files[i].symbol_table.size();j++){
            cout << dec << program.files[i].symbol_table[j].name << " " << program.files[i].symbol_table[j].section << " " << program.files[i].symbol_table[j].val << " " << program.files[i].symbol_table[j].glob_loc << " " << program.files[i].symbol_table[j].size << endl;
        }
        cout<<"|||||"<<endl;
        for(int j=0;j<(int)program.files[i].sections.size();j++){
            for(int k=0;k<(int)program.files[i].sections[j].code.size();k++){
                cout<<hex<<(program.files[i].sections[j].code[k] & 0xff)<<" ";
            }
            cout<<endl;
            for(int k=0;k<(int)program.files[i].sections[j].rel.size();k++){
                cout<<dec<<program.files[i].sections[j].rel[k].address<<" "<<program.files[i].sections[j].rel[k].type<<" "<<program.files[i].sections[j].rel[k].table_num<<" "<<program.files[i].sections[j].rel[k].byte_size<<endl;
            }
        }
        cout<<"xxxxxx"<<endl;
   }
   */
}