// Toni Koskinen

//Credit for fork-function https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
//Credit for checking the directory https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>


typedef struct path{
    char str[20];
    struct path *pNext;
} PATH;

typedef struct cmd{
    char *cmdstr;
    char *args[20];
    int argcount;
    char *file; //Used for redirection
    struct cmd *pNext;
    
} CMD;

PATH *addPath(PATH *pA, char *pth){
    PATH *pNew, *ptr;
    if ((pNew=(PATH*)malloc(sizeof(PATH)))==NULL){
        perror("Memory allocation failed.");
        exit(1);
    }
    strcpy(pNew->str,pth);
    pNew->pNext=NULL;
    if (pA==NULL){
        pA=pNew;
    } else {
        ptr=pA;
        while (ptr->pNext!=NULL){
            ptr=ptr->pNext;
        }
        ptr->pNext=pNew;
    }
    return(pA);
}
PATH *emptyPath(PATH *pA){
    PATH *ptr=pA;
    while(ptr!=NULL){
        pA=ptr->pNext;
        free(ptr);
        ptr=pA;
    }
    return(pA);
}

CMD *addCMD(CMD *pA, char *str){
    CMD *pNew, *ptr;
    int i=1;
    
    if ((pNew=(CMD*)malloc(sizeof(CMD)))==NULL){
        perror("Memory allocation failed.");
        exit(1);
    }
    pNew->file=NULL;
    // Parse str
    pNew->cmdstr=strtok(str," ");
    pNew->args[0]=" ";
    while ((pNew->args[i]=strtok(NULL," "))!=NULL){
        //Check whitespaces
        if (isspace(*pNew->args[i])==0){
            //Redirection (multiple ">":s not allowed)
            if (strcmp(pNew->args[i],">")==0){
                pNew->args[i]=NULL;
                if (pNew->file==NULL){
                    if ((pNew->file=strtok(NULL," "))==NULL){
                        fprintf(stderr,"Error: no file to redirect");
                        free(pNew);
                        return(pA);
                    }
                } else {
                    fprintf(stderr,"Error: Multiple redirections is not allowed.\n");
                    free(pNew);
                    return(pA);
                }
                i--;
                
            } else if (pNew->file!=NULL){
                // Check that no arguments or additional files after redirection
                fprintf(stderr,"Error: Multiple files is not allowed in redirection.");
                free(pNew);
                return(pA);
            }
            i++;
        } //Get rid of whitespaces
        
    }
    i--;
    //Remove the possible newlines
    pNew->cmdstr=strtok(pNew->cmdstr,"\n");
    pNew->args[i]=strtok(pNew->args[i],"\n");
    if (pNew->file!=NULL){
        pNew->file=strtok(pNew->file,"\n");
    }

    pNew->argcount=i;
    pNew->pNext=NULL;


    if (pA==NULL){
        pA=pNew;
    } else {
        ptr=pA;
        while (ptr->pNext!=NULL){
            ptr=ptr->pNext;
        }
        ptr->pNext=pNew;
    }
    return(pA);

}
CMD *emptyCMD(CMD *pA){
    CMD *ptr=pA;
    while(ptr!=NULL){
        pA=ptr->pNext;
        free(ptr);
        ptr=pA;
    }
    return(pA);
}


