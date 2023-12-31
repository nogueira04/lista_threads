#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t x_cond = PTHREAD_COND_INITIALIZER;

int lendoThreads = 0; //controla a sicronização de leitura
int escrevendo = 0; //controla a sicronização de escrita

long long int* database; 

void *escreverSGBD(void *t) {
    int thread_id = *((int*)t); //indentificador único de cada thread
    printf("Thread de escrita %d começou!\n", thread_id);
    long long int contador = 1;

    while (1) {
        pthread_mutex_lock(&mutex); //impede que outras threads acessem a região crítica

        while (lendoThreads > 0 || escrevendo) { // sincroniza as threads de leitura e escrita, se estviver escrevendo ou alguma lendo é para esperar
            pthread_cond_wait(&x_cond, &mutex);
        }

        escrevendo = 1;

        //vai gerar um numero aleatorio para escrita da posicao no array
        srand(time(NULL) + thread_id); //gerando números aleatorios apartir de uma semente única, baseada no identificador da thread
        int min = 0;
        long long int max = 1000;
        int random_number = min + rand() % (max - min + 1);

        database[random_number] = contador;

        printf("Thread de escrita %d: escreveu %lld na posição %d\n", thread_id, contador, random_number);
        contador++;

        escrevendo = 0;
        pthread_cond_broadcast(&x_cond); //notifica as threads de leitura, que podem executar
        pthread_mutex_unlock(&mutex);//permite que outras threads acessem a região crítica
        sleep(5);
    }
}

void *leituraSGBD(void *t) {
    int thread_id = *((int*)t);
    printf("Thread de leitura %d começou!\n", thread_id);

    while (1) {
        pthread_mutex_lock(&mutex);

        while (escrevendo) {
            pthread_cond_wait(&x_cond, &mutex);// como ele só não pode ler quando tiver thread escrevendo, ele espera até que seja liberado
        }

        lendoThreads++;

        //gera um número aleatório para a posição de leitura do array
        srand(time(NULL) + thread_id);
        int min = 0;
        long long int max = 1000;
        int random_number = min + rand() % (max - min + 1);

        printf("Thread de leitura %d: na posição %d, está escrito %lld\n", thread_id, random_number, database[random_number]);

        lendoThreads--;
        if (lendoThreads == 0) { //verifica se não há mais threads de leitura ativas, para avisar as threads de escritoras
            pthread_cond_broadcast(&x_cond);
        }

        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
}

int main() {
    pthread_t *ESCRITA;
    pthread_t *LEITURA;

    FILE *config_file = fopen("config.txt", "r");
    if (config_file == NULL) {
        perror("Erro ao abrir o arquivo de configuração");
        return 1;
    }

    int t_leitura = 0;
    int t_escrita = 0;

    char line[50];
    while (fgets(line, sizeof(line), config_file) != NULL) {
        if (sscanf(line, "THREAD_LEITORA=%d", &t_leitura) == 1) {
            continue;
        }
        if (sscanf(line, "THREAD_ESCRITORA=%d", &t_escrita) == 1) {
            break;
        }
    }

    fclose(config_file);

    //alocação de memoria para thread de escrita e leitura
    ESCRITA = (pthread_t *)malloc(t_escrita * sizeof(pthread_t));
    LEITURA = (pthread_t *)malloc(t_leitura * sizeof(pthread_t));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    const int tamanhoArray = 1000;
    //cria array de 1000 posições inicializado com 0, para não da erro com lixo de memória
    database = (long long int*)calloc(tamanhoArray, sizeof(long long int));

    for (int i = 0; i < t_escrita; i++) {
        int *tid = (int *) malloc(sizeof(int));
        *tid = i;
        pthread_create(&ESCRITA[i], NULL, escreverSGBD, tid);
    }

    for (int j = 0; j < t_leitura; j++) {
        int *tid = (int *) malloc(sizeof(int));
        *tid = j;
        pthread_create(&LEITURA[j], NULL, leituraSGBD, tid);
    }

    for (int i = 0; i < t_escrita; i++) {
        pthread_join(ESCRITA[i], NULL);
    }

    for (int j = 0; j < t_leitura; j++) {
        pthread_join(LEITURA[j], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&x_cond);
    free(database);
    free(ESCRITA);
    free(LEITURA);

    return 0;
}
