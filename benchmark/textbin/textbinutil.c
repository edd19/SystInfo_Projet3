/**
 * \file template.c
 * \brief Compare la sauvegarde de nombres en binaire et en ascii
 *
 * Écris et lis des nombres sauvegardés en binaire puis en ascii.
 * La sauvegarde en binaire se fait avec `read` et `write` et celle
 * en ascii se fait avec `fprintf` et `fscanf` auquel on
 * a désactivé le buffer de `stdio` pour que ce soit plus équitable.
 *
 * Problème
 * * `0xff` partout donne 0 pour un nombre à virgule flottante
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>

#include "benchmark.h"
#include "copy.h"
#include "textbinutil.h"

#define N 100
#define F "tmp.dat"
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_short (timer *t, recorder *read_rec, recorder *write_rec, short value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(short));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(short));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_short (timer *t, recorder *read_rec, recorder *write_rec, short value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%hd ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(short), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%hd ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(short), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_int (timer *t, recorder *read_rec, recorder *write_rec, int value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(int));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(int));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_int (timer *t, recorder *read_rec, recorder *write_rec, int value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%d ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(int), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%d ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(int), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_long_int (timer *t, recorder *read_rec, recorder *write_rec, long int value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(long int));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(long int));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_long_int (timer *t, recorder *read_rec, recorder *write_rec, long int value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%ld ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(long int), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%ld ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(long int), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_long_long_int (timer *t, recorder *read_rec, recorder *write_rec, long long int value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(long long int));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(long long int));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_long_long_int (timer *t, recorder *read_rec, recorder *write_rec, long long int value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%lld ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(long long int), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%lld ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(long long int), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_float (timer *t, recorder *read_rec, recorder *write_rec, float value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(float));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(float));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_float (timer *t, recorder *read_rec, recorder *write_rec, float value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%f ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(float), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%f ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(float), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_double (timer *t, recorder *read_rec, recorder *write_rec, double value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(double));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(double));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_double (timer *t, recorder *read_rec, recorder *write_rec, double value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%lf ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(double), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%lf ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(double), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
/**
 * \brief Benchmark l'écriture et lecture en binaire d'un nombre
 *        de taille `size`
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 */
void bin_long_double (timer *t, recorder *read_rec, recorder *write_rec, long double value) {
  int err, i, len;

  rm(F);

  int fdout = open(F, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fdout == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  int fdin = open(F, O_RDONLY);
  if (fdin == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fsync(fdin);
  fsync(fdout);

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = write(fdout, (void *) &value, sizeof(long double));
    if (len == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdout);
  write_record(write_rec, len, stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    len = read(fdin, (void *) &value, sizeof(long double));
    if (len == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
  }
  fsync(fdin);
  write_record(read_rec, len, stop_timer(t));

  err = close(fdin);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }
  err = close(fdout);
  if (err == -1){
    perror("close");
    exit(EXIT_FAILURE);
  }

  rm(F);
}


/**
 * \brief Benchmark l'écriture et lecture en ascii d'un nombre
 *        de taille `size`
 *
 * Mesure les performances d'écriture, lecture de `N` nombre
 * en ascii avec `fprintf`, `fscanf`.
 *
 * \param t `timer` à utiliser pour mesurer le temps
 * \param read_rec `recorder` pour la lecture
 * \param write_rec `recorder` pour l'écriture
 * \param size taille du nombre à écrire
 * \param format format du nombre pour `printf`, `scanf`.
 *        Il vaut mieux ajouter un charactère de séparation également
 *        e.g. "%d "
 */
void text_long_double (timer *t, recorder *read_rec, recorder *write_rec, long double value) {
  int err, i;

  rm(F);

  FILE *fout = fopen(F, "w");
  if (fout == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  FILE *fin = fopen(F, "r");
  if (fin == NULL) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  fflush(fin);
  fflush(fout);

  /**
   * Pour être équitable avec `write` et `read`,
   * on désactive le buffer de `stdio`
   */
  setvbuf(fin, NULL, _IONBF, 0);
  setvbuf(fout, NULL, _IONBF, 0);

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fprintf(fout, "%Lf ", value);
    if (err < 0) {
      perror("fprintf");
      exit(EXIT_FAILURE);
    }
  }
  fflush(fout); // Flush data to the kernel
  int fdout = fileno(fout);
  if (fdout == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdout);
  write_record(write_rec, sizeof(long double), stop_timer(t));

  start_timer(t);
  for (i = 0; i < N; i++) {
    err = fscanf(fin, "%Lf ", &value);
    if (err < 0) {
      perror("fscanf");
      exit(EXIT_FAILURE);
    }
  }
  int fdin = fileno(fin);
  if (fdin == -1) {
    perror("fileno");
    exit(EXIT_FAILURE);
  }
  fsync(fdin); // Write data to the disk
  write_record(read_rec, sizeof(long double), stop_timer(t));

  err = fclose(fin);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  err = fclose(fout);
  if (err == EOF){
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  rm(F);
}
