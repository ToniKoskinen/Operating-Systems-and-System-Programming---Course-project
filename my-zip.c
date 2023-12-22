// Toni Koskinen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Veeti Rajaniemi (https://github.com/veetirajaniemi) told me about the fgetc-function
//and I also used the following source
//https://www.tutorialspoint.com/c_standard_library/c_function_fgetc.htm
void readnwrite(FILE *file){
    int c1=-123;
    int c2;
    int count=0;
    while (1){
        c2=fgetc(file);
        if (c1!=c2){
            if (count!=0){
            fwrite(&count, sizeof(count),1,stdout);
            fwrite(&c1,sizeof(c1),1,stdout);
            }
            count=1;
        } else {
            count++;

        }
        c1=c2;
        if(feof(file)){
            break;
        }

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
        readnwrite(file);
        fclose(file);
        }
        exit(0);
        
    }

}

