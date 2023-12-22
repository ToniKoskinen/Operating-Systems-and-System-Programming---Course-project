// Toni Koskinen 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readnprint(FILE *file){
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread=getline(&line,&len,file))!=-1){
        fprintf(stdout,"%s",line);
    }
    free(line);
    return;
    
}

int main(int argc, char *argv[]){
    FILE *file;
    if (argc==1){
        fprintf(stdout,"usage: %s <input files>\n",argv[0]);
        exit(0);        
    } else if (argc==2) {
        file=fopen(argv[1],"r");
        if (file==NULL) {
            fprintf(stderr,"error: cannot open file '%s'.\n",argv[1]);
            exit(1);
        }
        readnprint(file);
        fclose(file);
        exit(0);

    }else {
        for (int i=1; i<argc;i++){
            file=fopen(argv[i],"r");
            if (file==NULL) {
                fprintf(stderr,"error: cannot open file '%s'.\n",argv[i]);
                continue;
            }
            readnprint(file);
            fclose(file);
        }
        exit(0);

    }
}

