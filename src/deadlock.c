#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

// Первый поток берет Мьютекс 1 и ждет Мьютекс 2
void* Thread1(void* args) {
    pthread_mutex_lock(&mut1);
    printf("Поток 1: Успешно заблокировал Мьютекс 1.\n");
    
    // Искусственная задержка, чтобы второй поток успел заблокировать свой мьютекс
    sleep(1); 
    
    printf("Поток 1: Пытаюсь заблокировать Мьютекс 2...\n");
    pthread_mutex_lock(&mut2);
    
    // Сюда программа никогда не дойдет
    pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut1);
    return NULL;
}

// Второй поток берет Мьютекс 2 и ждет Мьютекс 1
void* Thread2(void* args) {
    pthread_mutex_lock(&mut2);
    printf("Поток 2: Успешно заблокировал Мьютекс 2.\n");
    
    sleep(1); 
    
    printf("Поток 2: Пытаюсь заблокировать Мьютекс 1...\n");
    pthread_mutex_lock(&mut1);
    
    // Сюда программа никогда не дойдет
    pthread_mutex_unlock(&mut1);
    pthread_mutex_unlock(&mut2);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("Запуск демонстрации состояния Deadlock...\n");

    pthread_create(&t1, NULL, Thread1, NULL);
    pthread_create(&t2, NULL, Thread2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Программа успешно завершена (этого текста не должно быть видно).\n");
    return 0;
}