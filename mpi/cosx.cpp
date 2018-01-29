/***********************************/
/*                                 */
/* mpi_7.cpp                       */
/* Distributed cos(x)              */
/* Given x and n, calculate taylor */
/* expansion of cos(x) till        */
/* first n terms.                  */
/*                                 */
/***********************************/
#include "mpi.h"
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#define MASTER 0
using namespace std;
/**
 * Main function that starts off the process
 * Takes input <x> <n>
 * Outputs cos(x) upto 2 decimal points accurate upto n terms
 */
int main (int argc, char *argv[])
{
    int numtasks, taskid, dest, offset, i, j, tag1, tag2, source, chunksize; 
    double mysum=0, sum=0;
    
    double update(double x, int chunk, int myid, int n,double value);
    
    MPI_Status status;

    /***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    //printf ("MPI task %d has started...\n", taskid);
    //char* arg_size = argv[2];
    //char* arg_x = argv[1];
    sum = 0;
    double x;
    int size;
    double common_value = 1;
    tag2 = 1;
    tag1 = 2;
    /***** Master task only ******/
    if (taskid == MASTER)
    {
        /**
         * Input values 
         * x: cosine value parameter
         * size: upto n termns
         */
        cin>>x>>size;
        chunksize = (size / numtasks);
        double u=1;
        for(double i=1;i<=numtasks;i++)
        {
                common_value*=(x/u++);
        }
        /**
         * Broadcasting input values and common_value to all processors
         */
        MPI_Bcast(&x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&common_value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        /* Initialize the array */
        sum = 0;
        //cout << "Initialized Sum " << sum << endl;

        /* Send each task its portion of the array - master keeps 1st part */
        offset = chunksize;
        for (dest = 1; dest < numtasks; dest++) 
        {
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            //printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
        }

        /* Master does its part of the work */
        offset = 0;
        mysum = update(x,numtasks, taskid, size,common_value);


        /* Get Final Sum*/  
        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);        
        cout << setprecision(2)<<fixed<< sum << endl;

    }  /* end of master section */



    /***** Non-master tasks only *****/
    if (taskid > MASTER)
    {
        MPI_Bcast(&x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&common_value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        /* Receive my portion of array from the master task */
        source = MASTER;
        MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		mysum = update(x, numtasks, taskid, size,common_value);

        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
   		//cout << "Final Recieved Sum: " << sum << endl;

    } /* end of non-master */


    MPI_Finalize();

}/* end of main */
/**
 * value function
 * calculate 
 */
/**
 * Calculate x^offset/offset!
 * consider the case of number of processor greater than offset
 */
double value(double x, double offset,double val, int proc)
{
	double ans=1;
	
	double y=1;
	int j=0;
	if(offset+1 > proc)
	{
		j=proc;
		y=proc+1;
	}
	else
		val=1;
	for(int i=j;i < offset; i++)
	{
		ans *= x/(y++);
	}
	return ans*val;
}
/**
 * Update function
 * ith processor calculate summation{i=i+p} of x^i/i!
 * Each processor find its individual sum
 * x: input x: cos(x)
 * nump: number of processors 
 * myid: id of processor
 * n: upto n terms
 * val: common value shared among processors
 */

double update(double x, int nump, int myid, int n, double val)
{
    int i; 
    double mysum;
    
    mysum = 0;
    //cout << "Task: " << myid << " " << "Sum: " << mysum << endl;
    for(i = myid; i < n ; i+=nump)
    {
    	if(i == 0)
    		mysum += 1;
    	else if(i % 2 == 1)
    	{
    		mysum = mysum - value(x, i*2, val, nump);   
    	}
    	else
    	{
    		mysum = mysum + value(x, i*2, val, nump);  	
    	}
    }
    
    //cout << "Task: " << myid << " " << "Sum: " << mysum << endl;
    return(mysum);
}


