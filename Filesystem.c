#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DF 1024
#define DN 256
#define FILE 1
#define CART 0
#define CONT 25
#define NUM 2

typedef  struct testo_s{
    char conten[25];
    struct testo_s *next;
}testo_t;


typedef struct alb {
    char nome[DN];
    int carattere;      //1 cartella, 0 file
    int numfigli;
    int altezza;
    testo_t *contenuto;
    int *supporto;
    int *ordine;
    int dim;
    struct alb **figli;
} albero;

typedef struct percfind_s{
    struct alb *nodo;
    struct percfind_s *prev;
    struct percfind_s *next;
}percfind_t;

        //nodo->*figli=(**albero)malloc(sizeof(*albero)*DF);
        //for(i=0; i<DF; i++)
        //  nodo->figli[i]=NULL;

void create(albero*, int);
int percorsopadre(albero**, char *);
int percorsofiglio(albero**);
int modellohashing(char [], int, int, int);
void creanuovo(albero*, char[], int, int, int);
void deallocatutto(albero *);
void cancella(albero *);
void delete_r(albero *);
void write(albero*);
int inseriscicarattere(testo_t*);
void deallocafile(testo_t*);
void readfile(testo_t *);
void ordinafind(albero *);
int find(albero* , percfind_t *, char *);
void ordinafinddelete(albero *);
void preread(albero *);
void stampapercorso(percfind_t*);
void leggitutto(char);
void riallocavettori(albero*);



int main(){
    albero *head = NULL;
    int w, j;
    int check;
    char cerca[DN];
    int z;
    char crea[DN];
    head = (albero*)malloc(sizeof(albero));
    strcpy(head->nome, "radice");
    head->numfigli=0;
    head->carattere=CART;
    head->altezza=0;
    head->figli=NULL;
    head->ordine=NULL;
    head->supporto=NULL;
    head->dim=NUM*NUM;
    /*
    head->figli=(albero**)malloc(sizeof(albero*)*NUM*NUM);
    head->supporto = (int *) malloc(sizeof(int) * NUM*NUM);
    head->ordine = (int *) malloc(sizeof(int) * NUM*NUM);
    for(w=0; w<NUM*NUM; w++) {
        head->figli[w] = NULL;
    }
    for(w=0; w<NUM*NUM; w++)
        head->ordine[w]=-1;
    */

    int i=0;
    do {

        for(z=0; z<DN; z++)
            crea[z]='\0';
        scanf("%s", crea);          //prendo nome sottoprogramma (per ora CREA)


        if ((strcmp(crea, "create_dir")) == 0) {
            create(head, CART);
        }

        else if ((strcmp(crea, "create")) == 0) {
            create(head, FILE);
        }

        else if ((strcmp(crea, "read")) == 0) {
            preread(head);
        }


        else if ((strcmp(crea, "write")) == 0) {
            write(head);
        }


        else if ((strcmp(crea, "delete")) == 0) {
            cancella(head);
        }


        else if ((strcmp(crea, "delete_r")) == 0) {
            delete_r(head);
        }


        else if ((strcmp(crea, "find")) == 0) {
            scanf("%s", cerca);
            check = find(head, NULL, cerca);

            if(check==0)
                printf("no\n");

        }


        else if ((strcmp(crea, "exit")) == 0) {
            deallocatutto(head);
            //free(head->supporto);
            //free(head->ordine);
            //free(head->figli);
            //free(head);
            i=1;
        }


    }while (i==0);

    return 0;
}






//HASHING  OK

int modellohashing(char percorso[], int i, int r, int dim){                                          //i lunghezza stringa    r rehashing
    int c,j;
    int somma;
    c=i;

    somma=0;
    for(j=0; j<=c; j++){
        somma=somma+(((int)percorso[j]*j*r*(int)percorso[j])+(int)percorso[j]);             //r NECESSARIA REHASHING

    }
    somma=(somma%2041)%dim;                                                                         //prendo il resto per avere sempre risultati compresi fra 0 e 1024
    //printf("nome --> %s -->somma --> %d\n",percorso, somma);
    return somma;
}

