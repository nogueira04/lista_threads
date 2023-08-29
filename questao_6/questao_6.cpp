#include <iostream>
#include <pthread.h>
#include <vector>
#include <cmath>

#define STATIC 0
#define DYNAMIC 1
#define GUIDED 2

/* 
schedule   -> escalonamento
chunk_size -> número de iterações por thread
passo      -> incremento 
*/
//Argumento das threads
struct staticData {
    int beginIndex, endIndex, intervalIndex;
    int (*f)(int);
    int passo;
    staticData** staticDataMatrixStruct;
};

int func(int a) { return a + 1; }


int OMPNumThreads = 6; 
void ompFor(int inicio, int passo, int final, int schedule, int chunk_size, void (*f)(int));
void*rotinaStatic(void* data) {
    staticData* dataTemp = (staticData*) data;
    dataTemp->f = &func;
    //Roda pelas threads e intervalos e realiza a função
    for (int i = 0; i < dataTemp->intervalIndex; i++) {
        for (int j = dataTemp->beginIndex; j < dataTemp->endIndex; j++) {
            dataTemp->f(j);
        }
    }
}

int main(){
    
    int scheduleFlag;
    std::cout << "Forma de escalonamento (0: Static 1: Dynamic 2: Guided): ";
    std::cin >> scheduleFlag;
    


    return 0;
}


