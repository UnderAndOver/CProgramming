#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
typedef struct { char *key; char* val; } t_symstruct;
enum commandType {A,C,L} command;
int reg=16;
int currentline=0;
_Bool flag;
char dest[4],address[17]={"0000000000000000\0"};
char adest[4],acomp[4],ajmp[4];
void codeC();
void reset(){
    int i;
    for(i=0;i<16;i++)
        address[i]='0';
    address[16]='\0';
}
static t_symstruct lookuptable[] = {
  { "0",       "0101010"},
  { "1",       "0111111"},
  { "-1",      "0111010"},
  { "D",       "0001100"},
  {"A",        "0110000"},
  {"!D",       "0001101"},
  {"!A",       "0110001"},
  {"-D",       "0001111"},
  {"-A",       "0110011"},
  {"D+1",      "0011111"},
  {"A+1",      "0110111"},
  {"D-1",      "0001110"},
  {"A-1",      "0110010"},
  {"D+A",      "0000010"},
  {"D-A",      "0010011"},
  {"A-D",      "0000111"},
  {"D&A",      "0000000"},
  {"D|A",      "0010101"},
  {"M",        "1110000"},
  {"!M",       "1110001"},
  {"-M",       "1110011"},
  {"M+1",      "1110111"},
  {"M-1",      "1110010"},
  {"D+M",      "1000010"},
  {"D-M",      "1010011"},
  {"M-D",      "1000111"},
  {"D&M",      "1000000"},
  {"D|M",      "1010101"},
  {"",         "000"},
  {"JGT",      "001"},
  {"JEQ",      "010"},
  {"JGE",      "011"},
  {"JLT",      "100"},
  {"JNE",      "101"},
  {"JLE",      "110"},
  {"JMP",      "111"}
};
#define NKEYS (sizeof(lookuptable)/sizeof(t_symstruct))
char* valuefromkey(char *key){
  int i;
  t_symstruct *sym = lookuptable;
  for (i=0; i < NKEYS; i++) {
    if (strcmp(sym->key, key) == 0)
      return sym->val;
    sym++;
  }
  return NULL;
}
typedef struct S_Symstruct{
    char name[50];//max sym-name=50 chars
    int16_t value;
}symbolstruct;
int symboltable_end=23; // 23 pre-defined symbols
//2000 max amount of symbols allowed
symbolstruct symboltable[2000]={
  {"R0", 0},
  {"R1", 1},
  {"R2", 2},
  {"R3", 3},
  {"R4", 4},
  {"R5", 5},
  {"R6", 6},
  {"R7", 7},
  {"R8", 8},
  {"R9", 9},
  {"R10", 10},
  {"R11", 11},
  {"R12", 12},
  {"R13", 13},
  {"R14", 14},
  {"R15", 15},
  {"SP", 0},
  {"LCL", 1},
  {"ARG", 2},
  {"THIS", 3},
  {"THAT", 4},
  {"SCREEN", 16384},
  {"KBD", 24576},

  };  // Number of symbols must not exceed 500
