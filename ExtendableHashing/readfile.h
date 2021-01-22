/*
 * readfile.h
 *
 *  Created on: Sep 20, 2016
 *      Author: dina
 */

#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define MBUCKETS  32					//Number of BUCKETS 2 ** 5
#define RECORDSPERBUCKET 2				//No. of records inside each Bucket
#define BUCKETSIZE sizeof(Bucket)		//Size of the bucket (in bytes)


#define DIRECTORYSIZE sizeof(Directory)
#define FILESIZE BUCKETSIZE*MBUCKETS + DIRECTORYSIZE //Size of the file

//Data Record inside the file
struct DataItem {
   int valid;    //0 means invalid record, 1 = valid record
   int data;
   int key;
};


//Each bucket contains number of records
struct Bucket {
	struct DataItem  dataItem[RECORDSPERBUCKET];
    int Offset = 0;
    int LocalDepth = 0;
};

struct Directory {
    int globalDepth = 0;
    int BucketOffset [32];   
};


//Check the create File
int createFile(int size, char *);

//check the openAddressing File
int deleteItem(int key);
int insertItem(int fd,DataItem item);
int DisplayFile(int fd);
int deleteOffset(int filehandle, int Offset);
int  BinToDec (int * Arr , int size);
int Power(int x , int y);
int searchItem(int filehandle,struct DataItem* item,int *count);




#endif /* READFILE_H_ */
