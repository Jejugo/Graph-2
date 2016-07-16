#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXV 100
#define MAXA 200
#define _CV 1
#define _DV 2
#define _CA 3
#define _DA 4
#define _TV 5
#define _TA 6
#define _IG 7
#define _CM 8
#define _FM 9
#define INF 9999999

//TADGrafo////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct incidencia { //cria a lista de incidencias (adjacencias)
    struct incidencia *ant;
    struct aresta *aresta;
    struct incidencia *prox;
} incidencia;

typedef struct vertice{     //cria a lista de vertices
    struct vertice *ant;
    struct incidencia *lista;
    int conteudo;
    struct vertice *prox;
} vertice;

typedef struct aresta {     //cria a lista de arestas
    struct aresta *ant;
    struct vertice *esq;
    struct vertice *dir;
    struct incidencia *incEsq;
    struct incidencia *incDir;
    int conteudo;
    struct aresta *prox;
} aresta;

typedef struct {            //cria o grafo
    vertice *PV;
    aresta *PA;
    int nvertices;
    int narestas;
} grafo;

//Funcao que retorna uma referencia para o vertice oposto a v na aresta a
vertice* opposite(grafo *g, vertice *v, aresta *a){
    vertice *temp;
    if (a->esq == v){
        temp = a->dir;
    } else{
        temp = a->esq;
    }
    return temp;
}
//-------------------------------------------------------------------------------------------------------------------- OK


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//mapa////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {                        //mapa engloba:
    vertice *vertices[MAXV];            //um campo de ponteiros para vertices
    aresta *arestas[MAXA];              //um campo de ponteiros para arestas
    int indiceVert;                     //indice do ultimo vertice criado (comeca em 0)
    int indiceAres;                     //indice da ultima aresta  criada (comeca em 0)
} mapa;

vertice* pegaVertRef(mapa m, int ind){  //pega a referencia do vertice atraves do seu identificador
    return m.vertices[ind-1];
}

aresta* pegaAresRef(mapa m, int ind){  //pega a referencia da aresta atraves do seu identificador
    return m.arestas[ind-1];
}

int pegaAresInd(mapa m, aresta *A){    //pega o identificador da aresta atraves da sua referência
    int i=0;
    while (m.arestas[i] != A){
        i++;
    }
    return i+1;
}

int pegaVertInd(mapa m, vertice *V){    //pega o identificador do vertice atraves da sua referência
    int i=0;
    while (m.vertices[i] != V){
        i++;
    }
    return i+1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//fila de prioridade//////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct filaP{
    int vertice;
    int chave;
    struct filaP *prox; 
} filaP;

