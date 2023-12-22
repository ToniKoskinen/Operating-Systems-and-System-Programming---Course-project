// Toni Koskinen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void asknprintcontents(){
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    fprintf(stdout,"Please enter a line (press ENTER to stop): ");
    if ((nread=getline(&line,&len,stdin))!=1){
        asknprintcontents();
        fprintf(stdout,"%s",line);
    }
    free(line);
}

void readnprintcontents(FILE *file){
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    if ((nread=getline(&line,&len,file))!=-1){
        readnprintcontents(file);
        fprintf(stdout,"%s",line);
    }
    free(line);
}


void readnwritecontents(FILE *file1, FILE *file2){
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    if ((nread=getline(&line,&len,file1))!=-1){

        readnwritecontents(file1,file2);
        fprintf(file2,"%s",line);
    }
    free(line);
}


int main(int argc, char *argv[]) {
    FILE *file1;
    FILE *file2;

    if (argc==1) {
        asknprintcontents();
        

    } else if (argc==2) {
        file1=fopen(argv[1],"r");
        if (file1==NULL) {
            fprintf(stderr,"error: cannot open file '%s'.\n",argv[1]);
            exit(1);
        }
        readnprintcontents(file1);
        fclose(file1);


    } else if (argc==3) {
        
        if (strcmp(argv[1],argv[2])==0) {
            fprintf(stderr,"Input and output file must differ.\n");
            exit(1);
        }
        file1=fopen(argv[1],"r");
        file2=fopen(argv[2],"w");
        if (file1==NULL) {
            fprintf(stderr,"error: cannot open file '%s'.\n",argv[1]);
            exit(1);
        }
        if (file2==NULL) {
            fprintf(stderr,"error: cannot open file '%s'.\n",argv[2]);
            exit(1);
        }
        readnwritecontents(file1,file2);
        fclose(file1);
        fclose(file2);

    } else {
        fprintf(stdout,"usage: %s <input> <output>\n",argv[0]);
        exit(1);
        
    }
    }


//eof