//TROVAPERCORSOFINOALPADRE  OK
int percorsopadre(albero**nodo, char *nomefiglio) {

    char nomepercorso[DN];
    int j;
    int z;
    int i=0;
    int somma = 0;
    int r=1;          //rehashing
    int check;     //controllo per hashing, rehashing
    char c = getchar();
    while(c!='/')                   //arrivo fino al primo / per leggere tutto il percorso
        c=getchar();



    for(j=0;j<DN; j++)
        nomepercorso[j]='\0';               //azzero tutto il percorso

    while (c != '\n') {                         //WHILE PER ARRIVARE FINO ALLA FINE DEL PERCORSO
        c=getchar();
        z=1;
        while (c != '/' && c != '\n' && z<256){          //WHILE PER STUDIARE IL PERCORSO FINO ALLO SLASH
            nomepercorso[i] = c;
            i++;
            z++;
            c = getchar();
        }
        if(z>255) {
            leggitutto(c);
            return -1;

        }
        if(c=='/' && (*nodo)->carattere==FILE) {
            leggitutto(c);
            return -1;
        }



            nomepercorso[i]='\0';               //TERMINATORE
        if (c == '\n') {                        //sono arrivato al figlio. Ritorno il nome ora.
            strcpy(nomefiglio, nomepercorso);   //PASSO PER PUNTATORE IL NOME DEL FIGLIO AL SOTTOPROGRAMMA PRECEDENTE
            //printf("nome --> %s\n",nomefiglio);
            //fflush(stdout);
        } else if((*nodo)->figli!=NULL){
            r=1;
            do {
                somma = modellohashing(nomepercorso, i, r, (*nodo)->dim);                 //CHIAMO HASHING PER OTTENERE IL VALORE

                if((*nodo)->supporto[somma]==1 && (*nodo)->figli[somma]==NULL) {      //CHIAMA REHASHING NEL CASO TROVASSI UNO SPAZIO VUOTO MA PRIMA OCCUPATO
                    r++;
                    check = 1;
                }else {
                    if ((*nodo)->figli[somma] != NULL && (*nodo)->supporto[somma]==1) {               //QUI VA BENE: occupato e con 1
                        check = strcmp(nomepercorso, (*nodo)->figli[somma]->nome);                 //CONTROLLO CHE I NOMI COINCIDANO
                        if (check != 0) {                                   //NOMI NON COINCIDONO. NECESSARIO REHASHING.
                            r++;
                        } else {
                            (*nodo) = (*nodo)->figli[somma];
                        }
                    } else if ((*nodo)->figli[somma]==NULL && (*nodo)->supporto[somma]==0) {       //SE ARRIVO QUI NON ESISTE ELEMENTO.
                        leggitutto(c);
                        return -1;
                    }
                }
            } while (check != 0);
        }else if((*nodo)->figli==NULL)
            return -1;

        for(j=0; j<DN; j++)                 //AZZERO NOME PERCORSO PER SICUREZZA.
            nomepercorso[j]='\0';
        i = 0;                              //azzero contatore per stringa nomi file/cartelle

        if(c=='/' && (*nodo)->carattere==FILE) {
            leggitutto(c);
            return -1;
        }

    }
    return 1;
}



