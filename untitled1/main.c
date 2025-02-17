#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define MAX_VEHICLES 512
//#define INFINITY 99999





typedef struct {
    int autonomy;
    struct Auto *next;
} Auto;

typedef struct Stazione {
    int id;
    Auto *veicoli;
    int numVehicles;
    int maxAutonomy;
    struct Stazione *next;
    struct Stazione *before;
} Stazione;

/*typedef struct {
    //todo lista con testa before null e coda next null
    Stazione stations[MAX_STATIONS];
    int numStations;
} Highway;*/




//variabili globali
Stazione *testa = NULL;

int numTotStation = 0;


Auto *creaAuto(int autonomy) {
    printf("creaAuto : inizio funzione\n");
    Auto *v;
    v = (Auto *) malloc(sizeof(Auto));
    if (v == NULL) {
        printf("Impossibile creare veicolo: memoria esaurita\n");
        exit(0);
    }
    v->autonomy = autonomy;
    v->next = NULL;
    printf("auto con autonomia %d creata con successo\n", autonomy);
    return v;
}

bool inserisciAuto(Auto *vehicle, struct Stazione *station) {
    printf("inserisciAuto : inizio funzione\n");
    if (station->numVehicles < MAX_VEHICLES) {
        //inserisce il veicolo in testa alla lista aggiornando la massima autonomia se necessario
      //  if (station->veicoli == NULL) {
        //    vehicle->next = NULL;
          //  station->veicoli = vehicle;
        //} else {
            vehicle->next = (struct Auto *) station->veicoli;
            station->veicoli = vehicle;
        //}
        //eventuale modifica della autonomia massima della stazione
        if (station->maxAutonomy < vehicle->autonomy)
            station->maxAutonomy = vehicle->autonomy;
        printf("nuova auto con autonomia %d e'stata inserita nella stazione %d\n", vehicle->autonomy, station->id);
        return true;
    }
    printf("numero massimo di veicoli raggiunto nella stazione %d \n", station->id);
    return false;
}

void calculateMax(Stazione *station) {
    Auto *v = station->veicoli;
    for (int i = 0; v->next != NULL; ++i) {
        if (v->autonomy > station->maxAutonomy)
            station->maxAutonomy = v->autonomy;
        v = (Auto *) v->next;
    }

}


Stazione *creaStazione(int id, int numVehicles, int vehicles[]) {
    Stazione *station;
    //creo spazio nuova stazione di servizio
    station = (Stazione *) malloc(sizeof(Stazione));
    if (station == NULL) {
        printf("Impossibile creare stazione: memoria esaurita\n");
        exit(1);
    }
    //inserisco i dati della stazione
    printf("inizializzo i dati della stazione\n");
    for (int i = 0; i < numVehicles; ++i) {
        printf("%d\n", vehicles[i]);
    }
    station->numVehicles = numVehicles;
    station->id = id;
    station->veicoli = NULL;
    //inserisco i veicoli della stazione
    Auto *v;

    for (int i = 0; i < numVehicles; ++i) {
        v = creaAuto(vehicles[i]);
        inserisciAuto(v, station);
    }
    //calcolo maxAutonomy della stazione
    calculateMax(station);
    printf("calcolo massimo\n");
    numTotStation++;
    printf("stazione creata\n");
    return station;
}

Stazione *inserisciStazione(Stazione *station) {
    //da modificare in base a struttura dati utilizzata
    //ora imposto con lista
    Stazione *s = NULL, *head = testa;
    printf("inserisci stazione: entro nella funzione\n");
    if (head == NULL) {
        station->next = NULL;
        station->before = NULL;
        head = station;
        printf("inserisci stazione: stazione inserita\n");
        return station;
    } else { // todo da riguardare a mente fresca
        s = head;
        if (station->id < head->id) {
            s->next = head;
            head->before = s;
            s->before = NULL;
        } else {
            Stazione *tmp = testa, *tmp1;
            while (s->id < tmp->id || tmp->next != NULL){
                tmp = tmp->next;
            }
            tmp1 = tmp;
            tmp->next = s;
            s->next = tmp1->next;
            tmp->before = s;
            s->before = tmp1;

        }
        printf("inserisci stazione: stazione inserita\n");
        return testa;
    }


}



