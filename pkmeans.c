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
#include <mpi.h>

#define DEBUG // remove to disable verbose output

int i = 0; // number of points in input file
int k = 0; // number of centroids to use

// data structure to store points and their grouping

typedef struct point {
   long x, y;
   struct point *centroid;
   int cluster;
} point;

point *points;
point *centroids;

// debug print macro

#define dbprintf(msg, args...) \
   	int rank; \
        MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
	printf("%i:%d:%s(): ", rank, __LINE__, __func__); \
	printf(msg, ##args);


point *find_centroids(int n) {

   // select random centroids from the set of i input points
   
   int j=0;
   
   for(j=0; j<=n; j++) {

      int rnd = rand() % i;
      centroids[j] = points[rnd];

      dbprintf("Index %d selected as centroid: x=%d, y=%d.\n", rnd, centroids[j].x, centroids[j].y);

   }

   return centroids;

}

double pdistance(point *p1, point *p2) {

   // calculates the distance between two points
   
   dbprintf("distance between (%d, %d) and (%d, %d) = ", p1->x, p1->y, p2->x, p2->y);

   double dx = (p1->x - p2->x) * (p1->x - p2->x);
   double dy = (p1->y - p2->y) * (p1->y - p2->y);
   double d = sqrt(dx + dy);

   printf("%f\n", d);

   return d;
}

void pcentroid(point *p) {

   // given a point p, find the closest centroid and assign it
   // as the point's centroid property

   double cdist = 9999999;
   point *c = &centroids[0];

   int m=0;
   int n=0;

   for(n=0; n<=5; n++) {

      double dist = pdistance(p, &centroids[n]);

      dbprintf("current = %f | calculated = %f\n", cdist, dist);
		
      if(dist < cdist) {
         
	 dbprintf("cluster change found for (%d, %d).\n", p->x, p->y); 
         cdist = dist;
	 c = &centroids[n];
	 m = n;
 
      }

   }

   dbprintf("centroid x=%d, y=%d is closest.\n", c->x, c->y); 
   
   p->centroid = c;
   p->cluster = m;

}


int main(int argc, const char *argv[]) {

    // initialize MPI for future paralellization

    int rank,nprocs;
    char **pargv;

    MPI_Init(&argc, &pargv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // allocate memory for centroids to be used globally
    
    centroids = (point *) malloc(sizeof(point) * 6) ;

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
           
	    while(fscanf(input, "%d,%d", &p->x, &p->y) == 2) { 
                
                points[i] = *p;
          
                i++;
                
            }
          
            dbprintf("found %d points in input file.\n", i);
            
        }
	
	point *centroids = find_centroids(k); // get centroids
 
 	point *t = (point *) malloc(sizeof(point));

	int n=0;
	
	// for each data point find the closest centroid
	// TODO: add additional loops for cluster normalization
	// TODO: parallelize this iteration with MPI
	
	for(n=0; n<=i; n++) {
		t = &points[n];
		dbprintf("checking x=%d, y=%d.\n", t->x, t->y);
		pcentroid(t);
	} 

    }

    MPI_Finalize();
    
    return 0;
    
}