//TROVAPERCORSOFINOALFIGLIO  OK
int percorsofiglio(albero**nodo) {

    char nomepercorso[DN];
    int i=0;
    int j, z;
    int somma;
    int r;          //rehashing
    int check=0;     //controllo per hashing, rehashing
    char c;

    for(j=0;j<DN; j++)
        nomepercorso[j]='\0';           //INIZIALIZZO VETTORE

    c = getchar();
    while(c!='/')                          //MI MUOVO FINO A INIZIO PERCORSO
        c=getchar();

    while (c!=' ' && c!='\n') {             //WHILE PER ARRIVARE FINO AL FONDO
        c=getchar();
        z=1;
        while (c != '/' && c != ' ' && c!='\n' && z<256) {      //WHILE PER STUDIARE IL PERCORSO FINO ALLO SLASH
            nomepercorso[i] = c;
            i++;
            z++;
            c = getchar();
        }

        if(z>255) {
            leggitutto(c);
            return -1;
        }

        if(c=='/' && (*nodo)->carattere==FILE) {
            leggitutto(c);
            return -1;
        }

        if((*nodo)->figli==NULL && (*nodo)->carattere==CART){
            return -1;
        }

            nomepercorso[i]='\0';
            r = 1;
            do {
                somma = modellohashing(nomepercorso, i, r, (*nodo)->dim);                          //CHIAMATA PER HASHING

                if((*nodo)->supporto[somma]==1 && (*nodo)->figli[somma]==NULL) {          //PRIMO CASO: C'ERA MA E' STATO ELIMINATO --> REHASHING
                    r++;
                    check = 1;
                }else{
                    if ((*nodo)->supporto[somma]==0 && (*nodo)->figli[somma]==NULL) {        //SECONDO CASO: NON ESISTE. RITORNO NULL
                        leggitutto(c);
                        return -1;

                    }else if ((*nodo)->figli[somma] != NULL && (*nodo)->supporto[somma]==1) {               //controllo che sia presente
                            check = strcmp(nomepercorso, (*nodo)->figli[somma]->nome);
                            //printf("nomepercorso --> %s || nomefiglio --> %s\n", nomepercorso, (*nodo)->figli[somma]->nome);
                            //printf("check --> %d\n", check);
                            fflush(stdout);
                            if (check != 0) {
                                r++;
                            } else {
                                *nodo = (*nodo)->figli[somma];
                            }
                    }

                }
            } while (check != 0);
        i = 0; //azzero contatore per stringa nomi file/cartelle
        for(j=0;j<DN; j++)
            nomepercorso[j]='\0';           //AZZERO STRINGA

        if(c=='/' && (*nodo)->carattere==FILE) {
            leggitutto(c);
            return -1;
        }
    }
    return 1;
}



//INSERISCICARATTERE
int inseriscicarattere(testo_t* nodo){
    testo_t *n = NULL;           //temporaneo testo
    int contachar = 0;
    int maxval = 0;
    int tmp;
    char c;
    int i;


    n = (testo_t *) malloc(sizeof(testo_t));            //ALLOCO UN BLOCCO E LO INIZIALIZZO A ZERO
    for(i=0; i<25; i++)
        n->conten[i]='\0';

    n->next=NULL;
    do {
        c = getchar();
        n->conten[maxval]=c;
        maxval++;
    }while(maxval!=25 && c!='"' && c!='\n');
    if(c=='\n'){                    //CONTROLLO CHE CI SIANO LE VIRGOLETTE ALLA FINE.
        printf ("no\n");
        return 0;
    }


    if (c=='"') {//SONO ARRIVATO ALLA FINE: SALVO ULTIMI CARATTERI E ESCO
        tmp=maxval-1;
        n->conten[tmp] = '\0';
        nodo->next = n;
        maxval=maxval-1;
        return (contachar+maxval);                  //RITORNO CIO' CHE HO RICEVUTO PIU QUELLO CHE HO SCRITTO NEL MIO
    }else if(maxval==25) {
        n->conten[maxval] = '\0';
        nodo->next = n;
        contachar = inseriscicarattere(nodo->next);
    }
    return (contachar+maxval);
}


