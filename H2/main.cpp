#include <cstdio>
#include <omp.h>
#include <cstdlib>
#include <chrono>

void func6() {
    int n = 10;
    int a[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int b[]{2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    float avgA = 0;
    float avgB = 0;

    #pragma omp parallel for reduction(+:avgA) reduction(+:avgB)
        for (int i = 0; i < n; i++) {
            avgA += a[i];
            avgB += b[i];
        }

    avgA = avgA / n;
    avgB = avgB / n;

    printf("Average a = %f\n Average b  = %f \n", avgA, avgB);
    if (avgA > avgB) {
        printf("Average a is bigger");
    }
    if (avgB < avgA) {
        printf("Average B is bigger");
    }
    printf("Equals");
}

void func7() {
    int n = 12;
    int a[n];
    int b[n];
    int c[n];

    #pragma omp parallel for schedule(static, 3) num_threads(3)
        for (int i = 0; i < n; i++) {
            a[i] = rand() % 100 + 1;
            b[i] = rand() % 100 + 1;
            printf("%d: Thread %d of %d \n", i, omp_get_thread_num(), omp_get_num_threads());
        }
    printf("\n");

    #pragma omp parallel for schedule(dynamic, 3) num_threads(4)
        for (int i = 0; i < n; i++) {
            c[i] = a[i] + b[i];
            printf("%d: Thread %d of %d \n", i, omp_get_thread_num(), omp_get_num_threads());
        }
}

void func8() {
    int n = 1000;
    int m = 300;

    int matrix[n][m];
    int vector[m];
    int result[n];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix[i][j] = rand() % 100 + 1;
        }
    }
    for (int i = 0; i < m; i++) {
        vector[i] = rand() % 100 + 1;
    }
    for (int i = 0; i < n; i++) {
        result[i] = 0;
    }


    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    printf("in series: %f \n", std::chrono::duration_cast<std::chrono::duration<float>>(finish - start).count());


    for (int i = 0; i < n; i++) {
        result[i] = 0;
    }


    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for num_threads(4)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                result[i] += matrix[i][j] * vector[j];
            }
        }
    finish = std::chrono::high_resolution_clock::now();
    printf("in parallel: %f", std::chrono::duration_cast<std::chrono::duration<float>>(finish - start).count());
}


void func9() {

    int n = 6;
    int m = 8;
    int d[n][m];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            d[i][j] = rand() % 100 + 1;
//            printf("%d ", d[i][j]);
        }
    }



    int min = d[0][0];
    int max = d[0][0];
    #pragma omp parallel for num_threads(8)
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (d[i][j] > max) {
                    #pragma omp critical
                        max = d[i][j];
                }

                if (d[i][j] < min) {
                    #pragma omp critical
                        min = d[i][j];
                }
            }
        }

    printf("min = %d\nmax = %d", min, max);
}


void func10() {
    int n = 30;
    int a[n];
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 100 + 1;
    }

    int count = 0;

    #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (a[i] % 9 == 0) {
                #pragma omp atomic
                    count++;
            }
        }

    printf("count: %d", count);
}


void func11() {
    int n = 30;
    int a[n];
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 100 + 1;
    }

    int max = -100000000;

    #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (a[i] % 7 == 0 && a[i] > max) {
                #pragma omp critical
                    max = a[i];
            }
        }

    if (max != -100000000) {
        printf("count: %d", max);
    } else {
        printf("no such values");
    }
}


void func12() {
    int i = 7;
    #pragma omp parallel num_threads(8)
        stop:
            if (i == omp_get_thread_num()){
                printf("Hello world %d of %d \n", omp_get_thread_num(), omp_get_num_threads());
                i--;
            } else {
                goto stop;
            }
}


int main() {
//    func6();
//    func7();
//    func8();
//    func9();
//    func10();
//    func11();
    func12();

}
