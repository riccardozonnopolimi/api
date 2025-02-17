#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAX_VEHICLES 512

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
    struct Res *before;
};
/*struct Albero {
    int id;
    struct Albero *next;
    struct Albero *before;
};*/






struct Stazione *testa = NULL;
int numTotStazioni = 0;
struct Res *coda = NULL;
struct Res *risultatoAndata = NULL;
struct Res *lista2 = NULL;
//struct Albero *albero = NULL;

struct Res *nuovoRes(unsigned int res, struct Res *andata) {
    struct Res *nuovo = malloc(sizeof(struct Res));
    nuovo->id = res;
    nuovo->next = andata;
    nuovo->before = NULL;
    if (andata == NULL) {
        coda = nuovo;
    } else {
        andata->before = nuovo;
    }
    andata = nuovo;
    return andata;
}

void distruggiRes(struct Res *distruggi) {
    struct Res *succ;
    if (distruggi == risultatoAndata)
        risultatoAndata = NULL;
    else
        lista2 = NULL;
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
//void stampaResRit() {
//    struct Res *res = coda;
//    for (; res->before != NULL; res = res->before)
//        printf("%d ", res->id);
//    printf("%d", res->id);
//    printf("\n");
//}
/*struct Albero *creaNodo(int id){
    struct Albero *nuovo;
    nuovo = malloc(sizeof (struct Albero));
    if (nuovo == NULL){
        printf("non ce memoria\n");
        return NULL;
    }
    nuovo->id = id;
    return nuovo;
}*/

/*
void inserisciNodo(struct Albero *nodo){

}
*/













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
        // printf("non rottamata\n");
        return false;
    }
    struct Auto *corrente = stazione->veicoli, *precedente = NULL;
    for (; corrente != NULL && corrente->autonomy != autonomia; precedente = corrente, corrente = corrente->next);

    //auto non presente
    if (corrente == NULL) {
        // printf("non rottamata\n");
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
    //printf("rottamata\n");
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
        //printf("non aggiunta\n");
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
    // printf("aggiunta\n");
}
//
////todo ho provato mettendo una sola stazione e r=demolendola e non funziona
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


