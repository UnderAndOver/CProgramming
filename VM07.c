#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum commandType {ARITHMETIC,PUSH,POP,LABEL,GOTO,IF,FUNCTION,RETURN,CALL}command;
//0-8 arithmetic; 9:push;10:pop;...
char arg1[32];
char arg2[32];
int arg3,val,currentline=0;
int help=0;
char* fileName;
char* operations[17];
typedef struct opstruct{
    char* op;
    int hack;
}opstruct;
opstruct optable[17]={
    {"add", 0},
    {"sub", 1},
    {"neg", 2},
    {"eq", 3},
    {"gt", 4},
    {"lt", 5},
    {"and", 6},
    {"or", 7},
    {"not", 8},
    {"push", 9},
    {"pop", 10}
};
void init(){
operations[0]="@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D+M\nM=D\n";
operations[1]="@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=M-D\nM=D\n";
operations[2]="@SP\nA=M-1\nM=-M\n";
operations[3]="@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\nD=D-M\n@END$\nD;JEQ\nD=-1\n(END$)\n@SP\nA=M-1\nM=!D\n";
operations[4]="@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\nD=D-M\n@T$\nD;JGE\nD=-1\n@T$$\n0;JMP\n(T$)\nD=0\n(T$$)\n@SP\nA=M-1\nM=D\n";
operations[5]="@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\nD=D-M\n@T$\nD;JLE\nD=-1\n@T$$\n0;JMP\n(T$)\nD=0\n(T$$)\n@SP\nA=M-1\nM=D\n";
operations[6]="@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D&M\nM=D\n";
operations[7]="@SP\nAM=M-1\nD=M\n@SP\nA=M-1\nD=D|M\nM=D\n";
operations[8]="@SP\nA=M-1\nM=!M\n";
operations[9]="@i\nD=^\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
operations[10]="@i\nD=A\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n";
}
int getVal(char* op){
    int i;
    for(i=0;i<11;i++)
        if(strcmp(optable[i].op,op)==0)
            return optable[i].hack;
    return -1;
}
void getBase(){
    char* helper;
    int lenh=(arg3==0)? 1:(int)log10(arg3)+1;
    char iaa[lenh];
    char reset[strlen(fileName)+1];
    strncpy(reset,fileName,strlen(fileName));
    if(strcmp(arg2,"static")==0){
        itoa(arg3,iaa,10);
        helper=reset;
        strcat(helper,iaa);
        arg3=0;
        help=1;
    }
    else if(strcmp(arg2,"pointer")==0){
        if(arg3==0)
            helper="THIS";
        if(arg3==1)
            helper="THAT";
        arg3=0;
        help=1;
    }
    else{
        help=0;
        switch(arg2[2]){
            case 'c':
                helper="LCL";
                break;
            case 'g':
                helper="ARG";
                break;
            case 'i':
                helper="THIS";
                break;
            case 'a':
                helper="THAT";
                break;
            case 'm':
                helper="5"; //temp
                break;
            default:
                helper="0";
                break;
        }
    }
    memset(arg2,0,sizeof(arg2));
    strncpy(arg2,helper,strlen(helper));
}
void noComment(char* string,int *flag){
    int i=0;
    char helper[strlen(string)];
    *flag=1;
    while(i<strlen(string)){
        if(string[0]=='/'|| string[0]=='\n')
            return;
        if(string[i]=='\n')
            break;
        helper[i]=string[i];
        i++;
    }
    *flag=0;
    strcpy(string,helper);
    string[i]='\0';

}
void parser(char* string){
    int i,j=0,helper=0;
    arg3=-1;
    comType(string);
    if(command==ARITHMETIC){
        for(i=0;i<strlen(string);i++)
            arg1[i]=string[i];
        arg1[i]='\0';
        printf(" %s\n",arg1);
    }
    else{
        arg3=0;
        for(i=0;i<string[i]!='\0';i++){
            if(string[i]==' '){j++;helper=i+1;continue;}
            if(j==0){arg1[i]=string[i];arg1[i+1]='\0';}
            if(j==1){arg2[i-helper]=string[i];arg2[i-helper+1]='\0';}
            if(j==2){arg3=arg3*10+(string[i]-'0');}
        }
        printf(" arg1:%s arg2:%s arg3:%d\n",arg1,arg2,arg3);
        getBase();
    }

}
void comType(char* string){
    int i=0;
    char helper[strlen(string)];
    while(string[i]!='\0'){
        helper[i]=string[i];
        helper[i+1]='\0';
        val=getVal(helper);
        if(val<9 && val!=-1){command=ARITHMETIC; return;}
        if(val==9){command=PUSH; return;}
        if(val==10){command=POP; return;}
        i++;
    }
}
void codeWriter(FILE* output){
    init();
    char* helper=operations[val];
    char buff[1024]={NULL};
    /*digitss is used to get the amount of digits in arg3
    *numss is the string version of arg3
    *t is offset for buff array
    */
    int i,j,digits,digitss,t=0;
    char nums[16]={NULL};
    char numss[16]={NULL};
    char* string ="@#\nD=^\n";
    char* string2="\nA=A+D";
    if(arg3!=-1){
        itoa(arg3,nums,10);
        digits=(arg3==0)? 1:(int)log10(arg3)+1;
    }
    if(val==3 || val==4 || val==5){
        itoa(currentline,numss,10);
        digitss=(currentline==0)? 1:(int)log10(currentline)+1;
    }
    if(command!=ARITHMETIC && arg2[0]!='0'){
        for(i=0;string[i]!='\0';i++){
            if(string[i]=='^'){
                if(arg2[0]=='5'||help==1)
                    buff[i+t]='A';
                else
                    buff[i+t]='M';
                //t++;
            }
            else if(string[i]=='#'){
                for(j=0;j<strlen(arg2);j++){
                    buff[i+j+t]=arg2[j];
                }
                t+=j-1;
            }
            else
                buff[i+t]=string[i];
        }
        t+=i;
    }
    for(i=0;helper[i]!='\0';i++){
        if(helper[i]=='^'){
            if(arg2[0]=='0')
                buff[i+t]='A';
            else
                buff[i+t]='M';
            //t++;
        }
        else if(helper[i]=='$'){
            for(j=0;j<digitss;j++){
                buff[i+j+t]=numss[j];
            }
            currentline++;
            t+=j-1;
        }
        else if(helper[i]=='i'){
            for(j=0;j<digits;j++){
                buff[i+j+t]=nums[j];
            }
            t+=j;
            if(arg2[0]!='0'){
                for(j=0;j<strlen(string2);j++)
                    buff[i+j+t]=string2[j];
                t+=j-1;
            }
            else{t-=1;}
        }
        else
            buff[i+t]=helper[i];
    }
    fprintf(output,"%s",buff);
    printf("digits:%d",digits);
    printf("operation %d:\n%s",val,buff);
}
void main(int argc,char *argv[]){

    int len=strlen(argv[1])-3;
    char t[len];
    strncpy(t,argv[1],len);
    t[len]='\0';
    fileName=t;
    strcat(fileName,".");
    char buffer[256];
    FILE *input = fopen(argv[1],"r");
    FILE *output = fopen(argv[2],"w");
    int flag;
    while(fgets(buffer,sizeof(buffer),input)){
        noComment(buffer,&flag);
        if(flag)
        continue;
        printf("%s",buffer);
        fprintf(output,"// %s\n",buffer);
        parser(buffer);
        codeWriter(output);
        //fprintf(output,"%s\n",buffer);
    }
    fclose(input);
    fclose(output);
}




