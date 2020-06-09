#ifndef MATRIZ_ESPARSA_H
#define MATRIZ_ESPARSA_H


//------------------------------------CABEÇALHO MATRIZ ESPARSA--------------------------------------------------------------------------------



typedef struct node node; // definicao do nohs
typedef struct list list; // definicao da lista
typedef struct matriz_esparsa MATRIZ_ESPARSA; // definicao da matriz esparsa


/*
Aloca memoria para um noh e o retorna inicializado

@value: o valor do noh

O(1)
*/
node *newNode(double value);



/*
Aloca memoria para uma lista e a retorna inicizada

O(1)
*/
list *newList(void);





/*
Desaloca toda memoria e destroi todos os dados de uma lista(incluindo os nohs)

O(n) n := numero de nohs
*/
void destroyList(list **);


/*
Le uma matriz de uma arquivo e salva os dados numa matriz previamente alocada

@path: caminho para a stream de dados

O(n^2), n := numero de elementos do arquivo
*/
MATRIZ_ESPARSA *ler_matriz(const char *path);




/*
Aloca memoria para uma matriz esparsa e a inicializa com as dimensoes @rows e @cols
O(n) onde n := max(rows, cols)
*/
MATRIZ_ESPARSA *criar_matriz(int rows, int cols);




/*
desaloca toda a memoria de uma matriz esparsa e destroi todos seus dados internos(listas e nohs)

O(n^2) n:= dimensao da matriz

*/

void apagar_matriz(MATRIZ_ESPARSA *); 




/*
Atribui um valor @value no slot de coordenada (@row, @col) da matriz esparsa corrente

O(n) n := numero de nohs

*/

int set_matriz(MATRIZ_ESPARSA *, int row, int col, double value);




/*

recupera um valor da matriz esparsa do slot (@row, @col)

O(n) n := numero de nohs

*/
double get_matriz(MATRIZ_ESPARSA *, int row, int col);





/*
Multiplica duas matrizes esparsas (@first x @second) e retorna a matriz resultante

O(n^3) n := numero de elementos nao nulos

*/

MATRIZ_ESPARSA *multiplicar_matriz(MATRIZ_ESPARSA *first, MATRIZ_ESPARSA *second);


/*
Soma duas matrizes esparsas (@first + @second) e retorna a matriz resultante

O(n^2) n := numero de elementos nao nulos

*/

MATRIZ_ESPARSA *somar_matriz(MATRIZ_ESPARSA *, MATRIZ_ESPARSA *);




/*
Transforma a matriz @curr em sua transposta e retorna um ponteiro para ela mesma

O(n^2) n:= dimensao

*/
MATRIZ_ESPARSA *transposta_matriz(MATRIZ_ESPARSA *curr);




/*
Calcula o determinante(Algoritmo de Chio)  da matriz esparsa e retorna seu valor


O(n^3) n := dimensao
*/
double determinante_matriz(MATRIZ_ESPARSA *);


/*
Imprime a matriz inteira na saida padrao
*/
void print_matriz(MATRIZ_ESPARSA *);

/*
Imprime os elementos nao-nulos na saida padrao
*/
void resumo_matriz(MATRIZ_ESPARSA *);

#endif





















