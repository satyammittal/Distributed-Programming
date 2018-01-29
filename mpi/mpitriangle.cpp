/***********************************/
/*                                 */
/* mpi_12.cpp                      */
/* Distributed graph triangles     */
/* Given an undirected graph G,    */
/* output the total number of      */
/* triangles in G.                 */
/*                                 */
/***********************************/
#include <iostream>
#include <mpi.h>
using namespace std;
#define MASTER 0
#define FROM_WORKER 2

/**
 * Main function
 * Approach: Trace((Delta)^3)/6
 * input would be 2 integers, n and m followed by m lines of edges.
 * Output: Number of triangles
 */
int main (int argc, char *argv[])
{
    int numtasks, taskid;
    MPI_Status status;
    int a[100][100];
    /***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    printf ("MPI task %d has started...\n", taskid);
    int n,e;
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            a[i][j]=0;

    if(taskid == MASTER)
    {
        cin>>n>>e;
        
        for(int i=0;i<e;i++)
        {
            int v0,vf;
            cin>>v0>>vf;
            a[v0][vf]++;
            a[vf][v0]++;
        }
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i=0;i<n;i++)
        MPI_Bcast(&a[i][0], n, MPI_INT, 0, MPI_COMM_WORLD);
    //cout<<n*n<<'\n';
    if(taskid==10000)
          for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            cout<<a[i][j]<<' ';
        }
        cout<<'\n';
    }
    int p = min(n,numtasks);
    int division = n/p;
    int offset = division*taskid;
    int c[division][n];
    int d[division][n];
    int ind_sum=0;
    //checking last process to gave it largest share
    int u=1;
    if(taskid == numtasks-1)
    {
        u=0;
    }
    for(int i= offset; i<u*(offset+division)+(1-u)*n;i++)
    {
        for(int j=0;j<n;j++)
        {
            c[i][j]=0;
            for(int k=0;k<n;k++)
            {
                c[i][j]+=a[i][k]*a[k][j];
            }
        }
    }
    for(int i= offset; i<u*(offset+division)+(1-u)*n;i++)
    {
        for(int j=0;j<n;j++)
        {
            d[i][j]=0;
            for(int k=0;k<n;k++)
            {
                d[i][j]+=c[i][k]*a[k][j];
            }
        }
        ind_sum+=d[i][i];
    }
    if(taskid!=MASTER && taskid<p)
    {
        MPI_Send(&ind_sum, 1, MPI_INT, MASTER, FROM_WORKER, MPI_COMM_WORLD);
    
    }
    if(taskid==MASTER)
    {
        int ans[n][n];
        int x;
        int sum=0;
        for(int i=1;i<p;i++)
        {
         //   cout<<i*division<<' '<<n*(min(i*division+division,n)-i*division)<<'\n';
            MPI_Recv(&x, 1,MPI_INT, i, FROM_WORKER,MPI_COMM_WORLD, &status);
            //cout<<x<<'\n';
            sum+=x;
        }
        cout<<(ind_sum+sum)/6<<'\n';
    }
    MPI_Finalize();
    return 0;
}