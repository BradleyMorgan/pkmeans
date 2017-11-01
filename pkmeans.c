// *************************************************************
// 
// pkmeans Parallel K-Means Algorithm Implementation
// initial code by Bradley Morgan
// October 2017
//
// The k-means clustering algorithm is used to 
// perform grouping of data points based on their
// proximity to a randomly chosen centroid
//
// Algorithm Steps
// 1. Read in all data elements into i 
// 2. Randomly select k centroids
// 3. Find the closest centroid for each data element
// 4. Recalculate the centroid point 
// 5. Recalculate the closest centroid for each data element
// 6. Stop when no elements are changed
//
// ************************************************************
 
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <mpi.h>

#define DEBUG // remove to disable verbose output

int n = 0; // number of points in input file
int k = 0; // number of centroids to use

// data structure to store points and their grouping

typedef struct point {
   long x, y;
} point;

typedef struct cluster {
    int id;
    point centroid;
    point *instances;
} cluster;

point *points;
cluster *clusters;

// debug print macro

#define dbprintf(msg, args...) \
   	int rank; \
        MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
	printf("%i:%d:%s(): ", rank, __LINE__, __func__); \
	printf(msg, ##args);


void init_centroids() {

   // select random centroids from the set of i input points
   
   int i=0;
   
   for(i=0; i<=k; i++) {

      int rnd = rand() % n;
       
      clusters[i].centroid = points[rnd];
      clusters[i].instances = malloc(sizeof(point)*n);
      
      dbprintf("Index %d selected as centroid: x=%ld, y=%ld.\n", rnd, clusters[i].centroid.x, clusters[i].centroid.y);

   }
    
}

double pdistance(point *p1, point *p2) {

   // calculates the distance between two points
   
   dbprintf("distance between (%ld, %ld) and (%ld, %ld) = ", p1->x, p1->y, p2->x, p2->y);

   double dx = (p1->x - p2->x) * (p1->x - p2->x);
   double dy = (p1->y - p2->y) * (p1->y - p2->y);
   double d = sqrt(dx + dy);

   printf("%f\n", d);

   return d;
    
}

void pcentroid(point *p) {

   // given a point p, find the closest centroid and assign it
   // as the point's centroid property

   double cdist = INT_MAX;

   int i,j=0;

   for(i=0; i<=k; i++) { //

      double dist = pdistance(p, &clusters[i].centroid);

      dbprintf("current = %f | calculated = %f\n", cdist, dist);
		
      if(dist < cdist) {
         
          dbprintf("cluster change found for (%ld, %ld).\n", p->x, p->y);
     
          cdist = dist;
        
          clusters[i].instances[0] = *p;
          
          j = i;
 
      }

   }

   dbprintf("centroid x=%ld, y=%ld is closest.\n", clusters[j].instances[0].x, clusters[j].instances[0].y);

}


int main(int argc, const char *argv[]) {

    // initialize MPI for future paralellization

    int rank,nprocs;
    char **pargv;

    MPI_Init(&argc, &pargv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // allocate memory for centroids to be used globally
    
    clusters = (cluster *) malloc(sizeof(cluster) * 6) ;

    if ( argc != 3 ) {   // check passed arguments
        
        dbprintf("usage: %s <filename> <num_centroids>\n", argv[0]);
    
    } else {
       
        // read data points from input

        FILE *input = fopen(argv[1], "r");
	
	k = atoi(argv[2]);
        
        if (NULL == input) {
        
            dbprintf("[ERROR] could not open input file.\n");
        
        } else {
	    
	    points = (point *) malloc(sizeof(point) * 3000000);           
            point *p = (point *) malloc(sizeof(point));
           
	    while(fscanf(input, "%ld,%ld", &p->x, &p->y) == 2) {
                
                points[n] = *p;
            
                n++;
                
            }
          
            dbprintf("found %d points in input file.\n", n);
            
        }
	
        init_centroids(); // initialize centroids
 
 	point *t = (point *) malloc(sizeof(point));

	int i=0;
	
	// for each data point find the closest centroid
	// TODO: add additional loops for cluster normalization
	// TODO: parallelize this iteration with MPI
	
	for(i=0; i<=n; i++) {
		t = &points[i];
		dbprintf("checking x=%ld, y=%ld.\n", t->x, t->y);
		pcentroid(t);
	} 

    }

    MPI_Finalize();
    
    return 0;
    
}

