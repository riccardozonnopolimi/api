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

struct Adj {
    unsigned int id;
    struct Adj *next;
};

struct ArrT {
    unsigned int id;
    struct ArrT *p;
    int d;
    struct Adj *list;
};

struct Grafo {
    struct ArrT *adj[MAX_QUEUE];
    int size;
};

/*struct Coda {
    struct ArrT a[MAX_QUEUE];
    int estratti;
    int size;
};*/


struct Stazione *testa = NULL;
int numTotStazioni = 0;
struct Res *risultatoAndata = NULL;
struct Grafo *g = NULL;
/*struct Coda *q = NULL;*/


struct Stazione *cercaStazione(unsigned int id) {
    struct Stazione *result = testa;
    for (; result != NULL && result->id != id; result = result->next);
    return result;
}


struct Adj *creaAdj(unsigned int id, struct Adj *head) {
    struct Adj *nuovo = malloc(sizeof(struct Adj));
    nuovo->id = id;
    nuovo->next = head;
    head = nuovo;
    return head;
}

struct ArrT *creaArrT(unsigned int id, unsigned int adj[], int i) {
    struct ArrT *nuovo = malloc(sizeof(struct ArrT));
    nuovo->id = id;
    nuovo->d = INFINITO;
    nuovo->p = NULL;
    nuovo->list = NULL;
    for (int j = 0; j < i; j++) {
        nuovo->list = creaAdj(adj[j], nuovo->list);
    }
    return nuovo;
}

int calcoloAdjRit(unsigned int id, unsigned int adiacenze[]) {
    struct Stazione *corr = cercaStazione(id);
    int distMax = corr->id - corr->maxAutonomy;
    int i = 0;
    while (distMax <= corr->id) {
        if (corr->before != NULL) {
            corr = corr->before;
        } else
            break;
        adiacenze[i] = corr->id;
        i++;

    }
    return i;
}

struct Grafo *creaGrafoRit(unsigned int stazioni[], int i) {
    struct Grafo *nuovo = malloc(sizeof(struct Grafo));
    nuovo->size = i;
    int k;
    unsigned int adiacenze[100];
    for (int j = 0; j < i; ++j) {
        k = calcoloAdjRit(stazioni[j], adiacenze);
        nuovo->adj[j] = creaArrT(stazioni[j], adiacenze, k);
    }
    return nuovo;
}

int calcoloAdjAnd(unsigned int id, unsigned int adiacenze[]) {
    struct Stazione *corr = cercaStazione(id);
    int distMax = corr->id + corr->maxAutonomy;
    int i = 0;
    while (distMax >= corr->id) {
        adiacenze[i] = corr->id;
        i++;
        if (corr->next != NULL) {
            corr = corr->next;
        } else
            break;
    }
    return i;
}

struct Grafo *creaGrafoAnd(unsigned int stazioni[], int i) {
    struct Grafo *nuovo = malloc(sizeof(struct Grafo));
    nuovo->size = i;
    int k;
    unsigned int adiacenze[100];
    for (int j = 0; j < i; ++j) {
        k = calcoloAdjAnd(stazioni[j], adiacenze);
        nuovo->adj[j] = creaArrT(stazioni[j], adiacenze, k);
    }
    return nuovo;
}

int calcolaViaggio(struct Stazione *src, struct Stazione *dst, unsigned int stazioni[]) {
    int i = 0;
    if (src->id < dst->id) {
        while (src->id != dst->id) {
            stazioni[i] = src->id;
            i++;
            if (src->next != NULL) {
                src = src->next;
            } else break;
        }
        stazioni[i] = dst->id;
        i++;
        return i;
    } else {
        while (src->id != dst->id) {
            stazioni[i] = src->id;
            i++;
            if (src->before != NULL) {
                src = src->before;
            } else break;
        }
        stazioni[i] = dst->id;
        i++;
        return i;
    }
}

void distruggiArrT(struct ArrT *head) {


    if (head->list != NULL) {
        struct Adj *list = head->list;
        struct Adj *succ;
        if (list->next != NULL) {
            succ = list->next;
            while (succ != NULL) {
                if (list->next != NULL)
                    succ = list->next;
                else
                    succ = NULL;
                free(list);
                list = succ;
            }
        }
        else
            free(list);
    }
}

