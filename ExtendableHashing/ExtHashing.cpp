#include "readfile.h"
#include <cmath>

int* hashCode(int key)
{
    int hash = key % MBUCKETS;
    int * arr = new int [5];
    int temp = hash;

    for (int i = 4; i >= 0; i--)
    {
        arr[i] = temp % 2;
        temp /= 2;
    }

   return arr;
}


int insertItem(int fd,DataItem item)
{

    int* hashIndex = hashCode(item.key);

    int startingOffset =0;
    int Offset = startingOffset;
    int count = 0;
    struct Directory direc;
    int globaldepth = 0;

    ssize_t result = pread(fd,&globaldepth,sizeof(int), 0);
    count++;
    printf("here globaldepth== %d\n\n",globaldepth);
    printf("11\n\n\n");
    if(result <= 0)	{ printf("4");return -1;} // perror("some error occurred in pread");

    printf("10\n\n");
    if (globaldepth == 0) 
    {
        printf("here globaldepth == 0 \n\n");
        direc.globalDepth = 1;
        direc.Bucketptr = new Bucket * [2];
        direc.Bucketptr[0] = new Bucket;
        direc.Bucketptr[1] = new Bucket;

        direc.Bucketptr[0]->LocalDepth = 1;
        direc.Bucketptr[1]->LocalDepth = 1;
        direc.Bucketptr[0]->Offset = DIRECTORYSIZE + 0 * BUCKETSIZE;
        direc.Bucketptr[1]->Offset = DIRECTORYSIZE + 1 * BUCKETSIZE;
        result = pwrite(fd,&direc,sizeof(direc), 0);
    } 
    else 
    {
       result = pread(fd,&direc,DIRECTORYSIZE, 0);
       count++;
       
        if(result <= 0)	{ return -1;}
    }
    printf("5\n\n");

    int DecHash = 0;
    for (int i = 0; i < globaldepth; i++)
    {
        DecHash += Power(2,i)* hashIndex[i]; 
    }
    printf("DecHash = %d\n\n",DecHash);
    //printf("DecHash33%d\n\n",direc.Bucketptr[0]->Offset);
    startingOffset = 36;

    struct Bucket data;

    result = pread(fd,&data,BUCKETSIZE, startingOffset);
    count++;
    printf("1\n\n");
    if(result <= 0)	{ return -1;}
    
    if (data.dataItem[0].valid == 1 && data.dataItem[1].valid == 1){
        // Ya Ayah No empty space

        if (data.LocalDepth < globaldepth)
        {
            printf("here data.LocalDepth < globaldept \n\n");
            int* hash0 = hashCode(data.dataItem[0].key);
            int* hash1 = hashCode(data.dataItem[1].key);

            struct Bucket High;
            struct Bucket Low;

 

            // High.Offset = sizeof(int) + 2 ^ globaldepth + DecHash * BUCKETSIZE;
            High.LocalDepth = data.LocalDepth + 1;
            Low.LocalDepth = data.LocalDepth + 1;

            if (hash0[data.LocalDepth] == 0) 
            {
                High.dataItem[0].key = data.dataItem[0].key;
                High.dataItem[0].data = data.dataItem[0].data;
                High.dataItem[0].valid = data.dataItem[0].valid;
                High.Offset = DIRECTORYSIZE + BinToDec(hash0, High.LocalDepth) * BUCKETSIZE;
                
            }
            else
            {
                Low.dataItem[0].key = data.dataItem[0].key;
                Low.dataItem[0].data = data.dataItem[0].data ;
                Low.dataItem[0].valid = data.dataItem[0].valid ;
                Low.Offset = DIRECTORYSIZE + BinToDec(hash0, Low.LocalDepth) * BUCKETSIZE;
            }




            if (hash1[data.LocalDepth] == 0 && hash0[data.LocalDepth] == 0) 
            {
                High.dataItem[1].key = data.dataItem[1].key;
                High.dataItem[1].data = data.dataItem[1].data ;
                High.dataItem[1].valid = data.dataItem[1].valid ;
            }
            else if (hash1[data.LocalDepth] == 0 && hash0[data.LocalDepth] == 1) 
            {
                High.dataItem[0].key = data.dataItem[1].key;
                High.dataItem[0].data = data.dataItem[1].data;
                High.dataItem[0].valid = data.dataItem[1].valid;
                High.Offset = DIRECTORYSIZE + BinToDec(hash1, High.LocalDepth) * BUCKETSIZE;
            }
            else if (hash1[data.LocalDepth] == 1 && hash0[data.LocalDepth] == 0) 
            {
                Low.dataItem[0].key = data.dataItem[1].key;
                Low.dataItem[0].data = data.dataItem[1].data ;
                Low.dataItem[0].valid = data.dataItem[1].valid ;
                Low.Offset = DIRECTORYSIZE + BinToDec(hash1, Low.LocalDepth) * BUCKETSIZE;
            }
            else if (hash1[data.LocalDepth] == 1 && hash0[data.LocalDepth] == 1) 
            {
                Low.dataItem[1].key = data.dataItem[1].key;
                Low.dataItem[1].data = data.dataItem[1].data ;
                Low.dataItem[1].valid = data.dataItem[1].valid ;
            }

            bool Inserted = false;
            if (hashIndex[data.LocalDepth] == 0 && hash1[data.LocalDepth] + hash0[data.LocalDepth] == 1) 
            {
                High.dataItem[1].key = item.key;
                High.dataItem[1].data = item.data ;
                High.dataItem[1].valid = item.valid ;
                Inserted = true;
            }
            else if (hashIndex[data.LocalDepth] == 1 && hash1[data.LocalDepth] + hash0[data.LocalDepth] == 1) 
            {
                Low.dataItem[1].key = item.key;
                Low.dataItem[1].data = item.data ;
                Low.dataItem[1].valid = item.valid ;
                Inserted = true;
            }
            else if (hashIndex[data.LocalDepth] == 1 && hash1[data.LocalDepth] + hash0[data.LocalDepth] == 0) 
            {
                Low.dataItem[0].key = item.key;
                Low.dataItem[0].data = item.data ;
                Low.dataItem[0].valid = item.valid ;
                Low.Offset = DIRECTORYSIZE + BinToDec(hashIndex, Low.LocalDepth) * BUCKETSIZE;
                Inserted = true;
            }
            else if (hashIndex[data.LocalDepth] == 0 && hash1[data.LocalDepth] + hash0[data.LocalDepth] == 2) 
            {
                High.dataItem[0].key = item.key;
                High.dataItem[0].data = item.data ;
                High.dataItem[0].valid = item.valid ;
                High.Offset = DIRECTORYSIZE + BinToDec(hashIndex, High.LocalDepth) * BUCKETSIZE;
                Inserted = true;
            }
            else 
            {
                Inserted = false;
            }



            

        } 

        if (data.LocalDepth == globaldepth)
        {


        } else {return -1;}


    } 
    else 
    {
        printf("here else\n\n");
        if(data.dataItem[0].valid == 0) 
        {
            printf("here data.dataItem[0].valid \n\n");
            result = pwrite(fd,&item,sizeof(DataItem), startingOffset);
            return count;
        }
        else if (data.dataItem[1].valid != 1) 
        {
            printf("here data.dataItem[1].valid \n\n");
            result = pwrite(fd,&item,sizeof(DataItem), int (startingOffset+sizeof(DataItem)));
            return count;
        }
        else {return -1;}
    }

   	

   return count;
}