int main(int argc, char *argv[]){
    //Pointers for PATH-structure
    PATH *pStart1=NULL; 
    PATH *ptr1=NULL;
    //Pointers for CMD-structure
    CMD *pStart2=NULL;
    CMD *ptr2=NULL;
    //
    char *slash="/";
    char *compath;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char error_message[30] = "An error has occurred\n";
    char cwd[256];
    char inpath[10]="/bin";
    char *str;
    char *str1;
    char *str2;
    int n=0; //Number of child processes
    int file;
    int executed=0;
    //Set initial path
    pStart1=addPath(pStart1,inpath);
    if (argc==1){//Interactive mode
        while(1){
            pStart2=emptyCMD(pStart2);
            fprintf(stdout,"wish> ");
            nread=getline(&line,&len,stdin);
            //Get commands
            str1=strtok(line,"&");
            str=strtok(NULL,"\n");
            pStart2=addCMD(pStart2,str1);
            while (str!=NULL){
            
                str2=strtok(str,"&");
                str=strtok(NULL,"WANNA-BE-DELIMITER");
                pStart2=addCMD(pStart2,str2);
                
            }
           
            //Execute the commands
            ptr2=pStart2;
            char cwd[256];
            while (ptr2!=NULL){
                
                if (strcmp(ptr2->cmdstr,"exit")==0 && ptr2->argcount==0){
                    pStart1=emptyPath(pStart1);
                    pStart2=emptyCMD(pStart2);
                    exit(0);
                } else if (strcmp(ptr2->cmdstr,"cd")==0 && ptr2->argcount==1) {
                    
                    if(chdir(ptr2->args[1])!=0){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    } else {
                        
                        if (getcwd(cwd, sizeof(cwd)) == NULL){
                            //Check current working directory
                            fprintf(stderr,"getcwd() error");
                        } else{
                            fprintf(stdout,"Current working directory is: %s\n", cwd);
                        }
                   
                    }

                } else if (strcmp(ptr2->cmdstr,"path")==0){
                    
                    //Change the path(s)
                    pStart1=emptyPath(pStart1);
                    for (int i=0; i<ptr2->argcount; i++){
                        pStart1=addPath(pStart1,ptr2->args[i+1]);
                    
                    }
                    //Printing the path(s)
                    ptr1=pStart1;
                    printf("Paths:\n");
                    while(ptr1!=NULL){
                    
                    printf("%s\n",ptr1->str);
                    ptr1=ptr1->pNext;
                    }
                
                    } else {
                    ptr1=pStart1;
                    executed=0; //"Check-value" for execv
                    while(ptr1!=NULL){
                        int memneed=strlen(ptr1->str)+strlen(ptr2->cmdstr)+strlen(slash)+1;
                        compath=(char*)malloc(memneed);
                        strcpy(compath,ptr1->str);
                        strcat(compath,slash);
                        strcat(compath,ptr2->cmdstr);
                    //Check if the command is executable
                        if (access(compath,X_OK)==0){
                            n++;
                            executed=1;
                            if (fork()==0){
                            if (ptr2->file!=NULL){
                                FILE *tmpfile=fopen(ptr2->file,"w");
                                close(1);
                                fclose(tmpfile);
                                int file=open(ptr2->file,O_WRONLY | O_TRUNC);
                                dup(file);
                            }
                                if (execv(compath,ptr2->args)<0){
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                }
                        }
                    } 
                        if (executed==1){ //Command executed --> break
                            free(compath);
                            break;
                        } else {
                            ptr1=ptr1->pNext;
                            free(compath);
                            continue;
                        }

                } 

                if (executed==0){//Loop completed but no commands executed
                write(STDERR_FILENO, error_message, strlen(error_message));
                }
                }
           
            ptr2=ptr2->pNext; 
            
            }
            //Wait commands for child processes
            while(n>0){
                wait(NULL);
                n--;
            }

            }
        
    } else if (argc==2){//Batch mode
        FILE *cmdfile;
        cmdfile=fopen(argv[1],"r");
        if (cmdfile==NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
        }
        while ((nread=getline(&line,&len,cmdfile))!=-1){
            //Check whitespaces
            if (isspace(*line)!=0){
            continue;
            }
            pStart2=emptyCMD(pStart2);
            //Get commands
            str1=strtok(line,"&");
            str=strtok(NULL,"\n");
            pStart2=addCMD(pStart2,str1);
            while (str!=NULL){
            
                str2=strtok(str,"&");
                str=strtok(NULL,"WANNA-BE-DELIMITER");
                pStart2=addCMD(pStart2,str2);
                
            }
           
            //Execute the commands
            ptr2=pStart2;
            while (ptr2!=NULL){
                if (strcmp(ptr2->cmdstr,"exit")==0 && ptr2->argcount==0){
                    pStart1=emptyPath(pStart1);
                    pStart2=emptyCMD(pStart2);
                    fclose(cmdfile);
                    printf("\n");
                    exit(0);
                } else if (strcmp(ptr2->cmdstr,"cd")==0 && ptr2->argcount==1) {
                    
                    if(chdir(ptr2->args[1])!=0){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    } else {
                        //Check current working directory
                        if (getcwd(cwd, sizeof(cwd)) == NULL){
                            fprintf(stderr,"getcwd() error");
                        } else{
                            fprintf(stdout,"Current working directory is: %s\n", cwd);
                        }
                   
                    }
                    
                    

                } else if (strcmp(ptr2->cmdstr,"path")==0){
                    
                    //Change the path(s)
                    pStart1=emptyPath(pStart1);
                    for (int i=0; i<ptr2->argcount; i++){
                        pStart1=addPath(pStart1,ptr2->args[i+1]);
                    
                    }
                    //Printing the path(s)
                    ptr1=pStart1;
                    printf("Paths:\n");
                    while(ptr1!=NULL){
                    
                    printf("%s\n",ptr1->str);
                    ptr1=ptr1->pNext;
                    }
                
                    } else {
                    executed=0;
                    ptr1=pStart1;
                    while(ptr1!=NULL){
                        int memneed=strlen(ptr1->str)+strlen(ptr2->cmdstr)+strlen(slash)+1;
                        compath=(char*)malloc(memneed);
                        strcpy(compath,ptr1->str);
                        strcat(compath,slash);
                        strcat(compath,ptr2->cmdstr);
                    //Check if the command is executable
                    if (access(compath,X_OK)==0){
                        executed=1;
                        n++;
                        if (fork()==0){        
                        if (ptr2->file!=NULL){
                        FILE *tmpfile=fopen(ptr2->file,"w");
                        fclose(tmpfile);
                        file=open(ptr2->file,O_WRONLY | O_TRUNC);
                        dup2(file,1);
                        dup2(1,2);
                        }                    
                            execv(compath,ptr2->args);
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                    }
                    if (executed==1){ //Command executed --> break
                            free(compath);
                            break;
                        } else {
                            ptr1=ptr1->pNext;
                            free(compath);
                            continue;
                        }

                } 

                if (executed==0){//Loop completed but no commands executed
                write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
           
            ptr2=ptr2->pNext; 
            
            }
            //Wait-commands for child processes
            while(n>0){
                wait(NULL);
                n--;
            }

            }
        free(line);
        fclose(cmdfile);
        exit(0);
            
        } else { //wish called with too many arguments
        fprintf(stderr,"wish: Too many input arguments, usage: ./wish [file]\n");
        exit(1);

        }


    }
