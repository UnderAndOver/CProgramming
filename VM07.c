#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
enum commandType {ARITHMETIC,PUSH,POP,LABEL,GOTO,IF,FUNCTION,RETURN,CALL}command;
//0-8 arithmetic; 9:push;10:pop;...
char arg1[32];
char arg2[32];
int arg3,val,currentline=0;
int help=0;
char* fileName;
char* operations[17];
enum dataType {file,directory}type;
char** files;
char** fileNames;
//method to open directory and get files, or open file depending on input
void fileManager(char** argv){
    argv++;
    DIR *dir=NULL;
    files=malloc(16*sizeof(char*));
    fileNames=malloc(16*sizeof(char*));
    struct dirent *pent = NULL;
    int j=0,i,k,n;
    for(i=0;*(*argv+i)!='\0';i++)
        if(*(*argv+i)=='.'){type=file;break;}
        else type=directory;
    if(type==directory){
        char folderName[100]={NULL};
        strcat(folderName,"./");
        strcat(folderName,argv[0]);
        strcat(folderName,"/");
        int folderLen=strlen(folderName)+1;
        dir=opendir(folderName);
        if(dir==NULL){printf("\nERROR");exit(1);}
        while(pent=readdir(dir)){
            if(pent==NULL){printf("ERROR pent");exit(3);}
            if(pent->d_name[0]!='.'){
                //for(k=0;*(pent->d_name+k)!='.';k++);
                k=pent->d_namlen+1;
                fileNames[j]=(char*)malloc(k+1);
                memcpy(fileNames[j],pent->d_name,k);
                files[j]=(char*)malloc(k+2+folderLen);
                memcpy(files[j],folderName,folderLen);
                strcat(files[j],pent->d_name);
                //memcpy(files[j],pent->d_name,k);
                (files[j][k+folderLen])='\0'; fileNames[j][k]='\0';
                j++;}
        }files[j]=NULL;fileNames[j]=NULL;closedir(dir);
    }
    if(type==file){
        k=strlen(*argv);
        files[0]=(char*)malloc(k+1);
        memcpy(files[0],*argv,k);
        fileNames[0]=(char*)malloc(k+1);
        memcpy(fileNames[0],files[0],k);
        fileNames[0][k]='\0';
        fileNames[1]=NULL;
        (files[0][k])='\0';
        files[1]=NULL;
    }
}
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
    {"pop", 10},
    {"label",11},
    {"goto",12},
    {"if-goto",13},
    {"function",14},
    {"call",15},
    {"return",16}
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
operations[11]="(#)\n";
operations[12]="@#\n0;JMP\n";
operations[13]="@SP\nM=M-1\nA=M\nD=M\n@#\nD;JNE\n";
operations[14]="@SP\nA=M\nM=0\n@SP\nM=M+1\n";
operations[15]="@return$\nD=A\n@SP\nM=M+1\nA=M-1\nM=D\n@n\nM=0\n(LOOP_PUSH$)\n@n\nAM=M+1\nD=M\n@SP\nM=M+1\nA=M-1\nM=D\n@n\nD=M\n@4\nD=D-A\n@LOOP_PUSH$\nD;JNE\n"
                "@i\nD=A\n@SP\nD=M-D\n@5\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@g\n0;JMP\n(return$)\n";
