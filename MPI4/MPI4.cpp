
#include <iostream>
#include <string>
#include "mpi.h"
#include <algorithm>
#include <limits.h>
#include <chrono>

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


void func41(int argc, char** argv) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int const n = 1500;

    //PingPong
    if (rank == 0) {
        int* arr = generateArray(n);

        auto start = std::chrono::high_resolution_clock::now();

        MPI_Send(arr, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(arr, n, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        auto end = std::chrono::high_resolution_clock::now();

        float time_took = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
        printf("PingPong time: %f ms \n", time_took);
    }
    else if (rank == 1) {
        int* received = new int[n];

        MPI_Recv(received, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(received, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }


    //PingPing
    int* arr2 = generateArray(n);

    std::chrono::high_resolution_clock::time_point start;

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        start = std::chrono::high_resolution_clock::now();

        MPI_Send(arr2, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else if (rank == 1) {
        start = std::chrono::high_resolution_clock::now();

        MPI_Send(arr2, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int* received = new int[n];

        MPI_Recv(received, n, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else if (rank == 1) {
        int* received = new int[n];

        MPI_Recv(received, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    auto end = std::chrono::high_resolution_clock::now();

    if (rank == 0) {
        float time_took = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
        printf("PingPing time: %f ms \n", time_took);
    }

    MPI_Finalize();
}


void func42(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int const n = 12;
    int sendCount = ceil(((double)n) / size);



    if (rank == 0) {
        int* x = generateArray(n);
        printf("x: ");
        printArray(x, n);

        int sended = sendCount;
        for (int i = 1; i < size; i++) {
            int realSendCount = std::min(sendCount, n - sended);
            MPI_Send(&x[sended], realSendCount, MPI_INT, i, 0, MPI_COMM_WORLD);

            sended += realSendCount;
        }

        printf("rank %d: ", rank);
        printArray(x, sendCount);
    }
    else {
        MPI_Status status;
        int count;

        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int* received = new int[count];

        MPI_Recv(received, count, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        printf("rank %d: ", rank);
        printArray(received, count);
    }


    MPI_Finalize();
}

void func51(int argc, char** argv) {

    const int lastProcess = 5;
    const int n = 15;

    MPI_Comm comm;

    int* ranks = new int[5] {1, 3, 6, 8, 9};
    int rank, size;
    //int* arr[n];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int realRank = rank;

    MPI_Group wgroup, group1, group2;
    MPI_Comm_group(MPI_COMM_WORLD, &wgroup);
    MPI_Group_incl(wgroup, lastProcess, ranks, &group1);
    MPI_Group_excl(wgroup, lastProcess, ranks, &group2);

    MPI_Comm comm1, comm2;
    MPI_Comm_create(MPI_COMM_WORLD, group1, &comm1);
    MPI_Comm_create(MPI_COMM_WORLD, group2, &comm2);

    if (comm1 != MPI_COMM_NULL) {
        MPI_Comm_rank(comm1, &rank);
    }
    if (comm2 != MPI_COMM_NULL) {
        MPI_Comm_rank(comm2, &rank);
    }

    int* arr = createArray(n);
    if (comm1 != MPI_COMM_NULL && rank == 0) {
        arr = generateArray(n);
    }

    if (comm1 != MPI_COMM_NULL) {
        MPI_Bcast(&arr[0], n, MPI_INT, 0, comm1);

        printf("rank %d recieved: \n", realRank);
        printArray(arr, n);
    }

    if (comm1 != MPI_COMM_NULL && rank == lastProcess - 1) {
        MPI_Send(&arr[0], n, MPI_INT, 0, 1, comm1);
    }

    if (comm1 != MPI_COMM_NULL && rank == 0) {
        int b[n];

        MPI_Recv(&b[0], n, MPI_INT, lastProcess - 1, 1, comm1, MPI_STATUSES_IGNORE);

        printf("rank 0 received:\n");
        printArray(b, n);
    }

    if (comm1 != MPI_COMM_NULL) {
        MPI_Comm_free(&comm1);
    }
    if (comm2 != MPI_COMM_NULL) {
        MPI_Comm_free(&comm2);
    }

    MPI_Group_free(&group1);
    MPI_Group_free(&group2);

    MPI_Finalize();
}

void func52(int argc, char** argv) {

    int rank, size;

    int n = 8;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        MPI_Datatype type;
        MPI_Datatype row;

        MPI_Type_contiguous(n, MPI_INT, &row);
        MPI_Type_commit(&row);
        MPI_Type_vector(n / 2, 1, 2, row, &type);
        MPI_Type_commit(&type);

        int** a = generateArray2(n,n);
        printf("a: \n");
        printArray2(a, n, n);

        MPI_Send(&(a[0][0]), 1, type, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&(a[1][0]), 1, type, 2, 0, MPI_COMM_WORLD);
    }
    else if (rank % 2 == 1) {

        int** b = createArray2(n / 2, n); 
        MPI_Recv(&(b[0][0]), 4 * 8, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("nechet: \n");
        printArray2(b, n/2, n);
    }
    else {
        int** c = createArray2(n / 2, n);
        MPI_Recv(&(c[0][0]), 4 * 8, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("chet: \n");
        printArray2(c, n / 2, n);
    }

    MPI_Finalize();
}





struct ArrType {
    int* arr;
};

void func53(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int n = 8;
    int rank;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Datatype newtype;
    MPI_Type_create_struct(1, new int[1] { n }, new MPI_Aint[1]{ offsetof(struct ArrType, arr) }, new MPI_Datatype[1]{ MPI_INT }, &newtype);
    MPI_Type_commit(&newtype);


    if (rank == 4) {
        int** a = generateArray2(n, n);

        printf("a:\n");
        printArray2(a, n, n);

        for (int i = 0; i < size - 1; i++) {
            ArrType* arr = new ArrType[2];
            arr[0] = ArrType();
            arr[0].arr = a[i];
            arr[1] = ArrType();
            arr[1].arr = a[i + 4];

            MPI_Send(&arr, 2, newtype, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        ArrType* arr = new ArrType[2];
        MPI_Recv(&arr, 2, newtype, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("rank %d:\n", rank);
        printArray(arr[0].arr, n);
        printArray(arr[1].arr, n);
    }
    MPI_Type_free(&newtype);
    MPI_Finalize();
}

int main(int argc, char** argv)
{

    //func41(argc, argv);
    func42(argc, argv);
    //func51(argc, argv);
    //func52(argc, argv);
    //func53(argc, argv);

}
