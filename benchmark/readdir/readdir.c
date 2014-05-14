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
#define NBFILES 10

#include "benchmark.h"

int main (int argc, char *argv[]){
	long long int * timerR = calloc(100,sizeof(double));
	timer *t = timer_alloc();
	recorder *readdir_rec = recorder_alloc("readdir.csv");
	
	mkdir("./temp",777);//Permissions peut etre a modifier
	
	char filename[256];
	FILE *fp = NULL;
	int i;
	
	chdir("./temp/");  //AJOUTER VERIFICATION D'ERREUR
	int i;
	for(i = 0; i < MAX; i++){
	   sprintf(filename,"./fichier%i.txt",i);
	   fp = fopen(filename,"w");
	   memset(filename,0x00,256);
	   fclose(fp);
	   if(i%10==0){
		struct dirent f;
		start_timer(t);
		benchmark_readdir();
		timerR[MAX/10]=stop_timer(t);
	   }
	}
	//AJOUTE ECRITURE	
}

void benchmark_readdir (){
	
	struct dirent *lecture;
	DIR *rep;
	rep = opendir("./temp"); // a verifier
	while((lecture = readdir(rep))){
		printf("%s\n", lecture->d_name); //a retirer	
	}
	closedir(rep);
	

}
