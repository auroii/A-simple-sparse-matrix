#include <stdlib.h>
#include <stdio.h>
#include "matriz_esparsa.h"

/*definicao da estrutura noh, que guarada o valor e os indices do elemento e dois ponteiros, @back para o proximo da coluna
 * e @next para o proximo da linha
 */
struct node {
	node *next, *back; // ponteiros
	double value; // valor do elemento
	int i, j; // indices
	double sum; // estrutura auxiliar para somar/multuplicar a matriz e reduzir a complexidade
};
/*def. de lista, possui um noh cabeca e tamanho, eh nada mais que uma sequencia encadeada de nohs
 */
struct list {
	node *head; // noh cabeca
	int size; // tamanho

};
/*def. da matriz esparsa, possui dois vetores dinamicos de listas cruzadas, sua dimensão e tamanho
 */
struct matriz_esparsa {
	list **rows, **cols; //vetores dinamicos de listas cruzadas
	int dim1, dim2, size;//dimensao e tamanho da matriz
};

/*cria um no e retorna um ponteiro para o mesmo
 */

node *newNode(double value) {
	node *new = (node *) malloc(sizeof(node)); //aloca memoria
	if(new != NULL) { // verifica se deu certo
		new->next = NULL; // inicializa os elementos com valores padrao
		new->back = NULL;
		new->value = value;
		new->sum = 0;
	}
	return new; // returna um ponteiro para o noh
}

/*cria uma lista e retorna um ponteiro para a mesma
 */

list *newList() {
	list *new = (list *) malloc(sizeof(list)); //aloca memoria
	if(new != NULL) { // verifica se a alocacao deu certo
		new->head = newNode(-1); // cria o no cabeça com valor -1
		//new->end = NULL; inutil para esse trabalho
		new->size = 0;//seta o tamanho inicial como 0
	}
	
	return new; //retorna um ponteiro para o lista criada
}

/*remove todos os noh cujo elemento eh zero, faz isso com duas listas cruzadas entre si, uma representando a coluna e a outra a linha
 */

void remove_zeros(list *curr1, list *curr2) {
	node *it, *rm; //declara ponteiros auxiliares para percorrer as listas
	node **update = (node **) malloc(sizeof(node *) * 1001); // aloca um vetor auxiliar para apontar para os nohs com zero's
	int cnt = 0, i; // contadores
	for(it = curr1->head; it->next != NULL; ) { // loop na lista 1
		if(it->next->value == 0) { // se o elemento do noh for zero, aponto para esse noh com @update[] e
					//o remove sem desalocar da lista, para poder fazer o mesmo com a lista 2
			rm = it->next; 	
			it->next = it->next->next;
			update[cnt++] = rm;
			curr1->size--;
		} else it = it->next;
	}
	for(it = curr2->head; it->back != NULL; ) {// loop na lista 2
		if(it->back->value == 0) { // faz a mesma coisa que o loop anterior, entretanto, nao coloca o no em @update[], pois esse nos sao 
					//iguais pela definicao de lista cruzada
			rm = it->back;	
			it->back = it->back->back;
			curr2->size--;
		} else it = it->back;
	}
	for(i = 0; i < cnt; ++i) {//loop para apagar os nos com zero das duas listas
		if(update[i] != NULL) {
			free(update[i]);
			update[i] = NULL;
		}
	}
	free(update);//desaloca o vetor auxiliar
}

/*apaga uma lista e seta seu endereco como NULL
 */
void destroyList(list **rm) {
	if(rm != NULL && *rm != NULL) { //verifica a integridade do ponteiro
		node *it, *t;//nohs para percorrer a lista
		for(it = (*rm)->head; it != NULL;) { // loop na lista
			t = it; //processo de remocao de nohs
			it = it->next;
			free(t);
		}
		free(*rm);//libero a lista
		*rm = NULL;//seto como NULL
	}
}

		
		

			
//----------------------------------------SPARSE_MATRIX--------------------------------------------

/*cria uma matriz esparsa com dimensoes @row e @col e retorna um ponteiro para o mesma
 */
