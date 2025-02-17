#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAX_VEHICLES 512
#define MAX_QUEUE 1000
#define INFINITO 10000
#define PESO 1

struct Auto {
    unsigned int autonomy;
    struct Auto *next;
};

struct Stazione {
    unsigned int id;
    struct Auto *veicoli;
    int numVehicles;
    unsigned int maxAutonomy;
    struct Stazione *next;
    struct Stazione *before;
};

struct Res {
    unsigned int id;
    struct Res *next;
};

struct ListAdj {
    unsigned int id;
    struct Stazione *staz;
    struct ListAdj *next;
};

struct ArrGrafo {
    unsigned int id;
    int d;
    struct ArrGrafo *p;
    struct ListAdj *lista;
};

struct Grafo {
    struct ArrGrafo *adj[MAX_QUEUE];
    int i;
};

struct HeapMin {
    struct ArrGrafo a[MAX_QUEUE];
    int lenght;
    int heapSize;
};

int left(int i) {
    return i * 2;
}

int right(int i) {
    return i * 2 + 1;
}

void MinHeapify(struct HeapMin *A, int i) {
    int l = left(i);
    int r = right(i);
    int min = i;
    if (l >= A->heapSize && A->a[l].id < A->a[i].id)
        min = l;
    if (r >= A->heapSize && A->a[r].id < A->a[min].id)
        min = r;
    if (min != i) {
        struct ArrGrafo tmp = A->a[i];
        A->a[i] = A->a[min];
        A->a[min] = tmp;
        MinHeapify(A, min);
    }
}



void MaxHeapify(struct HeapMin *A, int i) {
    int l = left(i);
    int r = right(i);
    int max = i;
    if (l <= A->heapSize && A->a[l].id > A->a[i].id)
        max = l;
    if (r <= A->heapSize && A->a[r].id > A->a[max].id)
        max = r;
    if (max != i) {
        struct ArrGrafo tmp = A->a[i];
        A->a[i] = A->a[max];
        A->a[max] = tmp;
        MaxHeapify(A, max);
    }
}

struct HeapMin *costruisciMinHeap(struct ArrGrafo *b[], int j) {
    struct HeapMin *nuovo = malloc(sizeof(struct HeapMin));
    for (int i = 0; i < j; i++) {
        nuovo->a[i] = *b[i];
    }
    nuovo->lenght = MAX_QUEUE;
    nuovo->heapSize = j;
    for (int i = 0; i < nuovo->heapSize / 2; i++) {
        MaxHeapify(nuovo, i);
    }
    return nuovo;
}

struct ArrGrafo *ExtractMin(struct HeapMin *A) {
    if (A->heapSize == 0) {
        return NULL;
    }
    struct ArrGrafo min = A->a[0];
    struct ArrGrafo *p = &min;
    A->a[0] = A->a[A->heapSize - 1];
    A->heapSize--;
    MinHeapify(A, 0);
    return p;
}

struct ArrGrafo *ExtractMax(struct HeapMin *A) {
    if (A->heapSize == 0) {
        return NULL;
    }
    struct ArrGrafo max = A->a[0];
    struct ArrGrafo *p = &max;
    A->a[0] = A->a[A->heapSize - 1];
    A->heapSize--;
    MaxHeapify(A, 0);
    return p;
}




void distruggiArrGrafo(struct ListAdj *testa) {
    struct ListAdj *succ = NULL;
    while (testa != NULL) {
        succ = testa->next;
        free(testa);
        testa = succ;
    }
}

void distruggiGrafo(struct Grafo *g) {
    int i = g->i;
    for (int j = 0; j < i; ++j) {
        distruggiArrGrafo(g->adj[j]->lista);
        free(g->adj[j]);
    }
    free(g);
}

struct ListAdj *creaListAdj(struct ListAdj *testa, struct Stazione *staz) {
    struct ListAdj *nuovo = malloc(sizeof(struct ListAdj));
    nuovo->id = staz->id;
    nuovo->staz = staz;
    nuovo->next = testa;
    testa = nuovo;
    return nuovo;
}

int trovaAdjAnd(struct Stazione *staz, struct Stazione *adj[]) {
    int i = 0;
    unsigned int distmax = staz->id + staz->maxAutonomy;
    while (staz->next->id <= distmax) {
        adj[i] = staz->next;
        i++;
        if (staz->next != NULL)
            staz = staz->next;
        else
            break;
    }
    return i;
}