//funcao que insere na lista de prioridades
filaP* insertFilaP(filaP *Q, int v, int k){
    filaP *f = (filaP*) malloc(sizeof(filaP));
    f->vertice = v;
    f->chave = k;

    filaP *pF1 = Q;
    if (pF1 == NULL){                    //caso de lista vazia
        Q = f;
        f->prox = NULL;
    } else{                              //no meio da fila
        if (pF1->chave > k){             //caso de inserção no começo
            Q = f;
            f->prox = pF1;
        } else {                         //inserção no meio
            filaP *pF2 = pF1->prox;
            if (pF2 != NULL) {           //se não for no fim
                while (k > pF2->chave){  //cheva até a posição onde deve ser inserido
                    pF2 = pF2->prox;
                    pF1 = pF1->prox;
                }
            }
            pF1->prox = f;
            f->prox = pF2;
        }
    }
    return Q;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//funcao que retorna o primeiro vertice da lista de prioridades
int retornaFilaP(filaP *Q){
    int v = Q->vertice;
    return v;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//funcao que remove o primeiro elemento da lista de prioridades
filaP* removeFilaP(filaP *Q){
    filaP *pF = Q;
    Q = Q->prox;
    free(pF);
    return Q;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//substitui a chave do vertice v por k na fila de prioridades
filaP* replaceKeyFilaP(filaP *Q, int v, int k){
    filaP *pF1 = Q;
    if (pF1->vertice == v){
        Q = pF1->prox;
        free(pF1);
    } else {
        filaP *pF2 = pF1->prox;
        while (pF2->vertice != v){
            pF1 = pF1->prox;
            pF2 = pF2->prox;
        }
        pF1->prox = pF2->prox;
        free(pF2);
    }
    Q = insertFilaP(Q, v, k);
    return Q;
}
//-------------------------------------------------------------------------------------------------------------------- OK
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Funcao que imprime o caminho entre o vertice inicial e o vertice de destino do Dijkstra, de modo recursivo
void imprimeDijkstra(int v, int v2, int parent[], int i){
    if (v2 != v){
        imprimeDijkstra(v, parent[v2-1], parent, i+1);
    }
    if (i==0){              //se for o ultimo a imprimir(ou o primeiro a ser chamado), imprime com \n, senao com ' '
        printf("%d\n", v2);
    } else {
        printf("%d ", v2);
    }

}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que calcula o caminho minimo de um vertice a outro
int dijkstra(grafo *g, mapa m, int v, int v2){
    filaP *Q = NULL;
    int distance[100];      //vértice que armazena as distancias
    int parent[100];        //vertice que armazena os pais de cada vertice

    int i=1;
    while (i <= m.indiceVert){
        if (pegaVertRef(m, i) != NULL){
            if (i == v) {
                distance[i-1] = 0; //inicializa o vetor de distancias com 0 para o vertice inicial e INF para os demais
                parent[i-1] = 0;   //inicializa o vetor parent com 0 para o vertice inicial e INF para os demais
            } else {
                distance[i-1] = INF;
                parent[i-1] = INF;
            }
            Q = insertFilaP(Q, i, distance[i-1]);       //insere o vertice na fila de prioridades
        }
        i++;
    }

    int k = 0;
    while (k<g->nvertices){                     //para cada vertice
        int uI = retornaFilaP(Q);               //retorna o primeiro vertice da fila
        Q = removeFilaP(Q);                     //remove o vertice u da fila
        vertice *uR = pegaVertRef(m, uI);       //pega a referencia do vertice u
        incidencia *i = uR->lista;              //lista de incidencias do vertice u
        while (i != NULL){                      //para cada aresta da lista de incidencias do vertice u
            aresta *e = i->aresta;
            vertice *zR = opposite(g, uR, e);   //pega o vértice z oposto a u pela aresta e
            int zI = pegaVertInd(m, zR);        //pega o indentificador do vertice z
            if (distance[zI-1] > (distance[uI-1] + e->conteudo)){   //se z.distance > u.distance + e.weight
                distance[zI-1] = (distance[uI-1] + e->conteudo);    //z.distance = u.distance + e.weight
                parent[zI-1] = uI;                                  //z.parent = u
                if (Q != NULL){
                    Q = replaceKeyFilaP(Q, zI, distance[zI-1]);     //substitui a chave do vertice z na fila
                }
            }
            i = i->prox;    //vai para a proxima incidencia
        }
        k++;
    }

    //printf("%d\n", distance[v2-1]);
    //imprimeDijkstra(v, v2, parent, 0);
    return distance[v2-1];
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Função que calcula o caminho minimo entre todos os pares, utilizando djikstra
void minimoTodos(grafo *g, mapa m){
     int countA=0, countB=0, countC=0, countD=0;
     int matrizExc[g->nvertices][g->nvertices];
     int maxExc[g->nvertices];
     int menorExc=0;
     
     for (countA=0, countA < g->nvertices, countA++){
         for(countB=0, countB < g->nvertices, countB++){
                       matrizExc[countA][countB] = djikstra(g,m,countA,countB);
         }
     }
     
     for (countC=0, countC < g->nvertices, countC++){
         for (countD=0, countD < g->nvertices, countD++){
             
             }
         }
}
     
//Funcao que inicializa um grafo.
grafo inicializaGrafo(grafo *g, mapa *m){
    g->PV = NULL;       //primeiro vertice recebe NULL
    g->PA = NULL;       //primeira aresta recebe NULL
    g->nvertices = 0;   //zera todos os campos do grafo e do mapa
    g->narestas = 0;
    m->indiceVert = 0;
    m->indiceAres = 0;
    return *g;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que cria um novo vertice e retorna um ponteiro para ele
vertice* insertVertex(grafo *g, mapa *m, int o) {
    vertice *v = (vertice*) malloc(sizeof(vertice));    //cria ponteiro e aloca vertice
    if (g->nvertices == 0){         //caso nao tenha nenhum vertice (caso de insercao de primeiro elemento)
        v->ant = NULL;              //vertice anterior recebe NULL (nao tem outro vertice)
        g->PV = v;                  //aponta o grafo para o primeiro vertice
    }
    else {                          //caso ja exista algum vertice
        vertice *p = g->PV;         //cria um ponteiro do tipo vertice e o aponta para o primeiro vertice
        while (p->prox != NULL){    //chega até o ultimo vertice da lista de vertices
            p = p->prox;
        }
        p->prox = v;                //liga o vertice V ao ultimo vertice
        v->ant = p;                 //liga o ultimo vertice ao vertice V
    }
    v->prox = NULL;                 //proximo vertice recebe NULL (ultimo da lista)
    v->lista = NULL;                //nao possui nenhuma aresta incidente
    v->conteudo = o;                //coloca o conteudo do vertice
    m->indiceVert++;                //mapeia o vertice
    m->vertices[(m->indiceVert)-1] = v;
    g->nvertices++;                 //aumenta o numero de vertices.
    return v;                       //retorna um ponteiro para o vertice
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que cria uma nova aresta e retorna um ponteiro para ela
aresta* insertEdge(grafo *g, int v1, int v2, mapa *m, int o){
    vertice *v = pegaVertRef(*m, v1);            //pega as referencias dos vertices
    vertice *w = pegaVertRef(*m, v2);

    aresta *a = (aresta*) malloc(sizeof(aresta));             //cria ponteiro e aloca a aresta
    a->esq = v;         //liga o vertice v
    a->dir = w;         //liga o vertica w
    a->conteudo = o;    //coloca o conteudo da aresta

    incidencia *li = (incidencia*) malloc(sizeof(incidencia));     //cria uma incidencia para o vertice v

    if (v->lista == NULL){              //caso o vertice nao tenha incidencias (caso de inserção de primeiro elemento)
        v->lista = li;                  //liga a incidencia a aresta
        li->ant = NULL;
    }
    else {                              //caso o vertice ja tenha incidencias
        incidencia *pi = v->lista;      //cria um ponteiro do tipo incidencia e o aponta para a primeira incidencia
        while (pi->prox != NULL){       //chega até a ultima incidencia da lista de incidencias
            pi = pi->prox;
        }
        pi->prox = li;
        li->ant = pi;                   //liga a nova incidencia com a ultima da lista de incidencias
    }
    li->aresta = a;                     //aponta a incidencia para a aresta criada
    li->prox = NULL;
    a->incEsq = li;                     //aponta a aresta para a incidencia


    incidencia *li2 = (incidencia*) malloc(sizeof(incidencia));     //cria uma incidencia, dessa vez para o vertice w
    if (w->lista == NULL){               //caso o vertice nao tenha incidencias (caso de inserção de primeiro elemento)
        w->lista = li2;                  //liga a incidencia ao vertice
        li2->ant = NULL;                 //incidencia anterior recebe NULL (nao tem outra incidencia)
    }
    else {                               //caso o vertice ja tenha incidencias
        incidencia *pi2 = w->lista;      //cria um ponteiro do tipo incidencia
        while (pi2->prox != NULL){       //chega até a ultima incidencia da lista de incidencias
            pi2 = pi2->prox;
        }
        pi2->prox = li2;
        li2->ant = pi2;                  //liga a nova incidencia com a ultima da lista de incidencias
    }
    li2->prox = NULL;
    li2->aresta = a;                     //aponta a incidencia para a aresta criada
    a->incDir = li2;                      //aponta a aresta para a incidencia

    if (g->narestas == 0){               //caso o grafo nao tenha nenhuma aresta (caso de inserção de primeiro elemento)
        g->PA = a;                       //aponta o grafo para a primeira aresta
        a->ant = NULL;                   //aresta anterior recebe NULL (nao existe outra aresta)
    }
    else {                               //caso o grafo ja tenha alguma aresta
        aresta *p = g->PA;               //cria um ponteiro que aponta para a primeira aresta
        while (p->prox != NULL){         //chega até a ultima aresta da lista de arestas
            p = p->prox;
        }
        a->ant = p;                      //aresta anterior recebe ultima aresta da lista de arestas
        p->prox = a;
    }
    a->prox = NULL;                      //proxima aresta recebe NULL

    m->indiceAres++;                     //mapeia a aresta
    m->arestas[(m->indiceAres)-1] = a;

    g->narestas++;                       //aumenta o numero de arestas
    return a;                            //retorna um ponteiro para a aresta
}
//-------------------------------------------------------------------------------------------------------------------- OK

typedef struct stVert {     //Tipo auxiliar para retornar 2 referencias numa função (para a função endVertices)
    vertice *v1;
    vertice *v2;
} stVert;
//Funcao que retorna referencias para os dois vertices finais da aresta a
stVert endVertices(aresta *a){
    stVert vertices;
    vertices.v1 = a->esq;
    vertices.v1 = a->dir;
    return vertices;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que retorna o valor do vertice v
int vertexValue(int v, mapa m){
    vertice *vert = pegaVertRef(m, v);
    return vert->conteudo;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que retorna o valor da aresta a
int edgeValue(int a, mapa m){
    aresta *ares = pegaAresRef(m, a);
    return ares->conteudo;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que substitui o valor do vertice v por o
void replaceVertex(int v, int o, mapa m){
    vertice *vert = pegaVertRef(m, v);
    vert->conteudo = o;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que substitui o valor da aresta a por o
void replaceEdge(int a, int o, mapa m){
    aresta *ares = pegaAresRef(m, a);
    ares->conteudo = o;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que remove uma aresta a, retornando o valor dela
int removeEdge(grafo *g, int ares, mapa *m){
    aresta *a = pegaAresRef(*m, ares);
    m->arestas[ares-1] = NULL;          //remove o apontador do mapa para a aresta
    int temp = a->conteudo;      //valor a ser retornado

    vertice *vT = a->esq;               //comeca a eliminacao das incidencias de cada um dos 2 vertices
    incidencia *iT = a->incEsq;         //que a aresta incide. Primeiro pelo da esquerda
    incidencia *iTProx = iT->prox;
    incidencia *iTAnt = iT->ant;

    if (vT->lista == iT){               //se for o primeiro da lista de incidencias
        vT->lista = iTProx;
        if (iTProx != NULL){
            iTProx->ant = NULL;
        }
    }
    else {                              //se for no meio da lista de incidencias
        iTAnt->prox = iTProx;
        if (iTProx != NULL){            //se nao for o ultimo da lista de incidencias
            iTProx->ant = iTAnt;
        }
    }
    free(iT);                           //desaloca a incidencia

    vT = a->dir;                        //agora, para a incidencia da esquerda
    iT = a->incDir;
    iTProx = iT->prox;
    iTAnt = iT->ant;

    if (vT->lista == iT){               //se for o primeiro da lista de incidencias
        iTProx = iT->prox;
        vT->lista = iTProx;
        if (iTProx != NULL){
            iTProx->ant = NULL;
        }
    }
    else {                              //se for no meio da lista de incidencias
        iTAnt->prox = iTProx;
        if (iTProx != NULL){            //se nao for o ultimo da lista de incidencias
            iTProx->ant = iTAnt;
        }
    }
    free(iT);                           //desaloca a incidencia

    free(a);                            //desaloca a aresta
    g->narestas--;                      //diminui o numero de arestas
    return temp;                        //retorna o valor da aresta
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que remove um vertice v e suas arestas, retornando o valor dele
int removeVertex(grafo *g, int vert, mapa *m){
    vertice *v = pegaVertRef(*m, vert);
    int temp = v->conteudo;             //valor a ser retornado

    incidencia *i = v->lista;
    while (i != NULL){                  //se houverem arestas a serem removidas
        aresta *aT = i->aresta;
        i = i->prox;
        int ind = pegaAresInd(*m, aT);  //pega o identificador da aresta apontada pela incidencia
        removeEdge(g, ind, m);          //remove a aresta
    }

    vertice *pAnt = v->ant;             //cria ponteiro para o anterior da lista de vertices
    vertice *pProx = v->prox;           //cria ponteiro para o proximo  da lista de vertices
    if (pProx != NULL){                 //liga o proximo com o anterior
        pProx->ant = pAnt;
    }
    if (pAnt != NULL){                  //liga o anterior com o proximo
        pAnt->prox = pProx;
    } else {
        g->PV = pProx;                  //se o vertice removido for o primeiro
    }

    m->vertices[vert-1] = NULL;         //remove o apontador do mapa para o vertice
    free(v);                            //desaloca o vertice
    g->nvertices--;                     //diminui o numero de vertices

    return temp;                        //retorna o valor do vertice
}
//-------------------------------------------------------------------------------------------------------------------- OK

//Funcao que identifica o comando digitado pelo usuario
int identificaComando(char c[]){
    int esc;
    if (strncmp(c, "CV", 2) == 0){      //cria vértice
        esc = _CV;
    }
    else if (strncmp(c, "DV", 2) == 0){ //deleta vértice
        esc = _DV;
    }
    else if (strncmp(c, "CA", 2) == 0){ //cria aresta
        esc = _CA;
    }
    else if (strncmp(c, "DA", 2) == 0){ //deleta aresta
        esc = _DA;
    }
    else if (strncmp(c, "TV", 2) == 0){ //troca valor do vértice
        esc = _TV;
    }
    else if (strncmp(c, "TA", 2) == 0){ //troca valor da aresta
        esc = _TA;
    }
    else if (strncmp(c, "IG", 2) == 0){ //imprime grafo
        esc = _IG;
    }
    else if (strncmp(c, "CM", 2) == 0){ //caminho minimo (dijkstra)
        esc = _CM;
    }
    else if (strncmp(c, "FM", 2) == 0){ //finaliza programa
        esc = _FM;
    }
    else {
        esc = 0;
    }
    return esc;
}
//-------------------------------------------------------------------------------------------------------------------- OK

//main/////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
    mapa m;
    grafo g = inicializaGrafo(&g, &m);
    int i, i1, i2, i3;

    char esc[20];       //comando a ser digitado

    int FM = 1;
    while (FM == 1){

        scanf("%s", esc);  //lerá o comando até o primeiro espaço, para depois ler até o proximo espaço, e assim por diante
        int escI = identificaComando(esc);  //identifica o comando digitado
        switch(escI){
            case _CV:   //cria vértice
                scanf("%d", &i1);
                insertVertex(&g, &m, i1);
                break;

            case _DV:   //destrói vértice
                scanf("%d", &i1);
                removeVertex(&g, i1, &m);
                break;

            case _CA:   //cria aresta
                scanf("%d %d %d", &i1, &i2, &i3);
                insertEdge(&g, i1, i2, &m, i3);
                break;

            case _DA:   //destrói aresta
                scanf("%d", &i1);
                removeEdge(&g, i1, &m);
                break;

            case _TV:   //troca vértice
                scanf("%d %d", &i1, &i2);
                replaceVertex(i1, i2, m);
                break;

            case _TA:   //troca aresta
                scanf("%d %d", &i1, &i2);
                replaceEdge(i1, i2, m);
                break;

            case _IG:   //imprime grafo
                printf("%d\n", g.nvertices);
                for (i = 0; i < m.indiceVert; i++){
                    vertice *v = pegaVertRef(m, i+1);
                    if (v != NULL){
                        printf("%d ", i+1);
                        printf("%d\n", v->conteudo);
                    }
                }

                printf("%d\n", g.narestas);
                for (i = 0; i < m.indiceAres; i++){
                    aresta *a = pegaAresRef(m, i+1);
                    if (a != NULL){
                        printf("%d ", i+1);
                        vertice *vT = a->dir;
                        printf("%d ", pegaVertInd(m, vT));
                        vT = a->esq;
                        printf("%d ", pegaVertInd(m, vT));
                        printf("%d\n", a->conteudo);
                    }
                }
                break;

            case _CM:   //caminho mínimo
                scanf("%d %d", &i1, &i2);
                dijkstra(&g, m, i1, i2);
                break;

            case _FM:   //fim
                FM = 0;
                break;

        }
    }
    return 0;
}
