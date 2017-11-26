#ifndef DFT_H
#define DFT_H 

//Weigh op1 Key1  n1  op2 Key2  n2  op3 Key3  n3  op4 Key4  n4  op5 Key5  n5

//This struct will be used to store information by reading the trainset.in file
typedef struct dft 
{
float weight; 
char op1; 
char * key1; 
int n1; 
char op2; 
char * key2; 
int n2; 
float energy; 
}DFT; 

#endif
