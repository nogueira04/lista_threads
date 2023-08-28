#include <iostream>
#include <string>

std::string PASSWORD = "SENHA12345";
std::string SEARCHED_PASSWORD = "__________";
pthread_mutex_t print_mutex;

typedef struct {
    int char_start, char_end;
} data;

void* searchChar(void* args) {
    data* t_data = (data*) args;
    char tmp = 32;

    for (int i = t_data->char_start; i <= t_data->char_end; i++) {
        while (tmp != PASSWORD[i]) tmp++;
        pthread_mutex_lock(&print_mutex);
        std::cout << "Procurando senha: " << SEARCHED_PASSWORD << std::endl;
        SEARCHED_PASSWORD[i] = tmp;
        pthread_mutex_unlock(&print_mutex);
        tmp = 32;
        
    }

    pthread_exit(NULL);
}

int main() {

    int n_threads, char_per_thread, last_char;
    pthread_mutex_init(&print_mutex, NULL);

    std::cin >> n_threads;

    pthread_t threads_array[n_threads];
    data t_data_array[n_threads];

    char_per_thread = PASSWORD.length() / n_threads; 
    last_char = -1;

    for (int i = 0; i < n_threads; i++) {
        t_data_array[i].char_start = last_char + 1;
        t_data_array[i].char_end = t_data_array[i].char_start + char_per_thread;

        last_char = t_data_array[i].char_end;

        pthread_create(&threads_array[i], NULL, searchChar, &t_data_array[i]);
    }

    for (int i = 0; i < n_threads; i++) pthread_join(threads_array[i], NULL);

    std::cout << "Senha encontrada: " << SEARCHED_PASSWORD << std::endl;

    pthread_mutex_destroy(&print_mutex);

    return 0;
}