//CREANUOVOfilecartella
void creanuovo(albero *nodo, char nomerisorsa[], int categoria, int somma, int altezza){       //uso categoria per sapere se è file o cartella.
    albero *inserisci;
    int i;
        inserisci = (albero*)malloc(sizeof(albero));
        strcpy(inserisci->nome, nomerisorsa);
        inserisci->carattere = categoria;
        inserisci->numfigli = 0;
        inserisci->altezza = altezza+1;
        inserisci->contenuto=NULL;
        /*
        if(categoria==CART) {
            inserisci->dim=NUM*NUM;
            inserisci->figli = (albero **) malloc(sizeof(albero *) * NUM*NUM);
            inserisci->supporto = (int *) malloc(sizeof(int) * NUM*NUM);
            inserisci->ordine = (int *) malloc(sizeof(int) * NUM*NUM);
            for (i = 0; i < NUM*NUM; i++) {
                inserisci->supporto[i] = 0;
                inserisci->ordine[i] = -1;
                inserisci->figli[i] = NULL;
            }

        }else{*/
            inserisci->supporto=NULL;
            inserisci->figli=NULL;
            inserisci->ordine=NULL;
        //}

        nodo->figli[somma]=inserisci;
        nodo->supporto[somma]=1;
        nodo->numfigli++;
        i=0;
        while(nodo->ordine[i]!=-1) {
            i++;
        }
        nodo->ordine[i]=somma;

        //printf("supporto --> %d\n", nodo->supporto[somma]);
        ordinafind(nodo);
}

//CREATE_DIR_FILE
void create(albero*nodo, int car) {
    char nomefiglio[DN];
    int somma, check, len, r, check2, z, w, i;
    int somma1;
    char c;
    int risultato;
    for(z=0;z<DN;z++)           //INIZIALIZZO VETTORE PER NOME FIGLIO
        nomefiglio[z]='\0';
    c = getchar();             //prendo spazio

    risultato = percorsopadre(&nodo, nomefiglio);     //OTTENGO FINO AL PADRE
    //printf("risultato--> %d, nomepadrea--> %s\n", risultato, nodo->nome);

    if (risultato==1 && nodo->carattere==CART) {                       //CONTROLLO SUBITO CHE RISULTATO SIA CORRETTO, ALTRIMENTI ESCO.+

        if(nodo->figli==NULL){
            nodo->dim=NUM*NUM;
            nodo->figli = (albero **) malloc(sizeof(albero *) * NUM*NUM);
            nodo->supporto = (int *) malloc(sizeof(int) * NUM*NUM);
            nodo->ordine = (int *) malloc(sizeof(int) * NUM*NUM);
            for (i = 0; i < NUM*NUM; i++) {
                nodo->supporto[i] = 0;
                nodo->ordine[i] = -1;
                nodo->figli[i] = NULL;
            }
        }

        if(nodo->numfigli>=nodo->dim/NUM-1){
            riallocavettori(nodo);
        }


        len = strlen(nomefiglio);
        check = 1;
        r = 1;
        if (nodo->carattere != FILE) {
            if(nodo->numfigli<1024 && nodo->altezza<255) {
                do {

                    somma = modellohashing(nomefiglio, len, r, nodo->dim);
                    //printf("sommafiglio -->%d, supportopadre --> %d\n", somma, nodo->supporto[somma]);
                    //fflush(stdout);

                    if (nodo->figli[somma] != NULL) {
                        check2 = strcmp(nodo->figli[somma]->nome, nomefiglio);
                        if (check2 == 0) {
                            printf("no\n");
                            return;
                        } else
                            r++;
                    }else{

                        check = 0;
                        r++;
                        /*somma1=modellohashing(nomefiglio, len, r, nodo->dim);
                        printf("somma1-->%d\n", somma1);
                        while(nodo->ordine[somma1]!=-1){
                            if(strcmp(nodo->figli[somma1]->nome, nomefiglio)==0){
                                printf("no\n");
                                return;
                            }
                            r++;
                            printf("ciao");
                            fflush(stdout);
                            somma1=modellohashing(nomefiglio, len, r, nodo->dim);
                        }*/
                        creanuovo(nodo, nomefiglio, car, somma, nodo->altezza);
                        printf("ok\n");
                    }
                } while (check != 0);
                return;
            }else if(nodo->numfigli>=1024 || nodo->altezza>=255){
                printf("no\n");
                return;
            }

        } else {
            printf("no\n");
            return;
        }
    }else {
        printf("no\n");                     //SE NODO=NULL ESCO.
        return;
    }
}


