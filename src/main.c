#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define MAX_VEHICLES 512
#define MAX_QUEUE 20000
#define INFINITO 100
#define PESO 1


struct Auto {
    int autonomy;
    struct Auto *next;
};

struct Stazione {
    int id;
    struct Auto *veicoli;
    int numVehicles;
    int maxAutonomy;
    struct Stazione *next;
    struct Stazione *before;
};

struct Res {
    int id;
    struct Res *next;
};

struct Nodo {
    int id;
    struct Nodo *p;
    int d;
    int adj;
};

struct Grafo {
    struct Nodo *adj[MAX_QUEUE];
    int size;
};

struct Stazione *testa = NULL;
int numTotStazioni = 0;
struct Res *risultatoAndata = NULL;
struct Grafo *g = NULL;

struct Stazione *cercaStazione(int id) {
    struct Stazione *result = testa;
    for (; result != NULL && result->id != id; result = result->next);
    return result;
}

struct Nodo *creaArrT(struct Stazione *id) {
    struct Nodo *nuovo = malloc(sizeof(struct Nodo));
    nuovo->id = id->id;
    nuovo->d = INFINITO;
    nuovo->p = NULL;
    return nuovo;
}

int calcoloAdjRit(struct Stazione *staz, struct Stazione *dst) {
    struct Stazione *corr = staz;
    int distMax = corr->id - corr->maxAutonomy;
    int i = 0;
    if (corr->id == dst->id)
        return 0;
    corr = corr->before;
    while (distMax <= corr->id && corr->id >= dst->id) {
        i++;
        if (corr->before != NULL) {
            corr = corr->before;
        } else
            break;
    }
    return i;
}

struct Grafo *creaGrafoRit(void *stazioni[], int i) {
    struct Grafo *nuovo = malloc(sizeof(struct Grafo));
    nuovo->size = i;
    int k;
    for (int j = i - 1; j >= 0; j--) {
        nuovo->adj[j] = creaArrT(stazioni[j]);
        k = calcoloAdjRit(stazioni[j], stazioni[i - 1]);
        nuovo->adj[j]->adj = k;
    }
    return nuovo;
}

int calcolaViaggio(struct Stazione *src, struct Stazione *dst, void *stazioni[]) {
    int i = 0;
    if (src->id < dst->id) {
        while (src->id != dst->id) {
            stazioni[i] = src;
            i++;
            if (src->next != NULL) {
                src = src->next;
            } else break;
        }
        stazioni[i] = dst;
        i++;
        return i;
    } else {
        while (src->id != dst->id) {
            stazioni[i] = src;
            i++;
            if (src->before != NULL) {
                src = src->before;
            } else break;
        }
        stazioni[i] = dst;
        i++;
        return i;
    }
}

/*void distruggiArrT(struct Nodo *head) {
    free(head);
}*/

void distruggiGrafo() {
    for (int i = g->size; i > 0; --i) {
        free(g->adj[i - 1]);
    }
    free(g);
}

void Dijkstra(struct Grafo *grafo) {
    grafo->adj[0]->d = 0;
    //q = creaCoda(grafo);
    struct Nodo *u,*v;
    int i = 0;
    while (i < grafo->size) {
        u = grafo->adj[i];
            for (int l = u->adj; l > 0;l-- ) {
                v = grafo->adj[i + l];
                if (v->d >= u->d + PESO) {
                    v->d = u->d + PESO;
                    v->p = u;
                }
            }
        i++;
    }
}

struct Res *nuovoRes(int res, struct Res *andata) {
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

bool inserisciAuto(int autonomia, struct Stazione *stazione) {
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

bool rottamaAuto(struct Stazione *stazione, int autonomia) {
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

bool nuovaStazione(int id, int numAuto, int veicoli[]) {
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
    //se è la prima
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
    //se nella lista non è presente
    if (corrente == NULL) {
        return false;
    }
        //se prima stazione della lista
    else if (corrente->before == NULL) {
        //se non e'l'unico nella lista
        if (corrente->next != NULL) {
            testa = corrente->next;
            corrente->next->before = corrente->before;
        } else {
            testa = NULL;
        }
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
        free(corrente);
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
}
struct Stazione *pianificaPercorsoAndata(struct Stazione *src, struct Stazione *dst) {
    if (src->id == dst->id) {
        return NULL;
    }
    if (src->id + src->maxAutonomy >= dst->id) {
        return src;
    } else {
        if (src->next != NULL) {
            return pianificaPercorsoAndata(src->next, dst);
        } else
            return 0;
    }
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
            int autonomy, id;
            if (scanf("%d %d", &id, &autonomy) == 0) { printf("input errato"); }
            if (inserisciAuto(autonomy, cercaStazione(id)))
                printf("aggiunta\n");

        } else if (strcmp(str, aggiungi_stazione) == 0) {
            int id, numAuto;
            if (scanf("%d %d", &id, &numAuto) == 0) { printf("input errato\n"); }
            int autonomy[numAuto];
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
            int id;
            if (scanf("%d", &id) == 0)
                printf("input errato\n");
            if (demolisciStazione(cercaStazione(id)))
                printf("demolita\n");
            else
                printf("non demolita\n");

        } else if (strcmp(str, rottama_auto) == 0) {
            int id, autonomia;
            if (scanf("%d %d", &id, &autonomia) == 0)
                printf("input errato\n");
            if (rottamaAuto(cercaStazione(id), autonomia))
                printf("rottamata\n");
            else printf("non rottamata\n");

        } else if (strcmp(str, pianifica_percorso) == 0) {
            int src, dest;
            bool ok = true;
            if (scanf("%d %d", &src, &dest) == 0)
                printf("input errato\n");
            struct Stazione *sorg = cercaStazione(src), *dst = cercaStazione(dest);
            if (sorg == NULL || dst == NULL) {
                printf("nessun percorso\n");
            } else if (src < dest) {
                struct Stazione *i;
                i = pianificaPercorsoAndata(sorg, dst);
                if (i == 0) {
                    printf("nessun percorso\n");
                } else {
                    risultatoAndata = nuovoRes(dest, risultatoAndata);
                    risultatoAndata = nuovoRes(i->id, risultatoAndata);
                    struct Stazione *k = i;
                    for (int j = 0; sorg != k; j++) {
                        k = pianificaPercorsoAndata(sorg, k);
                        if (k == NULL && src != risultatoAndata->id) {
                            printf("nessun percorso\n");
                            ok = false;
                            distruggiRes();
                            break;
                        }
                        risultatoAndata = nuovoRes(k->id, risultatoAndata);
                    }

                    if (ok) {
                        stampaResAnd(risultatoAndata);
                        distruggiRes();
                    }
                }
            } else {
                void *arr[numTotStazioni];
                int i = calcolaViaggio(sorg, dst, arr);
                g = creaGrafoRit(arr, i);
                Dijkstra(g);
                struct Nodo *corr;
                corr = g->adj[g->size - 1];
                while (corr->id != src) {
                    risultatoAndata = nuovoRes(corr->id, risultatoAndata);
                    if (corr->p != NULL)
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