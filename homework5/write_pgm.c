#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "matrix.h"

int scale(double value, double max){
	int i;
	i = 127 / max * value + 127;
	return i;
}

void write_pgm(Matrix mat, int printEveryNPoints, char *filename){

	FILE *fp;
	double num = fabs(mat.data[0][0]);

	fp = fopen(filename, "w");
	fprintf(fp, "P2\n");
	fprintf(fp, "%d %d\n",mat.columns, mat.rows);
	fprintf(fp, "255\n");

	for(int i = 0; i < mat.rows; i++){

		for(int  j = 0; j < mat.columns; j++){

			if(fabs(mat.data[i][j]) > num){

				num = fabs(mat.data[i][j]);

			}

		}

	}

	for(int i = 0; i < mat.rows; i++){

		for(int j = 0; j < mat.columns; j++){

			//mat.data[i][j] = scale(mat.data[i][j], num);
			fprintf(fp, "%0d ", scale(mat.data[i][j], num  ));

		}

		fprintf(fp, "\n");

	}

	fclose(fp);

}