//WRITE SU FILE

void write(albero*nodo) {
    testo_t *n;
    int numerocaratteri = 0;
    int controllo;
    char c = getchar();
    controllo = percorsofiglio(&nodo);

    if(controllo==1) {
        if (nodo->carattere == FILE) {
            if (nodo->contenuto != NULL)
                deallocafile(nodo->contenuto);

            //MI OCCUPO ORA DI SCRIVERE TUTTI I CARATTERI SUL FILE
            do {
                c = getchar(); //evito le virgolette
            } while (c != '"' && c!='\n');
            if(c=='\n'){
                printf("no\n");
                return;
            }



            n = (testo_t *) malloc(sizeof(testo_t));
            strcpy(n->conten, "begin");
            nodo->contenuto=n;

            numerocaratteri=inseriscicarattere(nodo->contenuto);
            if(numerocaratteri!=0)
                printf("ok %d\n", numerocaratteri);
        } else
            printf("no\n");
    }else
        printf("no\n");
}



//DEALLOCAOGNIQUALSIASICOSA

void deallocatutto(albero *nodo) {
    int i;
    if (nodo->carattere == CART && nodo->figli!=NULL) {
        for (i = 0; i < nodo->dim; i++) {
            if (nodo->figli[i] != NULL) {
                deallocatutto(nodo->figli[i]);
            }
        }
    }

    if(nodo->contenuto!=NULL && nodo->carattere==FILE) {
        deallocafile(nodo->contenuto);
    }else if(nodo->carattere==CART) {
        free(nodo->ordine);
        free(nodo->supporto);
        free(nodo->figli);
    }
    free(nodo);

}


//DELETESINGOLARISORSA
void cancella(albero *nodo){
    char nomefiglio[DN];
    int somma, w, z;
    int r=1;
    int i;
    int check=0;
    int flag = 0;
    char c;
    int controllo;

    for(z=0; z<DN; z++)
        nomefiglio[z]='\0';

    c = getchar();
    controllo = percorsopadre(&nodo, nomefiglio);
    if(nodo->figli==NULL){
        printf("no\n");
        return;
    }


    if(controllo==1) {
        w = strlen(nomefiglio);
        do {
            somma = modellohashing(nomefiglio, w, r, nodo->dim);
            //printf("supp --> %d\n", nodo->supporto[somma]);
            //printf("supporto %d\n", nodo->supporto[somma]);
            if(nodo->supporto[somma]==1 && nodo->figli[somma]==NULL) {
                r++;
                check = 1;
            }

            if (nodo->figli[somma] != NULL) {
                check = strcmp(nomefiglio, nodo->figli[somma]->nome);
                r++;
            }

            if(nodo->supporto[somma]==0 && nodo->figli[somma]==NULL){
                printf("no\n");
                check=0;
                flag=1;
            }

        } while (check != 0);


        if (flag == 0) {
            if (nodo->figli[somma]->numfigli==0) {
                nodo->numfigli--;
                //printf("\nnome -->%s --> figli --> %d-->nomefiglio-->%s\n",nodo->nome, nodo->numfigli, nodo->figli[nodo->ordine[0]]->nome);
                if(nodo->figli[somma]->carattere==FILE && nodo->figli[somma]->contenuto!=NULL){
                    deallocafile(nodo->figli[somma]->contenuto);
                    nodo->figli[somma]->contenuto=NULL;         //tanto ho gia eliminato tutto il contenuto.
                }
                if(nodo->figli[somma]->carattere==FILE) {
                    free(nodo->figli[somma]);
                }else if (nodo->figli[somma]->carattere==CART) {
                    free(nodo->figli[somma]->supporto);
                    free(nodo->figli[somma]->ordine);
                    free(nodo->figli[somma]->figli);
                    free(nodo->figli[somma]);
                }
                printf("ok\n");
            } else if (nodo->figli[somma]->numfigli != 0) {
                printf("no\n");
                return;
            }

            nodo->figli[somma] = NULL;

            i=0;
            while(nodo->ordine[i]!=somma)
                i++;
            nodo->ordine[i]=0;
            ordinafinddelete(nodo);
        }
    }else
        printf("no\n");


}