void distruggiGrafo() {
    for (int i = g->size; i > 0; --i) {
        distruggiArrT(g->adj[i-1]);
    }
    free(g);
    g = NULL;
}


/*struct Coda *creaCoda(struct Grafo *grafo) {
    struct Coda *c = malloc(sizeof(struct Coda));
    for (int j = 0; j < grafo->size; j++) {
        c->a[j] = *(grafo->adj[j]);
    }
    c->size = grafo->size;
    c->estratti = 0;
    return c;
}


struct ArrT estraiCoda(struct Coda *c) {
    struct ArrT ver;
    if (c->estratti < c->size) {
        ver = c->a[c->estratti];
        c->estratti++;
        return ver;
    } else {
        ver.id = 0;
        ver.p = NULL;
        ver.list = NULL;
        ver.d = 0;
        return ver;
    }
}*/

struct ArrT *cercaGrafo(unsigned int id, struct Grafo *grafo) {
    struct ArrT *corr = grafo->adj[0];
    for (int i = 1; i < grafo->size && corr->id != id; i++) {
        corr = grafo->adj[i];
    }
    return corr;
}

void Relax(struct ArrT *u, struct ArrT *v) {
    if (v->d > u->d + PESO + v->id) {
        v->d = u->d + PESO + v->id;
        v->p = cercaGrafo(u->id, g);
    }
}

void Dijkstra(struct Grafo *grafo) {
    grafo->adj[0]->d = 0;
    //q = creaCoda(grafo);
    struct ArrT *u;
    int i = 0;
    while (i< g->size) {
        u = g->adj[i];
        i++;
        struct Adj *vl = u->list;

        for (; vl != NULL; vl = vl->next) {
            Relax(u, cercaGrafo(vl->id, grafo));
        }
    }
/*    free(q);
    q = NULL;*/
}

struct Res *nuovoRes(unsigned int res, struct Res *andata) {
    struct Res *nuovo = malloc(sizeof(struct Res));
    nuovo->id = res;
    nuovo->next = andata;
    andata = nuovo;
    return andata;
}

void distruggiRes() {
    struct Res *succ = NULL;
    for (; risultatoAndata != NULL;) {
        succ = risultatoAndata->next;
        free(risultatoAndata);
        risultatoAndata = succ;
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


                unsigned int arr[numTotStazioni];
                int i = calcolaViaggio(cercaStazione(src), cercaStazione(dest), arr);

                g = creaGrafoAnd(arr, i);

                Dijkstra(g);
                // cerca g.adj[i].id == dest

                struct ArrT *corr;
                corr = g->adj[g->size - 1];


                while (corr->id != src) {
                    risultatoAndata = nuovoRes(corr->id, risultatoAndata);
                    if (corr->p != NULL && corr->id != src)
                        corr = corr->p;
                    else {
                        ok = false;
                        distruggiRes();
                        distruggiGrafo();
                        printf("nessun percorso\n");
                        break;
                    }
                }

                if (ok) {
                    risultatoAndata = nuovoRes(src, risultatoAndata);
                    stampaResAnd(risultatoAndata);
                    distruggiRes();
                    distruggiGrafo();
                }
            } else {
                unsigned int arr[numTotStazioni];
                int i = calcolaViaggio(cercaStazione(src), cercaStazione(dest), arr);
                g = creaGrafoRit(arr, i);
                Dijkstra(g);
                struct ArrT *corr = g->adj[g->size - 1];
                assert(corr != NULL);
                while (corr->id != src) {
                    risultatoAndata = nuovoRes(corr->id, risultatoAndata);
                    if (corr->p != NULL && corr->id != src)
                        corr = corr->p;
                    else {
                        distruggiRes();
                        distruggiGrafo();
                        ok = false;
                        printf("nessun percorso\n");
                        break;
                    }
                }
                if (ok) {
                    risultatoAndata = nuovoRes(src, risultatoAndata);
                    stampaResAnd(risultatoAndata);
                    distruggiRes();
                    if (g != NULL)
                        distruggiGrafo();
                }
            }

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