//void pianificaPercorsoAndata(struct Stazione *src, struct Stazione *dest) {
//    unsigned int risultatoAndata[numTotStazioni], k = 1;
//    struct Stazione *head = src;
//    risultatoAndata[0] = src->id;
//
//    if (src == NULL || dest == NULL) {
//        printf("nessun percorso\n");
//        return;
//    }
//
//    //finche non trovo una stazione che mi fa arrivare a destinazione
//    while (head->id + head->maxAutonomy <= dest->id) {
//
//        //distanza massima che la stazione in cui sono mi permette di raggiungere
//        unsigned int distMax = head->id + head->maxAutonomy, max = 0;
//
//        struct Stazione *corrente = head->next;
//
//        int i;
//
//        if (corrente->id <= distMax) {
//            //trovo numero di stazioni raggiungibili da dove mi trovo
//            for (i = 0; corrente->id <= distMax; i++) {
//                if (corrente->next != NULL)
//                    corrente = corrente->next;
//                else
//                    break;
//            }
//
//            //todo capire come dimensionare l'array di possibili in un altro modo
//            struct Stazione *possibili[i];
//
//            //salvo le possibili stazioni su cui devo effettuare scelta
//            for (i = 0, corrente = head->next; corrente->id <= distMax; i++) {
//                possibili[i] = corrente;
//                if (corrente->next != NULL)
//                    corrente = corrente->next;
//                else
//                    break;
//            }
//            //scelgo tra le possibili
//            for (int j = 0; j < i; j++) {
//                if (possibili[j]->id + possibili[j]->maxAutonomy > max) {
//                    max = possibili[j]->id + possibili[j]->maxAutonomy;
//                    corrente = possibili[j];
//                }
//            }
//
//            //salvo risultatoAndata e sposto head alla stazione in cui ho scelto di andare
//            risultatoAndata[k] = corrente->id;
//            k++;
//            head = corrente;
//        } else {
//            printf("nessun percorso\n");
//            return;
//        }
//    }
//
//    //aggiungo destinazione all'elenco di stazioni
//    risultatoAndata[k] = dest->id;
//
//    //stampo l'elenco di stazioni
//    for (int i = 0; i < k + 1; ++i) {
//        printf("%d ", risultatoAndata[i]);
//    }
//    printf("\n");
//}
//
//
//void pianificaPercorsoRitorno(struct Stazione *src, struct Stazione *dest) {
//    unsigned int result[numTotStazioni], k = 1;
//    struct Stazione *head = src;
//    result[0] = src->id;
//    //finche non trovo una stazione che mi fa arrivare a destinazione
//    while (head->id - head->maxAutonomy > dest->id) {
//
//        //distanza massima che la stazione in cui sono mi permette di raggiungere
//        unsigned int distMax = head->id - head->maxAutonomy, max = 0;
//
//        struct Stazione *corrente = head->before;
//        int i;
//        if (corrente->id - corrente->maxAutonomy >= distMax) {
//            //trovo numero di stazioni raggiungibili da dove mi trovo
//            for (i = 0; corrente->id >= distMax; i++)
//                if (corrente->before != NULL)
//                    corrente = corrente->before;
//
//            //todo capire come dimensionare l'array di possibili in un altro modo
//            struct Stazione *possibili[i];
//
//            //salvo le possibili stazioni su cui devo effettuare scelta
//            for (i = 0, corrente = head->before; corrente->id >= distMax; i++) {
//                possibili[i] = corrente;
//                if (corrente->before != NULL)
//                    corrente = corrente->before;
//            }
//
//            //scelgo tra le possibili
//            for (int j = 0; j < i; j++) {
//                if (possibili[j]->id - possibili[j]->maxAutonomy <= max) {
//                    max = possibili[j]->id - possibili[j]->maxAutonomy;
//                    corrente = possibili[j];
//                }
//            }
//
//            //alla fine sposto head alla stazione in cui ho scelto di andare
//            result[k] = corrente->id;
//            head = corrente;
//            k++;
//        } else {
//            printf("nessun percorso\n");
//            return;
//        }
//    }
//
//    //aggiungo destinazione all'elenco di stazioni
//    result[k] = dest->id;
//
//    //stampo l'elenco di stazioni
//    for (int i = 0; i < k + 1; ++i) {
//        printf("%d ", result[i]);
//    }
//    printf("\n");
//}
//
///*void printAll() {
//    struct Stazione *head = testa;
//
//    struct Auto *veicolo;
//    int j = 1;
//    for (int i = 0; head != NULL; ++i) {
//        printf("Stazione al chilometro: %d  \n", head->id);
//        printf("Ha %d veicoli \n", head->numVehicles);
//        printf("Autonomia massima: %d km \n", head->maxAutonomy);
//        veicolo = head->veicoli;
//        j = 1;
//        while (veicolo != NULL) {
//            printf("Il veicolo n.%d ha %d km di autonomia \n", j, veicolo->autonomy);
//            veicolo = veicolo->next;
//            j++;
//        }
//        head = head->next;
//    }
//}*/

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

//
//
//void appendice2(struct Stazione *src, struct Stazione *dst,unsigned  int passi) {
//    if (src->id == dst->id || passi == 0) {
//        if (ritorno != NULL)
//            distruggiRes(ritorno);
//        return;
//    }
//    if (src->id - src->maxAutonomy <= dst->id) {
//        ritorno = nuovoRes( src->id , ritorno);
//    } else {
//        //printf("richiamo pianifica percorso\n");
//        if (src->before != NULL) {
//            appendice2(src->before, dst, passi);
//            return;
//        } else
//            return;
//    }
//}
//
//bool appendiceRitorno(struct Stazione *src, struct Stazione *t, struct Stazione *q, unsigned int passi) {
//    struct Stazione *p = t->before;
//    if (p->id == q->id) {
//        return false;
//    }
//    if (p->id - p->maxAutonomy <= q->id) {
//        appendice2(src, p,passi);
//        if (ritorno != NULL){
//            stampaResAnd(ritorno);
//            distruggiRes(ritorno);
//            distruggiRes(risultatoAndata);
//            return true;
//        }
//        return false;
//    } else{
//        return appendiceRitorno(src,p,q,passi);
//    }
//}


