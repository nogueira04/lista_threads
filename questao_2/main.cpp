#include <iostream>
#include <queue>

#define UNVISITED 0
#define VISITED 1

template <typename E>
class Link {
    public:
        Link* next;
        E element;

    public:
        Link(const E& elemval, Link* nextval = nullptr) {
            element = elemval;
            next = nextval;
        }

        Link(Link* nextval = nullptr) {
            next = nextval;
        }
};

template <typename E>
class List {
    public:

        virtual void clear() = 0;
        // virtual void print() = 0;
        virtual void insert(const E& it) = 0;
        virtual void append(const E& it) = 0;
        virtual E remove() = 0;
        virtual void moveToStart() = 0;
        virtual void moveToEnd() = 0;
        virtual void prev() = 0;
        virtual void next() = 0;
        virtual int length() const = 0;
        virtual int currPos() const = 0;
        virtual void moveToPos(int& it) = 0;
        virtual const E& getValue() const = 0;
};

template <typename E>
class LList : public List<E> {
    private:
        Link<E>* head;
        Link<E>* tail;
        Link<E>* curr;
        int size;

    public:
        LList() {
            head = tail = curr = new Link<E>();
        }

        ~LList() {
            clear();
        }

        void clear() override {
            while (head != nullptr) {
                curr = head;
                head = head->next;
                delete curr;
            }
        }

        // void print() override {
        //     Link<E>* temp = head->next;
        //     while (temp != nullptr) {
        //         std::cout << temp->element << std::endl;
        //         temp = temp->next;
        //     }
        // }

        void insert(const E& it) override {
            Link<E>* newNode = new Link<E>(it, curr->next);
            curr->next = newNode;
            if (tail == curr) tail = curr->next;
            size++;
        }

        void append(const E& it) override {
            Link<E>* newNode = new Link<E>(it, nullptr);
            tail->next = newNode;
            tail = newNode;
            size++;
        }

        E remove() override {
            Link<E>* tmp = curr->next;
            E it = tmp->element;
            curr->next = curr->next->next;
            if (tail == curr->next) tail = curr;
            delete tmp;
            size--;

            return it;
        }

        void moveToStart() override {
            curr = head;
        }

        void moveToEnd() override {
            curr = tail;
        }

        void next() override {
            if (curr == tail) return;
            curr = curr->next;
        }

        void prev() override {
            if (curr == head) return;
            Link<E>* tmp = head;
            while (tmp != curr) {
                tmp = tmp->next;
            }
            curr = tmp;
        }

        int length() const override {
            return size;
        }

        int currPos() const override {
            Link<E>* tmp = head;
            int i = 0;
            for (; tmp != curr; i++) tmp = tmp->next;
            
            return i;
        }

        void moveToPos(int& it) override {
            if (it < 0 || it >= size) return;
            curr = head;
            for (int i = 0; i < it; i++) curr = curr->next;
        }

        const E& getValue() const override {
            if (curr->next == nullptr) abort();
            return curr->next->element;
        }
};

class Graph {
    private:
        void operator = (const Graph&) {}    // Atribuição (?)
        Graph(const Graph&) {}               // Construtor cópia (?)

    public:

        Graph() {}          // Construtor padrão
        virtual ~Graph() {} // Destrutor base

        // Inicializar um grafo de n vértices
        virtual void Init(int n) = 0;

        // Retornam o número de vértices e arestas, respectivamente
        virtual int n() = 0;
        virtual int e() = 0;

        // Retorna o primeiro vizinho de v
        virtual int first(int v) = 0;

        // Retorna o próximo vizinho de v
        virtual int next(int v, int w) = 0;

        // Define o peso de uma aresta
        // i, j: Vértices
        // wght: Peso da aresta
        virtual void setEdge(int v1, int v2, int wght) = 0;
        
        // Apaga uma aresta 
        // i, j: Vértices
        virtual void delEdge(int v1, int v2) = 0;

        // Determina se uma aresta está no grafo 
        // i, j: Os vértices 
        // Retorna verdadeiro se a aresta i, j possui um peso diferente de zero
        virtual bool isEdge(int i, int j) = 0;

        // Retorna o peso de uma aresta
        // i, j: Vértices
        // Retorna o peso da aresta i, j ou zero
        virtual int weight(int v1, int v2) = 0;

        // Retorna e define o valor mark para um vértice
        // v: O vértice
        // val: O valor que deve ser usado para definir
        virtual int getMark(int v) = 0;
        virtual void setMark(int v, int val) = 0;
        
};