MATRIZ_ESPARSA *criar_matriz(int rows, int cols) {
	MATRIZ_ESPARSA *new = (MATRIZ_ESPARSA *) malloc(sizeof(MATRIZ_ESPARSA)); // aloca memoria
	if(new != NULL) { // verifica se deu certo a alocacao
		new->dim1 = rows;   //inicializa com as dimensoes 
		new->dim2 = cols;
		new->size = 0;
		new->rows = (list **) malloc(sizeof(list *)*rows); //aloca memoria pros vetores de listas cruzadas	
		new->cols = (list **) malloc(sizeof(list *)*cols);

		int i; //contador
		for(i = 0; i < rows; ++i) new->rows[i] = newList(); // aloca memoria para cada uma das listas
		for(i = 0; i < cols; ++i) new->cols[i] = newList();
	}
	return new; //retorna o ponteiro
}	
	
/*le a matriz de uma arquivo de nome @path
 */
MATRIZ_ESPARSA *ler_matriz(const char *path) {
	FILE *stream = fopen(path, "r"); // abre o arquivo para leitura e o localiza com o ponteiro @stream
	int dim1, dim2, i, j;// dimensoes e contadores
	double element;//valor do elemento corrente
	fscanf(stream, "%d %d", &dim1, &dim2);//leio as dimensoes
	MATRIZ_ESPARSA *m = criar_matriz(dim1, dim2);//cria a matriz com as mesmas dimensoes
	
	for(i = 0; i < dim1; ++i) {
		for(j = 0; j < dim2; ++j) { //loop no arquivo
			fscanf(stream, "%lf", &element);// leio o elemento
			set_matriz(m, i, j, element);//seto na matriz com seus respectivos indices
		}
	}
	fclose(stream);//fecho o arquivo
	 
	return m;//retorna um ponteiro para a matriz
}

/*desaloca uma matriz da memoria
 */		

void apagar_matriz(MATRIZ_ESPARSA *rm) {
	if(rm != NULL) { //verifica a integridade do ponteiro
		int i; //contador
		for(i = 0; i < rm->dim1; ++i) { //loop no vetor de linhas
			destroyList(&(rm->rows[i]));//desaloca e deleta os nos das listas das linhas
		}
		for(i = 0; i < rm->dim2; ++i) {//loop no vetor de colunas
			free(rm->cols[i]->head); //apenas desaloca a lista e seu no cabeca, pois seus nohs ja foram apagados no loop anterior
			free(rm->cols[i]);
		}

		free(rm->rows);//libera os vetores e a matriz
		free(rm->cols);
		free(rm);
	}
}

/*seta um elemento @value de indices @row e @col na matriz @curr e retorna se deu certo
 */
int set_matriz(MATRIZ_ESPARSA *curr, int row, int col, double value) {
	if(curr != NULL && row < curr->dim1 && col < curr->dim2) { //verifica o ponteiro @curr e se os indices sao validos
		node *it; // noh contador
		for(it = curr->rows[row]->head->next; it != NULL; it = it->next) {//loop na linha coluna
			if(it->i == row && it->j == col) { //se ja existir um noh alocado com esses indices, apenas muda o valor
				it->value = value;
				
				remove_zeros(curr->rows[row], curr->cols[col]); // remove possiveis zeros
				return 1;//deu certo
			}

		}
		node *new = newNode(value); //aloco um novo noh com o valor @value e com os indices passados como parametros
		new->i = row; 
		new->j = col;
			
		for(it = curr->rows[row]->head; it->next != NULL; it = it->next) {//loop no vetor linha
			if(it->next->j < col) {// se o indice coluna do proximo noh for menor, entao insere entre noh atual e o proximo
				new->next = it->next; // insercao e incremento do tamanho da lista
				it->next = new;
				curr->rows[row]->size++;
				break;
			}
		}
		if(it->next == NULL) {//o noh sera inserido no final da lista caso chegue aqui
			it->next = new;
			curr->rows[row]->size++;
		}
		for(it = curr->cols[col]->head; it->back != NULL; it = it->back) { // loop no vetor coluna
			if(it->back->i < row) { // se o indice linha do proximo noh for menor, entao insere entre o noh atual e o proximo
				new->back = it->back; // insercao e incremento
				it->back = new;
				curr->cols[col]->size++;
				break;
			}
		}
		if(it->back == NULL) {// se chegar aqui, entao o noh sera inserido no final da lista coluna
			it->back = new;
			curr->cols[col]->size++;
		}
			
		remove_zeros(curr->rows[row], curr->cols[col]); // remove possiveis zeros
		return 1; // deu certo
	}
	return 0; //deu errado
}
		