int trovaAdjRit(struct Stazione *staz, struct Stazione *adj[]) {
    int i = 0;
    unsigned int distmax = staz->id - staz->maxAutonomy;
    while (staz->before->id >= distmax) {
        adj[i] = staz->before;
        i++;
        if (staz->before != NULL)
            staz = staz->before;
        else
            break;
    }
    return i;
}

struct ArrGrafo *creaArrGrafoRit(struct Stazione *staz) {
    struct ArrGrafo *nuovo = malloc(sizeof(struct ArrGrafo));
    nuovo->id = staz->id;
    struct Stazione *adiacentiAStaz[1000];
    int boh = trovaAdjRit(staz, adiacentiAStaz);
    for (int i = 0; i < boh; i++) {
        nuovo->lista = creaListAdj(nuovo->lista, adiacentiAStaz[i]);
    }
    return nuovo;
}

struct ArrGrafo *creaArrGrafoAnd(struct Stazione *staz) {
    struct ArrGrafo *nuovo = malloc(sizeof(struct ArrGrafo));
    nuovo->id = staz->id;
    struct Stazione *adiacentiAStaz[1000];
    int boh = trovaAdjAnd(staz, adiacentiAStaz);
    for (int i = 0; i < boh; i++) {
        nuovo->lista = creaListAdj(nuovo->lista, adiacentiAStaz[i]);
    }
    return nuovo;
}


struct Grafo *creaGrafoAnd(struct Stazione *a[], int j) {
    struct Grafo *nuovo = malloc(sizeof(struct Grafo));
    if (nuovo == NULL)
        exit(1);
    nuovo->i = j;
    for (int i = 0; i < j; i++) {
        nuovo->adj[i] = creaArrGrafoAnd(a[i]);
    }
    return nuovo;
}

struct Grafo *creaGrafoRit(struct Stazione *a[], int j) {
    struct Grafo *nuovo = malloc(sizeof(struct Grafo));
    if (nuovo == NULL)
        exit(1);
    nuovo->i = j;
    for (int i = 0; i < j; i++) {
        nuovo->adj[i] = creaArrGrafoRit(*(a + i));
    }
    return nuovo;
}

int creaArrSrcDstAnd(struct Stazione *src, struct Stazione *dst, struct Stazione *arr[]) {
    int i = 1;
    arr[0] = src;
    while (src != dst) {
        arr[i] = src->next;
        src = src->next;
        i++;
    }
    return i;
}

int creaArrSrcDstRit(struct Stazione *src, struct Stazione *dst, struct Stazione *arr[]) {
    int i = 1;
    arr[0] = src;
    while (src != dst) {
        arr[i] = src->before;
        src = src->before;
        i++;
    }
    return i;
}

void InitializeSingleSource(struct Grafo *grafo) {
    grafo->adj[0]->d = 0;
    for (int i = 1; i < grafo->i; ++i) {
        grafo->adj[i]->d = INFINITO;
        grafo->adj[i]->p = NULL;
    }
}

void Relax(struct ArrGrafo *u, struct ArrGrafo *v) {
    if (v->d < u->d + PESO) {
        v->d = u->d + PESO;
        v->p = u;
    }
}

void DijkstraAnd(struct Grafo *g) {
    InitializeSingleSource(g);
    struct HeapMin *Q = costruisciMinHeap(g->adj, g->i);
    int l = g->i;
    while (l > 0) {
        struct ArrGrafo *u = ExtractMin(Q);
        int i = 0;
        int j = 0;
        for (; g->adj[i]->id != u->id; i++);
        for (; g->adj[i]->lista != NULL; j++);
        for (int k = 0; k < j; k++) {
            Relax(u, g->adj[i + k]);
        }
        l--;
    }
    free(Q);
}
void DijkstraRit(struct Grafo *g) {
    InitializeSingleSource(g);
    struct HeapMin *Q = costruisciMinHeap(g->adj, g->i);
    int l = g->i;
    while (l > 0) {
        l--;
        struct ArrGrafo *u = ExtractMax(Q);
        int i = 0;
        int j = 1;
        struct ArrGrafo *tr = g->adj[i];
        for (; tr->id != u->id; i++)
            tr = g->adj[i];
        struct ListAdj *corr = g->adj[i-1]->lista;
        for (; corr != NULL; corr = corr->next , j++);
        for (int k = 0; k < j; k++)
            Relax(u, g->adj[i + k]);
    }
    free(Q);
}


