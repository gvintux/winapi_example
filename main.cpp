#include <iostream>
#include <iomanip>
#include <ctime>
#include <cfloat>
#include <windows.h>

using namespace std;

typedef double (*integrable)(double);

static double parabola(double x)
{ return x * x; }

typedef struct
{
    unsigned long long bstep;
    unsigned long long estep;
    double h;
    double a;
    double b;
    integrable f;
    double result;
} integration_args;

static DWORD WINAPI integate(void *args)
{
    integration_args *params = (integration_args *)args;
    params->result = 0;
    for (unsigned int i = params->bstep; i < params->estep; i++)
    {
        double x = params->a + i * params->h;
        params->result += params->h * params->f(x);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    const unsigned int thread_count = 4;
    const unsigned long long steps = 1000000000;
    double a = 0;
    double b = 1;
    double h = (b - a) / (steps * 1.0);
    double sum = 0;
    integrable f = parabola;
    HANDLE threads[thread_count];
    integration_args args[thread_count];
    unsigned int steps_per_thread = steps / thread_count;

    clock_t begin = clock();
    for (unsigned int i = 0; i < thread_count; i++)
    {
        args[i].h = h;
        args[i].a = a;
        args[i].b = b;
        args[i].f = f;
        args[i].bstep = i *  steps_per_thread;
        args[i].estep = args[i].bstep + steps_per_thread;
        threads[i] = CreateThread(NULL, 0, integate, &args[i], 0, NULL);
    }
    WaitForMultipleObjects(thread_count, &threads[0], TRUE, INFINITE);
    for(unsigned int i=0; i<thread_count; i++) CloseHandle(threads[i]);
    for (unsigned int i = 0; i < thread_count; i++) sum += args[i].result;
    cout << fixed << setprecision(DBL_DIG) << sum << endl;
    clock_t end = clock();
    cout << fixed << setprecision(DBL_DIG) << double(end - begin) / (1.0 * CLOCKS_PER_SEC) << " seconds." << endl;
    return 0;
}

