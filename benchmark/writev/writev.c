#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "benchmark.h"

#define SIZE  128 // taille du buffer en bytes
#define MAX   1024 //multiplicateur maximum pour la taille du buffer
#define FILE_SIZE  131072 //taille du fichier a ecrire en bytes


/*
 *Benchmark de writev.
 * Prends un descipteur de fichier 'fd', une taille de buffer 'buffer_size', une taille de fichier 'file_size',
 * un pointeur vers un timer 't' et un pointeur vers un recorder 'rec'.
 * Calcule le temps pris pour écrire un fichier de taille 'file_size'.
 */
void benchmark_writev(int fd, int buffer_size, int file_size, timer *t, recorder *rec){
	int iovcnt = file_size / buffer_size;
	struct iovec iov[iovcnt];
	int iov_len = sizeof(char) * buffer_size;
	int i = 0;
	
	for(i = 0; i < iovcnt; i++){
		iov[i].iov_base = malloc(iov_len);
		if(iov[i].iov_base == NULL){
			fprintf(stderr, "malloc at benchmark_writev");
			exit(0);
		}
		memset(iov[i].iov_base, 0, iov_len);
		iov[i].iov_len = iov_len;
	}// remplit les iovec par des '0'

	start_timer(t);
	ssize_t err = writev(fd, iov, iovcnt);
	write_record(rec, buffer_size, stop_timer(t));

	if(err == -1) {
		printf("iovcnt : %d \n", iovcnt);
		//perror("writev");		
		//exit(0);
	}

	for(i = 0; i < iovcnt; i++){
		free(iov[i].iov_base);
	}
}

/*
 * Benchmark de lseek+write.
 * Prends un descipteur de fichier 'fd', une taille de buffer 'buffer_size', une taille de fichier 'file_size',
 * un pointeur vers un timer 't' et un pointeur vers un recorder 'rec'.
 * Calcule le temps pris pour écrire un fichier de taille 'file_size'.
 */
void benchmark_lseek(int fd, int buffer_size, int file_size, timer *t, recorder *rec){
	int len = sizeof(char) * buffer_size;
	int num = file_size / buffer_size;	
	char *s = malloc(len);
	if(s == NULL){
		fprintf(stderr, "malloc at benchmark_lseek");
		exit(0);
	}
	memset(s, 0, len); //remplit la chaîne de caractere par des '0'
	int i = 0;
	int err = 0;
	start_timer(t);
	for(i = 0; i < num; i++){
		lseek(fd, i * buffer_size, SEEK_SET); //positionne le curseur a la fin du fichier
		err = write(fd, s, buffer_size);
		if(err == -1){
			perror("write");
			exit(0);
		}
	}
	write_record(rec, buffer_size, stop_timer(t));

	free(s);
}

int main(int argc, char *argv[]){
	timer *t = timer_alloc();
	recorder *writev_rec = recorder_alloc("writev.csv");
	recorder *lseek_rec = recorder_alloc("lseek.csv");
	

	/*BENCHMARK DE WRITEV*/
	int fd = 0; 
	int i = 0;	
	for(i = 1; i <= MAX; i = 2*i){
		fd = creat("tmp1", 0700);
		benchmark_writev(fd, SIZE * i, FILE_SIZE, t, writev_rec);
		close(fd);
	}
	
	/*BENCHMARK DE LSEEK + WRITE*/

	for(i = 1; i <= MAX; i = 2*i){
		fd = creat("tmp2", 0700);
		benchmark_lseek(fd, SIZE * i, FILE_SIZE, t, lseek_rec);
		close(fd);
	}	         

	//FREE
	free(t);
	free(writev_rec);
	free(lseek_rec);
	
	return EXIT_SUCCESS;
}