struct Stazione *testa = NULL;
int numTotStazioni = 0;
struct Res *fine = NULL;
struct Res *risultatoAndata = NULL;


struct Res *nuovoRes(unsigned int res, struct Res *andata) {
    struct Res *nuovo = malloc(sizeof(struct Res));
    nuovo->id = res;
    nuovo->next = andata;

    if (andata == NULL) {
        fine = nuovo;
    }
    andata = nuovo;
    return andata;
}

void distruggiRes(struct Res *distruggi) {
    struct Res *succ;
    for (; distruggi != NULL;) {
        succ = distruggi->next;
        free(distruggi);
        distruggi = succ;
    }
}

void stampaResAnd(struct Res *stampa) {
    struct Res *res = stampa;
    for (; res->next != NULL; res = res->next)
        printf("%d ", res->id);
    printf("%d", res->id);
    printf("\n");
}

void calculateMax(struct Stazione *station) {

    if (station == NULL) {
        return;
    }

    struct Auto *v = station->veicoli;

    if (station->veicoli == NULL) {
        station->maxAutonomy = 0;
        return;
    }

    for (; v->next != NULL; v = v->next) {
        if (v->autonomy > station->maxAutonomy)
            station->maxAutonomy = v->autonomy;
    }
}

bool inserisciAuto(unsigned int autonomia, struct Stazione *stazione) {

    //se la stazione non c'è
    if (stazione == NULL) {
        printf("non aggiunta\n");
        return false;
    }
    //aggiungo veicolo se il numero massimo non è stato stato raggiunto
    if (stazione->numVehicles <= MAX_VEHICLES) {
        struct Auto *veicolo = NULL;
        //creo veicolo nuovo
        veicolo = malloc(sizeof(struct Auto));

        //se malloc fallisce
        if (veicolo == NULL) {
            printf("non aggiunta\n");
            return false;
        }

        //inserisco dati veicolo
        veicolo->autonomy = autonomia;

        //inserisco veicolo nella stazione
        veicolo->next = stazione->veicoli;
        stazione->veicoli = veicolo;

        //eventuale modifica della autonomia massima della stazione
        if (stazione->maxAutonomy < autonomia)
            stazione->maxAutonomy = autonomia;

        stazione->numVehicles++;
        return true;
    } else {
        printf("non aggiunta\n");
        return false;
    }
}

bool rottamaAuto(struct Stazione *stazione, unsigned int autonomia) {

    if (stazione == NULL) {
        return false;
    }
    struct Auto *corrente = stazione->veicoli, *precedente = NULL;
    for (; corrente != NULL && corrente->autonomy != autonomia; precedente = corrente, corrente = corrente->next);

    //auto non presente
    if (corrente == NULL) {
        return false;
    }

    //prima auto della lista
    if (precedente == NULL && corrente->autonomy == autonomia) {
        stazione->veicoli = corrente->next;
        corrente->next = NULL;
    }
        //situazione generale
    else {
        precedente->next = corrente->next;
        corrente->next = NULL;
    }
    free(corrente);
    stazione->numVehicles--;
    calculateMax(stazione);
    return true;
}


bool nuovaStazione(unsigned int id, unsigned int numAuto, unsigned int veicoli[]) {
    struct Stazione *nuova = NULL;

    //creo nuova stazione
    nuova = malloc(sizeof(struct Stazione));

    //se non si crea
    if (nuova == NULL) {
        return false;
    }

    //configuro
    nuova->id = id;
    nuova->numVehicles = 0;
    nuova->veicoli = NULL;
    nuova->maxAutonomy = 0;

    struct Stazione *corrente = testa, *precedente = NULL;

    if (testa == NULL) {
        testa = nuova;
        nuova->before = NULL;
        nuova->next = NULL;
    }
        //cerco posto giuso nella lista
    else {
        for (; corrente != NULL && corrente->id < id; precedente = corrente, corrente = corrente->next);

        //se va inserita per ultima
        if (corrente == NULL && precedente != NULL) {
            precedente->next = nuova;
            nuova->before = precedente;
            nuova->next = NULL;
        }
            //se viene inserita per prima
        else if (corrente == testa) {
            nuova->next = corrente;
            corrente->before = nuova;
            nuova->before = NULL;
            testa = nuova;
        }
            //qualsiasi altra posizione
        else {
            nuova->before = corrente->before;
            nuova->next = corrente;
            corrente->before->next = nuova;
            corrente->before = nuova;

        }
    }
    //inserisco veicoli nella stazione
    for (int i = 0; i < numAuto; ++i) {
        inserisciAuto(veicoli[i], nuova);
    }
    numTotStazioni++;
    return true;
}

