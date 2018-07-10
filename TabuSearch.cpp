#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>
#include <conio.h>
#include<graphics.h>
#include<dos.h>
#include <time.h>
#include <windows.h>

using namespace std;

#define NumofCity 700
#define NumofCol 5
#define NumofRow 2
#define NumofClus NumofCol*NumofRow
#define TabuTenure 8
#define NONEIGHB 100
#define NOITERATION 1000
#define RUNNO 1


int originalcity[NumofCity][4];
int city[NumofCity][4];
/*int city[NumofCity][4]={{30,23,2000,0},
                   {17,29,2100,1},
                   {29,27,2100,2},
                   {12,43,3000,3},
                   {22,96,1500,4},
                   {107,35,1900,5},
                   {24,55,2400,6},
                   {32,60,1700,7}}; */

int cluster[NumofClus][NumofCity];
int Newcluster[NumofClus][NumofCity];             
int tempcluster[NumofClus][NumofCity], tempclusterOPT[NumofClus][NumofCity];
int RandomCluster[NumofClus][NumofCity], BestRandomCluster[NumofClus][NumofCity];
int center[NumofClus][4];
int TabuList[NumofCity][NumofCity];
float mean[NumofClus][2];

//-------------------------------------------
void ReadFile()
{
    int i=0;
    float x = 0, y = 0;
    ifstream cityfile("CitiesEnd.txt" , ifstream::in);
    string str = "";
    while( i< NumofCity)
    {
        cityfile >> city[i][3] >> str >> x >> y >> city[i][2];
        city[i][1] =  (int) (x * 100);
        city[i][0] =  (int) (y * 100);
        i++;
    }
    
    cityfile.close();
}

//-------------------------------------------
void InitializationRandom()
{
    int r,b;
    ReadFile();
    for(r=0;r<NumofCity;r++)
    {
             mean[r][0]=0;
             mean[r][1]=0;
    }
    
    for(r=0;r<NumofClus;r++)
    {
         for(b=0 ; b<NumofCity ; b++)
         {
           RandomCluster[r][b]=0; 
           BestRandomCluster[r][b]=0; 
         }
    }
               
    for(r=0 ; r<NumofCity ; r++)
    {
         
            originalcity[r][0]=city[r][0];
            originalcity[r][1]=city[r][1];
            originalcity[r][2]=city[r][2];
            originalcity[r][3]=city[r][3];  
           //    printf("%d\t",originalcity[r][0]);    
    }
}
//-----------------------------------------------------------
void Initialization()
{
    int r,b;
    ReadFile();
    for(r=0;r<NumofCity;r++)
    {
             mean[r][0]=0;
             mean[r][1]=0;
    }
    
    for(r=0;r<NumofClus;r++)
    {
         for(b=0 ; b<NumofCity ; b++)
         {
           cluster[r][b]=0; 
           Newcluster[r][b]=0; 
           tempcluster[r][b]=0; 
           tempclusterOPT[r][b]=0;
         }
    }
               
    for(r=0 ; r<NumofCity ; r++)
    {
            originalcity[r][0]=city[r][0];
            originalcity[r][1]=city[r][1];
            originalcity[r][2]=city[r][2];
            originalcity[r][3]=city[r][3];      
    }
}
//-------------------------------
int GetRand(int low, int high)
{
	return (rand() % (high + 1 - low)) + low;
}
//----------------------------
void SecondSort(int low, int high, int centerIdx)
{
     int i=0, j=0, x=0, y=0, t=0, r=0, a=0, clusterNo = 0, cityInRow = 0;
     // Then sort them descending    
     for(i=low ; i < high ; i++)
     {
         for(j=i+1 ; j < high; j++)
         {
             if(city[i][1] > city[j][1])
             {
             x=city[i][0];
             y=city[i][1];
             t=city[i][2];
             r=city[i][3];
             city[i][0]=city[j][0];
             city[i][1]=city[j][1];
             city[i][2]=city[j][2];
             city[i][3]=city[j][3];
             city[j][0]=x;
             city[j][1]=y;
             city[j][2]=t;
             city[j][3]=r;        
             }
         }
     }

     // Then set center of all these cities to that center     
     cityInRow = (int)ceil((float(high - low )) / NumofRow);
     for(clusterNo = centerIdx * NumofRow ; clusterNo < centerIdx * NumofRow + NumofRow ; clusterNo++)
     {
        for(a = clusterNo * cityInRow;  a < min((clusterNo+1) * cityInRow, high); a++)
             cluster[clusterNo][city[a][3]] = 1;
     }

}
//-----------------------------------
void InitialSort()
{
     int i=0,j=0,x=0,y=0,t=0,r=0,c=0,d=0,e=0,h=0,a=0,counter=0, cityInCluster = 0, low, high;
     // Then sort them descending    
     for(i=0 ; i < NumofCity ; i++)
     {
         for(j=i+1 ; j < NumofCity ; j++)
         {
             if(city[i][0] > city[j][0])
             {
             x=city[i][0];
             y=city[i][1];
             t=city[i][2];
             r=city[i][3];
             city[i][0]=city[j][0];
             city[i][1]=city[j][1];
             city[i][2]=city[j][2];
             city[i][3]=city[j][3];
             city[j][0]=x;
             city[j][1]=y;
             city[j][2]=t;
             city[j][3]=r;        
             }
         }
     }
     
     cityInCluster = (int)ceil((float)NumofCity/NumofCol);
     i = 0;
     while(counter < NumofCol)
     {
      low=i;
      a = low + cityInCluster;
      high= min(a, NumofCity);
      SecondSort(low, high, counter);
  
      i += cityInCluster;
      e++;
      counter++;
     }
     
     // Copy cluster to Newcluster
     for(i=0 ; i < NumofClus; i++)
         for(j=0 ; j < NumofCity ; j++)
             tempclusterOPT[i][j] = Newcluster[i][j] = cluster[i][j];
}  
//-----------------------------------------------------        

