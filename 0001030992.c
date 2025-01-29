/*
    Michele     Olivieri    0001030992      A       michele.olivieri3@studio.unibo.it
    ToDo: verifica con valgrind lanciando: valgrind ./nome_eseguibile nome_file_di_input
*/

#include <stdio.h>
#include <stdlib.h>

#define INF INT_MAX

typedef struct {
    int x, y, cost;
} Node;

typedef struct {
    Node *data;
    int size, capacity;
} MinHeap;

/* inizializza l'heap vuoto con un vettore di capacity elementi*/
void heap_init(MinHeap *heap, int capacity);

/* aggiunge un elemento all'heap */
void heap_push(MinHeap *heap, Node node);

/* estrae il primo elemento */
Node heap_pop(MinHeap *heap);

/* calcola il cammino di costo minimo utilizzando il MinHeap come priority queue*/
int dijkstra(int **heights, int n, int m, int Ccell, int Cheght);


int main(int argc, char *argv[]) {

    FILE *file;
    int **heights; 
    int Ccell, Cheight, n, m, i, j, result;

    /* termina con errore in caso di numero di agomenti passati non validi */
    if (argc != 2){
        fprintf(stderr, "Usare: %s <nome_file_di_input>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* termina con errore in caso il file di input passato sia vuoto o nullo */
    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return EXIT_FAILURE;
    }

    /* procesa il file di input considerando le specifiche date */
    fscanf(file, "%d %d %d %d", &Ccell, &Cheight, &n, &m);
    heights = (int**)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        heights[i] = (int *)malloc(m * sizeof(int));
        for (j = 0; j  < m; j++) {
            fscanf(file, "%d", &heights[i][j]);
        }
    }
    fclose(file);

    /* calcola il cammino di costo minimo e lo stampa */
    result = dijkstra(heights, n, m, Ccell, Cheight);
    printf("%d", result);

    /* libero la memoria allocata */
    for (i = 0; i < n; i++) free(heights[i]);
    free(heights);

    return EXIT_SUCCESS;
}

void heap_init(MinHeap *heap, int capacity) {
    heap->capacity = capacity;
    heap->size = 0;
    heap->data = (Node*)malloc(capacity * sizeof(Node));
}

void heap_push(MinHeap *heap, Node node) {
    Node temp;
    int i, parent;
    if (heap->size == heap->capacity)   return;
    heap->data[heap->size] = node;
    i = heap->size++;
    while (i > 1){
        parent = (i-1) / 2;
        if (heap->data[parent].cost <= heap->data[i].cost) break;
        temp = heap->data[i];
        heap->data[i] = heap->data[parent];
        heap->data[parent] = temp;
        i = parent;
    }    
}

Node heap_pop(MinHeap *heap) {
    Node top = heap->data[0], temp;
    int i = 0, left, right, min;
    heap->data[0] = heap->data[--heap->size];
    while (i * 2 + 1 < heap->size){
        left = i * 2 + 1, right = i * 2 + 2, min = i;
        if (heap->data[left].cost < heap->data[i].cost) min = left;
        if (right < heap->size && heap->data[right].cost < heap->data[min].cost)  min = right;
        if (min == i)   break;
        temp = heap->data[i];
        heap->data[i] = heap->data[min];
        heap->data[min] = temp;
        i = min;
    }    
    return top;
}

/* stampa il percorso usando la ricorsione visto che partiamo dal nodo destinazione */
void print_path( int **prev, int m, int x, int y) {
    if (prev[x][y] == -1) {
        printf("%d,%d\n", x,y);
        return;
    }
    /* risale il percorso e stampa dopo la ricorsione */
    print_path(prev, m, prev[x][y] / m, prev[x][y] % m);
    printf("%d,%d\n", x, y);
}

int dijkstra(int **heights, int n, int m, int Ccell, int Cheght) {

    MinHeap heap;
    Node temp, current = {0,0,0};
    int direction[4][2] = {{0,1} , {1,0} , {0,-1}, {-1,0}};
    int i, j, d, x, y, nx, ny, cost, result, heightDiff;
    int **dist = (int **)malloc(n * sizeof(int *)); /* matrice per il calcolo delle distanze */
    int **prev = (int **)malloc(n * sizeof(int *)); /* matrice per tracciare il percorso */
    for (i = 0; i < n; i++) {
        dist[i] = (int*)malloc(m * sizeof(int));
        prev[i] = (int*)malloc(m * sizeof(int));
        for (j = 0; j < m; j++) {
            dist[i][j] = INF;
            prev[i][j] = -1;
        }
    }
    dist[0][0] = Ccell;
    
    /* usiamo l'heap come priority queue per tenere traccia dei nodi da esplorare */
    heap_init(&heap, m * n);
    heap_push(&heap, current);

    while (heap.size != 0) {
        current = heap_pop(&heap);
        x = current.x;
        y = current.y;
        /* analizzo il nodo solo se il suo costo è conventite */
        if (current.cost > dist[x][y]) continue;

        /* controllo le ipotetiche 4 direzioni che potrei percorrere */
        for (d = 0; d < 4; d++) {
            nx = x + direction[d][0];
            ny = y + direction[d][1];
            /* verifico che l'ipotetico nodo in posizione (nx,ny) sia nella matrice */
            if (nx >= 0 && nx < n && ny >= 0 && ny < m) {
                heightDiff = heights[x][y] - heights[nx][ny];
                cost = Ccell + Cheght * heightDiff * heightDiff;
                /* se il nuovo percorso ha un costo migliore viene aggiunto all'heap */
                if (dist[x][y] + cost < dist[nx][ny]) {
                    dist[nx][ny] = dist[x][y] + cost;
                    /* memorizzo il predecessore in modo da poter estrarre x e y */
                    prev[nx][ny] = x * m + y; 
                    temp.x = nx;
                    temp.y = ny;
                    temp.cost = dist[nx][ny];
                    heap_push(&heap, temp);
                }
            }
        }
    }
    
    /* assegno il valore del costo del cammino minimo che ritornerà la mia funzione*/
    result = dist[n-1][m-1];
    print_path(prev, m, n-1, m-1);
    printf("-1,-1\n"); /* indica la fine del percorso */

    /* libero la memoria precedentemente allocata */
    for( i = 0; i < n; i++ ) {
        free(dist[i]);
        free(prev[i]);
    }
    free(dist);
    free(prev);
    free(heap.data);
    
    return result;
}