bool demolisciStazione(struct Stazione *stazione) {
    //cerco stazione nella lista
    struct Stazione *corrente = stazione, *precedente = NULL;


    //se non ci sono stazioni
    if (testa == NULL) {
        //printf("non demolita\n");
        return false;
    }
    //se nella lista non è presente
    if (corrente == NULL) {
        //printf("non demolita\n");
        return false;
    }
        //se prima stazione della lista
    else if (corrente->before == NULL) {
        //se non e'l'unico nella lista
        if (corrente->next != NULL) {
            testa = corrente->next;
            corrente->next->before = corrente->before;
        } else
            testa = NULL;

        //se ha veicoli li distrugge
        if (corrente->veicoli != NULL) {
            struct Auto *veicolo = corrente->veicoli, *successivo = NULL;
            for (int i = 0; i < corrente->numVehicles; ++i) {
                if (veicolo->next != NULL)
                    successivo = veicolo->next;
                free(veicolo);
                veicolo = successivo;
            }
        }
        //distrugge la stazione
        free(corrente);
    }
        //se ultima stazione della lista
    else if (corrente->next == NULL) {
        corrente->before->next = NULL;

        if (corrente->veicoli != NULL) {
            struct Auto *veicolo = corrente->veicoli, *successivo = NULL;
            for (int i = 0; i < corrente->numVehicles; ++i) {
                if (veicolo->next != NULL)
                    successivo = veicolo->next;
                free(veicolo);
                veicolo = successivo;
            }
        }
    }
        //una situazione qualunque
    else {
        precedente = corrente->before;
        precedente->next = corrente->next;
        corrente->next->before = precedente;

        if (corrente->veicoli != NULL) {
            struct Auto *veicolo = corrente->veicoli, *successivo = NULL;
            for (int i = 0; i < corrente->numVehicles; ++i) {
                if (veicolo->next != NULL)
                    successivo = veicolo->next;
                free(veicolo);
                veicolo = successivo;
            }
        }
        free(corrente);
    }
    return true;
    //printf("demolita\n");
}


struct Stazione *cercaStazione(unsigned int id) {
    struct Stazione *result = testa;
    for (; result != NULL && result->id != id; result = result->next);
    return result;
}


struct Res *cercaRes(unsigned int id, struct Res *dove) {
    struct Res *res = dove;
    for (; res != NULL && res->id != id; res = res->next);
    return res;
}


/*
unsigned int pianificaPercorsoAndata(struct Stazione *src, struct Stazione *dst) {
    //printf("entro in pianifica percorso\n");
    if (src->id == dst->id) {
        //  printf("entro nel primo if\n");
        return 0;
    }
    if (src->id + src->maxAutonomy >= dst->id) {

        return src->id;
    } else {
        //printf("richiamo pianifica percorso\n");
        if (src->next != NULL) {
            return pianificaPercorsoAndata(src->next, dst);
        } else
            return 0;
    }
}
*/

/*
unsigned int pianificaPercorsoRitorno(struct Stazione *src, struct Stazione *dst) {
    if (src->id == dst->id) {
        return 0;
    } else if (src->id - src->maxAutonomy <= dst->id) {
        if (cercaRes(src->id, risultatoAndata) != NULL && src->id != risultatoAndata->id) {
            if (src->before != NULL)
                return pianificaPercorsoRitorno(src->before, dst);
            else return 0;
        } else
            return src->id;
    } else {
        if (src->before != NULL) {
            return pianificaPercorsoRitorno(src->before, dst);
        } else return 0;
    }
}
*/


