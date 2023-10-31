
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
    int** arr = new int* [countOfRows];
    for (int i = 0; i < countOfRows; i++) {
        arr[i] = new int[countOfColumns];
    }
    return arr;
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

void func1(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int mainRank = 0;
    const int tag = 0;

    const int a = 5;
    const int b = 7;
    const int n = 15;

    int sizeForEach = n / (size - 1);
    int sizeForLast = sizeForEach;
    if (n % (size - 1) > 0) {
        sizeForEach++;
        sizeForLast = n % sizeForEach;
    }

    if (rank == mainRank) {
        int* x = generateArray(n);
        int* y = generateArray(n);
        int* z = createArray(n);

        printf("x: ");
        printArray(x, n);
        printf("y: ");
        printArray(y, n);

        
        int startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == mainRank)
                continue;
            int sizeToTransfer = sizeForEach;
            if (i == size - 1)
                sizeToTransfer = sizeForLast;

            MPI_Send(&x[startIndex], sizeToTransfer, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&y[startIndex], sizeToTransfer, MPI_INT, i, tag, MPI_COMM_WORLD);

            startIndex += sizeForEach;
        }
        
        
        startIndex = 0;
        for (int i = 0; i < size; i++) {
            if (i == mainRank)
                continue;

            int sizeToTransfer = sizeForEach;
            if (i == size - 1)
                sizeToTransfer = sizeForLast;

            MPI_Recv(&z[startIndex], sizeToTransfer, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            startIndex += sizeForEach;
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

        int* x = new int[count];
        int* y = new int[count];
        int* z = new int[count];
        
        MPI_Recv(&x[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&y[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);

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

    const int countOfRows = 15;
    const int countOfColumns = 20;

    int countOfSendsForEach = countOfRows / (size - 1);
    int countOfSendsForLast = countOfSendsForEach;
    if (countOfRows % (size - 1) > 0) {
        countOfSendsForEach++;
        countOfSendsForEach = countOfRows % countOfSendsForEach;
    }

    if (rank == mainRank) {

        int** a = generateArray2(countOfRows, countOfColumns);
        int* b = generateArray(countOfColumns);
        int** c = createArray2(countOfRows, countOfColumns);

        printf("A:\n");
        printArray2(a, countOfRows, countOfColumns);
        printf("B:\n");
        printArray(b, countOfColumns);


        int sended = 0;
        int dest = 1;
        while (sended < countOfRows) {
            MPI_Send(&a[sended][0], countOfColumns, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&b[sended], 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            sended++;
            dest++;
            if (dest >= size)
                dest = 1;
        }

        int recieved = 0;
        dest = 1;
        while (recieved < countOfRows) {
            MPI_Recv(&c[recieved][0], countOfColumns, MPI_INT, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            recieved++;
            dest++;
            if (dest >= size)
                dest = 1;
        }

        printf("C:\n");
        printArray2(c, countOfRows, countOfColumns);
    }
    else {
        MPI_Status status;
        int count;

        int countOfSends = countOfSendsForEach;
        if (rank = size - 1) {
            countOfSends = countOfSendsForLast;
        }

        while (countOfSends > 0) {
            MPI_Probe(mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            int* a = createArray(count);
            int b;
            int* c = createArray(count);

            MPI_Recv(&a[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&b, 1, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);

            for (int i = 0; i < count; ++i) {
                c[i] = a[i] * b;
            }
            MPI_Send(&c[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD);

            countOfSends--;
        }
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

    int countOfSendsForEach = countOfRows / (size - 1);
    int countOfSendsForLast = countOfSendsForEach;
    if (countOfRows % (size - 1) > 0) {
        countOfSendsForEach++;
        countOfSendsForEach = countOfRows % countOfSendsForEach;
    }

    if (rank == mainRank) {
        
        int** a = generateArray2(countOfRows, countOfColumns);
        int** b = generateArray2(countOfRows, countOfColumns);
        int** c = createArray2(countOfRows, countOfColumns);

        printf("A:\n");
        printArray2(a, countOfRows, countOfColumns);
        printf("B:\n");
        printArray2(b, countOfRows, countOfColumns);


        int sended = 0;
        int dest = 1;
        while (sended < countOfRows) {
            MPI_Send(&a[sended][0], countOfColumns, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&b[sended][0], countOfColumns, MPI_INT, dest, tag, MPI_COMM_WORLD);
            sended++;
            dest++;
            if (dest >= size)
                dest = 1;
        }

        int recieved = 0;
        dest = 1;
        while (recieved < countOfRows) {
            MPI_Recv(&c[recieved][0], countOfColumns, MPI_INT, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            recieved++;
            dest++;
            if (dest >= size)
                dest = 1;
        }
        
        printf("C:\n");
        printArray2(c, countOfRows, countOfColumns);
    }
    else {
        MPI_Status status;
        int count;

        int countOfSends = countOfSendsForEach;
        if (rank = size - 1) {
            countOfSends = countOfSendsForLast;
        }

        while (countOfSends > 0) {
            MPI_Probe(mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            int* a = createArray(count);
            int* b = createArray(count);
            int* c = createArray(count);

            MPI_Recv(&a[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&b[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);

            for (int i = 0; i < count; ++i) {
                c[i] = a[i] * b[i];
            }
            MPI_Send(&c[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD);

            countOfSends--;
        }
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

    const int countOfRowsA = 4;
    const int countOfColumnsA = 2;

    const int countOfRowsB = 2;
    const int countOfColumnsB = 3;

    const int countOfRows = countOfRowsA;
    const int countOfColumns = countOfColumnsB;

    if (countOfColumnsA != countOfRowsB) {
        printf("матрицы не подходят.");
        MPI_Finalize();
        exit(10);
    }
        
    


    int n = countOfRows * countOfColumns;
    int countOfSendsForEach = n / (size - 1);
    int countOfSendsForLast = countOfSendsForEach;
    if (n % (size - 1) > 0) {
        countOfSendsForEach++;
        countOfSendsForEach = n % countOfSendsForEach;
    }

    if (rank == mainRank) {

        int** a = generateArray2(countOfRowsA, countOfColumnsA);
        int** b = generateArray2(countOfRowsB, countOfColumnsB);
        int** c = createArray2(countOfRows, countOfColumns);

        printf("A:\n");
        printArray2(a, countOfRowsA, countOfColumnsA);
        printf("B:\n");
        printArray2(b, countOfRowsB, countOfColumnsB);

        int ** bT = transponate(b, countOfRowsB, countOfColumnsB);

        //printf("BT:\n");
        //printArray2(bT, countOfColumnsB, countOfRowsB);


        int sendedA = 0;
        int dest = 1;
        while (sendedA < countOfRows) {
            int sendedB = 0;
            while (sendedB < countOfColumns) {
                MPI_Send(&a[sendedA][0], countOfColumnsA, MPI_INT, dest, tag, MPI_COMM_WORLD);
                MPI_Send(&bT[sendedB][0], countOfRowsB, MPI_INT, dest, tag, MPI_COMM_WORLD);
                sendedB++;

                dest++;
                if (dest >= size)
                    dest = 1;
            }
            sendedA++;  
        }

        int recievedA = 0;
        dest = 1;
        while (recievedA < countOfRows) {
            int recievedB = 0;
            while (recievedB < countOfColumns) {
                MPI_Recv(&c[recievedA][recievedB], 1, MPI_INT, dest, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                recievedB++;

                dest++;
                if (dest >= size)
                    dest = 1;
            }
            recievedA++;
        }

        printf("C:\n");
        printArray2(c, countOfRows, countOfColumns);
    }
    else {
        MPI_Status status;
        int count;

        int countOfSends = countOfSendsForEach;
        if (rank = size - 1) {
            countOfSends = countOfSendsForLast;
        }

        while (countOfSends > 0) {
            MPI_Probe(mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            int* a = createArray(count);
            int* b = createArray(count);
            int c = 0;

            MPI_Recv(&a[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&b[0], count, MPI_INT, mainRank, tag, MPI_COMM_WORLD, &status);

            for (int i = 0; i < count; ++i) {
                c += a[i] * b[i];
            }
            MPI_Send(&c, 1, MPI_INT, mainRank, tag, MPI_COMM_WORLD);

            countOfSends--;
        }
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
