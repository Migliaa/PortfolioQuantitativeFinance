
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX_AEREI 5

#ifdef DEBUG
#define D(...) fprintf(stderr, __VA_ARGS__)
#else
#define D(...)
#endif


typedef struct {
    int x, y;
    int costo;   // Costo terrestre (0 = intransitabile, max 100)

    int aereo_count;                   
    int aereo_x[MAX_AEREI];  
    int aereo_y[MAX_AEREI];   
    int aereo_costo[MAX_AEREI];3 
} hex;

typedef struct {
    int col; 
    int rig;
    hex **m;  // mappa[y][x]
} map;

map mappa = {0, 0, NULL};

//direzioni
const int dx[6] =           { 0, +1, +1, 0, -1,-1 };
const int dy_pari[6] =      { -1, 0, +1, +1,+1, 0 };
const int dy_dispari[6]  =  { -1,-1,  0, +1, 0,-1 };


void toggle_air_route(){
     int x1, y1, x2, y2;
    if (scanf("%d %d %d %d",&x1,&y1,&x2,&y2) != 4) {
        printf("KO\n");
        return;
    }
    if(x1<0 || x1>= mappa.col || y1<0||y1>=mappa.rig||x2<0||x2>=mappa.col||y2<0||y2>=mappa.rig){
        printf("KO\n");
        return;
    }
    
    hex *cella=&mappa.m[y1][x1];

    for(int i=0;i<cella->aereo_count;i++){
        if(x2==cella->aereo_x[i] && y2==cella->aereo_y[i]){
            //rimuovi rotta
            for (int j=i; j < cella->aereo_count - 1; j++) {
                cella->aereo_x[j] = cella->aereo_x[j + 1];
                cella->aereo_y[j] = cella->aereo_y[j + 1];
                cella->aereo_costo[j]=cella->aereo_costo[j + 1];
            }
            cella->aereo_count--;
            printf("OK\n");
            D("rimossa una rotta\n");
            D("Riassunto tratte aeree con relativi costi: ");
            for(int i=0;i<cella->aereo_count;i++){
                D(" %d", cella->aereo_costo[i]);
            }
             D("\n");
            return;
        }
    }

    if (cella->aereo_count>=MAX_AEREI) {
        printf("KO\n");return;
    }

    //Calcola media (intera per difetto) dei costi esistenti + costo terrestre e aggiungere rotta
    int media = cella->costo;
    for (int i=0; i < cella->aereo_count; i++) {
        media += cella->aereo_costo[i];
    }
    media = media / (cella->aereo_count + 1); //da vedere se mantenere +1
    if (media > 100) media = 100;
    if (media < 0) media = 0;
    int k=cella->aereo_count;
    cella->aereo_x[k]=x2;
    cella->aereo_y[k]=y2;
    cella->aereo_costo[k]=media;
    cella->aereo_count++;
    D("Aggiunta una rotta\n costo tratta aerea: %d\n", media);
    printf("OK\n");

    D("Riassunto tratte aeree con relativi costi: ");
    for(int i=0;i<cella->aereo_count;i++){
        D(" %d", cella->aereo_costo[i]);
    }
    D("\n");

}

void libera_mappa() {
    if (mappa.m != NULL) {
        for (int i=0; i<mappa.rig; i++)
            free(mappa.m[i]);
        free(mappa.m);
        mappa.m = NULL;
    }
}

void init() {
    int c, r;
    if (scanf("%d %d",&c,&r) != 2||c <= 0||r <= 0) {
        printf("KO\n");
        return;
    }

    if (mappa.m!=NULL){
        libera_mappa();
    }
    
    mappa.col = c;
    mappa.rig = r;
    mappa.m= malloc(r*sizeof(hex *)); // ogni riga mappa.m[y] punta a un array di colonne (hex)

    if (mappa.m==NULL) {
        printf("KO\n");
        return;
    }
    //si può ottimizzare un pochino la malloc con "1 blocco + array di puntatori" ma difficile da scrivere
    for (int y=0; y<r; y++) {
        mappa.m[y] = malloc(c*sizeof(hex));

        if (mappa.m[y]==NULL) {
            printf("KO\n");
            return;
        }

        for (int x=0; x<c; x++) {
            hex *cella = &mappa.m[y][x];
            cella->x= x;
            cella->y= y;
            cella->costo= 1;
            cella->aereo_count= 0;
        }
    }

    printf("OK\n");
}

int DistEsagoni(int x1, int y1, int x2, int y2) {
    int dx= x2-x1;
    int dy= y2-y1;
    int dz= (0 -dx- dy);
    return ((abs(dx) + abs(dy) + abs(dz)) / 2);
}

