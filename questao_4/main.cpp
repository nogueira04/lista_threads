#include <iostream>
#include <pthread.h>

class ParPtrTree{
    private:
        int size;
        int* array;

    public:
        ParPtrTree() { size = 0; }

        ParPtrTree(int s) {
            size = s;
            array = new int[s];

            for (int i = 0; i < size; i++) array[i] = i;
        }

        ~ParPtrTree() {
            delete [] array;
        }

        int find (int curr) const { 
            if(array[curr] == curr) return curr;
            array[curr] = find(array[curr]);

            return array[curr];
        }

        void UNION(int a, int b) {
            int root1 = find(a);
            int root2 = find(b);
            
            if(root1 != root2) array[root2] = root1;
        }

        bool differ(int a,int b) {
            if(find(a) != find(b)) return true;
            else return false;
        }

        int count_sets() {
            int index = 0;
            for (int i = 0; i < size; i++) {
                if (array[i] == i) index++;
            }

            return index;
        }

        void print_array() {
            for (int i = 0; i < size; i++) std::cout << array[i] << " ";
            std::cout << std::endl;
        }
};

int N_THREADS_MAX;
int all_zero = 1; // flag para caso não exista ilhas
int **matrix;
ParPtrTree* disjoint_set;

typedef struct { // args enviados pra rotina da thread
    int row_first, row_last;
    int rows, cols;
} data;

int map(int i, int j, int m) { // mapeia índice na matriz para a de um vetor
    return (i * m) + j;
}


void* set_islands(void* args) {

    data* t_data = (data*) args;
    int set_index, rows = t_data->rows, cols = t_data->cols;

    for (int i = t_data->row_first; i < t_data->row_last; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j]) { // lógica de checar adjacentes e adcionar no conjunto
                all_zero = 0;
                set_index = map(i, j, cols);
                if (i < rows - 1 && matrix[i + 1][j]) disjoint_set->UNION(set_index, map(i + 1, j, cols));
                if (i > 0 && matrix[i - 1][j]) disjoint_set->UNION(set_index, map(i - 1, j, cols));
                if (j < cols - 1 && matrix[i][j + 1]) disjoint_set->UNION(set_index, map(i, j + 1, cols));
                if (j > 0 && matrix[i][j - 1]) disjoint_set->UNION(set_index, map(i, j - 1, cols));
                if (i > 0 && j > 0 && matrix[i - 1][j - 1]) disjoint_set->UNION(set_index, map(i - 1, j - 1, cols));
                if (i > 0 && j < cols - 1 && matrix[i - 1][j + 1]) disjoint_set->UNION(set_index, map(i - 1, j + 1, cols));
                if (i < rows - 1 && cols > 0 && matrix[i + 1][j - 1]) disjoint_set->UNION(set_index, map(i + 1, j - 1, cols));
                if (i < rows - 1 && j < cols - 1 && matrix[i + 1][j + 1]) disjoint_set->UNION(set_index, map(i + 1, j + 1, cols));
            } else disjoint_set->UNION(0, map(i, j, cols));
        }
    }

    pthread_exit(NULL);

}

void print_matrix(int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) std::cout << matrix[i][j] << " ";
        std::cout << std::endl;
    }
}

int main() {

    int cols, rows, n_islands;

    std::cout << "Número de threads: ";
    std::cin >> N_THREADS_MAX;

    pthread_t thread_array[N_THREADS_MAX];
    data t_data_array[N_THREADS_MAX];

    std::cout << "Tamanho NxM da matriz: ";
    std::cin >> rows >> cols;

    disjoint_set = new ParPtrTree(rows * cols);

    matrix = new int*[rows];
    for (int i = 0; i < rows; i++)
        matrix[i] = new int[cols];

    std::cout << "Matriz: " << std::endl;
    for (int i = 0; i < rows; i++)   // montar matriz
        for (int j = 0; j < cols; j++) std::cin >> matrix[i][j];

    int thread_rows = rows / N_THREADS_MAX;
    int last_row = -1;
    
    for (int i = 0; i < N_THREADS_MAX; i++) {
        t_data_array[i].row_first = last_row + 1;
        t_data_array[i].row_last = (i == N_THREADS_MAX - 1 && rows % N_THREADS_MAX != 0) ? t_data_array[i].row_first + thread_rows + 1 : t_data_array[i].row_first + thread_rows;
        // para que a relação da divisão das linhas por threads ficasse correta, foi necessário somar + 1 na primeira linha da última thread, e apenas quando a divisão não fosse exata
        t_data_array[i].rows = rows;
        t_data_array[i].cols = cols;

        last_row = t_data_array[i].row_last - 1;

        pthread_create(&thread_array[i], NULL, set_islands, &t_data_array[i]);
    }

    for (int i = 0; i < N_THREADS_MAX; i++) pthread_join(thread_array[i], NULL);

    for (int i = thread_rows - 1; i < rows - 1; i += thread_rows) { // juntar ilhas na intersecção
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] && matrix[i + 1][j]) disjoint_set->UNION(map(i, j, cols), map(i + 1, j, cols));
        }
    }

    n_islands = disjoint_set->count_sets();

    std::cout << "Número de ilhas: " << (all_zero ? 0 : n_islands) << std::endl;

    return 0;
}