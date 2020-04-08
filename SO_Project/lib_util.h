//
//  lib_util.h
//  SO_Project
//
//  Created by João  Sousa on 26/03/2020.
//  Copyright © 2020 João  Sousa. All rights reserved.
//

#ifndef lib_util_h
#define lib_util_h

#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#if defined _WIN64 || defined _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

void mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int readInts(char * filename, int * intvec, int nmax);
int gettimeuseconds(long long * time_usec);
int uniform(int val_min, int val_max);
void printArray(int* a, int N);
int * newIntArray(int N); // alloc new int arrayvoid freeIntArray(int * v); // frees new int array
int uniformArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed values (returns -1 if error)
int uniformDistinctArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed distinct values (returns -1 if error)
int binarySearch(int a[], int n, int key);
int writeInts(char * filename, int * intvec, int n);


#endif /* lib_util_h */