bool controllo(unsigned int pre, unsigned int cor, unsigned int suc) {
    struct Stazione *prec = cercaStazione(pre);
    struct Stazione *corr = cercaStazione(cor);
    struct Stazione *succ = cercaStazione(suc);
    struct Res *nuovo = NULL;
    bool flag = false;

    while (corr->next != succ) {

        unsigned int distmax = prec->id - prec->maxAutonomy;

        unsigned int corrnextid = corr->before->id;
        unsigned int corrnextmaxaut = corr->before->maxAutonomy;
        unsigned int succid = succ->id;
        if (corrnextid - corrnextmaxaut <= succid) {
            if (distmax <= corrnextid) {
                nuovo = cercaRes(corr->id, lista2);
                if (nuovo != NULL) {
                    nuovo->id = corr->before->id;
                    flag = true;
                } else
                    break;
            }
        }
        corr = corr->before;
    }
    return flag;
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
            unsigned int i;
            bool ok = true;
            if (scanf("%d %d", &src, &dest) == 0)
                printf("input errato\n");
            if (src < dest) {
                i = pianificaPercorsoAndata(cercaStazione(src), cercaStazione(dest));
                if (i == 0) {
                    printf("nessun percorso\n");
                } else {
                    risultatoAndata = nuovoRes(dest, risultatoAndata);
                    risultatoAndata = nuovoRes(i, risultatoAndata);
                    unsigned int k = i;
                    for (int j = 0; src != k; j++) {
                        k = pianificaPercorsoAndata(cercaStazione(src), cercaStazione(k));
                        if (k == 0 && src != risultatoAndata->id) {
                            printf("nessun percorso\n");
                            ok = false;
                            distruggiRes(risultatoAndata);
                            break;
                        }
                        risultatoAndata = nuovoRes(k, risultatoAndata);
                    }

                    if (ok) {
                        stampaResAnd(risultatoAndata);
                        distruggiRes(risultatoAndata);
                    }
                }
            } else {
                i = pianificaPercorsoRitorno(cercaStazione(src), cercaStazione(dest));
                if (i == 0) {
                    printf("nessun percorso\n");
                } else {
                    risultatoAndata = nuovoRes(dest, risultatoAndata);
                    risultatoAndata = nuovoRes(i, risultatoAndata);
                    unsigned int k = i;
                    for (int j = 0; src != k; j++) {
                        k = pianificaPercorsoRitorno(cercaStazione(src), cercaStazione(k));
                        if (k == 0 && src != risultatoAndata->id) {
                            printf("nessun percorso\n");
                            ok = false;
                            distruggiRes(risultatoAndata);
                            break;
                        }
                        risultatoAndata = nuovoRes(k, risultatoAndata);
                    }
                    if (ok) {
//
                        i = pianificaPercorsoRitorno(cercaStazione(src), cercaStazione(dest));
                        if (i == 0) {
                            printf("nessun percorso\n");
                        } else {
                            lista2 = nuovoRes(dest, lista2);
                            lista2 = nuovoRes(i, lista2);
                            unsigned int l = i;
                            for (int j = 0; src != l; j++) {
                                l = pianificaPercorsoRitorno(cercaStazione(src), cercaStazione(l));
                                if (l == 0 && src != lista2->id) {
                                    printf("nessun percorso alternativo\n");
                                    ok = false;
                                    stampaResAnd(lista2);
                                    distruggiRes(lista2);
                                    stampaResAnd(risultatoAndata);
                                    distruggiRes(risultatoAndata);
                                    break;
                                }
                                lista2 = nuovoRes(l, lista2);
                            }
                            if (ok) {
                                bool control = true;
                                struct Res *ris = lista2;
                                if (ris->next != NULL && ris->next->next != NULL)
                                    while (control || ris->next->next->id != dest || ris->id == src) {
                                        if ( ris->next->next->id != dest) {
                                            if (ris->next != NULL && ris->next->next != NULL) {
                                                control = controllo(ris->id, ris->next->id, ris->next->next->id);
                                                ris = ris->next;
                                            }
                                        }
                                        else
                                            ris = lista2;
                                    }
                                stampaResAnd(lista2);
                                distruggiRes(lista2);
                                distruggiRes(risultatoAndata);
                            }
                        }
                    }
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
