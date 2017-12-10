#include <stdio.h>
#include <stdlib.h>

double grabEnergy (char *fileName);
int main () {
   
   
   char fileName[1000] ="out1.txt" ;
   double f = grabEnergy (fileName);
   printf("Read enegry: |%lf|\n", f );			
   
   return(0);
}

double grabEnergy (char *fileName)
{
	FILE * fp;
	char str1[1000];
	fp = fopen (fileName, "r");
	double f1 = 0.0;
   while (fscanf(fp, " %s", str1) == 1)
   {
	if (strcmp("Energy",str1) ==0)
	{
		printf("Read String1 |%s|\n", str1 );	
		int i = 0;
		//skips bunch of string
		for (i = 0; i < 4 ; i++)
		{
			fscanf(fp, " %s", str1);
		}
		
		fscanf(fp, " %lf", &f1);//frist energy
		fscanf(fp, " %lf", &f1);//second energy
		fscanf(fp, " %lf", &f1);//third energy
		
		break;
	}
		
   }
	

   fclose(fp);
   return f1;
}