/*imprime os elementos da matriz na saida padrao
 */
void print_matriz(MATRIZ_ESPARSA *curr) {
	if(curr != NULL) {//verifica a integridade do ponteiro
		int i, j;//contadores
		printf("%d %d\n", curr->dim1, curr->dim2);//printo as dimensoes
		for(i = 0; i < curr->dim1; ++i) { // loop na matriz
			for(j = 0; j < curr->dim2; ++j) 
				printf("%.2lf ", get_matriz(curr, i, j)); //printo os elementos
			puts("");// quebra de linha
		}
	}
}

//printa apenas os elementos nao-nulos da matriz
void resumo_matriz(MATRIZ_ESPARSA *curr) {
	if(curr == NULL) return;// verifica a integridade do ponteiro
	int i;//contador
	node *it;//noh iterador
	for(i = 0; i < curr->dim1; ++i) { // loop no vetor de linhas
		it = curr->rows[i]->head->next;// seta o iterator como o primeiro noh da lista atual
		while(it != NULL) { // loop na lista atual
			if(it->value != 0) printf("matriz[%d][%d] = %.2lf\n", it->i, it->j, it->value); //se o elemento nao for nulo, imprime o mesmo
			it = it->next;// incrementa o iterador
		}
	}
}

/*copia um noh com alocacao
 */
node *cpyNode(node *curr) {
	if(curr != NULL) {//verifico o ponteiro
		node *new = newNode(curr->value); //crio outro noh com o mesmo valor do original
		if(new != NULL) {
			new->i = curr->i; // os indices e a soma serao iguais
			new->j = curr->j;
			new->sum = curr->sum;
		}
		return new;// retorna o ponteriro para o novo noh
	}
	return NULL; // se der errado, retorna NULL
	
}

/*soma duas matrizes esparsas e retorna a matriz resultante
 *
 * Neste codigo preferiu-se adotar um algoritmo alternativo para somar as matrizes para a complexidade ser proporcional ao numero de elementos nao
 * nulos ao inves das dimensoes das matrizes parcela
 */
MATRIZ_ESPARSA *somar_matriz(MATRIZ_ESPARSA *first, MATRIZ_ESPARSA *second) {
	if(first != NULL && second != NULL && first->dim1 == second->dim1 && second->dim2 == first->dim2) { // verifica integridade dos ponteiros
		//@first e @second e se possuem a mesma dimensao
		MATRIZ_ESPARSA *new = criar_matriz(first->dim1, first->dim2);// cria a matriz resultante
		node *aux1, *aux2;//nohs auxiliares
		int i, j, k;//contadores
		for(i = 0; i < first->dim1; ++i) {//loop no vetor linha da matriz @first
			aux1 = first->rows[i]->head->next;// defini @aux1 como sendo o primeiro elemento da lista de linhas de @first
			aux2 = second->rows[i]->head->next; // defini @aux2 como sendo o primeiro elemento da lista de linhas de @second
			j = k = 0;//seta os contadores como zero

			while(j < first->rows[i]->size && k < second->rows[i]->size) { // loop para percorrer os nohs respeitando os indices
				if(aux1->j < aux2->j) { // se o indice da linha de @first for menor que o de @second
					set_matriz(new, aux1->i, aux1->j, aux1->value); // seta a matriz resultando com o valor de @first(soma
											// com zero)
					aux1 = aux1->next; // avanco aux1
					j++;//incremento j
				}
				else if(aux1->j == aux2->j) { // se os indices forem iguais
					if(aux1->value + aux2->value != 0) // e a sua de seus elementos for nao nula
						set_matriz(new, aux1->i, aux1->j, aux1->value + aux2->value); // seta a resultando com a soma
														// dos elementos
					aux1 = aux1->next;// avanca ambos iteradores
					aux2 = aux2->next;
					j++;//incrementa ambos os contadores
					k++;
				}
				else { // do contario apenas soma com @second[i][j]
					set_matriz(new, aux2->i, aux2->j, aux2->value); //seta a resultante com o valor em @second
					aux2 = aux2->next;//avanco o iterador de @second
					k++;//avanco o contador
				}
			}
			
			if(j == first->rows[i]->size) // se acabou os elementos de @first antes
				while(k < second->rows[i]->size) { // soma o restante de @second
					set_matriz(new, aux2->i, aux2->j, aux2->value); // seta os valores
					aux2 = aux2->next; // avanca os ponteiros e contadores
					k++;
			}
			
			if(k == second->rows[i]->size) //se acabou os elementos de @second antes
				while(j < first->rows[i]->size) { // soma o restante de @first
					set_matriz(new, aux1->i, aux1->j, aux1->value); // seta os valores
					aux1 = aux1->next; // avanca os ponteiros e contadores
					j++;
				}
		}
		return new;// retorno o pontero para a matriz esparsa resultante
	} 
	return NULL; // se der algo errado retorno NULL
}

