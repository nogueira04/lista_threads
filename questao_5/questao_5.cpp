#include <iostream>

#include <vector>
#include <pthread.h>

bool* numbers;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printMutexDebug = PTHREAD_MUTEX_INITIALIZER;
int numThreads,sizeVectorCrivo;

typedef struct{
    int begin;
    int end;
}argumentThread;
//Ideia: Separar o vetor em intervalos e utilizar uma thread para cada intervalo
void* crivo(void *threadArg){
    argumentThread threadArgument = *(argumentThread *)threadArg; // ou     argumentThread *threadArgument = (argumentThread *)threadArg;

    for(int i = threadArgument.begin; i <= threadArgument.end;i++){
        if(numbers[i]){
            for(int j = i*i; j < sizeVectorCrivo; j+=i){
                pthread_mutex_lock(&mutex);
                numbers[j] = false;
                pthread_mutex_unlock(&mutex);
            }
        }
       
    }
    pthread_exit(NULL);

}
int main(){
    //Recebe variáveis
    std::cin >> numThreads >> sizeVectorCrivo;

    //Declaração de variáveis
    int lastChar = -1;
    numbers = new bool[sizeVectorCrivo];
    pthread_t threads[numThreads];
    std::vector<int> primes;

    //Inicialização do vetor de números
    for(int i = 0; i < sizeVectorCrivo; i++){
        if(i == 0 || i == 1) numbers[i] = false;
        else numbers[i] = true;
    }

    int indexPerThread = sizeVectorCrivo/numThreads;
    argumentThread argumentThreads[numThreads];

    //Inicialização das threads
    for(int i = 0;i < numThreads;i++){
        argumentThreads[i].begin = lastChar + 1; 
        argumentThreads[i].end = argumentThreads[i].begin + indexPerThread; 
        lastChar = argumentThreads[i].end;
        if(i == numThreads - 1) argumentThreads[i].end = sizeVectorCrivo;

        pthread_create(&threads[i], NULL, crivo ,&argumentThreads[i]);
    }



    //Checa se threads acabaram
    for(int i = 0;i < numThreads;i++){
        pthread_join(threads[i], NULL);
    }

    for(int i = 0;i < sizeVectorCrivo;i++){
        if(numbers[i]) primes.push_back(i);
    }

    //Printando os primos
    for(int i = 0; i < primes.size(); i++){
        if(i != (primes.size() - 1)) std::cout << primes[i] << ",";
        else std::cout << primes[i] << std::endl;
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&printMutexDebug);
    delete [] numbers;
    return 0;
}

