// Toni Koskinen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void readb(FILE *file){
    int luku;
    int c1;
    int c2;
    while (1){
        fread(&luku,sizeof(luku),1,file);
        fread(&c2,sizeof(c1),1,file);
        if (c1==c2){
            break;
        }
        for (int j=0; j<luku; j++){
            printf("%c",c2);

        }
        c1=c2;
        
    }
    
    
}
int main(int argc, char *argv[]){
    FILE *file;
    if (argc==1){
        fprintf(stderr,"usage: %s file1 [file2...]\n",argv[0]);
        exit(1);
    } else {
        for (int i=1; i<argc; i++){

        file=fopen(argv[i],"r");
        if (file==NULL) {
            fprintf(stderr,"error: cannot open file '%s'.\n",argv[1]);
            continue;
        }
        readb(file);
        fclose(file);
        }
        exit(0);
        
    }
}