//checks if symboltable has symbol. use isnumericsymbol first
int16_t getval(char* t_sym){
    int i;
    for(i=0;i<symboltable_end;i++)
        if(strcmp(symboltable[i].name,t_sym)==0)
            return symboltable[i].value;
    return (-1);
}
//if returns 0 then it is variable, if it returns 1 it is a number
//if 0 => L, if 1 => A
int isnumericsymbol(char* symbol,size_t len){
    int i;
    for(i=0;i<len;i++){
        if(!isdigit(symbol[i]))
            return 0;
    }
    return 1;
}
//removes comments from instruction
void noComment(char* buff,char* newbuff){
    int lenh=strlen(buff);
    int i=0,j=0;
    flag=0;
    //skip over indents in beginning
    while(buff[i]==' '){
        i++;
    }
    while(buff[i]!=' ' && buff[i]!='\0' && i<lenh){
        if(buff[i]=='\r' || buff[i]=='\n'){
            if(!flag)
            flag=0;
            break;
        }
        if(buff[i]=='/' && buff[i+1]=='/'){
            if(!flag)
            flag=0;
            break;
        }
        //printf("this is buff:%c\n",buff[i]);
        newbuff[j]=buff[i];
        newbuff[j+1]='\0';
        //printf("this is newbuff:%c\n",newbuff[i]);
        flag=1;
        i++;
        j++;
    }
    //printf("length of buff: %d \n",lenh);
}
void comType(char* string){
    command=C;
    if(string[0]=='@')
        command=A;
    if(string[0]=='(')
        command=L;
}
void Ainstruction(char* string){
    int dec;
    if(!isnumericsymbol((string+1),strlen(string+1))){
        dec= getval(string+1);
    }
    else
        dec = atoi(string+1);
    if(dec==-1){
        strcpy(symboltable[symboltable_end].name,string+1);
        symboltable[symboltable_end].value=reg;
        symboltable_end++;
        dec=reg;
        reg++;
    }
    char binary[16];
    itoa(dec,binary,2);
    int i;
    int len=strlen(binary);
    for(i=0;i<len;i++){
    address[16-len+i]=binary[i];
    }

}
void Cinstruction(char* string){
    int i,len=strlen(string);
    int lenh=0;
    int d=0,jm=0;
    for(i=0;i<len;i++){
        int j;
        if(string[i]=='='){// then there is a dest dest[0:i-1]
            d=i;
            for(j=0;j<i;j++)
                adest[j]=string[j];
        }
        if(string[i]==';'){
            jm=i;
            for(j=i;j<len;j++){
                ajmp[j-i]=string[j+1];
                lenh++;
            }
        }
    }
    adest[d]='\0';
    ajmp[lenh]='\0';
    int cstart=0,cend=len;
    if(d)
        cstart=d+1;
    if(jm)
        cend=jm;
    lenh=0;
    for(i=cstart;i<cend;i++){
        acomp[i-cstart]=string[i];
        lenh++;
    }
    acomp[lenh]='\0';
    codeC();
}
void Linstruction(char* string){

}
void addSymbol(char* string){
    int i,j=0;
    char symbol[strlen(string)];
    for(i=0;i<strlen(string);i++){
        if(string[i]!='(' && string[i]!=')' && string[i]!='\0'){
            symbol[j]=string[i];
            j++;
        }
    }
    symbol[j]='\0';
    strcpy(symboltable[symboltable_end].name,symbol);
    symboltable[symboltable_end].value=currentline;
}
void codeC(){
    char *comp;
    char *jump;
    comp=valuefromkey(acomp);
    jump=valuefromkey(ajmp);
    int i=0;
    dest[0]='0';dest[1]='0';dest[2]='0';
    while(adest[i]!='\0'){
        if(adest[i]=='A')dest[0]='1';
        if(adest[i]=='D')dest[1]='1';
        if(adest[i]=='M')dest[2]='1';
        i++;
    }
    int j=0;
    while(j<3){address[j]='1';j++;}
    for(i=3;i<16;i++){
        if(i<10)
            address[i]=comp[i-3];
        else if(i>=10 && i<13)
            address[i]=dest[i-10];
        else
            address[i]=jump[i-13];
    }


}
int main(int argc,char *argv[])
{
    if(argc!=3)
        printf("no files inputed");
    char buff[256];
    char newbuff[strlen(buff)];
    FILE *inputFile = fopen(argv[1],"r"); //opening .asm file for reading content
    FILE *outputFile = fopen(argv[2],"w");

    //*********************PASS 1**********************
    while(fgets(buff,sizeof(buff),inputFile))
    {
        if(buff==NULL){buff[0]='\0';continue;}
        newbuff[0]='\0';
        noComment(buff,newbuff);
        if(!flag){buff[0]='\0';continue;}
        comType(newbuff);
        printf("newbuff: %s\n",newbuff);
        if(command==A || command==C){currentline++;continue;}
        //L-COMMAND (xxx)
        else{//add symbol to table
            addSymbol(newbuff);
            symboltable_end++;
        }
    }
    fclose(inputFile);
    inputFile = fopen(argv[1],"r");

    //while loop reads current line PASS 2***************
    while(fgets(buff,sizeof(buff),inputFile))
    {
        if(buff==NULL){
            buff[0]='\0';
            continue;
        }
        newbuff[0]='\0';
        noComment(buff,newbuff);
        if(!flag){
            buff[0]='\0';
            continue;
        }
       // printf("this is buff:%s \n",buff);
       // printf("this is newbuff:%s \n",newbuff);
        comType(newbuff);
        //@xxx type instruction
        if(command==A){
            printf("this is a a instruction:%s \n",newbuff);
            Ainstruction(newbuff);
           // printf("this is address:%s \n",address);
            fprintf(outputFile,"%s\n",address);
        }
        if(command==C){
            printf("this is a c instruction:%s \n",newbuff);
            Cinstruction(newbuff);
            fprintf(outputFile,"%s\n",address);
        }
        reset();
        currentline++;

    }
   // if(feof(inputFile)) use this to end compilation
   fclose(inputFile);
   fclose(outputFile);

    return 0;
}
