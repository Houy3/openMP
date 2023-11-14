
#include <iostream>
#include <string>
#include "mpi.h"
#include <algorithm>
#include <limits.h>

int rand(int min, int max) {
    return min + std::rand() % max;
}

int* createArray(int length) {
    return new int[length];
}

int** createArray2(int countOfRows, int countOfColumns) {
    int* arr = new int [countOfRows * countOfColumns];
    int** arr2 = new int* [countOfRows];
    for (int i = 0; i < countOfRows; i++) {
        arr2[i] = &arr[i * countOfColumns];
    }
    return arr2;
}

int* generateArray(int length) {
    int* arr = createArray(length);
    for (int i = 0; i < length; ++i) {
        arr[i] = rand(0, 100);
    }
    return arr;
}

int** generateArray2(int countOfRows, int countOfColumns) {
    int** arr = createArray2(countOfRows, countOfColumns);
    for (int i = 0; i < countOfRows; ++i) {
        for (int j = 0; j < countOfColumns; j++) {
            arr[i][j] = rand(0, 10);
        }
    }
    return arr;
}

void printArray(int* arr, int length) {

    for (int i = 0; i < length; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n\n");
}

void printArray2(int** arr, int countOfRows, int countOfColumns) {
    
    for (int i = 0; i < countOfRows; ++i) {
        for (int j = 0; j < countOfColumns; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


int CalculateRecvCount(int size,  int n) {
    int avg = n / size;
    if (n % size != 0)
        avg++;
    return avg;
}

int CalculateRealRecvCount(int size, int rank, int n) {
    int avg = n / size;
    if (n % size != 0)
        avg++;

    n -= rank * avg;
    return std::min(std::max(0, n), avg);
}



void func1(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mainRank = 0;

    int const n = 100;
    int recvCount = CalculateRecvCount(size, n);
    int realRecvCount = CalculateRealRecvCount(size, rank, n);

    int* x = createArray(n);
    int* recieved = createArray(recvCount);

    if (rank == mainRank) {
        x = generateArray(n);
        printf("x: ");
        printArray(x, n);
    }
    
    MPI_Scatter(&x[0], recvCount, MPI_INT, &recieved[0], recvCount, MPI_INT, mainRank, MPI_COMM_WORLD);

    int sum = 0;
    for (int i = 0; i < realRecvCount; i++) {
        sum += abs(recieved[i]);
    }

    int result;
    MPI_Reduce(&sum, &result, 1, MPI_INT, MPI_SUM, mainRank, MPI_COMM_WORLD);

    if (rank == mainRank) {
        printf("||x||: %d\n", result);
    }

    MPI_Finalize();
}

void func2(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mainRank = 0;

    int const n = 100;
    int recvCount = CalculateRecvCount(size, n);
    int realRecvCount = CalculateRealRecvCount(size, rank, n);

    int* x = createArray(n);
    int* y = createArray(n);

    int* recievedX = createArray(recvCount);
    int* recievedY = createArray(recvCount);

    if (rank == mainRank) {
        x = generateArray(n);
        printf("x: ");
        printArray(x, n);

        y = generateArray(n);
        printf("y: ");
        printArray(y, n);
    }

    MPI_Scatter(&x[0], recvCount, MPI_INT, &recievedX[0], recvCount, MPI_INT, mainRank, MPI_COMM_WORLD);
    MPI_Scatter(&y[0], recvCount, MPI_INT, &recievedY[0], recvCount, MPI_INT, mainRank, MPI_COMM_WORLD);


    int sum = 0;
    for (int i = 0; i < realRecvCount; i++) {
        sum += recievedX[i] * recievedY[i];
    }

    int result;
    MPI_Reduce(&sum, &result, 1, MPI_INT, MPI_SUM, mainRank, MPI_COMM_WORLD);

    if (rank == mainRank) {
        printf("scalar product: %d\n", result);
    }

    MPI_Finalize();
}

void func3(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mainRank = 0;

    int n = 6;
    int m = 5;

    int recvCount = CalculateRecvCount(size, n);
    int realRecvCount = CalculateRealRecvCount(size, rank, n);

    int** matrix = createArray2(n, m);
    int* vector = createArray(m);

    int** recievedMatrix = createArray2(recvCount, m);
    int* answer = createArray(recvCount);

    if (rank == mainRank) {
        matrix = generateArray2(n, m);
        printf("matrix: \n");
        printArray2(matrix, n, m);

        vector = generateArray(m);
        printf("vector: ");
        printArray(vector, m);
    }

    MPI_Bcast(vector, m, MPI_INT, mainRank, MPI_COMM_WORLD);
    MPI_Scatter(&matrix[0][0], recvCount * m, MPI_INT, &recievedMatrix[0][0], recvCount * m, MPI_INT, mainRank, MPI_COMM_WORLD);
    
    
    int sum = 0;
    for (int i = 0; i < realRecvCount; i++) {
        answer[i] = 0;
        for (int j = 0; j < m; j++) {
            answer[i] += recievedMatrix[i][j] * vector[j];
        }
    }
    
    int* result = createArray(recvCount * size);
    MPI_Gather(&answer[0], recvCount, MPI_INT, &result[0], recvCount, MPI_INT, mainRank, MPI_COMM_WORLD);

    if (rank == mainRank) {
        printf("result: ");
        printArray(result, n);
    }
    
    MPI_Finalize();
}

void func4(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mainRank = 0;

    int n = 5;

    int recvCount = CalculateRecvCount(size, n);
    int realRecvCount = CalculateRealRecvCount(size, rank, n);

    int** matrix = createArray2(n, n);

    int** recievedMatrix = createArray2(recvCount, n);

    if (rank == mainRank) {
        matrix = generateArray2(n, n);
        printf("matrix: \n");
        printArray2(matrix, n, n);
    }

    MPI_Scatter(&matrix[0][0], recvCount * n, MPI_INT, &recievedMatrix[0][0], recvCount * n, MPI_INT, mainRank, MPI_COMM_WORLD);


    int max = INT_MIN;
    for (int i = 0; i < realRecvCount; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += recievedMatrix[i][j] ;
        }
        if (sum > max)
            max = sum;
    }

    int result;
    MPI_Reduce(&max, &result, 1, MPI_INT, MPI_MAX, mainRank, MPI_COMM_WORLD);

    if (rank == mainRank) {
        printf("result: %d", result);
    }

    MPI_Finalize();
}

void func5(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int mainRank = 0;

    int n = 120;

    int recvCount = CalculateRecvCount(size, n);
    int realRecvCount = CalculateRealRecvCount(size, rank, n);

    int* vector = createArray(n);

    int* recievedVector = createArray(n);

    if (rank == mainRank) {
        vector = generateArray(n);
        printf("vector: \n");
        printArray(vector, n);
    }

    MPI_Scatter(&vector[0], recvCount, MPI_INT, &recievedVector[0], recvCount, MPI_INT, mainRank, MPI_COMM_WORLD);

    int max = INT_MIN;
    for (int i = 0; i < realRecvCount; i++) {
        if (recievedVector[i] > max)
            max = recievedVector[i];
    }
    int answer[2];
    answer[0] = max;
    answer[1] = rank;

    int* localResult = createArray(size * 2);
    MPI_Gather(&answer[0], 2, MPI_INT, &localResult[0], 2, MPI_INT, mainRank, MPI_COMM_WORLD);

    int globalResult[2];
    MPI_Reduce(&answer[0], &globalResult[0], 1, MPI_2INT, MPI_MAXLOC, mainRank, MPI_COMM_WORLD);



    if (rank == mainRank) {
        printf("global max: %d\n", globalResult[0]);
        printf("proccess with global max: %d\n\n", globalResult[1]);

        for (int i = 0; i < size; i++) {
            if (localResult[2 * i] != INT_MIN)
                printf("local max of process %d is %d\n", localResult[2*i + 1], localResult[2*i]);
            else
                printf("process %d didn't work", localResult[2*i + 1]);
        }
    }

    MPI_Finalize();
}


int main(int argc, char** argv)
{

    //func1(argc, argv);
    //func2(argc, argv);
    //func3(argc, argv);
    //func4(argc, argv);
    func5(argc, argv);

}