//todo da separare ricerca veicolo da rimozione e cambiare parametro da int a veicolo
bool rottamaAuto(int autonomy, Stazione *station) {
    Auto *v = station->veicoli;
    Auto *prec = NULL;
    for (int i = 0; i < station->numVehicles; ++i) {
        if (v != NULL && v->autonomy != autonomy) {
            prec = v;
            v = (Auto *) v->next;
            continue;
        }
        if (v != NULL && v->autonomy == autonomy) {
            if (station->maxAutonomy == v->autonomy)
                calculateMax(station);
            if (v->next != NULL)
                prec->next = v->next;
            if (v->next == NULL)
                prec->next = NULL;
            free(v);
            station->numVehicles--;
            return true;
        }
    }
    return false;
}


bool demolisciStazione(Stazione *stazione) {
    Stazione *s = stazione;
    Stazione *tmp;
    Auto *v;
    Auto *next;

    next = s->veicoli;
    for (int i = 0; i < s->numVehicles; ++i) {
        v = next;
        next = (Auto *) next->next;
        rottamaAuto(v->autonomy, s);
    }

    tmp = s->before;
    tmp->next = s->next;
    s->next->before = tmp;
    free(s);
    numTotStation--;
    return true;
}


void pianificaPercorso(Stazione *src, Stazione *dst) {
    //todo
}

Stazione *cercaStazione(int idStazione) {
    Stazione *result = testa;
    while (result->id != idStazione) {
        result = result->next;
    }
    //todo
    return result;
}

void printAll() {
    Stazione *head = testa;

    Auto *veicolo = head->veicoli;
    int j = 1;
    for (int i = 0; head != NULL; ++i) {
        printf("Stazione al chilometro: %d  \n", head->id);
        printf("Ha %d veicoli \n", head->numVehicles);
        printf("Autonomia massima: %d km \n", head->maxAutonomy);

        while (veicolo->next != NULL) {
            printf("Il veicolo n.%d ha %d km di autonomia \n", j, veicolo->autonomy);
            veicolo = (Auto *) veicolo->next;
            j++;
        }
        head = head->next;
    }
}

int main() {
    char str[1000];
    int i = 0;
    while (i < 2) {
        i++;
        scanf("%s", str);
        switch (str[0]) {
            case 'a':
                switch (str[9]) {
                    case 'a': {
                        int autonomy, id;
                        Stazione *stazione;
                        Auto *nuova;
                        scanf("%d %d", &id, &autonomy);
                        stazione = cercaStazione(id);
                        nuova = creaAuto(autonomy);
                        if (inserisciAuto(nuova, stazione))
                            stazione->numVehicles++;
                        break;
                    }
                    case 's': {
                        int id, numAuto;
                        Stazione *stazione;
                        scanf("%d %d", &id, &numAuto);
                        int autonomy[numAuto];
                        for (int j = 0; j < numAuto; ++j) {
                            scanf("%d", &autonomy[j]);
                        }
                        stazione = creaStazione(id, numAuto, autonomy);
                        testa = inserisciStazione(stazione);
                        break;
                    }
                    default:
                        printf("default interno\n");
                        break;
                }
                break;
            case 'd': {
                int id;
                Stazione *stazione;
                scanf("%d", &id);
                stazione = cercaStazione(id);
                demolisciStazione(stazione);
                break;
            }
            case 'p': {
                int src, dst;
                scanf("%d %d", &src, &dst);
                Stazione *source, *destination;
                source = cercaStazione(src);
                destination = cercaStazione(dst);
                pianificaPercorso(source, destination);
                break;
            }
            case 'r': {
                int id, autonomia;
                Stazione *stazione;
                scanf("%d %d", &id, &autonomia);
                stazione = cercaStazione(id);
                rottamaAuto(autonomia, stazione);
                break;
            }
            default:
                printf("default esterno\n");
                break;
        }
    }
    printf("Stampo tutte le stazioni:\n");
    printAll();
    printf("%d\n", testa->numVehicles);
    return 0;
}




/*
 * forse serve per dijkstra
typedef struct {
    int id;
    int distance;
    bool visited;
    int previous; // ID della stazione precedente nel cammino minimo
} Station;
*/




