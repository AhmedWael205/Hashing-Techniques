//============================================================================
// Name        : hashskeleton.cpp
// Author      :
// Version     :
// Copyright   : Code adapted From https://www.tutorialspoint.com/
// Description : Hashing using open addressing
//============================================================================

#include "readfile.h"

int insert(int key,int data);
int deleteItem(int key);
struct DataItem * search(int key);


int filehandle;   //handler for the database file
int SearchCount = 0;
int DeleteCount = 0;

/* DBMS (DataBase Management System) needs to store its data in something non-volatile
 * so it stores its data into files (manteqy :)).

 * Some DBMS or even file-systems constraints the size of that file.

 * for the efficiency of storing and retrieval, DBMS uses hashing
 * as hashing make it very easy to store and retrieve, it introduces
 * another type of problem which is handling conflicts when two items
 * have the same hash index

 * In this exercise we will store our database into a file and experience
 * how to retrieve, store, update and delete the data into this file

 * This file has a constant capacity and uses external-hashing to store records,
 * however, it suffers from hashing conflicts.
 *
 * You are required to help us resolve the hashing conflicts

 * For simplification, consider the database has only one table
 * which has two columns key and data (both are int)

 * Functions in this file are just wrapper functions, the actual functions are in openAddressing.cpp

*/

int main(){

//here we create a sample test to read and write to our database file

  //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE,(char *)"chainning");
  //2. Display the database file, check openAddressing.cpp

  printf("The File before insertion\n");
   DisplayFile(filehandle);

     printf("\n\nInserting to the file:\n");    
  //3. Add some data in the table
  int InsertCount = 0;
  float InsertAvg;

   InsertCount += insert(1, 20);
   InsertCount += insert(2, 70);
   InsertCount += insert(42, 80);
   InsertCount += insert(4, 25);
   InsertCount += insert(12, 44);
   InsertCount += insert(14, 32);
   InsertCount += insert(17, 11);
   InsertCount += insert(13, 78);
   InsertCount += insert(37, 97);
   InsertCount += insert(11, 34);
   InsertCount += insert(22, 730);
   InsertCount += insert(46, 840);
   InsertCount += insert(9, 83);
   InsertCount += insert(21, 424);
   InsertCount += insert(41, 115);
   InsertCount += insert(71, 47);
   InsertCount += insert(31, 92);
   InsertCount += insert(73, 45);

   InsertAvg = InsertCount / 18.0;

   //4. Display the database file again
   printf("\n\nThe File after insertion\n");
   DisplayFile(filehandle);
   printf("\n\n");
   //5. Search the database
   
   float SearchAvg;

   search(13);
   search(41);
   search(73);
   search(12);
   search(1);

   SearchAvg = SearchCount/5.0;

   printf("\n\n");
   //6. delete an item from the database
   float DeleteAvg;

   deleteItem(31);
   deleteItem(22);
   deleteItem(42);
   deleteItem(21);
   deleteItem(17);

   DeleteAvg = DeleteCount/5.0;

   //7. Display the final data base
   printf("\n\nThe final file\n");
   DisplayFile(filehandle);
   printf("\n\n");
   // And Finally don't forget to close the file.
   close(filehandle);
   printf("Insertion Average = %f\nSearch Average = %f\nDelete Average = %f\n",InsertAvg,SearchAvg,DeleteAvg);
   return 0;



}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: print statement with the no. of comparisons
*/
int insert(int key,int data){
     struct DataItem item ;
     item.data = data;
     item.key = key;
     item.valid = 1;
     int result= insertItem(filehandle,item);  //TODO: implement this function in openAddressing.cpp
     printf("Insert: No. of searched Buckets:%d\n",abs(result));
     return result;
}

/* Functionality: search for a data in the table using the key

   Input:  key
   Output: the return data Item
*/
struct DataItem * search(int key)
{
  struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
     item->key = key;
     int diff = 0;
     int Offset= searchItem(filehandle,item,&diff); //this function is implemented for you in openAddressing.cpp
     SearchCount += diff;
     printf("Search: No of Buckets searched is %d\n",diff);
     if(Offset <0)  //If offset is negative then the key doesn't exists in the table
       printf("Item not found\n");
     else
        printf("Item found at Offset: %d,  Data: %d and key: %d\n",Offset,item->data,item->key);
  return item;
}

/* Functionality: delete a record with a certain key

   Input:  key
   Output: return 1 on success and -1 on failure
*/
int deleteItem(int key){
   struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
   item->key = key;
   int diff = 0;
   int Offset= searchItem(filehandle,item,&diff);
   DeleteCount += (diff);
   printf("Delete: No of Buckets searched is %d\n",diff);
   if(Offset >=0 )
   {
    DeleteCount += deleteOffset(filehandle,Offset);
    return 1;
   }
   return -1;
}