class Edge {
    private:
        int vert, wt;

    public:
        Edge() { vert = -1; wt = -1; }
        Edge(int v, int w) { vert = v; wt = w; }
        
        int vertex() { return vert; }
        int weight() { return wt; }
};

class Graphl : public Graph {
    private:
        List<Edge>** vertex;        // Cabeças da lista
        int numVertex, numEdge;          // Número de vértices, arestas
        int *mark;                       // Ponteiro para o vetor de marcações

    public:
        Graphl(int numVert) { Init(numVert); }

        ~Graphl() {
            delete [] mark;
            for (int i = 0; i < numVertex; i++) delete [] vertex[i];
            delete [] vertex;
        }

        void Init(int n) {
            int i;
            numVertex = n;
            numEdge = 0;
            mark = new int[n];
            for (i = 0; i < numVertex; i++) mark[i] = UNVISITED;

            vertex = (List<Edge>**) new List<Edge>*[numVertex];
            for (i = 0; i < numVertex; i++)
                vertex[i] = new LList<Edge>();
        }

        int n() { return numVertex; }
        int e() { return numEdge; }

        int first(int v) {
            if (vertex[v]->length() == 0)
                return numVertex;
            vertex[v]->moveToStart();
            Edge it = vertex[v]->getValue();
            return it.vertex();
        }

        int next(int v, int w) {
            Edge it;
            if (isEdge(v, w)) {
                if ((vertex[v]->currPos() + 1) < vertex[v]->length()) {
                    vertex[v]->next();
                    it = vertex[v]->getValue();
                    
                    return it.vertex();
                }
            }

            return n();
        }

        void setEdge(int i, int j, int weight) {
            if (weight <= 0) return;

            Edge currEdge(j, weight);
            if (isEdge(i, j)) {
                vertex[i]->remove();
                vertex[i]->insert(currEdge);
            } else {
                numEdge++;
                for (vertex[i]->moveToStart(); vertex[i]->currPos() < vertex[i]->length(); vertex[i]->next()) {
                    Edge temp = vertex[i]->getValue();
                    if (temp.vertex() > j) break;
                }
                vertex[i]->insert(currEdge);
            }
        }

        void delEdge(int i, int j) {
            if (isEdge(i, j)) {
                vertex[i]->remove();
                numEdge--;
            }
        }

        bool isEdge(int i, int j) {
            Edge it;
            for (vertex[i]->moveToStart(); vertex[i]->currPos() < vertex[i]->length(); vertex[i]->next()) {
                Edge temp = vertex[i]->getValue();
                if (temp.vertex() == j) return true;
            }

            return false;
        }

        int weight(int i, int j) {
            Edge curr;
            if (isEdge(i, j)) {
                curr = vertex[i]->getValue();
                return curr.weight();
            } else return 0;
        }

        int getMark(int v) { return mark[v]; }
        void setMark(int v, int val) { mark[v] = val; }
        
};

void DFS(Graph* G, int v) {
    // previsit
    std::cout << v << std::endl;
    G->setMark(v, VISITED);
    for (int w = G->first(v); w < G->n(); w = G->next(v, w)) 
        if (G->getMark(w) == UNVISITED)
            DFS(G, w);
    // postvisit
    
}

void BFS(Graph* G, int start, std::queue<int>* Q) {
    int v, w;
    Q->push(start);
    G->setMark(start, VISITED);
    while (Q->size() != 0) {
        v = Q->front();
        Q->pop();
        // previsit
        std::cout << v << std::endl;
        for (w = G->first(v); w < G->n(); w = G->next(v, w))
            if (G->getMark(w) == UNVISITED) {
                G->setMark(w, VISITED);
                Q->push(w);
            }
    }
}

void graphTraverse(Graph* G) {
    std::queue<int>* queue = new std::queue<int>;
    int v;
    for (v = 0; v < G->n(); v++)
        G->setMark(v, UNVISITED);
    for (v = 0; v < G->n(); v++)
        if (G->getMark(v) == UNVISITED)
            BFS(G, v, queue);
}

int main() {

    Graphl* grafo = new Graphl(6);

    grafo->setEdge(0, 1, 1);
    grafo->setEdge(0, 2, 1);
    grafo->setEdge(0, 3, 1);
    grafo->setEdge(3, 5, 1);
    grafo->setEdge(3, 4, 1);

    graphTraverse(grafo);

    return 0;
}