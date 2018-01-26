#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0
using namespace std;
int main (int argc, char *argv[])
{
    int numtasks, taskid, rc, dest, offset, i, j, tag1, tag2, source, chunksize; 
    double mysum=0, sum=0;
    
    double update(double x,int offset, int chunk, int myid, int n,double value, int proc);
    
    MPI_Status status;

    /***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    printf ("MPI task %d has started...\n", taskid);
    char* arg_size = argv[2];
    char* arg_x = argv[1];
    double x = atof(arg_x);
    int size = atoi(arg_size);
    //cout<<'m'<<x<<' '<<size<<'\n';
    chunksize = (size / numtasks);
    tag2 = 1;
    tag1 = 2;
    double common_value = 1;
    double u=1;
    for(double i=1;i<=numtasks;i++)
    {
        	common_value*=(x/u++);
    }
    sum = 0;
    /***** Master task only ******/
    if (taskid == MASTER)
    {
        /* Initialize the array */
        sum = 0;
        cout << "Initialized Sum " << sum << endl;

        /* Send each task its portion of the array - master keeps 1st part */
        offset = chunksize;
        for (dest = 1; dest < numtasks; dest++) 
        {
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
        }

        /* Master does its part of the work */
        offset = 0;
        mysum = update(x,taskid, numtasks, taskid, size,common_value, numtasks);


        /* Get Final Sum*/  
        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);        
        cout << "Final Recieved Sum: " << sum << endl;

    }  /* end of master section */



    /***** Non-master tasks only *****/
    if (taskid > MASTER)
    {
        /* Receive my portion of array from the master task */
        source = MASTER;
        MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		mysum = update(x,taskid, numtasks, taskid, size,common_value, numtasks);

        MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
   		//cout << "Final Recieved Sum: " << sum << endl;

    } /* end of non-master */


    MPI_Finalize();

}/* end of main */

double value(double x, double offset,double val, int proc)
{
	double ans=1;
	//cout<<'t'<<x<<' '<<val<<' '<<offset<<' '<<proc<<'\n';
	
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
	//cout<<'p'<<x<<offset<<' '<<ans<<'\n';
	}
	return ans*val;
}
double update(double x,int offset, int chunk, int myid, int n, double val, int proc)
{
    int i; 
    double mysum;
    
    mysum = 0;
    //cout<<"adsda"<<offset<<' '<<chunk<<'\n';
    for(i = offset; i < n ; i+=chunk)
    {
    //	cout<<x<<" -> "<<i<<'\n';
    	if(i == 0)
    		mysum += 1;
    	else if(i % 2 == 1)
    	{
    		mysum = mysum - value(x, i*2, val, proc);   
    	}
    	else
    	{
    		mysum = mysum + value(x, i*2, val, proc);  	
    	}
    	//cout<<x<<' '<<i<<' '<<value(x,i*2)<<' '<<mysum <<'\n';
    }
    
    cout << "Task: " << myid << " " << "Sum: " << mysum << endl;
    return(mysum);
}