//----------------------------------------------------------
void ComputeMean()
{
     int i = 0, c = 0, sumX=0, sumY=0, nCity = 0;
     //mean of x
     for(c = 0 ; c < NumofClus ; c++)
     {
         nCity = 0;
         sumX = 0;
         sumY = 0;
         for(i=0 ; i < NumofCity ; i++)
         {
             if(cluster[c][i] == 1)
             {
                nCity++;
                sumX += originalcity[i][0];
                sumY += originalcity[i][1];
             }
         }
         
         mean[c][0]= (float)sumX/nCity;
         mean[c][1]= (float)sumY/nCity;
     }
}

//-------------------------------------------------------------------------- 
float Calculate()
{
    int i = 0, c = 0;
    float distance=0;
   
    ComputeMean();
    for(c=0 ; c<NumofClus;c++)
    {
       for(i=0 ; i<NumofCity ; i++)
       {
            if(cluster[c][i] == 1)
                distance += (pow(originalcity[i][0]-mean[c][0], 2) + pow(originalcity[i][1]-mean[c][1], 2));
       }
       
   //    distance += (pow(center[c][0]-mean[c][0], 2) + pow(center[c][1]-mean[c][1], 2));
     } 

    return distance;
}

//--------------------------------------------------------
float CalculateTemp()
{
    int i = 0, c = 0;
    float distance=0;
    
    ComputeMean();
    for(c=0 ; c<NumofClus;c++)
    {
       for(i=0 ; i<NumofCity ; i++)
       {
            if(tempcluster[c][i] == 1)
                distance += (pow(originalcity[i][0]-mean[c][0], 2) + pow(originalcity[i][1]-mean[c][1], 2));
       }
       
     //  distance += (pow(center[c][0]-mean[c][0], 2) + pow(center[c][1]-mean[c][1], 2));
     } 

    return distance;
}
//---------------------------------
void ComputeRandomMean()
{
     int i = 0, c = 0, sumX=0, sumY=0, nCity = 0;
     //mean of x
     for(c = 0 ; c < NumofClus ; c++)
     {
         nCity = 0;
         sumX = 0;
         sumY = 0;
         for(i=0 ; i < NumofCity ; i++)
         {
             if(RandomCluster[c][i] == 1)
             {
                nCity++;
                sumX += originalcity[i][0];
                sumY += originalcity[i][1];
             }
         }
         
         mean[c][0]= (float)sumX/nCity;
         mean[c][1]= (float)sumY/nCity;
     }
}
//---------------------------------
float CalculateRandom()
{
    int i = 0, c = 0;
    float Rdistance=0;
    
    ComputeRandomMean();
    for(c=0 ; c<NumofClus;c++)
    {
       for(i=0 ; i<NumofCity ; i++)
       {
            if(RandomCluster[c][i] == 1)
                Rdistance += (pow(originalcity[i][0]-mean[c][0], 2) + pow(originalcity[i][1]-mean[c][1], 2));
       }
    } 

    return Rdistance;
}            
//-----------------------------------
void swap(int m, int n, int r1 , int r2)
{
    int x;
	for (int g = 0; g < NumofClus ; g++)
	    for(int f=0 ; f < NumofCity ; f++)
     		tempcluster[g][f] = Newcluster[g][f];
     
     x = tempcluster[m][r1];
     tempcluster[m][r1] = tempcluster[n][r1];
     tempcluster[n][r1] = x;
     
     x = tempcluster[m][r2];
     tempcluster[m][r2] = tempcluster[n][r2];
     tempcluster[n][r2] = x;
}

