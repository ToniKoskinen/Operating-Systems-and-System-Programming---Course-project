// Toni Koskinen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void searchnprint(char *term, FILE *file){
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    char *pline;
    while ((nread=getline(&line,&len,file))!=-1){
//credit for strstr-function: https://www.tutorialspoint.com/c_standard_library/c_function_strstr.htm
        if ((strstr(line,term))!=NULL){
            pline=strtok(line,"\n");
            fprintf(stdout,"%s\n",pline);
        }
    }
    free(line);
    
}

int main(int argc, char *argv[]){
    FILE *file;
    if (argc==1){
        fprintf(stdout,"my-grep: searchterm [file ...].\n");
        exit(1);
    }
    if (strlen(argv[1])==0){
        exit(0);
    }
    for (int i=2;i<argc;i++){
    file=fopen(argv[i],"r");
    if (file==NULL) {
                fprintf(stderr,"my-grep: error: cannot open file '%s'.\n",argv[i]);
                exit(1);
            }  
    searchnprint(argv[1],file);
    fclose(file);
    }
}