void omp_for(int inicio, int passo, int final, int schedule, int chunk_size, void (*f)(int)) {

    

    
        switch(schedule) {
            case STATIC: {
                int ratio = (final - inicio) / passo; //iterações
                int ratioRemainder = (final - inicio) % passo; //resto de iterações
                int iterationDivide = ratio / (OMPNumThreads * chunk_size); //quantos intervalos cada thread deve receber
                int iterationDivideRemainder = ratio % (OMPNumThreads * chunk_size); //ver se é possivél dividir igualmente

                pthread_t threadArray[OMPNumThreads]; //array de threads
                staticData threadArgument[OMPNumThreads]; //argumento de cada thread
                staticData** staticDataMatrix = new staticData*[OMPNumThreads]; //matriz de dados guardando o início e o fim de cada intervalo
                    for (int i = 0; i < OMPNumThreads; i++) staticDataMatrix[i] = new staticData[iterationDivide];        

                if(ratioRemainder == 0 && iterationDivideRemainder == 0){ //caso dê para dividir igualmente
                    int intervalIndex = 0; //indice do intervalo

                                     
                //Aqui se dividem cada intervalo e cada indice de inicio e fim para cada thread 
                    while (intervalIndex < iterationDivide) {
                        for (int i = 0; i < OMPNumThreads; i++) {
                            staticDataMatrix[i][intervalIndex].beginIndex = (passo * chunk_size) * (i + (OMPNumThreads * intervalIndex));
                            staticDataMatrix[i][intervalIndex].endIndex = staticDataMatrix[i][intervalIndex].beginIndex + ((chunk_size - 1) * passo);
                            staticDataMatrix[i][intervalIndex].intervalIndex = intervalIndex;
                            threadArgument[i].passo = passo;
                            threadArgument[i].intervalIndex = intervalIndex;
                            
                           
                        }
                        intervalIndex++;
                    }
                    //Aqui se iguala a matriz feita com a matriz do argumento da thread
                    for(int i = 0;i < intervalIndex || i == 0;i++){
                        for(int j = 0; j < OMPNumThreads;j++){
                            threadArgument[j].staticDataMatrixStruct[i][j] = staticDataMatrix[i][j];
                        }
                    }
                    //Criações das threads
                    for(int i = 0; i < OMPNumThreads;i++){
                        pthread_create(&threadArray[i],NULL,rotinaStatic,(void*) &threadArgument[i]);
                    }
                    for (int i = 0; i < OMPNumThreads; i++) pthread_join(threadArray[i], NULL);
                    
                } else { //Caso a divisão não seja igual
                    int indexTemp = 0; //indice de cada thread
                    bool endLoop = false; // booleano para saber se o loop deve acabar
                    int intervalIndex = 0; //indice do intervalo
                    int chunkSizeSum = 0; //soma para ver quantos indices foram processados
                    
                    //Aqui tenta dividir o máximo que der com o chunk_size fornecido
                    while(!endLoop){
                        staticDataMatrix[indexTemp][intervalIndex].beginIndex = (passo * chunk_size) * (indexTemp + (OMPNumThreads * intervalIndex));
                        staticDataMatrix[indexTemp][intervalIndex].endIndex = staticDataMatrix[indexTemp][intervalIndex].beginIndex + ((chunk_size - 1) * passo);
                        indexTemp++;
                        ratio = ratio - chunk_size;
                        if(indexTemp == OMPNumThreads - 1) {
                            indexTemp = 0;
                            intervalIndex++;
                        }
                        chunkSizeSum += chunk_size;
                        //Se o chunk_size não for suficiente para terminar o número total de indices ele para o loop
                        if(ratio/((OMPNumThreads - 1) - indexTemp) > chunk_size) endLoop = true;
                    }

                    //Se o que sobrou de indices for divisivel pelas threads que falta, ele divide igualmente entre essas duas threads
                    if(ratio % ((OMPNumThreads - 1) - indexTemp) == 0){
                        int ratioPerThread = ratio/((OMPNumThreads - 1) - indexTemp);
                        int lastIndex = staticDataMatrix[indexTemp][intervalIndex].endIndex;
        
                        
                        while(indexTemp < (OMPNumThreads - 1)){
                            indexTemp++;
                            staticDataMatrix[indexTemp][intervalIndex].beginIndex = passo * lastIndex + 1;
                            staticDataMatrix[indexTemp][intervalIndex].endIndex = staticDataMatrix[indexTemp][intervalIndex].beginIndex + (ratioPerThread - 1)*passo;
                            
                            lastIndex = staticDataMatrix[indexTemp][intervalIndex].endIndex;
                            if(indexTemp == OMPNumThreads - 1) {
                            indexTemp = 0;
                            intervalIndex++;
                            }
                        }
                       for(int i = 0;i < intervalIndex || i == 0;i++){
                            for(int j = 0; j < OMPNumThreads;j++){
                                threadArgument[j].staticDataMatrixStruct[i][j] = staticDataMatrix[i][j];
                            }
                        }
                        for (int i = 0; i < OMPNumThreads; i++) {
                            if (intervalIndex > 0)
                                for (int j = 0; j < intervalIndex; j++) {
                                    threadArgument[i].passo = passo;
                                    threadArgument[i].intervalIndex = intervalIndex;
                                    pthread_create(&threadArray[i], NULL, rotinaStatic, (void*) &threadArgument[i]);
                                }
                            else {
                                threadArgument[i].passo = passo;
                                pthread_create(&threadArray[i], NULL, rotinaStatic, (void*) &threadArgument[i]);
                            }
                            
                        }
                        for (int i = 0; i < OMPNumThreads; i++) pthread_join(threadArray[i], NULL);

                    } else {
                        //Se não ele faz um cálculo para ver quantos indices cada thread deve receber, geralmente colocando o resto desse cálculo nas outras threads se houver mais de uma
                        
                        int ratioPerThreadRemainder = ratio % OMPNumThreads; // O resto do que sobrou pelo número de threads
                        int ratioInitial = (final - inicio) / passo; //Indice total
                        int ratioRemainder = ratioInitial - chunkSizeSum; //O que sobrou de indices
                        int ceilRatioRemainder = ceil(ratioRemainder / ratioPerThreadRemainder) + 1; //O que sobrou de indices dividido pelo resto de indices que cada thread deve receber
                        int lastIndex = staticDataMatrix[indexTemp][intervalIndex].endIndex; //ultimo indice processado
                        while(ratioRemainder >= ceilRatioRemainder){
                            indexTemp++;
                            staticDataMatrix[indexTemp][intervalIndex].beginIndex = (lastIndex + 1) * passo;
                            staticDataMatrix[indexTemp][intervalIndex].endIndex = staticDataMatrix[indexTemp][intervalIndex].beginIndex + (ceilRatioRemainder - 1) * passo;
        
                            lastIndex = staticDataMatrix[indexTemp][intervalIndex].endIndex;
                            if(indexTemp == OMPNumThreads - 1) {
                            indexTemp = 0;
                            intervalIndex++;
                            }
                            ratioRemainder -= ceilRatioRemainder;
                                
                        } 
                        while(indexTemp < (OMPNumThreads - 1)){
                            indexTemp++;
                            staticDataMatrix[indexTemp][intervalIndex].beginIndex = lastIndex + 1;
                            staticDataMatrix[indexTemp][intervalIndex].endIndex = staticDataMatrix[indexTemp][intervalIndex].beginIndex + ratioRemainder;

                            lastIndex = staticDataMatrix[indexTemp][intervalIndex].endIndex;
                            

                        }
                        for(int i = 0;i < intervalIndex || i == 0;i++){
                            for(int j = 0; j < OMPNumThreads;j++){
                                threadArgument[j].staticDataMatrixStruct[i][j] = staticDataMatrix[i][j];
                            }
                        }
                        for (int i = 0; i < OMPNumThreads; i++) {
                            if (intervalIndex > 0)
                                for (int j = 0; j < intervalIndex; j++) {
                                    threadArgument[i].passo = passo;
                                    threadArgument[i].intervalIndex = intervalIndex;
                                    pthread_create(&threadArray[i], NULL, rotinaStatic, (void*) &threadArgument[i]);
                                }
                            else {
                                threadArgument[i].passo = passo;
                                pthread_create(&threadArray[i], NULL, rotinaStatic, (void*) &threadArgument[i]);
                            }
                            
                        }
                       
                    }
                       
            
            break;
            }

        
    }
}

}