#include <iostream>
#include <pthread.h>
#include <vector>

#define STATIC 0
#define DYNAMIC 1
#define GUIDED 2

/* 
schedule   -> escalonamento
chunk_size -> número de iterações por thread
passo      -> incremento 
*/

typedef struct {
    int beginIndex, endIndex;
} staticData;

int OMPNumThreads = 6; 
void ompFor(int inicio, int passo, int final, int schedule, int chunk_size, void (*f)(int));
void* rotinaStatic(void* data);

int main(){
    
    int scheduleFlag;
    std::cout << "Forma de escalonamento (0: Static 1: Dynamic 2: Guided): " << scheduleFlag << std::endl;
    std::cin >> scheduleFlag;
    


    return 0;
}


void omp_for(int inicio, int passo, int final, int schedule, int chunk_size, void (*f)(int)) {

    pthread_t threadArray[OMPNumThreads];

    for(int i = inicio; i < final;i+=passo){
        switch(schedule) {
            case STATIC: {
                int ratio = (final - inicio) / passo; 
                int ratioRemainder = (final - inicio) % passo; 
                int iterationDivide = ratio / (OMPNumThreads * chunk_size); 
                int threadsMultiply = OMPNumThreads * chunk_size;
                int iterationDivideRemainder = ratio % (OMPNumThreads * chunk_size); 
                pthread_t threadArray[OMPNumThreads];

                if(ratioRemainder == 0 && iterationDivideRemainder == 0){
                    int intervalIndex = 0;

                    staticData** staticDataMatrix = new staticData*[OMPNumThreads]; 
                    for (int i = 0; i < OMPNumThreads; i++) staticDataMatrix[i] = new staticData[iterationDivide];                 

                    while (intervalIndex < iterationDivide) {
                        for (int i = 0; i < OMPNumThreads; i++) {
                            staticDataMatrix[i][intervalIndex].beginIndex = (passo * chunk_size) * (i + (OMPNumThreads * intervalIndex));
                            staticDataMatrix[i][intervalIndex].endIndex = staticDataMatrix[i][intervalIndex].beginIndex + ((chunk_size - 1) * passo);
                        
                            pthread_create(&threadArray[i], NULL, rotinaStatic, (void*) &staticDataMatrix[i][intervalIndex]);
                        }
                        intervalIndex++;

                        for (int i = 0; i < OMPNumThreads; i++) pthread_join(threadArray[i], NULL);
                    }
                } else {
                    int intervalIndex = 0;
                    int chunkSizeSum = 0;
                    staticData** staticDataMatrix = new staticData*[OMPNumThreads]; 
                    for (int i = 0; i < OMPNumThreads; i++) staticDataMatrix[i] = new staticData[iterationDivide];                 

                    while (ratio >= threadsMultiply) {
                        for (int i = 0; i < OMPNumThreads; i++) {
                            staticDataMatrix[i][intervalIndex].beginIndex = (passo * chunk_size) * (i + (OMPNumThreads * intervalIndex));
                            staticDataMatrix[i][intervalIndex].endIndex = staticDataMatrix[i][intervalIndex].beginIndex + ((chunk_size - 1) * passo);
                        
                        }
                        intervalIndex++;
                        ratio -= threadsMultiply;
                        chunkSizeSum += chunk_size;
                    }
                    
                    if(ratio % OMPNumThreads == 0){
                        int ratioPerThread = ratio/OMPNumThreads;
                        int lastIndex = -1;
                        
                        for (int i = 0; i < OMPNumThreads; i++) {
                            staticDataMatrix[i][intervalIndex].beginIndex = passo * lastIndex + 1;
                            staticDataMatrix[i][intervalIndex].endIndex = staticDataMatrix[i][intervalIndex].beginIndex + ratioPerThread;
                            
                            lastIndex = staticDataMatrix[i][intervalIndex].endIndex;
                        } 

                    } else {
                        int index = 0;
                        int ratioPerThreadRemainder = ratio % OMPNumThreads; //2
                        int ratioInitial = (final - inicio) / passo; //12 
                        int ratioTemp = 0;
                        int ratioRemainder = ratioInitial - chunkSizeSum; //12 - 4 = 8
                        int ceilRatioRemainder = ceil(ratioRemainder / OMPNumThreads) + 1; //8/5 = 2 + 1 = 3
                        while(ratioTemp >= ratioRemainder){
                           
                                staticDataMatrix[index][intervalIndex].beginIndex = ceilRatioRemainder;
                                staticDataMatrix[index][intervalIndex].endIndex = staticDataMatrix[i][intervalIndex].beginIndex + ceilRatioRemainder;
                                index++;
                                ratioTemp = ratioRemainder - ceilRatioRemainder;
                                
                        } 

                        }
//12,5,2 : 2,2,3,3,2



                        
                    }
                }

            }
            break;

            case DYNAMIC:
                //
            break;
            case GUIDED:
                //
            break;
            default:
                //
            break;
        }
    }
}
