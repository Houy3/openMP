
#include <iostream>
#include <iostream>
#include <string>
#include "mpi.h"
#include <algorithm>


void func1(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("Hello world!\n");
    MPI_Finalize();
}

void func2(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("number: %d, total: %d\n", rank, size);

    MPI_Finalize();
}

void func3(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    const int n = 10;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size == 2) {
        if (rank == 0) {
            int array[n];
            srand(time(NULL));
            for (int i = 0; i < n; i++) {
                array[i] = rand() % 100;
            }

            MPI_Send(array, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
        else if (rank == 1) {
            int receivedArray[n];

            MPI_Recv(receivedArray, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            printf("numbers: ");
            for (int i = 0; i < n; i++) {
                printf("%d ", receivedArray[i]);
            }
            printf("\n");
        }
    }
    else 
    {
        printf("wrong count of processes.\n");
    }


    MPI_Finalize();
}

void func4(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    const int n = 10;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 1) {
        int array[n];
        srand(time(NULL));
        for (int i = 0; i < n; i++) {
            array[i] = rand() % 100;
        }

        MPI_Send(array, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        for (int i = 2; i < size; i++) {
            MPI_Send(array, n, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Status status;
        int count;

        MPI_Probe(1, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);

        int* receivedArray = new int[count];

        MPI_Recv(receivedArray, count, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

        printf("rank %d, numbers: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", receivedArray[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
}

void func5(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int number = rank * 10;

    if (rank == 0) {
        int* numbers = new int[size];
        MPI_Status status;

        for (int i = 1; i < size; i++) {
            MPI_Recv(&numbers[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }

        std::sort(numbers, numbers + size);
        printf("ranks: ");
        for (int i = 1; i < size; i++) {
            printf("%d ", numbers[i]);
        }
    }
    else {
        MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}


int main(int argc, char** argv)
{

    func1(argc, argv);
    //func2(argc, argv);
    //func3(argc, argv);
    //func4(argc, argv);
    //func5(argc, argv);

}
