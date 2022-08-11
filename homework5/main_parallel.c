#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "write_pgm.h"
#include "matrix.h"

void setInitialCondition(Matrix matrix){
	double sigma = 0.01;
	double mu = 0.5;
	for(int i = 0; i < matrix.rows; i++){
		double x = 1.0/(matrix.rows-1)*i;
		for(int j =0; j < matrix.columns; j++){
			double y = 1.0/ (matrix.columns-1)*j;
//			matrix.data[i][j] = initialCondition(x,y);
			matrix.data[i][j] = (1.0/(2.0*M_PI*sigma*sigma))*exp(-0.5*( ((x-mu)/sigma)*((x-mu)/sigma) +  ((y-mu)/sigma)*((y-mu)/sigma)   ));
		}
	}
}

void step(double k, double h, Matrix tip1, Matrix ti, Matrix tim1) {
	#pragma omp parallel num_threads(2)
	{
		int x = omp_get_thread_num();
		int start;
		int end;
		if(x==0){
///			printf("%d \n", x);
			start = 1;
			end = (tip1.rows - 1)/2;
		}else{
//			printf("%d \n", x);
			start = (tip1.rows-1)/2;
			end = tip1.rows-1;
		}
//		printf("%d \n", x);
		for(int row = start; row < end; row++){
        	    for (int col=1; col<tip1.columns-1; col++){
	        	  tip1.data[row][col] = k*k/(h*h)*(ti.data[row-1][col]-4*ti.data[row][col]+ti.data[row+1][col]+ti.data[row][col-1]+ti.data[row][col+1])+2*ti.data[row][col]-tim1.data[row][col];
		    }
        	}
	}
}

void main(int argc, char *argv[]){
	char fileName[16];
        double h = atof(argv[1]);
        double k = atof(argv[2]);
        double endTime = atof(argv[3]);
        int printEveryNSteps = atoi(argv[4]);
        int printEveryNPoints = atoi(argv[5]);

        if (k>=h) {
        	fprintf(stderr, "error, cfl condition violated");
        }
        int N = (int)(1.0/h) + 1;
//	sprintf(fileName, "%06d.pgm",0);

//        Matrix *states = malloc(3*sizeof(Matrix));
  	Matrix states[3];
        states[0] = create_matrix(N,N);
        setInitialCondition(states[0]);
        states[1] = create_matrix(N,N);
        setInitialCondition(states[1]);
        states[2] = create_matrix(N,N);

//	print_matrix(states[0]);

//        write_pgm(states[0],printEveryNPoints, fileName);

        int steps = 1;
        double t = 0.0;

        while (t<=endTime) {
        	Matrix uk = states[(steps+1)%3];
        	Matrix ukm1 = states[steps%3];
        	Matrix ukm2 = states[(steps-1)%3];

        	step(k,h,uk,ukm1,ukm2);

		if (steps%printEveryNSteps == 0 && t == endTime) {
	               	sprintf(fileName,"%06d.pgm",steps/printEveryNSteps);
			printf("%s, t=%f \n",fileName, t);
	               	write_pgm(uk, printEveryNPoints, fileName);
	       	}
	       	++steps;
	        t=k*steps;
	}
}