void change_cost(){
    int x,y,v,raggio, dist;
    if(scanf("%d %d %d %d", &x,&y,&v,&raggio) != 4 || x<0 || x>=mappa.col || y<0 || y>=mappa.rig || v<-10 || v>10 || raggio<=0){
        printf("KO\n");
        return;
    }

    int costoAggiornato;
    hex *cella = NULL;
    int mediaAereo;
    for(int yi=0;yi<mappa.rig;yi++){
        for(int xi=0;xi<mappa.col;xi++){
            dist = DistEsagoni(x,y,xi,yi);
            if (dist >= raggio)
                continue;

            cella = &mappa.m[yi][xi];
            float rapporto = (float)(raggio - dist) / raggio;
            costoAggiornato = cella->costo + (int)floor(v * fmax(0, rapporto));
            if (costoAggiornato<0) costoAggiornato = 0;
            if (costoAggiornato>100) costoAggiornato = 100;

            cella->costo = costoAggiornato;
            
            
             for (int i=0; i<cella->aereo_count; i++) {
                mediaAereo = cella->costo;
                for (int j=0; j<cella->aereo_count; j++){
                    mediaAereo += cella->aereo_costo[j];
                }
                mediaAereo = mediaAereo / (cella->aereo_count+1);
                if (mediaAereo < 0) mediaAereo = 0;
                if (mediaAereo > 100) mediaAereo = 100;
                cella->aereo_costo[i] = mediaAereo; //ogni costo aereo è lo stesso, tutti hanno la stessa media alla fine
            }
                if(cella->aereo_count!=0){
                     D("\n\nChange_cost:\n Costo attuale cella: %d \nAggiornamento tratte aeree con relativi costi: ", cella->costo);
                     for(int i=0;i<cella->aereo_count;i++){
                         D(" %d", cella->aereo_costo[i]);
                     }
                D("\n");
            }
        }
    }
    printf("OK\n");
}

//TRAVEL_COST
//______________________________________________________________________
//inizializzazione dati heap
typedef struct {
    int x, y;
    int costo; // distanza stimata dalla sorgente
} nodo;

#define MAX_HEAP_DIM 999999
nodo heap[MAX_HEAP_DIM];
int heap_size;
//--------------------------

//funzioni per heap
void heap_push(nodo n){
        if (heap_size >= MAX_HEAP_DIM)
        return;
    
    int i= heap_size;
    heap_size++;
    heap[i]= n;

    //porta in alto il più piccolo
    while (i>0) {
        int p= (i-1)/2; //p è la posizione del padre quando l'albero è in formato array
        if (heap[p].costo <= heap[i].costo)
            break;
        nodo temp= heap[i];
        heap[i]= heap[p];
        heap[p]= temp;
        i=p;
    }
}
int heap_empty(){
    return heap_size == 0;
}

nodo heap_pop() {
    nodo min = heap[0];
    heap_size--;
    heap[0] = heap[heap_size];
    int i=0;

    while (1) {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int smallest= i;

        if (left < heap_size && heap[left].costo < heap[smallest].costo)
            smallest= left;
        if (right < heap_size && heap[right].costo < heap[smallest].costo)
            smallest= right;

        if (smallest == i)
            break;

        nodo temp= heap[i]; // = heap[0];
        heap[i]= heap[smallest]; //heap[0]=heap[smallest]
        heap[smallest]= temp; 
        i = smallest;
    }

    return min;
}
//______________

