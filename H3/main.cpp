#include <cstdio>
#include <omp.h>
#include <cstdlib>
#include <chrono>
#include <valarray>
#include <vector>
#include <iostream>

void func13() {
    int n = 30;
    int base = 2;
    int a[]{1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1};

    int res = 0;

    #pragma omp parallel for reduction(+:res)
    for (int i = 0; i < n; i++) {
        res += a[i] * (int)pow(base, n - 1 - i);
    }

    printf("Right lower digit: %d\n", res);
    //Читаем число слева направо


    #pragma omp parallel for reduction(+:res)
    for (int i = 0; i < n; i++) {
        res += a[i] * (int)pow(base, i);
    }

    printf("Left lower digit: %d", res);
    //Читаем число справа налево
}


void func14() {
    int num = 210;
    int res = 0;

    #pragma omp parallel for reduction(+:res)
    for (int i = 0; i < num; i++) {
        res += 2*i + 1;
    }

    printf("%d", res);
}

void func15() {
    int start, end;
    std::vector<int> res;

    printf("Enter start number: ");
    std::cin >> start;
    printf("Enter end number: ");
    std::cin >> end;

    bool isSimple = true;
    int sqr = 0;
    #pragma omp parallel for private(isSimple, sqr) num_threads(9)
        for (int i = start; i <= end; i++) {

            isSimple = true;
            sqr = (int)sqrt(i);
            for (int j = 2; j <= sqr; j++) {
                if (i % j == 0) {
                    isSimple = false;
                    break;
                }
            }

            if (isSimple && i != 1) {
                #pragma omp critical
                    res.insert(res.end(), i);
            }
        }
    sort(res.begin(), res.end());

    for (int p : res) {
        printf("%d ", p);
    }
}

int main() {


//    func13();
//    func14();
    func15();

}