//DELETETUTTO
void delete_r(albero *nodo){
    int somma;
    int check;
    char nomefiglio[DN];
    int w, r, z;
    char c;
    int controllo;

    for(z=0;z<DN; z++)
        nomefiglio[z]='\0';

    c = getchar();
    controllo = percorsopadre(&nodo, nomefiglio);


    r=1;
    if(controllo==1 && nodo->figli!=NULL) {
        w = strlen(nomefiglio);
        do {
            somma = modellohashing(nomefiglio, w, r, nodo->dim);
            if(nodo->supporto[somma]==1 && nodo->figli[somma]==NULL) {
                r++;
                check = 1;
            }else if(nodo->supporto[somma]==1 && nodo->figli[somma]!=NULL) {
                check = strcmp(nomefiglio, nodo->figli[somma]->nome);
                r++;
            }else if(nodo->supporto[somma]==0 && nodo->figli[somma]==NULL){
                printf("no\n");
                return;
            }
        } while (check != 0);
        nodo->numfigli = (nodo->numfigli)-1;
        deallocatutto(nodo->figli[somma]);
        nodo->supporto[somma]=1;
        printf("ok\n");

        nodo->figli[somma]=NULL;

        w=0;
        while(nodo->ordine[w]!=somma)
            w++;
        nodo->ordine[w]=0;
        ordinafinddelete(nodo);


    }else{
        printf("no\n");
        return;
    }

}

void deallocafile(testo_t* nodo){
    if(nodo->next!=NULL)
        deallocafile(nodo->next);

    free(nodo);

}


void readfileex(albero *head){
    albero* nodo;
    nodo=head;
    int i;
    int controllo;
    controllo = percorsofiglio(&nodo);
    if(controllo==1) {
        if (nodo->carattere == CART) {
            printf("no\n");
        } else {
            printf("contenuto ");
            if (nodo->contenuto != NULL) {
                do {
                    i = 0;
                    while (nodo->contenuto->conten[i] != '\0' && i <= 25) {
                        printf("%c", nodo->contenuto->conten[i]);
                        i++;
                    }
                    if (nodo->contenuto->conten[i] == '\0' && nodo->contenuto->next != NULL)
                        nodo->contenuto = nodo->contenuto->next;
                    else if (nodo->contenuto->conten[i] == '\0' && nodo->contenuto->next == NULL)
                        return;
                } while (nodo->contenuto != NULL);
                printf("\n");
            } else
                printf("\n");
        }
    }else
        printf("no\n");
}


void ordinafind(albero *nodo){
    int i=0;
    int flag=0;
    int nuovo, altro, tmp, check;
    while(nodo->ordine[i]!=-1) {
        i++;
    }

    i--;
    if(i==0){
        flag=1;
    }

    while (flag == 0 && i>0){
        nuovo = nodo->ordine[i];
        altro = nodo->ordine[i - 1];
        check = strcmp(nodo->figli[nuovo]->nome, nodo->figli[altro]->nome);
        if (check < 0) {
            tmp = nodo->ordine[i];
            nodo->ordine[i] = nodo->ordine[i - 1];
            nodo->ordine[i - 1] = tmp;
            i--;
            flag = 0;
        }else if (check == 0) {
            flag = 1;
        } else if (check > 0) {
            flag = 1;
        }
    }
}


