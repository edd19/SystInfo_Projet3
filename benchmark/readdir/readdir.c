#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "benchmark.h"

#define NBFILES 10000

void benchmark_readdir(); 

int main (int argc, char *argv[]){
 	long long int * timerR = calloc(NBFILES,sizeof(double));
	recorder *readdir_rec = recorder_alloc("readdir.csv");
	
  	//Creation du sous-dossier temp
	
	int err = mkdir("./temp", 0700);
	if(err == -1){	
		perror("readdir");
		exit(0);
	}
	
	//Changement du dossier courant au sous-dossier temp	
	
	err = chdir("./temp/");
	if(err == -1){	
		perror("readdir");
		exit(0);
	}
	
	//Creation de NBFILES avec lecture du repertoire toutes les 100 iterations
	
	char filename[256]; //A modifier
	FILE *fp = NULL;
	int i;
	for(i = 1; i < NBFILES; i++){
	   sprintf(filename,"./fichier%i.txt",i);
	   fp = fopen(filename,"w");
	   memset(filename,0x00,256);
	   fclose(fp);
	   
  	   if(i%100==1){
		DIR *rep = opendir(".");
		if(rep == NULL){	
		   printf("OPENDIR\n");
		   perror("readdir");
		   exit(0);
		}
			   
	        benchmark_readdir(rep,readdir_rec/*,i*/);
	        err=closedir(rep);
	        if(err == -1){	
		   printf("CHDIR\n");
		   perror("readdir");
		   exit(0);
	        }  
	   }
	}
	//Remplacement du dossier courant par le dossier parent
	err = chdir("..");
	if(err == -1){
               perror("readdir");
               exit(0);
        }
	//Suppression du dossier temporaire
	err = system("rm -r ./temp");
	if(err == -1){
               perror("readdir");
               exit(0);
        }
	
	free(timerR);
	free(readdir_rec);
	
}

void benchmark_readdir (DIR *rep,recorder *rec){
	timer *t = timer_alloc();
	struct dirent *lecture=NULL;
	int i=0;
	start_timer(t);
	while((lecture = readdir(rep))!=NULL){
		i++; //Nombre de fichier lus INCREMENTATION DE I NEGLIGEABLE
	}

	write_record(rec,i,stop_timer(t));	
	free(t);
}
