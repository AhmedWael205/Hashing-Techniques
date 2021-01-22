#include "readfile.h"

int hashCode(int key){
   return key % MBUCKETS;
}



int insertItem(int fd,DataItem item){
   int hashIndex = hashCode(item.key);
   int startingOffset = hashIndex*sizeof(Bucket);
   int Offset = startingOffset;
   int count = 0;
   struct Bucket data;


  ssize_t result = pread(fd,&data,sizeof(Bucket), Offset);

    count++;

      if(result <= 0)	{ return -1;} // perror("some error occurred in pread");
      else if (data.dataItem[0].valid == 0) {
        // The place is empty
        // Write to this place
        data.dataItem[0].valid = item.valid;
        data.dataItem[0].key = item.key;
        data.dataItem[0].data = item.data;
        result = pwrite(fd,&data,sizeof(Bucket), Offset);
        if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

      return count;

      } else if (data.dataItem[1].valid == 0) {
        // The place is empty
        // Write to this place
        data.dataItem[1].valid = item.valid;
        data.dataItem[1].key = item.key;
        data.dataItem[1].data = item.data;

      result = pwrite(fd,&data,sizeof(Bucket), Offset);
      if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

      return count;
      } else { //not the record I am looking for
        struct OverFlowRecord data2;
        int Offset2;

        Offset2 = MBUCKETS * sizeof(Bucket);
        for (int i = Offset2; i < FILESIZE; i += sizeof(OverFlowRecord))
        {
          count++;
          result = pread(fd,&data2,sizeof(OverFlowRecord), i);
          if(result <= 0)	{ return -1;} 

          if (data2.dataItem.valid != 1)
          {
            data2.dataItem.data=item.data;
            data2.dataItem.key=item.key;
            data2.dataItem.valid=item.valid;
            data2.NextOffset = 0;

            result = pwrite(fd,&data2,sizeof(OverFlowRecord), i);
            if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

            if (data.OverflowOffset == 0) { 
              data.OverflowOffset = i;
              result = pwrite(fd,&data,sizeof(Bucket), Offset);
              if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");
            } else {
              
              struct OverFlowRecord dummy;
              result = pread(fd,&dummy,sizeof(OverFlowRecord), data.OverflowOffset);
              int currentOffset = data.OverflowOffset;
              int nextOffset = dummy.NextOffset;

              while (nextOffset != 0) {
                count++;
                result = pread(fd,&dummy,sizeof(OverFlowRecord), nextOffset);
                if(result <= 0)	{ return -1;}
                currentOffset = nextOffset;
                nextOffset = dummy.NextOffset;
              }

              dummy.NextOffset = i;
              result = pwrite(fd,&dummy,sizeof(OverFlowRecord), currentOffset);
              if(result <= 0)	{ return -1;} // perror("some error occurred in pwrite");

            }

            return count;
          }
        
        } // END FOR LOOP
        
      }

  return count;
}



int searchItem(int fd,struct DataItem* item,int *count)
{


	struct Bucket data;   //a variable to read in it the records from the db
	*count = 0;				//No of accessed records
	int hashIndex = hashCode(item->key);  				//calculate the Bucket index
	int startingOffset = hashIndex*sizeof(Bucket);		//calculate the starting address of the bucket
	int Offset = startingOffset;						//Offset variable which we will use to iterate on the db


	ssize_t result = pread(fd,&data,sizeof(Bucket), Offset);

	(*count)++;

    if(result <= 0) { return -1; }
    else if (data.dataItem[0].valid == 1 && data.dataItem[0].key == item->key) {
    	//I found the needed record
    			item->data = data.dataItem[0].data ;
    			return Offset;

    } else if (data.dataItem[1].valid == 1 && data.dataItem[1].key == item->key) {
    	//I found the needed record
    			item->data = data.dataItem[1].data ;
    			return Offset+ sizeof(DataItem);

    } else if (data.OverflowOffset == 0) {
    	return -1;
    } else {
      int nextOffset = data.OverflowOffset;
      int currentOffset;
      struct OverFlowRecord dummy;

      while (nextOffset != 0) {
        (*count)++;
        result = pread(fd,&dummy,sizeof(OverFlowRecord), nextOffset);
        if(result <= 0)	{ return -1;}
        currentOffset = nextOffset;
        nextOffset = dummy.NextOffset;
      }
      item->data = dummy.dataItem.data ;
      return currentOffset;
    }
    return -1;
}


