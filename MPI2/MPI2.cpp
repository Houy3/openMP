
#include <iostream>
#include <string>
#include "mpi.h"
#include <algorithm>


int rand(int min, int max) {
    return min + std::rand() % max;
}

int* createArray(int length) {
    return new int[length];
}

int** createArray2(int countOfRows, int countOfColumns) {
    int* arr = new int[countOfRows * countOfColumns];
    int** arr2 = new int* [countOfRows];
    for (int i = 0; i < countOfRows; i++) {
        arr2[i] = &arr[i * countOfColumns];
    }
    return arr2;
}

int* generateArray(int length) {
    int* arr = createArray(length);
    for (int i = 0; i < length; ++i) {
        arr[i] = rand(0, 10);
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
    printf("\n");
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

int** transponate(int** arr, int countOfRows, int countOfColumns) {
    int** arrT = createArray2(countOfColumns, countOfRows);
    for (int i = 0; i < countOfRows; ++i) {
        for (int j = 0; j < countOfColumns; j++) {
            arrT[j][i] = arr[i][j];
        }
    }
    return arrT;
}

int CalculateSendCount(int size, int rank, int n) {
    int recvCount = n / (size - 1);
    if (rank - 1 < n % (size - 1))
        recvCount++;
    return recvCount;
}



void func1(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int mainRank = 0;
    const int tag = 0;

    const int a = 1;
    const int b = 1;
    const int n = 11;


    if (rank == mainRank) {
        int* x = generateArray(n);
        int* y = generateArray(n);
        int* z = createArray(n);

        printf("x: ");
        printArray(x, n);
        printf("y: ");
        printArray(y, n);

        
        int startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, n);
            MPI_Send(&x[startIndex], sendCount, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&y[startIndex], sendCount, MPI_INT, i, tag, MPI_COMM_WORLD);
            startIndex += sendCount;
        }
        
        startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, n);
            MPI_Recv(&z[startIndex], sendCount, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            startIndex += sendCount;
        }

        printf("z: ");
        printArray(z, n);
    }
    else 
    {
        int count;
        MPI_Status status;
  
        MPI_Probe(mainRank, tag, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int* x = createArray(count);
        int* y = createArray(count);
        int* z = createArray(count);
        
        MPI_Recv(&x[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < count; i++) {
            z[i] = a * x[i] + b * y[i];
        }
        
        MPI_Send(&z[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}

void func2(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int mainRank = 0;
    const int tag = 0;

    const int countOfRows = 5;
    const int countOfColumns = 7;

    if (rank == mainRank) {

        int** a = generateArray2(countOfRows, countOfColumns);
        int* b = generateArray(countOfColumns);
        int* c = createArray(countOfRows);

        printf("A:\n");
        printArray2(a, countOfRows, countOfColumns);
        printf("B:\n");
        printArray(b, countOfColumns);


        int startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Send(&a[startIndex][0], sendCount * countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&b[0], countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD);
            startIndex += sendCount;
        }

        startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Recv(&c[startIndex], sendCount, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            startIndex += sendCount;
        }

        printf("C:\n");
        printArray(c, countOfRows);
    }
    else {
        int recvCount = CalculateSendCount(size, rank, countOfRows);

        int** a = createArray2(recvCount, countOfColumns);
        int* b = createArray(countOfColumns);
        int* c = createArray(recvCount);

        MPI_Recv(&a[0][0], recvCount * countOfColumns, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b[0], countOfColumns, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < recvCount; i++) {
            c[i] = 0;
            for (int j = 0; j < countOfColumns; j++) {
                c[i] += a[i][j] * b[j];
            }
        }
        MPI_Send(&c[0], recvCount, MPI_INT, mainRank, tag, MPI_COMM_WORLD);

    }

    MPI_Finalize();
}

void func3(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int mainRank = 0;
    const int tag = 0;

    const int countOfRows = 15; 
    const int countOfColumns = 20;

    if (rank == mainRank) {
        
        int** a = generateArray2(countOfRows, countOfColumns);
        int** b = generateArray2(countOfRows, countOfColumns);
        int** c = createArray2(countOfRows, countOfColumns);

        printf("A:\n");
        printArray2(a, countOfRows, countOfColumns);
        printf("B:\n");
        printArray2(b, countOfRows, countOfColumns);


        int startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Send(&a[startIndex][0], sendCount * countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&b[startIndex][0], sendCount * countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD);
            startIndex += sendCount;
        }

        startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Recv(&c[startIndex][0], sendCount * countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            startIndex += sendCount;
        }
        
        printf("C:\n");
        printArray2(c, countOfRows, countOfColumns);
    }
    else {
        int recvCount = CalculateSendCount(size, rank, countOfRows);

        int** a = createArray2(recvCount, countOfColumns);
        int** b = createArray2(recvCount, countOfColumns);
        int** c = createArray2(recvCount, countOfColumns);

        MPI_Recv(&a[0][0], recvCount * countOfColumns, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&b[0][0], recvCount * countOfColumns, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < recvCount; i++) {
            for (int j = 0; j < countOfColumns; j++) {
                c[i][j] = a[i][j] * b[i][j];
            }
        }
        MPI_Send(&c[0][0], recvCount * countOfColumns, MPI_INT, mainRank, tag, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
}

void func4(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int mainRank = 0;
    const int tag = 0;

    const int countOfRowsA = 10;
    const int countOfColumnsA = 20;

    const int countOfRowsB = 20;
    const int countOfColumnsB = 15;

    const int countOfRows = countOfRowsA;
    const int countOfColumns = countOfColumnsB;

    if (countOfColumnsA != countOfRowsB) {
        printf("матрицы не подходят.");
        MPI_Finalize();
        exit(10);
    }
        
    if (rank == mainRank) {

        int** a = generateArray2(countOfRowsA, countOfColumnsA);
        int** b = generateArray2(countOfRowsB, countOfColumnsB);
        int** c = createArray2(countOfRows, countOfColumns);

        printf("A:\n");
        printArray2(a, countOfRowsA, countOfColumnsA);
        printf("B:\n");
        printArray2(b, countOfRowsB, countOfColumnsB);

        int** bT = transponate(b, countOfRowsB, countOfColumnsB);

        //printf("BT:\n");
        //printArray2(bT, countOfColumnsB, countOfRowsB);
        
        int startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Send(&a[startIndex][0], sendCount * countOfColumnsA, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&bT[0][0], countOfRowsB * countOfColumnsB, MPI_INT, i, tag, MPI_COMM_WORLD);
            startIndex += sendCount;
        }
        
        startIndex = 0;
        for (int i = 1; i < size; i++) {
            int sendCount = CalculateSendCount(size, i, countOfRows);
            MPI_Recv(&c[startIndex][0], sendCount * countOfColumns, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            startIndex += sendCount;
        }

        printf("C:\n");
        printArray2(c, countOfRows, countOfColumns);
    }
    else {
        int recvCount = CalculateSendCount(size, rank, countOfRows);

        int** a = createArray2(recvCount, countOfColumnsA);
        int** bT = createArray2(countOfColumnsB, countOfRowsB);
        int** c = createArray2(recvCount, countOfColumnsB);
        
        MPI_Recv(&a[0][0], recvCount * countOfColumnsA, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&bT[0][0], countOfRowsB * countOfColumnsB, MPI_INT, mainRank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int i = 0; i < recvCount; i++) {
            for (int j = 0; j < countOfColumnsB; j++) {
                c[i][j] = 0;
                for (int k = 0; k < countOfColumnsA; k++) {
                    c[i][j] += a[i][k] * bT[j][k];
                }
            }
        }

        MPI_Send(&c[0][0], recvCount * countOfColumnsB, MPI_INT, mainRank, tag, MPI_COMM_WORLD);

    }

    MPI_Finalize();
}

void func5(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int tag = 0;

    const int countOfRows = 4;
    const int countOfColumns = 2;

    if (rank == 0) {

        int** a = generateArray2(countOfRows, countOfColumns);
        int** aT = createArray2(countOfColumns, countOfRows);

        printf("A:\n");
        printArray2(a, countOfRows, countOfColumns);


        for (int sended = 0; sended < countOfRows; sended++) 
            MPI_Send(&a[sended][0], countOfColumns, MPI_INT, 1, tag, MPI_COMM_WORLD);
        
        
        for (int recieved = 0; recieved < countOfColumns; recieved++) 
            MPI_Recv(&aT[recieved][0], countOfRows, MPI_INT, 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        
        printf("aT:\n");
        printArray2(aT, countOfColumns, countOfRows);
    }
    else if (rank == 1)
    {
        int** a = createArray2(countOfRows, countOfColumns);

        for (int recieved = 0; recieved < countOfRows; recieved++)
            MPI_Recv(&a[recieved][0], countOfColumns, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int** aT = transponate(a, countOfRows, countOfColumns);
       
        for (int sended = 0; sended < countOfColumns; sended++)
            MPI_Send(&aT[sended][0], countOfRows, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}


int main(int argc, char** argv)
{

    //func1(argc, argv);
    //func2(argc, argv);
    //func3(argc, argv);
    func4(argc, argv);
    //func5(argc, argv);

}
