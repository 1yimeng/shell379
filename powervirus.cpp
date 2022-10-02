#include <iostream>

using namespace std;

int main() {

    int size = 10000000;

    for (int j = 0; j < 300; j++) {
        int * a = new int[size];
        int * b = new int[size];
        int * c = new int[size];

        for (int i = 0; i < size; i++) {
            a[i] = i;
            b[i] = i;
        }

        for (int i = 0; i < size; i++) {
            c[i] = a[i] + b[i];
        }

        delete[] a;
        delete[] b;
        delete[] c;
    }
    return 0;
}