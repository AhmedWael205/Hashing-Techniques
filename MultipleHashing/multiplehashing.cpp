#include "readfile.h"

int hashCode_1(int key){
   return key % MBUCKETS;
}

int hashCode_2(int key){
   return (PRIME - (key % PRIME)); 
}


int insertItem(int fd,DataItem item){
   int hashIndex = hashCode_1(item.key);
   int hashIndex2 = hashCode_2(item.key);
   int startingOffset = hashIndex*sizeof(Bucket);
   int Offset = startingOffset;
   int count = 0;
   int i = 0;
   struct Bucket data;



   	RESEEK:
     ssize_t result = pread(fd,&data,BUCKETSIZE, Offset);

    	count++;

        if(result <= 0)	{ return -1;} // perror("some error occurred in pread");
        else if (data.dataItem[0].valid == 0) {
        	result = pwrite(fd,&item,sizeof(DataItem), Offset);
            if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

        	return count;

        } else if (data.dataItem[1].valid == 0) {
        	result = pwrite(fd,&item,sizeof(DataItem), int(Offset+sizeof(DataItem)));
            if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

        	return count;

        } else { //not the record I am looking for
        		Offset =((hashIndex + i * hashIndex2) %  MBUCKETS)* sizeof(Bucket);  
        		i++;
                if(i >= MBUCKETS )
        		 { 
                     return -1;
        	     } 
            goto RESEEK;
        }

   return count;
}


int searchItem(int fd,struct DataItem* item,int *count)
{

	struct Bucket data;
	*count = 0;
	int i = 0;
	int hashIndex = hashCode_1(item->key); 
    int hashIndex2 = hashCode_2(item->key); 
	int startingOffset = hashIndex*sizeof(Bucket);
	int Offset = startingOffset;

	RESEEK:

	ssize_t result = pread(fd,&data,BUCKETSIZE, Offset);

	(*count)++;

    if(result <= 0) { return -1;  }
    else if (data.dataItem[0].valid == 1 && data.dataItem[0].key == item->key) {
    		
            item->data = data.dataItem[0].data ;
    		return Offset;
    } else if (data.dataItem[1].valid == 1 && data.dataItem[1].key == item->key) {
    		
            item->data = data.dataItem[1].data ;
    		return int(Offset + sizeof(DataItem));
    } else { 
    		Offset =((hashIndex + i * hashIndex2) %  MBUCKETS)* sizeof(Bucket);  
        	i++;
            if(i >= MBUCKETS )
        	{ 
                return -1;
        	} 
            goto RESEEK;
    }
}

int DisplayFile(int fd){

	struct DataItem data;
	int count = 0;
	int Offset = 0;
	for(Offset =0; Offset< FILESIZE;Offset += sizeof(DataItem))
	{
		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",int(Offset/BUCKETSIZE),int(Offset));
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",int(Offset/BUCKETSIZE),int(Offset),data.data,data.key);
					 count++;
		}
	}
	return count;
}


int deleteOffset(int fd, int Offset)
{
	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
	return result;
}