/*multiplica duas matrizes esparsas e retorna a matriz resultante
 *
 * Neste codigo preferiu-se adotar um algoritmo alternativo para multiplicar as matrizes para a complexidade ser
 * proporcional ao numero de elementos nao nulos ao inves das dimensoes das matrizes fator
 */

MATRIZ_ESPARSA *multiplicar_matriz(MATRIZ_ESPARSA *first, MATRIZ_ESPARSA *second) {
	if(first != NULL && second != NULL && first->dim2 == second->dim1) { // verifica a integridade dos ponteiros e a coerencia das dimensoes
		MATRIZ_ESPARSA *res = criar_matriz(first->dim1, second->dim2); // cria uma matriz resultante com as dimensoes especificadas

		node *aux1, *aux2;// nohs iteradores
		int i, j, k, l, sum;// variaveis auxiliares
		for(i = 0; i < first->dim1; ++i) {  // loop aninhado na dimensao da matriz resultante
			for(j = 0; j < second->dim2; ++j) {
				l = sum = k = 0; // seta @l e @k e a soma temporaria como zero
				aux1 = first->rows[i]->head->next; // seta os noh iteradores no inicio das listas linha e coluna
				aux2 = second->cols[j]->head->back;
				
				while(k < first->rows[i]->size && l < second->cols[j]->size) { // loop na linha de @first e na coluna de @second
					if(aux1->j == aux2->i) { // se os indices casarem, realiza a operacao caracteristica da multiplicacao de
								//matrizes
						sum += (aux1->value)*(aux2->value); //soma na variavel sum
						aux1 = aux1->next; // incrementa os iteradores
						aux2 = aux2->back;
						k++;//incrementa os contadores
						l++;
					}
					else if(aux1->j < aux2->i) { // se os indices nao casarem, significa que um fator eh nulo, logo, deve
									// ser ignorado e apenas avancado
						k++;
						aux1 = aux1->next;
					} else { // a mesma coisa do condicional anterior
						l++;
						aux2 = aux2->back;
					}
				}
				
				if(sum != 0) set_matriz(res, i, j, sum); // se a soma for diferente de nulo, seta na matriz o novo valor
			}
		}
		return res;// retorna um ponteiro pra matriz esparsa resultante
	}
	return NULL; // se der errado, retorna NULL
}		
	

/* pego o elemento de indices @row e @col na matriz esparsa @curr
 */
double get_matriz(MATRIZ_ESPARSA *curr, int row, int col) {
	if(curr != NULL && row < curr->dim1 && col < curr->dim2) { // integridade de @curr e dimensoes corretas
		list *tmp = curr->rows[row];//lista temporaria que aponta para a lista da linha @row
		node *it;//noh iterador
		for(it = tmp->head->next; it != NULL; it = it->next) {//loop na lista temporaria @tmp
			if(it->j == col) return it->value; //se o indice coluna do noh for igual a @col, entao retorno este valor
		}
	}

	return 0; // do contrario retorno zero
}
		
/*transforma a matriz esparsa @m em sua transposta
 */
MATRIZ_ESPARSA *transposta_matriz(MATRIZ_ESPARSA *m) {
	if(m == NULL) return NULL; // verifico o ponteiro
	
	int i, j;//contadores
	double val, val2;//variaveis auxiliares para fazer a troca
	for(i = 0; i< m->dim1; ++i) { 
		for(j = i+1; j < m->dim2; ++j) { // loop no primeiro triangulo da matriz
			val = get_matriz(m, i, j); // pega o valor de (i, j)
			val2 = get_matriz(m, j, i); // pega o valor de (j, i) 

			set_matriz(m, i, j, val2); // coloca-os em suas posicoes invertidas
			set_matriz(m, j, i, val);
		}
	}
	return m;//retorna a propria matriz
}