//---------------------------------
void GetNewNeighbor(int *z, int *y)
{
     int x;
     int m=GetRand(0, NumofClus-1);
     int n=GetRand(0, NumofClus-1);
     
     while(m == n)
        n=GetRand(0,NumofClus-1);
     
     int r1=GetRand(0, NumofCity - 1);
     int r2=GetRand(0, NumofCity - 1);
     while(r1 == r2)
        r2=GetRand(0, NumofCity - 1);
	swap(m, n, r1, r2);
	*z = r1;
	*y = r2;
}
//--------------------------------
void MakeRandomCluster()
{
     int R=0;
     for(int g=0;g<NumofClus;g++)
             for(int d=0;d<NumofCity;d++)
                    RandomCluster[g][d]=0;   
     for(int j=0; j<NumofCity ; j++)
     {
             R=GetRand(0,NumofClus-1);
             RandomCluster[R][j]=1;
     }
 /*    for(int i=0;i<NumofClus;i++)
     {
             printf("\n\n");
             for(int f=0; f<NumofCity; f++)
                     printf("%d\t",RandomCluster[i][f]); 
     } */
}
//------------------------------------                  
int mainFunc(int run, ofstream& outf)
{
   	int r1 = 0, r2 = 0, tempR1 = 0, tempR2 = 0, i, j;
    int Count=0, iterationcount=0, NonImprovedNum =0,maxIndx=0;
    float initialcost = 0, tempcost=0, newcost=0, optimalcost , bestcost;
    double RandomDistance=0;
    double best = 1000000000;
    int NumOfNonIMP=0;

    system("CLS");
    srand(time(NULL));

    clock_t start, end;
    double cpu_time_used;
    start = clock();
      
    Initialization();
    InitialSort();
    initialcost = Calculate();
    newcost= bestcost= initialcost;
 //   outf << endl << "Run : " << run << endl;
  //  outf << "Initial clustering distance" << initialcost/10000 << endl;
  printf("initial %f",initialcost/10000);

    optimalcost= initialcost;
    

    while(NumOfNonIMP < 4000)// Count++ < NOITERATION)
    {
         iterationcount = 0;
         while (iterationcount++ < NONEIGHB)//NonImprovedNum <1000)            
		 {
             GetNewNeighbor(&r1 , &r2);
             tempcost = CalculateTemp();
             if (tempcost < newcost)
        	 {
                if (TabuList[r1][r2] == 0)
        		{
        			newcost = tempcost;
       			    tempR1 = r1;
        			tempR2 = r2;
        			for (i = 0; i < NumofClus ; i++)
                        for(int j=0 ; j < NumofCity ; j++)
                       	    tempclusterOPT[i][j] = tempcluster[i][j];
   		     	}
   		     	else if(tempcost < bestcost)
   		     	 {
                    bestcost = tempcost; 
                    newcost = tempcost;
       			    tempR1 = r1;
        			tempR2 = r2;
        			for (i = 0; i < NumofClus ; i++)
                        for(int j=0 ; j < NumofCity ; j++)
                       	    tempclusterOPT[i][j] = tempcluster[i][j];   
                 }    
             }else
                  NonImprovedNum++;
        	if(tempcost < bestcost)
                    bestcost = tempcost; 
          }
         if(newcost >= bestcost)
                     NumOfNonIMP++;  
          for(i = 1 ; i < NumofCity ; i++)
			   for (j = 1 ; j < NumofCity ; j++)
			   {
				  if (TabuList[i][j] > 0)
					TabuList[i][j]--;
			   }
			    
          TabuList[tempR1][tempR2] = TabuTenure;
          for(i = 0; i < NumofClus ; i++)
              for(j = 0; j < NumofCity; j++)
                   	Newcluster[i][j] = tempclusterOPT[i][j];
          	optimalcost=newcost;
   //   printf("\nnew clustering distance %f ",optimalcost);
  //  printf("\ntempcost = %f\tnewcost=%f", tempcost, newcost);
    }
  /*    printf("\nOriginal city network\n");
    printf("City Code\tX axis\tY axis\tPopulation");
    for(i=0;i<NumofCity;i++)
        printf("\n%d\t\t%d\t%d\t%d",originalcity[i][3],originalcity[i][0],originalcity[i][1],originalcity[i][2]);
    
    printf("\n\nInitial Clustering was\n");
    for(i=0;i<NumofClus;i++) 
    { 
          printf("\n\nThe %d th Cluster is %d ,  ", i+1, center[i][3]);
          for(j=0;j<NumofCity;j++)
               if(cluster[i][j]==1)
                       printf("%d , ",j);
    }
*/                          
   // outf << "Optimal Clustering is" << endl;   
    bool first = true;     
    for(i=0;i<NumofClus;i++)
    {
          first = true;     
          printf("\n\n\nThe %d th Cluster is  ,  ", i+1);
          if(tempclusterOPT[i][0]==1)
               printf("%d , ",0);
          
          for(j=1;j<NumofCity;j++)
               if(tempclusterOPT[i][j]==1)
               {
                    if(first)
                    {
                        first = false;
                        maxIndx = j;
                    }
                    
                    printf("%d , ", j);
                    if(originalcity[j][2]>originalcity[maxIndx][2])
                          maxIndx = j;
               }     
          printf("\n\n%d is the best center of class considering population\n", maxIndx); 
    } 
  //  outf << "\nnew clustering distance " << optimalcost/10000; 
       outf << optimalcost/10000; 
    
    int driver, mode;
    driver = DETECT;
    initgraph(&driver, &mode, "");
    
    for(i=0;i<NumofClus;i++)
    {     
          setcolor(i%15+1);
          for(j=0;j<NumofCity;j++)
          {
               if(tempclusterOPT[i][j]==1)
               {
               for(int k = 0 ; k <5 ; k++)
                       circle(originalcity[j][0]%5000,originalcity[j][1]%400,k);
               }
          } 
    }    
    //////////////////////////////////////////////////////////////////////
 /*   //RANDOM GENERATION PART
   InitializationRandom();
    MakeRandomCluster();
    for(int i=0; i<10000;i++)
    {
            MakeRandomCluster();
            RandomDistance = CalculateRandom();
     //       printf("%lf",RandomDistance);
            if(RandomDistance <best)
            {
                  best=RandomDistance;
                  for(int k=0; k<NumofClus;k++)
                          for(int h=0; h< NumofCity; h++)
                                  BestRandomCluster[k][h]= RandomCluster[k][h];
            }
    }
          outf << best/10000; 
   // printf("\n\noptimal distance for Random clustering was %lf\n", best/10000); 
    
    /////////////////////////////////////////////////////End of Random Part */
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
   // outf << "execution time" << cpu_time_used << endl; 
        outf <<"\t"<< cpu_time_used << endl; 
  //  getch();
   // return EXIT_SUCCESS;
}

//------------------------------------                  
int main()
{
    ofstream outfile;
    outfile.open("result.txt");
    for(int run = 0 ; run < RUNNO ; run++)
    {
            mainFunc(run + 1, outfile);
    }
    outfile.close();
}
