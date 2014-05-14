#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "benchmark.h"

#define SIZE  1024 // taille du buffer
#define FILE_SIZE1  1048576 //tailles des fichier
#define FILE_SIZE2  4194304


/*
 *Benchmark de writev.
 * Prends un descipteur de fichier 'fd', une taille de buffer 'buffer_size', une taille de fichier 'file_size',
 * un pointeur vers un timer 't' et un pointeur vers un recorder 'rec'.
 * Calcule le temps moyen pris pour écrire un bloc de taille 'buffer_size'.
 */
void benchmark_writev(int fd, int buffer_size, int file_size, timer *t, recorder *rec){
	int iovcnt = file_size / buffer_size;
	struct iovec iov[iovcnt];
	int iov_len = sizeof(char) * buffer_size;
	int i = 0;
	
	for(i = 0; i < iovcnt; i++){
		iov[i].iov_base = malloc(iov_len);
		memset(iov[i].iov_base, 0, iov_len);
		iov[i].iov_len = iov_len;
	}// remplit les iovec par des '0'

	start_timer(t);
	ssize_t err = writev(fd, iov, iovcnt);
	write_record_n(rec, buffer_size, stop_timer(t), iovcnt);

	if(err == 1){
		perror("writev");
		exit(0);
	}

	for(i = 0; i < iovcnt; i++){
		free(iov[i].iov_base);
	}
}

/*
 * Benchmark de lseek+write.
 * Prends un descipteur de fichier 'fd', une taille de buffer 'buffer_size', une taille de fichier 'file_size',
 * un pointeur vers un timer 't' et un pointeur vers un recorder 'rec'.
 * Calcule le temps moyen pris pour écrire un bloc de taille 'buffer_size'.
 */
void benchmark_lseek(int fd, int buffer_size, int file_size, timer *t, recorder *rec){
	int len = sizeof(char) * buffer_size;
	int num = file_size / buffer_size;	
	char *s = malloc(len);
	memset(s, 0, len); //remplit la chaîne de caractere par des '0'
	int i = 0;
	start_timer(t);
	for(i = 0; i < num; i++){
		lseek(fd, i * buffer_size, SEEK_SET); //positionne le curseur a la fin du fichier
		write(fd, s, buffer_size);
	}
	write_record_n(rec, buffer_size, stop_timer(t), num);
	free(s);
}

int main(int argc, char *argv[]){
	timer *t = timer_alloc();
	recorder *writev_rec = recorder_alloc("writev.csv");
	recorder *lseek_rec = recorder_alloc("lseek.csv");
	recorder *writev2_rec = recorder_alloc("writev2.csv");
	recorder *lseek2_rec = recorder_alloc("lseek2.csv");
	

	/*BENCHMARK DE WRITEV*/
	int fd = 0; 
	int i = 0;	
	for(i = 1; i <= 256; i = 2*i){
		fd = creat("tmp1", 0700);
		benchmark_writev(fd, SIZE * i, FILE_SIZE1, t, writev_rec);
		close(fd);
	}
	
	
	for(i = 1; i <= 256; i = 2*i){
		fd = creat("tmp1", 0700);
		benchmark_writev(fd, SIZE * i, FILE_SIZE2, t, writev2_rec);
		close(fd);
	}	
	

	/*BENCHMARK DE LSEEK + WRITE*/

	for(i = 1; i <= 256; i = 2*i){
		fd = creat("tmp2", 0700);
		benchmark_lseek(fd, SIZE * i, FILE_SIZE1, t, lseek_rec);
		close(fd);
	}	

	
	for(i = 1; i <= 256; i = 2*i){
		fd = creat("tmp2", 0700);
		benchmark_lseek(fd, SIZE * i, FILE_SIZE2, t, lseek2_rec);
		close(fd);
	}	
                

	//FREE
	free(t);
	free(writev_rec);
	free(writev2_rec);
	free(lseek_rec);
	free(lseek2_rec);
	
	return EXIT_SUCCESS;
}

