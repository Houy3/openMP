#include <cstdio>
#include <omp.h>
#include <cstdlib>


void func1() {
#pragma omp parallel num_threads(8)
    {
        printf("Hello World: thread %d/%d\n",
               omp_get_thread_num(), omp_get_num_threads());
    }
}
//Вывод неидентичен из-за гонки данных

//----------------------------------------------------------------------------------------------------

void func2() {
    omp_set_num_threads(3);
    #pragma omp parallel  if (omp_get_max_threads() > 1)
    {
        printf("area1 %d/%d \n", omp_get_thread_num(), omp_get_num_threads());
    }

    printf("\n");

    omp_set_num_threads(1);
    #pragma omp parallel if (omp_get_max_threads() > 1)
    {
        if (omp_get_num_threads() > 1) {
            printf("area2 %d/%d \n", omp_get_thread_num(), omp_get_num_threads());
        }
//        printf("area2 %d/%d \n", omp_get_thread_num(), omp_get_num_threads());
    }

}

//----------------------------------------------------------------------------------------------------

void func3() {
    int a = 13;
    int b = 19;

    printf("Before area1: a = %d, b = %d \n", a, b);
    #pragma omp parallel private(a) firstprivate(b) num_threads(2)
    {
        a += omp_get_thread_num();
        b += omp_get_thread_num();
        printf("area1: thread = %d, a = %d, b = %d \n", omp_get_thread_num(), a, b);
    }
    printf("After area1: a = %d, b = %d \n", a, b);


    printf("Before area2: a = %d, b = %d \n", a, b);
    #pragma omp parallel num_threads(4) shared(a) private(b)
    {
        a -= omp_get_thread_num();
        b -= omp_get_thread_num();
        printf("In area2: thread = %d, a = %d, b = %d \n", omp_get_thread_num(), a, b);
    }
    printf("After area2: a = %d, b = %d", a, b);
}

//----------------------------------------------------------------------------------------------------

void func4() {
    int n = 10;
    int a[n];
    int b[n];
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 100 + 1;
        b[i] = rand() % 100 + 1;
        printf("%d", a[i]);
        printf(" ");
        printf("%d\n", b[i]);
    }

    int min;
    int max;
    #pragma omp parallel num_threads(2)
    {
        #pragma omp  master
        {
            printf("min: %d\n", omp_get_thread_num());
            min = a[0];
            for (int i = 1; i < n; i++) {
                if (a[i] < min) {
                    min = a[i];
                }
            }
        }

        #pragma omp single
        {
            printf("max: %d\n", omp_get_thread_num());
            max = b[0];
            for (int i = 1; i < n; i++) {
                if (b[i] > max) {
                    max = b[i];
                }
            }
        }
    }
    printf("Min: %d\nMax: %d", min, max);
}

//----------------------------------------------------------------------------------------------------

void func5() {
    int n1 = 6;
    int n2 = 8;
    int d[n1][n2];
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            d[i][j] = rand() % 100 + 1;
            printf("%d ", d[i][j]);
        }
    }
    printf("\n");



    #pragma omp parallel sections
    {
        #pragma omp section
        {
            float avg = 0.0;
            for (int i = 0; i < n1; ++i) {
                for (int j = 0; j < n2; j++) {
                    avg += d[i][j];
                }
            }
            avg = avg / (n1 * n2);

            printf("Thread %d, avg = %f \n", omp_get_thread_num(), avg);
        }

        #pragma omp section
        {
            int min = d[0][0];
            int max = d[0][0];
            for (int i = 0; i < n1; ++i) {
                for (int j = 1; j < n2; j++) {
                    if (d[i][j] < min) {
                        min = d[i][j];
                    }

                    if (d[i][j] > max) {
                        max = d[i][j];
                    }
                }
            }

            printf("Thread %d, min = %d, max = %d \n", omp_get_thread_num(), min, max);
        }

        #pragma omp section
        {
            int count = 0;
            for (int i = 0; i < n1; ++i) {
                for (int j = 0; j < n2; j++) {
                    if (d[i][j] % 3 == 0) {
                        count++;
                    }
                }
            }

            printf("Thread %d, count = %d \n", omp_get_thread_num(), count);
        }
    }

}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
//    func1();
//    func2();
//    func3();
//    func4();
    func5();


}