operations[16]="@LCL\nD=M\n@R13\nM=D\n@5\nD=D-A\n@R14\nM=D\nA=D\nD=M\n@R15\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n"
                "@ARG\nD=M\n@SP\nM=D+1\n@R14\nAM=M+1\nD=M\n@LCL\nM=D\n@R14\nAM=M+1\nD=M\n@ARG\nM=D\n@R14\nAM=M+1\nD=M\n@THIS\nM=D\n@R14\nAM=M+1\nD=M\n@THAT\nM=D\n@R15\nA=M\n0;JMP\n";
}
#define nKeys  (sizeof(optable)/sizeof(opstruct))
int getVal(char* op){
    int i;
    for(i=0;i<nKeys;i++)
        if(strcmp(optable[i].op,op)==0)
            return optable[i].hack;
    return -1;
}
void getBase(){
    //printf("get base\n");
    int k=strlen(fileName);
    char* helper;
    if(val>=11 && val<14){
        char helper2[1024];
        strncpy(helper2,fileName,k);
        helper2[k-1]='\0';
        strcat(helper2,"$");
        strcat(helper2,arg2);
        helper=helper2;
    }
    else if(strcmp(arg2,"static")==0){
        char helper2[1024];
        printf("static test\n");
        int lenh=(arg3==0)? 1:(int)log10(arg3)+1;
        char iaa[lenh];
        itoa(arg3,iaa,10);
        strncpy(helper2,fileName,k);
        helper2[k-1]='\0';
        strcat(helper2,iaa);
        helper=helper2;
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
    if(val<14){
    memset(arg2,0,sizeof(arg2));
    strncpy(arg2,helper,strlen(helper));
    }
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
        for(i=0;i<strlen(string);i++){
            if(string[i]==' '&&string[i+1]==' ')
                break;
            arg1[i]=string[i];
        }
        arg1[i]='\0';
        printf("%s\n",arg1);
    }
    else{
        arg3=0;
        for(i=0;i<string[i]!='\0';i++){
            if(string[i]=='/')break;
            if(string[i]==' '){j++;helper=i+1;continue;}
            if(j==0){arg1[i]=string[i];arg1[i+1]='\0';}
            if(j==1){arg2[i-helper]=string[i];arg2[i-helper+1]='\0';}
            if(j==2){arg3=arg3*10+(string[i]-'0');}
        }
        printf("arg1:%s arg2:%s arg3:%d\n",arg1,arg2,arg3);
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
        if(val==11){command=LABEL; return;}
        if(val==12){command=GOTO; return;}
        if(val==13){command=IF; return;}
        if(val==14){command=FUNCTION; return;}
        if(val==15){command=CALL; return;}
        if(val==16){command=RETURN; return;}
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
    if(val==3 || val==4 || val==5 || val==15){
        itoa(currentline,numss,10);
        digitss=(currentline==0)? 1:(int)log10(currentline)+1;
    }
    if(command!=ARITHMETIC && arg2[0]!='0' && val<11){
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
    if(val!=14 && val!=16)
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
            if(arg2[0]!='0' && val!=15){
                for(j=0;j<strlen(string2);j++)
                    buff[i+j+t]=string2[j];
                t+=j-1;
            }
            else{t-=1;}
        }
        else if(helper[i]=='#'){
            for(j=0;j<strlen(arg2);j++)
                buff[i+j+t]=arg2[j];
            t+=j-1;
        }
        else if(helper[i]=='g'){
            for(j=0;j<strlen(arg2);j++)
                buff[i+j+t]=arg2[j];
            t+=j-1;
        }
        else
            buff[i+t]=helper[i];
    }
    else if(val==14){
        strcat(buff,"(");
        strcat(buff,arg2);
        strcat(buff,")\n");
        for(i=0;i<arg3;i++){
            strcat(buff,helper);
            //fprintf(output,"%s",helper);
        }
    }
    else
        strcat(buff,helper);

    fprintf(output,"%s",buff);
    printf("digits:%d",digits);
    printf("operation %d:\n%s",val,buff);
}
void writeInit(FILE* output){
    char* bootstrap="@256\nD=A\n@SP\nM=D\n";
    fprintf(output,"%s",bootstrap);
}
void main(int argc,char *argv[]){
    fileManager(argv);
    int i,l;
    FILE *input;
    FILE *output = fopen(argv[2],"w");
    int lennn=(strlen(fileNames)/4)-1;
    for(i=lennn;i>=0;i--){
        l=strlen(*(fileNames+i))-2;
        input=fopen(files[i],"r");
        fileName=(char*)malloc(l);
        memcpy(fileName,fileNames[i],l);
        fileName[l]='\0';
        char buffer[256];
        int flag;
        if(type==directory && i==lennn){fprintf(output,"//SP=256\n");writeInit(output);char* helper="call Sys.init 0";fprintf(output,"// %s\n",helper); parser(helper);codeWriter(output);}
        while(fgets(buffer,sizeof(buffer),input)){
            noComment(buffer,&flag);
            if(flag)
            continue;
            printf("%s\n",buffer);
            fprintf(output,"// %s\n",buffer);
            parser(buffer);
            codeWriter(output);
            //fprintf(output,"%s\n",buffer);
        }
        fclose(input);
    }
    fclose(output);
}