int find(albero* nodo_n, percfind_t *p, char *nome){
    int trova=0;
    percfind_t *posizione=(percfind_t*)malloc(sizeof(percfind_t));
    posizione->prev=p;
    if(p!=NULL)
        p->next=posizione;
    posizione->nodo=nodo_n;
    posizione->next=NULL;
    if(strcmp(nodo_n->nome, nome)==0){
        printf("ok ");
        stampapercorso(posizione);
        trova=1;
    }
    //printf("nome -->%s ||figli-->%d\n",nodo_attuale->nome, nodo_attuale->numfigli);
    fflush(stdout);

    if(nodo_n->numfigli>0 &&nodo_n->carattere==CART)
        for(int i=0;i<nodo_n->numfigli;i++){
            //printf("nomeprima --> %s", nodo_attuale->figli[nodo_attuale->ordine[i]]->nome);
            fflush(stdout);
            if(find(nodo_n->figli[nodo_n->ordine[i]],posizione, nome))
                trova=1;
        }

    //printf("aaa");
    fflush(stdout);

    if(p!=NULL)
        p->next=NULL;
    free(posizione);



    return trova;
}



void ordinafinddelete(albero*nodo){

    int tmp, i;
    int check=0;

    i=0;
    while(nodo->ordine[i]!=0) {
        i++;
    }


    while(check!=1) {
        if(nodo->ordine[i + 1]==-1)
            check=1;
        else{
            tmp = nodo->ordine[i];
            nodo->ordine[i] = nodo->ordine[i+1];
            nodo->ordine[i+1] = tmp;
            i++;
        }
    }
    nodo->ordine[i]=-1;
}


void preread(albero *nodo){
    int controllo;
    controllo = percorsofiglio(&nodo);
    if(controllo==1) {
        if (nodo->carattere == CART) {
            printf("no\n");
        } else {
            if(nodo->contenuto==NULL){
                printf("contenuto\n");
                return;
            }else {
                printf("contenuto ");
                readfile(nodo->contenuto->next);
                printf("\n");
            }
        }
    }else
        printf("no\n");
}



void readfile(testo_t *nodo){
    int i;
    if(nodo!=NULL) {
        printf("%s", nodo->conten);
        readfile(nodo->next);
    }
}


void stampapercorso(percfind_t* nodo){

    while(nodo->prev!=NULL) {
        nodo = nodo->prev;
    }

    nodo=nodo->next; //per radice dell'albero

    while(nodo!=NULL){
        printf("/%s",nodo->nodo->nome);
        nodo=nodo->next;
    }
    printf("\n");
}

void leggitutto(char c){
    while(c!='\n'&& c!='\0'){
        c=getchar();            //finisco i caratteri input
    }
}

void riallocavettori(albero* nodo){

    int j, somma, r;
    int n_dim=nodo->dim*NUM;
    albero **n_figli=(albero**)malloc(sizeof(albero)*n_dim);
    int *n_supporto=(int*)calloc(n_dim, sizeof(int*));
    int *n_ordine=(int*)malloc(sizeof(int*)*n_dim);
    for(j=0;j<n_dim;j++){
        n_ordine[j]=-1;
        n_figli[j]=NULL;
    }

    for(j=0; j<nodo->dim/NUM-1; j++){
        r=1;
        somma=modellohashing(nodo->figli[nodo->ordine[j]]->nome, strlen(nodo->figli[nodo->ordine[j]]->nome), r, n_dim);

        while(n_figli[somma]!=NULL) {
            r++;
            somma = modellohashing(nodo->figli[nodo->ordine[j]]->nome, strlen(nodo->figli[nodo->ordine[j]]->nome), r, n_dim);
        }

        n_figli[somma]=nodo->figli[nodo->ordine[j]];
        n_ordine[j]=somma;
        n_supporto[somma]=1;

    }

    //LIBERO TUTTO PER SOSTITUIRE SUCCESSIVAMENTE CON I NUOVI

    free(nodo->ordine);
    free(nodo->supporto);
    free(nodo->figli);


    nodo->figli=n_figli;
    nodo->ordine=n_ordine;
    nodo->supporto=n_supporto;
    nodo->dim=n_dim;
}