/*
// Funzione per inserire una stazione nell'autostrada in modo ordinato
void insertStationSorted(Highway *highway, Stazione station) {
    if (highway->numStations >= MAX_STATIONS) {
        return; // Array pieno, non è possibile inserire ulteriori stazioni
        //aggiungere funzione che fa malloc aggiungendo un 20/30% in piu all'array
    }
    Stazione tmp;
    int i = highway->numStations - 1;
    while (i >= 0 && highway->stations[i].id > station.id) {
        // Shift degli elementi maggiori per fare spazio alla nuova stazione
        tmp = highway->stations[i];
        highway->stations[i + 1] = highway->stations[i];
        i--;
    }

    highway->stations[i + 1] = station;
    highway->numStations++;
}
*/





/*

// Funzione per cancellare una stazione dall'autostrada
void deleteStation(Highway *highway, int stationId) {
    int i, index = -1;

    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].id == stationId) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        // Shift dell'array per rimuovere la stazione
        for (i = index; i < highway->numStations - 1; i++) {
            highway->stations[i] = highway->stations[i + 1];
        }
        highway->numStations--;
    }
}




// Funzione per inserire un veicolo in una stazione specifica
void insertVehicle(Stazione *station, Auto vehicle) {
    if (station->numVehicles < MAX_VEHICLES) {
        station->vehicles[station->numVehicles++] = vehicle;
    }
}

// Funzione per cancellare un veicolo da una stazione specifica
void deleteVehicle(Stazione *station, int vehicleId) {
    int i, index = -1;

    for (i = 0; i < station->numVehicles; i++) {
        if (station->vehicles[i].autonomy == vehicleId) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        // Shift dell'array per rimuovere il veicolo
        for (i = index; i < station->numVehicles - 1; i++) {
            station->vehicles[i] = station->vehicles[i + 1];
        }
        station->numVehicles--;
    }
}
*/











/*

// Funzione per calcolare il percorso ottimale usando l'algoritmo di Dijkstra
void dijkstra(Highway *highway, int start, int end) {
    Station stations[MAX_STATIONS];
    int numStations = highway->numStations;


    // Inizializzazione delle stazioni
    for (int i = 0; i < numStations; i++) {
        stations[i].id = highway->stations[i].id;
        stations[i].distance = (i == start ? 0 : INFINITY);
        stations[i].visited = false;
        stations[i].previous = -1; // Nessuna stazione precedente nel cammino minimo
    }

    for (int i = 0; i < numStations - 1; i++) {
        // Trova la stazione con la distanza minima non visitata
        int currentStationIndex = findMinDistanceStation(stations, numStations);
        if (currentStationIndex == -1) {
            break; // Tutte le stazioni sono state visitate
        }

        // Imposta la stazione corrente come visitata
        stations[currentStationIndex].visited = true;

        // Aggiorna le distanze dei nodi adiacenti
        for (int j = 0; j < numStations; j++) {
            int distance = stations[currentStationIndex].distance > stations[j].distance ? stations[currentStationIndex].distance - stations[j].distance : stations[j].distance - stations[currentStationIndex].distance;
            if (!stations[j].visited && distance != INFINITY &&
                stations[currentStationIndex].distance + distance < stations[j].distance) {
                stations[j].distance = stations[currentStationIndex].distance + distance;
                stations[j].previous = currentStationIndex; // Salva l'ID della stazione precedente nel cammino minimo
            }
        }
    }

    // Il percorso ottimale dalla stazione di partenza alla stazione di destinazione è
    // contenuto in stations[end].distance.
    // Puoi seguire il percorso utilizzando la lista delle stazioni precedenti nel cammino minimo
    // e stampare gli ID delle stazioni che compongono il cammino.
    if (stations[end].distance == INFINITY) {
        printf("Nessun cammino trovato.\n");
        return;
    }

    printf("Cammino minimo da stazione %d a stazione %d:\n", start, end);
    int currentStation = end;
    while (currentStation != -1) {
        printf("%d ", stations[currentStation].id);
        currentStation = stations[currentStation].previous;
    }
    printf("\n");
}


// Funzione per trovare l'indice della stazione con la distanza minima non visitata
int findMinDistanceStation(Station stations[], int numStations) {
    int minDistance = INFINITY;
    int minIndex = -1;

    for (int i = 0; i < numStations; i++) {
        if (!stations[i].visited && stations[i].distance < minDistance) {
            minDistance = stations[i].distance;
            minIndex = i;
        }
    }

    return minIndex;
}
*/
