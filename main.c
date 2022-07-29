#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

struct Point
{
    double x;
    double y;
};
struct Distance
{
    double arr[10]; //size of clusters
};

int main(int argc, char *argv[])
{
    FILE *filePtr;
    int x=0;
    int y=0;
    int j=0;
    int K;
    printf("Enter No. of Clusters: ");
    scanf("%d", &K);
    int Datasize = 0;
    struct Point DataList[20];

    filePtr= fopen("/shared/dataset.txt","r");
    int i;

    while (fscanf(filePtr, "%d %d", &x, &y) != EOF)
    {
        DataList[j].x= x;
        DataList[j].y= y;
        j++;

    }
    int num_points=j;
    struct Point centroids[K];
    int index[K];
    bool flag;
    int counter=0;
    while(counter<K)
    {
        flag=false;
        int o=rand()%num_points;
        for(j=0; j<counter; j++)
        {
            if(index[j]==o)
            {
                flag=true;
                break;

            }
        }
        if(flag==true)
            continue;
        index[counter]=o;
        centroids[counter].x=DataList[o].x;
        centroids[counter].y=DataList[o].y;
        counter++;
    }
    printf("Intial Centroids:\n");
    for(i=0; i<K; i++)
    {
        printf("%f %f \n",centroids[i].x,centroids[i].y);
    }
    int cluster[num_points];
    for(i=0; i<num_points; i++)
    {
        cluster[i]=0;
    }

    double xs,ys,dist,xsum,ysum;
    struct Distance distarr[num_points];
    double min;
    int check=0,count;
    while(check<9000)
    {
        #pragma omp parallel shared(centroids,DataList,distarr) private(i,xs,ys)
        {


            #pragma omp for schedule(static)
            for(i=0; i<num_points; i++) //cluster
            {
                xs=pow(DataList[i].x-centroids[0].x,2);
                ys=pow(DataList[i].y-centroids[0].y,2);
                distarr[i].arr[0]=pow(xs+ys,0.5);
                for(j=1; j<K; j++)
                {
                    xs=pow(DataList[i].x-centroids[j].x,2);
                    ys=pow(DataList[i].y-centroids[j].y,2);
                    distarr[i].arr[j]=pow(xs+ys,0.5);

                }
            }
        }

        for(i=0; i<num_points; i++)
        {
            min=distarr[i].arr[0];
            cluster[i]=0;
            for(j=1; j<K; j++)
            {
                if(distarr[i].arr[j]<min)
                {
                    min=distarr[i].arr[j];
                    cluster[i]=j;
                }
            }
        }

        #pragma omp parallel shared(cluster,DataList,centroids) private(i,xsum,ysum,count)
        {

            #pragma omp for schedule(static)
            for(i=0; i<K; i++)
            {
                count=0;
                xsum=0;
                ysum=0;
                for(j=0; j<num_points; j++)
                {
                    if(cluster[j]==i)
                    {
                        count++;
                        xsum+=DataList[j].x;
                        ysum+=DataList[j].y;
                    }
                }
                if(count!=0)
                {
                    centroids[i].x=xsum/count;
                    centroids[i].y=ysum/count;
                }
                else
                {
                    centroids[i].x=0.0;
                    centroids[i].y=0.0;
                }

            }

        }

        check++;
    }//end of for loop
    FILE *fout = fopen("FinalOutput.txt", "w");

    for(j=0; j<K; j++)
    {
        fprintf(fout, "Cluster: %d\n",j);

        for (i = 0; i < num_points; i++)
        {
            if(cluster[i]==j)
            {
                fprintf(fout,"(%f,%f) \n",DataList[i].x,DataList[i].y);
            }
        }
    }
    fclose(fout);
    printf("File FinalOutput is created successfully");

    return 0;
}
