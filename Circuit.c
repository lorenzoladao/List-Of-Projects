#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct tempStore
{
    char *tempName;
    int *tempOut;
} tempStore;

int main(int argc, char *argv[])
{
    FILE *fp = fopen(argv[1], "r");

    char *ptrFile = malloc(50 * sizeof(char));
    char delims[] = " \t\n";
    //First line of file (INPUTVAR)
    fgets(ptrFile, 99, fp);
    char **inputVar = malloc(50 * sizeof(char *));
    for (int i = 0; i < 50; i++)
    {
        inputVar[i] = malloc(50 * sizeof(char));
    }
    char *ptr = strtok(ptrFile, delims);
    int wordInd = 0;
    while (ptr != NULL)
    {
        strcpy(inputVar[wordInd], ptr);
        ptr = strtok(NULL, delims);
        wordInd++;
    }
    //inputVar[0]=INPUTVAR, inputVar[1]=#ofVars, inputVar[2-x]=vars

    //Second line of file (OUTPUTVAR)
    fgets(ptrFile, 99, fp);
    char **outputVar = malloc(50 * sizeof(char *));
    for (int i = 0; i < 50; i++)
    {
        outputVar[i] = malloc(50 * sizeof(char));
    }
    ptr = strtok(ptrFile, delims);
    wordInd = 0;
    while (ptr != NULL)
    {
        strcpy(outputVar[wordInd], ptr);
        ptr = strtok(NULL, delims);
        wordInd++;
    }
    //outputVar[0]=OUTPUTVAR, outputVar[1]=#ofVars, outputVar[2-x]=vars

    int numInput = atoi(inputVar[1]);
    int numOutput = atoi(outputVar[1]);
    int numRows = pow(2, numInput);
    int totalSize = numInput + numOutput;

    //Truth Table
    int **circuit = malloc(numRows * sizeof(int *));
    for (int i = 0; i < numRows; i++)
    {
        circuit[i] = calloc(totalSize, sizeof(int));
    }

    //Fills out table for all inputs
    for (int j = 0; j < numInput; j++)
    {
        int counter = 0;
        for (int i = 0; i < numRows; i++)
        {
            if (counter < numRows / pow(2, j + 1))
            {
                circuit[i][j] = 0;
                counter++;
            }
            else if (counter / 2 == numRows / pow(2, j + 1))
            {
                circuit[i][j] = 0;
                counter = 1;
            }
            else
            {
                circuit[i][j] = 1;
                counter++;
            }
        }
    }

    tempStore *temps = malloc(50 * sizeof(struct tempStore));
    for (int i = 0; i < 50; i++)
    {
        temps[i].tempName = malloc(50 * sizeof(char));
        temps[i].tempOut = malloc(numRows * sizeof(int));
    }
    for (int i = 0; i < 50; i++)
    {
        strcpy(temps[i].tempName, " ");
    }
    //put the premade variables into temp struct
    for (int i = 0; i < numInput; i++)
    {
        strcpy(temps[i].tempName, inputVar[i + 2]);
    }
    for (int i = 0; i < numOutput; i++)
    {
        strcpy(temps[numInput + i].tempName, outputVar[i + 2]);
    }

    for (int i = 0; i < numInput; i++)
    {
        for (int j = 0; j < numRows; j++)
        {
            temps[i].tempOut[j] = circuit[j][i];
        }
    }
    int tempCnt = totalSize;

    while (fgets(ptrFile, 99, fp) != NULL)
    {
        char **tempVar = malloc(50 * sizeof(char *));
        for (int i = 0; i < 50; i++)
        {
            tempVar[i] = malloc(50 * sizeof(char));
        }
        ptr = strtok(ptrFile, delims);
        wordInd = 0;
        while (ptr != NULL)
        {
            strcpy(tempVar[wordInd], ptr);
            ptr = strtok(NULL, delims);
            wordInd++;
        }
        // tempVar[0]=operator "depends"tempVar[1-2]= vars tempVar[3]=output

        if (strcmp(tempVar[0], "NOT") == 0)
        {
            //looks for the first variable in list and stores index
            int cntIn = 0;
            while (cntIn < 50)
            {
                if (strcmp(temps[cntIn].tempName, tempVar[1]) == 0)
                    break;
                cntIn++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[2]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[2]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] = 1 ^ temps[cntIn].tempOut[i];
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] = 1 ^ temps[cntIn].tempOut[i];
                }
                tempCnt++;
            }
        }

        else if (strcmp(tempVar[0], "AND") == 0)
        {
            //looks for the first variable in list and stores index
            int cntIn1 = 0;
            while (cntIn1 < 50)
            {
                if (strcmp(temps[cntIn1].tempName, tempVar[1]) == 0)
                    break;
                cntIn1++;
            }
            //looks for the second variable in list and stores index
            int cntIn2 = 0;
            while (cntIn2 < 50)
            {
                if (strcmp(temps[cntIn2].tempName, tempVar[2]) == 0)
                    break;
                cntIn2++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[3]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[3]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] =
                        temps[cntIn1].tempOut[i] & temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] =
                        temps[cntIn1].tempOut[i] & temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
        }
        else if (strcmp(tempVar[0], "OR") == 0)
        {
            //looks for the first variable in list and stores index
            int cntIn1 = 0;
            while (cntIn1 < 50)
            {
                if (strcmp(temps[cntIn1].tempName, tempVar[1]) == 0)
                {
                    break;
                }
                cntIn1++;
            }
            //looks for the second variable in list and stores index
            int cntIn2 = 0;
            while (cntIn2 < 50)
            {
                if (strcmp(temps[cntIn2].tempName, tempVar[2]) == 0)
                {
                    break;
                }
                cntIn2++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[3]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[3]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] =
                        temps[cntIn1].tempOut[i] | temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] =
                        temps[cntIn1].tempOut[i] | temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
        }

        else if (strcmp(tempVar[0], "NAND") == 0)
        {
            //looks for the first variable in list and stores index
            int cntIn1 = 0;
            while (cntIn1 < 50)
            {
                if (strcmp(temps[cntIn1].tempName, tempVar[1]) == 0)
                    break;
                cntIn1++;
            }
            //looks for the second variable in list and stores index
            int cntIn2 = 0;
            while (cntIn2 < 50)
            {
                if (strcmp(temps[cntIn2].tempName, tempVar[2]) == 0)
                    break;
                cntIn2++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[3]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[3]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] =
                        1 ^ (temps[cntIn1].tempOut[i] & temps[cntIn2].tempOut[i]);
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] =
                        1 ^ (temps[cntIn1].tempOut[i] & temps[cntIn2].tempOut[i]);
                }
                tempCnt++;
            }
        }

        else if (strcmp(tempVar[0], "NOR") == 0)
        {
            //looks for the first variable in list and stores index
            int cntIn1 = 0;
            while (cntIn1 < 50)
            {
                if (strcmp(temps[cntIn1].tempName, tempVar[1]) == 0)
                    break;
                cntIn1++;
            }
            //looks for the second variable in list and stores index
            int cntIn2 = 0;
            while (cntIn2 < 50)
            {
                if (strcmp(temps[cntIn2].tempName, tempVar[2]) == 0)
                    break;
                cntIn2++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[3]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[3]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] =
                        1 ^ (temps[cntIn1].tempOut[i] | temps[cntIn2].tempOut[i]);
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] =
                        1 ^ (temps[cntIn1].tempOut[i] | temps[cntIn2].tempOut[i]);
                }
                tempCnt++;
            }
        }

        else
        {
            //looks for the first variable in list and stores index
            int cntIn1 = 0;
            while (cntIn1 < 50)
            {
                if (strcmp(temps[cntIn1].tempName, tempVar[1]) == 0)
                    break;
                cntIn1++;
            }
            //looks for the second variable in list and stores index
            int cntIn2 = 0;
            while (cntIn2 < 50)
            {
                if (strcmp(temps[cntIn2].tempName, tempVar[2]) == 0)
                    break;
                cntIn2++;
            }
            //looks for output and sees if name aready exists, and stores index
            int notContain = 1;
            int cntOut = 0;
            for (cntOut = 0; cntOut < 50; cntOut++)
            {
                if (strcmp(temps[cntOut].tempName, " ") == 0)
                {
                    break;
                }
                if (strcmp(temps[cntOut].tempName, tempVar[3]) == 0)
                {
                    notContain = 0;
                    break;
                }
            }
            //if output name doesnt exists
            if (notContain)
            {
                strcpy(temps[tempCnt].tempName, tempVar[3]);
                for (int i = 0; i < numRows; i++)
                {
                    temps[tempCnt].tempOut[i] =
                        temps[cntIn1].tempOut[i] ^ temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
            //name already exists
            else
            {
                for (int i = 0; i < numRows; i++)
                {
                    temps[cntOut].tempOut[i] =
                        temps[cntIn1].tempOut[i] ^ temps[cntIn2].tempOut[i];
                }
                tempCnt++;
            }
        }
        for (int j = 0; j < 50; j++)
        {
            free(tempVar[j]);
        }
        free(tempVar);
    }

    //fills out output
    int outIndex = 0;
    while (outIndex < numOutput)
    {
        for (int k = 0; k < numRows; k++)
        {
            circuit[k][numInput + outIndex] = temps[numInput + outIndex].tempOut[k];
        }
        outIndex++;
    }

    // prints truth table
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < totalSize; j++)
        {
            printf("%d ", circuit[i][j]);
        }
        printf("\n");
    }

    // printf("%d\n", numRows);
    for (int i = 0; i < 50; i++)
    {
        free(temps[i].tempName);
        free(temps[i].tempOut);
    }
    free(temps);

    for (int i = 0; i < 50; i++)
    {
        free(inputVar[i]);
        free(outputVar[i]);
    }
    for (int i = 0; i < numRows; i++)
    {
        free(circuit[i]);
    }

    free(circuit);
    free(inputVar);
    free(outputVar);
    fclose(fp);
}