/* Functionality: Display all the file contents
 *
 * Input:  fd: filehandler which contains the db
 *
 * Output: no. of non-empty records
 */
int DisplayFile(int fd){

	struct Bucket data;
  struct OverFlowRecord data2;
	int count = 0;
	int Offset = 0;
	for(Offset =0; Offset< (MBUCKETS - 1) * BUCKETSIZE;Offset += BUCKETSIZE)
	{
		ssize_t result = pread(fd,&data,BUCKETSIZE, Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} 

    if (result == 0 ||  data.dataItem[0].valid == 0 ) { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",int(Offset/BUCKETSIZE),int(Offset));
		} else {
			  printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",int(Offset/BUCKETSIZE),int(Offset),data.dataItem[0].data,data.dataItem[0].key);
				count++;
		}

    if (result == 0 ||  data.dataItem[1].valid == 0 ) { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",int(Offset/BUCKETSIZE),int(int(Offset) + sizeof(DataItem)));
		} else {
			  printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",int(Offset/BUCKETSIZE),int(int(Offset)+ sizeof(DataItem)),data.dataItem[1].data,data.dataItem[1].key);
				count++;
		}
	}
  int j = 0;
  for(Offset = MBUCKETS*BUCKETSIZE; Offset < FILESIZE;Offset += OF_RECORD_SIZE)
	{
    ssize_t result = pread(fd,&data2,OF_RECORD_SIZE, Offset);
    if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} 

    if (result == 0 ||  data2.dataItem.valid == 0 ) { //empty space found or end of file
			printf("Overflow Bucket: %d, Offset %d:~\n",j,int(Offset));
		} else {
			printf("Overflow Bucket: %d, Offset: %d, Data: %d, key: %d\n",j,int(Offset),data2.dataItem.data,data2.dataItem.key);
			count++;
		}
    j++;
  }
	return count;
}


/* Functionality: Delete item at certain offset
 *
 * Input:  fd: filehandler which contains the db
 *         Offset: place where it should delete
 *
 * Hint: you could only set the valid key and write just and integer instead of the whole record
 */
int deleteOffset(int fd, int Offset)
{
  int count = 0;
  if (Offset < MBUCKETS * BUCKETSIZE)
  {
    struct DataItem dummyItem;
    dummyItem.valid = 0;
    dummyItem.key = -1;
    dummyItem.data = 0;
    int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
    return 0;
  } else {
    struct OverFlowRecord dummy;
    int result = pread(fd,&dummy,OF_RECORD_SIZE, Offset);
    count++;
    if(result < 0)	{ return -1;}


    struct OverFlowRecord dummy2;

    result = pread(fd,&dummy2,OF_RECORD_SIZE, dummy.NextOffset);
    count++;
    if(result < 0)	{ return -1;}

    int nextOffset =  dummy.NextOffset;

    dummy.dataItem.key = dummy2.dataItem.key;
    dummy.dataItem.valid = dummy2.dataItem.valid;
    dummy.dataItem.data = dummy2.dataItem.data;
    dummy.NextOffset = dummy2.NextOffset;

    dummy2.dataItem.key = -1;
    dummy2.dataItem.data = 0;
    dummy2.dataItem.valid = 0;

    result = pwrite(fd,&dummy,sizeof(DataItem), Offset);
    result = pwrite(fd,&dummy2,sizeof(DataItem), nextOffset);
    return count;    
  }
  return count;
}