int searchItem(int fd,struct DataItem* item,int *count)
{

	//Definitions
	struct Bucket data;   //a variable to read in it the records from the db
	*count = 0;				//No of accessed records
	int* hashIndex = hashCode(item->key);  				//calculate the Bucket index



	struct Directory direc;
    int globaldepth = 0;

    ssize_t result = pread(fd,&globaldepth,sizeof(int), 0);
    count++;
    if(result <= 0)	{ return -1;} // perror("some error occurred in pread");

    if (globaldepth == 0)
    {
       return -1;
    }
    else
    {
        result = pread(fd,&direc,sizeof(int) + 2^globaldepth, 0);
        count++;
        if(result <= 0)	{ return -1;}
    }


    int DecHash = 0;
    for (int i = 0; i < globaldepth; i++)
    {
        DecHash += 2^i* hashIndex[i]; 
    }

    int Offset = direc.Bucketptr[DecHash]->Offset;

    result = pread(fd,&data,BUCKETSIZE, Offset);
    count++;
    if(result <= 0)	{ return -1;}
    else if (data.dataItem[0].valid == 1 && data.dataItem[0].key == item->key)
    {
    	//I found the needed record
    			item->data = data.dataItem[0].data ;
    			return Offset;

    }
    else if (data.dataItem[1].valid == 1 && data.dataItem[1].key == item->key)
    {
    	//I found the needed record
    			item->data = data.dataItem[1].data ;
    			return int(Offset + sizeof(DataItem));
    } 
    else {return -1;}
}

int  BinToDec (int * Arr , int size) {

    int DecHash = 0;
    for (int i = 0; i < size; i++)
    {
        DecHash += 2^i* Arr[i]; 
    }
    return DecHash;
}




int DisplayFile(int fd){

	struct Bucket data;
	int count = 0;
	int Offset = 0;
    int globalDepth = 0;

    ssize_t result = pread(fd,&globalDepth,sizeof(int), 0);
    if(result <= 0)	{ return -1;}

    printf("Global Depth = %d\n\n",globalDepth);

	for(Offset = DIRECTORYSIZE ; Offset< FILESIZE;Offset += BUCKETSIZE)
	{
		result = pread(fd,&data,BUCKETSIZE, Offset);
        count++;
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		}
        printf("Bucket: %d,LocalDepth  = %d   Offset = %d \n",int(Offset/BUCKETSIZE),data.LocalDepth,data.Offset);
        if (data.dataItem[0].valid == 0 ) 
        { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",int(Offset/BUCKETSIZE),int(Offset));
            
		}
        else 
        {
			//pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",int(Offset/BUCKETSIZE),int(Offset),data.dataItem[0].data,data.dataItem[0].key);
			//		 count++;
		}


        if (data.dataItem[1].valid == 0 ) 
        { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",int(Offset/BUCKETSIZE),int(Offset + sizeof(DataItem)));
            
		}
        else 
        {
			//pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",int(Offset/BUCKETSIZE),int(Offset),data.dataItem[1].data,data.dataItem[1].key);
			//		 count++;
		}
	}
	return count;
}


int Power(int x , int y){
    int sum = x;
    if (y == 0) return 1;
    for (int i = 0;i<y-1;i++)
    {
        sum *= x;

    }
    return sum;
}