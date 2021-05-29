/* 
 * 
 * This code calculates the house price of a house by learing from
 * training data. It uses pseudo inverse of a given matrix to find the 
 * weight of different features.
 * 
 * Predicted Price : Y = W0 + W1*x1 + W2*X2 + W3*X3 + W4*X4
 * Weight Matrix : W = pseudoInv(X)*Y
 * pseudoInv(X) = inverse(transpose(X)*X) * transpose(X)  
 * 
 * weight(w) = pseudoInv(X) * Y
 * 			where	X = Input data matrix
 * 					Y = Target vector
 * 
 */
 
#include<stdio.h>
#include<stdlib.h>

// all methods declarations
double** multiplyMatrix(double **matA, double **matB, int r1, int c1, int r2, int c2);
double** transposeMatrix(double** mat, int row, int col);
double** inverseMatrix(double **matA, int dimension);
void freeArr(double **mat, int rows);

// main method starts here
int main(int argc, char** argv){

     FILE *fp = fopen(argv[1], "r");

     int attr;
     int data;
     fscanf(fp, "%d", &attr);
     fscanf(fp, "%d", &data);

     //  double trainingData[data][attr+1];
     double **trainingData = malloc(data * sizeof(double*));
     for (int i = 0; i < data; i++){
       trainingData[i] = malloc((attr+1) * sizeof(double));
     }
     
     for (int i = 0; i < data; i++){

       trainingData[i][0] = 1.0;

     }
     
     double** housePrice = malloc(data*sizeof(double*));
     for (int i = 0; i < data; i++){
       housePrice[i] = malloc(sizeof(double));
     }
     
     int count = 0;
     int r = 0;
     
     for (int i = 0; i < (data)*(attr+1); i++){ 

       double val;
       fscanf(fp, "%lf,", &val);
         
       if (count == (attr)){

	 housePrice[r][0] = val;
	 count = 0;
	 r++;

       }
       
       else{
	 
	 trainingData[r][count+1] = val;
	 count++;

       }
       
     }

     double** Xt = transposeMatrix(trainingData, data, attr+1);
     double** Xsq = multiplyMatrix(Xt, trainingData, attr+1, data, data, attr+1);
     double** XsqInv = inverseMatrix(Xsq, attr+1);
     double** pseudoInv = multiplyMatrix(XsqInv, Xt, attr+1, attr+1, attr+1, data);
     double** Weights = multiplyMatrix(pseudoInv, housePrice, attr+1, data, data, 1);
       // double** Weights = multiplyMatrix( multiplyMatrix( inverseMatrix( multiplyMatrix( transposeMatrix( trainingData, data, attr+1), trainingData, attr+1, data, data, attr+1), attr+1), transposeMatrix( trainingData, data, attr+1), attr+1, attr+1, attr+1, data), housePrice, attr+1, data, data, 1);
     
     FILE *fp2 = fopen(argv[2], "r");

     int num;
     double attribute;

     fscanf(fp2, "%d", &num);

     for (int i = 0; i < num; i++){

       double value = Weights[0][0];
       
       for (int j = 0; j < attr; j++){
	 
	 fscanf(fp2, "%lf, ", &attribute);
	 value += Weights[j+1][0] * attribute;
	 
       }

       printf("%.0f\n" , value);

     }
     
     /*
     for (int x = 0; x < data; x++){
       free(trainingData[x]);
     }
     free(trainingData);

     for (int x = 0; x < data; x++){
	 free(housePrice[x]);
       }
     free(housePrice);
     
     for (int x = 0; x < attr+1; x++){
       free(Weights[x]);
       }
     free(Weights);
     */
     freeArr(Xt, attr+1);
     freeArr(Xsq, attr+1);
     freeArr(XsqInv, attr+1);
     freeArr(pseudoInv, attr+1);
     freeArr(trainingData, data);
     freeArr(housePrice, data);
     freeArr(Weights, attr+1);
     
     fclose(fp);
     fclose(fp2);
     return 0;
}

void freeArr(double **mat, int rows){

     for (int x = 0; x < rows; x++){
	 free(mat[x]);
       }
     free(mat);

}  

double** multiplyMatrix(double **matA, double **matB, int r1, int c1, int r2, int c2)
{
    double** result = malloc(r1 * sizeof(double*)); 
    
    for (int i = 0; i < r1; i++){

      result[i] = malloc(c2 * sizeof(double));

    }

    double total = 0;

    for (int r = 0; r < r1; r++){
      
      for (int i = 0; i < c2; i++){

	for (int j = 0; j < r2; j++){
	  //  printf("%lf * %lf + \n", matA[r][j], matB[j][i]);
	  total += (matA[r][j] * matB[j][i]);
	  // printf("\n %lf \n", total);
	}
	
	result[r][i] = total;
	total = 0.0;
	//printf("\n");
      }

    }
    // printf("\n \n");
    return result;
}


double** transposeMatrix(double** mat, int row, int col)
{
  
    double** matTran=malloc(col*sizeof(double*)); 

    for (int i = 0; i < col; i++){

      matTran[i] = malloc(row*sizeof(double));

    }

    for (int i = 0; i < row; i++){
      
      for (int j = 0; j < col; j++){

	matTran[j][i] = mat[i][j];
        
      }

    }
    
    return matTran;        
}


double** inverseMatrix(double **matA, int dimension)
{

    double** matI=malloc(dimension*sizeof(double*));
    
    for (int i = 0; i < dimension; i++){

      matI[i] = malloc(dimension * sizeof(double));

    }

    for (int i = 0; i < dimension; i++){

      for (int j = 0; j < dimension; j++){

	if (i == j) matI[i][j] = 1.0;

	else matI[i][j] = 0.0;

      }
      
    }

    for (int p = 0; p < dimension; p++){

      double x = matA[p][p];

      for (int c = 0; c < dimension; c++){

	matA[p][c] = (matA[p][c]) / x;
	matI[p][c] = (matI[p][c]) / x;

      }
      
      for (int i = p + 1; i < dimension; i++){

	x = matA[i][p];

	for (int c = 0; c < dimension; c++){

	  matA[i][c] = matA[i][c] - (matA[p][c] * x);
	  matI[i][c] = matI[i][c] - (matI[p][c] * x);

	}

      }

    }
        
    for (int p = dimension - 1; p >=  0; p--){

      for (int i = p - 1; i >= 0; i--){
 
	double x = matA[i][p];

	for (int c = dimension - 1; c >= 0; c--){
	 
	   matA[i][c] = matA[i][c] - (matA[p][c] * x);
	   matI[i][c] = matI[i][c] - (matI[p][c] * x);

	}
	
      }
      
    }
    
    return matI;
}