//copia uma matriz esparsa
MATRIZ_ESPARSA *copiar_matriz(MATRIZ_ESPARSA *m) {
	MATRIZ_ESPARSA *res = criar_matriz(m->dim1, m->dim2); // cria a matriz destinio da copia
	node *it;//noh iterador
	int i;//contador
	for(i = 0; i < m->dim1; ++i) { // loop aninhado pelas listas do vetor de listas linhas
		for(it = m->rows[i]->head->next; it != NULL; it = it->next) {
			set_matriz(res, it->i, it->j, it->value); //seta a matriz @res com os elementos correspondente de @m
		}
	}

	return res; // retorno @res como copia
}


//troca duas linhas de indice @i e @j de respectivamente
void swapLine(MATRIZ_ESPARSA *m, int i, int j) {
	list *temp = m->rows[i];
	m->rows[i] = m->rows[j];
	m->rows[j] = temp;
}

/*tranforma o elemento m[index][index] da matriz esparsa em 1 utilizando operacoes invariantes da deternimante
 */
int toOne(MATRIZ_ESPARSA *m, int index){
	double mono = get_matriz(m, index, index); //pega o elemento de m[index][index]
	if(mono == 1) return 1; //se ja for 1, nao faz nada
	double curr;//valor atual
	int signal = 1, i, j, p; // variaveis auxiliares
	
	for(i = index; i < m->dim1; ++i) { //loop aninhado pela sub matriz que comeca em matriz[index][index+1]
		for(j = index+1; j < m->dim2; ++j) {
			
			if((curr = get_matriz(m, i, j)) != 0) { //se o elemento for diferente de zero
				if(i != index) { //se a linha do elemento nao nulo for diferente de index
					signal *= -1; // multiplica @signal por -1 para inverter o sinal do determinante
					swapLine(m, i, index); // troco de linha i com index
					
				}
				mono = get_matriz(m, index, index);// pega o primeiro elemento da nova matriz 
				double k = (1-mono)/(double) curr;//seta k como uma contanste de modo que a coluna j vezes k mais a coluna
								// index tenha o elemento @m[index][index] igual a 1
				for(p = index; p < m->dim1; ++p) { // loop na coluna index
					set_matriz(m, p, index, get_matriz(m, p, index) + k*get_matriz(m, p , j)); //soma de multiplo de coluna
														   //com coluna 
				}
				return signal; // deu certo e retorna o sinal que a determinante tem atualmemte
			}
		}
	}
	
	return 0;// se nao achar elemento nao nulo, entao o determinante eh zero
}				
				
/*calcula o determinante de @m atravez da regra de chioh
 */				
double determinante_matriz(MATRIZ_ESPARSA *m) {
	MATRIZ_ESPARSA *temp = copiar_matriz(m); // copia a matriz @m para @temp

	int i, j, flag = 0, k; // contadores e flags
	double value, x, y, det = 0; // determinante e auxilios

	for(i = 0; i < temp->dim1-1; ++i) { // loop na diagonal matriz ateh o penultino elemento da diagonal principal
		flag = toOne(temp, i);// transforma o elemento @temp[i][j] em 1
		if(flag == 0) break; // se @flag == 0 entao o determinante eh zero

		for(k = i+1; k < m->dim1; ++k) { // loop na linha da submatriz imediatamente posterior a @temp[i][i]
			x = get_matriz(temp, i, k);// pega o elemento da linha de cima da submatriz

			for(j = i; j < temp->dim1; ++j) { // loop na coluna da sub matriz 

				y = get_matriz(temp, j, i);//pega o elemento da coluna de i
				value = get_matriz(temp, j, k);//pega o elemento corrente da submatriz
					
				set_matriz(temp, j, k, value - x*y);// realiza o calculo caracteristico da regra de chio
			}
		}
	}

	det = get_matriz(temp, temp->dim1-1, temp->dim1-1);// a matriz @temp for sendo reduzida ate o penultimo elemento da diagonal principal
							   // logo o ultimo elemento eh a propria determinante
	
	apagar_matriz(temp); // apaga a matriz temporaria
	
	return (flag == 0) ? 0 : det; // se flag == 0 o determinante eh zero do contrario retorna o valor encontrado
}