void travel_cost() {
    int x1,y1,x2,y2,risp;
    int raggiunto = 0;

    //input
    if(scanf("%d %d %d %d",&x1,&y1,&x2,&y2) != 4 || x1<0 || x1>=mappa.col || y1<0 || y1>=mappa.rig || x2<0 || x2>=mappa.col || y2<0 || y2>=mappa.rig ){
        risp=-1;
        printf("%d\n", risp);
        return;
    }

    //caso banale
    if (x1==x2 && y1==y2) {
        printf("0\n");
        return;
    }

    //preparazione Dijsktra
    int dimMappa = mappa.col * mappa.rig;
    int* distanza = malloc(dimMappa * sizeof(int));
    char* visitato = malloc(dimMappa * sizeof(char));
    if (distanza==NULL || visitato==NULL) {
        risp = -1;
        printf("%d\n",risp);
        return;
    }
    if (mappa.m[y1][x1].costo == 0 && !(x1==x2 && y1==y2)) {
        printf("-1\n");
        free(distanza);
        free(visitato);
        return;
    }
    //distanze a infinito(-1) e non visitato
    for (int i=0; i<dimMappa; i++) {
        distanza[i]= -1;
        visitato[i]= 0;
    }

    //distanza del punto di partenza = 0
    int index_start = y1 * mappa.col + x1;
    distanza[index_start]= 0;

    //inizializza heap con nodo di partenza
    heap_size= 0; //è inizializzato a livello globale
    nodo iniziale= {x1,y1, 0}; //x,y,costo
    heap_push(iniziale); //compatibile?

    //Dijkstra
    while (!heap_empty()) {
        nodo corrente = heap_pop(); //estrae la radice, che ha il costo minore da raggiungere. con la pop la elimino perchè ho la certezza di aver trovato il percorso minimo per arrivare lì
        int x = corrente.x;
        int y = corrente.y;
        int costo_corrente = corrente.costo;
        int i_corrente = y * mappa.col + x;

        if (visitato[i_corrente]){
            continue;
        }
        visitato[i_corrente] = 1;

        if(x==x2 && y==y2){
            printf("%d\n", corrente.costo);
            raggiunto = 1;
            free(distanza);
            free(visitato);
            //libera_mappa();
            return;
        }

//MOVIMENTO
        hex* esagono_corrente = &mappa.m[y][x];
        
        //parità dy
        const int* dy;
        if(esagono_corrente->y % 2 == 0)
            dy = dy_pari; //puntatori alla prima cella dell'array
        else
            dy = dy_dispari;
        
        //movimento terrestre
        for(int i=0; i<6;i++){
            int ax = esagono_corrente->x + dx[i];
            int ay = esagono_corrente->y + dy[i];

            
            //controllo la validità
            if(ax>=mappa.col || ay>=mappa.rig || ax<0 || ay<0 || esagono_corrente->costo == 0)
                continue;
             // hex* adiacente = &mappa.m[ax][ay]; //se valido, allora è definibile, ma non serve effettivamnete, è integrabile successivamente

            //calcolo costo nuovo e aggiorno se è il minore
            int nuovoCosto = costo_corrente + esagono_corrente->costo; //questo è il costo per raggiungere tutti gli adiacenti, se raggiungibili e se esistono
            int i_univoco_dest_adiac = ay * mappa.col + ax; 

            if(nuovoCosto < distanza[i_univoco_dest_adiac] || distanza[i_univoco_dest_adiac] == -1) {//la prima condizione significa che è stata appena trovato un nuovo percorso per raggiungere il nodo adiacente con un costo inferiore. ovvero passando dall'esagono_corrente
                distanza[i_univoco_dest_adiac]= nuovoCosto;
                nodo adiacente= {ax,ay,nuovoCosto};
                heap_push(adiacente);
            }
        }

        for(int i=0;i<esagono_corrente->aereo_count;i++){
            int destAereoX = esagono_corrente->aereo_x[i];
            int destAereoY= esagono_corrente-> aereo_y[i];
            int aereoCosto = esagono_corrente->aereo_costo[i];
            if (aereoCosto == 0)
                continue;
            int nuovoCosto = costo_corrente + aereoCosto; //per calcolare il costo per raggiungere la destinazione non devo più guardare il costo della cella corrente ma della rotta erea i-esima
            int i_univoco_dest_aerea = destAereoY * mappa.col + destAereoX; 

            if (nuovoCosto < distanza[i_univoco_dest_aerea] || distanza[i_univoco_dest_aerea] == -1){
                distanza[i_univoco_dest_aerea]= nuovoCosto;
                nodo adiacente = {destAereoX,destAereoY,nuovoCosto};
                heap_push(adiacente);
            }
        }
    }//while
    // nessun percorso trovato
    if (!raggiunto) {
    printf("-1\n");
    }
    else{
        risp = -1;
        printf("%d\n", risp);
    }
    free(distanza);
    free(visitato);
    //libera_mappa();

}

int main() {
    char comando[32];

    D("DEBUG ATTIVO\n");
    while (scanf("%s", comando) == 1) {
        if (strcmp(comando, "init") == 0) {
            init();
        } 
        else if (strcmp(comando, "change_cost") == 0) {
            change_cost();
        }
        else if (strcmp(comando, "travel_cost") == 0) {
            travel_cost();
        }
        else if (strcmp(comando, "toggle_air_route") == 0) {
            toggle_air_route();
        }
        else if(strcmp(comando, "terminatore") == 0){
            libera_mappa();
        }
    }
    libera_mappa();

    return 0;
}