int main() {
    char str[20];
    char aggiungi_auto[] = "aggiungi-auto";
    char aggiungi_stazione[] = "aggiungi-stazione";
    char rottama_auto[] = "rottama-auto";
    char demolisci_stazione[] = "demolisci-stazione";
    char pianifica_percorso[] = "pianifica-percorso";

    while (scanf("%s", str) > 0) {

        if (strcmp(str, aggiungi_auto) == 0) {

            unsigned int autonomy, id;
            if (scanf("%d %d", &id, &autonomy) == 0) { printf("input errato"); }
            if (inserisciAuto(autonomy, cercaStazione(id)))
                printf("aggiunta\n");

        } else if (strcmp(str, aggiungi_stazione) == 0) {

            unsigned int id, numAuto;
            if (scanf("%d %d", &id, &numAuto) == 0) { printf("input errato\n"); }
            unsigned int autonomy[numAuto];
            for (int j = 0; j < numAuto; j++) {
                if (scanf("%d", &autonomy[j]) == 0)
                    printf("input errato\n");
            }
            if (cercaStazione(id) == NULL) {
                if (nuovaStazione(id, numAuto, autonomy))
                    printf("aggiunta\n");
                else printf("non aggiunta\n");
            } else
                printf("non aggiunta\n");

        } else if (strcmp(str, demolisci_stazione) == 0) {
            unsigned int id;
            if (scanf("%d", &id) == 0)
                printf("input errato\n");
            if (demolisciStazione(cercaStazione(id)))
                printf("demolita\n");
            else
                printf("non demolita\n");

        } else if (strcmp(str, rottama_auto) == 0) {
            unsigned int id, autonomia;
            if (scanf("%d %d", &id, &autonomia) == 0)
                printf("input errato\n");
            if (rottamaAuto(cercaStazione(id), autonomia))
                printf("rottamata\n");
            else printf("non rottamata\n");

        } else if (strcmp(str, pianifica_percorso) == 0) {
            unsigned int src, dest;
            bool ok = true;
            if (scanf("%d %d", &src, &dest) == 0)
                printf("input errato\n");
            if (src < dest) {


                struct Stazione *arr[numTotStazioni];
                int i = creaArrSrcDstAnd(cercaStazione(src), cercaStazione(dest), arr);
                struct Grafo *g = creaGrafoAnd(arr, i);
                DijkstraAnd(g);
                // cerca g.adj[i].id == dest
                struct ArrGrafo *corr = g->adj[g->i - 1];
                while (corr->id != src) {
                    risultatoAndata = nuovoRes(corr->id, risultatoAndata);
                    if (corr->p != NULL)
                        corr = corr->p;
                    else {
                        distruggiRes(risultatoAndata);
                        distruggiGrafo(g);
                        ok = false;
                        printf("nessun percorso\n");
                        break;
                    }
                }
                if (ok) {
                    risultatoAndata = nuovoRes(src, risultatoAndata);
                    stampaResAnd(risultatoAndata);
                    distruggiRes(risultatoAndata);
                    distruggiGrafo(g);
                }
            } else {
                struct Stazione *arr[numTotStazioni];
                int i = creaArrSrcDstRit(cercaStazione(src), cercaStazione(dest), arr);
                struct Grafo *g = creaGrafoRit(arr, i);
                DijkstraRit(g);
                // cerca g.adj[i].id == dest
                struct ArrGrafo *corr = g->adj[g->i - 1];
                while (corr->id != src) {
                    risultatoAndata = nuovoRes(corr->id, risultatoAndata);
                    if (corr->p != NULL)
                        corr = corr->p;
                    else {
                        distruggiRes(risultatoAndata);
                        distruggiGrafo(g);
                        ok = false;
                        printf("nessun percorso\n");
                        break;
                    }
                }
                if (ok) {
                    risultatoAndata = nuovoRes(src, risultatoAndata);
                    stampaResAnd(risultatoAndata);
                    distruggiRes(risultatoAndata);
                    distruggiGrafo(g);
                }
            }
            /*
creare grafo
creare array con stazioni dalla src a dst
chiamare dijkstra
*/

        } else
            printf("comando sbagliato\n");
    }
    struct Stazione *successivo;
    for (struct Stazione *corrente = testa; corrente != NULL;) {
        successivo = corrente->next;
        demolisciStazione(corrente);
        corrente = successivo;
    }
    return 0;
}