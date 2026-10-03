#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <stdbool.h>

// Глобальная переменная для хранения итогового результата и мьютекс для ее защиты
unsigned long long global_result = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct FactArgs {
    int begin;
    int end;
    int mod;
};

// Функция вычисления части факториала для отдельного потока
void* FactorialThread(void* arguments) {
    struct FactArgs* args = (struct FactArgs*)arguments;
    unsigned long long local_result = 1;

    // Вычисляем локальное произведение на выделенном отрезке с применением модуля
    for (int i = args->begin; i <= args->end; i++) {
        local_result = (local_result * i) % args->mod;
    }

    // Критическая секция: безопасно умножаем глобальный результат на локальный
    pthread_mutex_lock(&mut);
    global_result = (global_result * local_result) % args->mod;
    pthread_mutex_unlock(&mut);

    return NULL;
}

int main(int argc, char **argv) {
    int k = -1;
    int pnum = -1;
    int mod = -1;

    while (true) {
        int current_optind = optind ? optind : 1;
        static struct option options[] = {
            {"pnum", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        // Флаг k: означает, что -k это короткий аргумент, требующий значения
        int c = getopt_long(argc, argv, "k:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0: pnum = atoi(optarg); break;
                    case 1: mod = atoi(optarg); break;
                }
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case '?':
                break;
        }
    }

    if (k == -1 || pnum == -1 || mod == -1) {
        printf("Usage: %s -k \"num\" --pnum=\"num\" --mod=\"num\"\n", argv[0]);
        return 1;
    }

    if (k < 1 || pnum < 1 || mod < 1) {
        printf("Все аргументы должны быть положительными числами.\n");
        return 1;
    }

    pthread_t threads[pnum];
    struct FactArgs args[pnum];

    // Распределяем числа от 1 до k между заданным количеством потоков
    int step = k / pnum;
    for (int i = 0; i < pnum; i++) {
        args[i].begin = i * step + 1;
        args[i].end = (i == pnum - 1) ? k : (i + 1) * step;
        args[i].mod = mod;

        if (pthread_create(&threads[i], NULL, FactorialThread, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Factorial %d! mod %d = %llu\n", k, mod, global_result);
    return 0;
}