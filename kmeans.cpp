#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <float.h>

using namespace std;

int i = 0;
int k = 0;

struct point {
   long x, y;
   struct point *c;
};

point points[3000000];
point *centroids = (point *) malloc(sizeof(point) * 6) ;

point *find_centroids(int n) {

   for(int j=0; j<=n; j++) {

      int rnd = rand() % i;
      centroids[j] = points[rnd];

      printf("Index %d selected as centroid: x=%d, y=%d.\n", rnd, centroids[j].x, centroids[j].y);

   }

   return centroids;

}

double pdistance(point *p1, point *p2) {

   printf("distance between (%d, %d) and (%d, %d) = ", p1->x, p1->y, p2->x, p2->y);

   double dx = (p1->x - p2->x) * (p1->x - p2->x);
   double dy = (p1->y - p2->y) * (p1->y - p2->y);
   double d = sqrt(dx + dy);

   printf("%f\n", d);

   return d;
}

void pcentroid(point *p) {

   double curr = 9999999;
   point *centroid = &centroids[0];

   for(int n=0; n<=5; n++) {

      double distance = pdistance(p, &centroids[n]);

      printf("current = %f | calculated = %f\n", curr, distance);
		
      if(distance < curr) {
         
	 printf("Cluster change found for (%d, %d).\n", p->x, p->y); 
         curr = distance;
	 centroid = &centroids[n];
 
      }

   }

   printf("Centroid x=%d, y=%d is closest.\n", centroid->x, centroid->y); 
   
   p->c = centroid;

}


int main(int argc, const char * argv[]) {
    
    if ( argc != 2 ) {
        
        cout << "usage: " << argv[0] << " <filename>\n";
    
    } else {
       
        ifstream input(argv[1]);
        
        if (!input.is_open()) {
        
            cout<<"ERROR: Could not open input file.\n";
        
        } else {
            
            point *p = (point *) malloc(sizeof(point));
            
            char delim;
            
            while(input >> p->x >> delim >> p->y) {
                
                points[i] = *p;
          
                i++;
                
            }
          
            printf("Found %d points in input file.\n", i);
            
        }

	point *centroids = find_centroids(5);     
 	point *t = (point *) malloc(sizeof(point));
 
	for(int k=0; k<=i; k++) {
		t = &points[k];
		printf("Checking x=%d, y=%d.\n", t->x, t->y);
		pcentroid(t);
	} 

    }
    
    return 0;
    
}
