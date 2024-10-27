#include <arpa/inet.h> 
 #include <sys/types.h> 
 #include <sys/socket.h> 
 #include <netinet/in.h> 
 #include <string.h> 
 #include <stdio.h> 
 #include <stdlib.h> 
 #include <unistd.h> 
 #include <time.h>
 #include <sys/select.h>


struct sessione_di_gioco{
    int sd;
    char utente[1024];
    char password[1024];
    int iniziata;
    int terminata;
    int in_chat;
    int token_rimanenti;
    time_t ora_inizio;
    //int richiesta_chiusura;
    char oggetto_in_mano[1024];
    //char ultimo_inviato[1024];
    char ultimo_ricevuto[1024];
    struct oggetti_raccolti* oggetti;
    struct sessione_di_gioco* pun;
};
struct sessione_di_gioco* inizio;







struct oggetti_raccolti{
    char oggetto[1024];
    struct oggetti_raccolti* pun;
};

struct oggetti_totali{
    char oggetto[1024];
    struct oggetti_totali* pun;
};
struct oggetti_totali* chiorba;

struct utenti_in_chat{
    char utente[1024];
    char password[1024];
    struct utenti_in_chat* pun;
};

struct chat{
    char messaggio[1024];
    int quanti;
    struct utenti_in_chat* capo;
};
char* prendiCassaComune(struct oggetti_totali** head){
    if (*head == NULL) {
        printf("La lista è già vuota.\n");
        return NULL; 
    }

    
    struct oggetti_totali* temp = *head;
    
    *head = (*head)->pun;
    
    temp->pun = NULL;
    return temp->oggetto; 
}
char* removeHead(struct oggetti_raccolti** head) {
    if (*head == NULL) {
        printf("La lista è già vuota.\n");
        return NULL; 
    }

    
    struct oggetti_raccolti* temp = *head;
    
    *head = (*head)->pun;
    
    temp->pun = NULL;
    return temp->oggetto; 
}
struct oggetti_totali* nuovo(char* oggetto){
    struct oggetti_totali* newNode = (struct oggetti_totali*)malloc(sizeof(struct oggetti_totali));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->oggetto,oggetto);
    return newNode;
}
void inserisciInCoda(struct oggetti_totali** prova, char* oggetto){
    struct oggetti_totali* newNode = nuovo(oggetto);
    if (*prova == NULL) {
        *prova = newNode;
        return;
    }
    struct oggetti_totali* temp = *prova;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}
struct chat* inizializza_chat(){
    struct chat* p = (struct chat*)malloc(sizeof(struct chat));
    if(p==NULL){
        printf("errore di memoria");
        exit(1);
    }
    p->quanti = 0;
    strcpy(p->messaggio," ");
    p->capo = NULL;
    return p;
}
struct utenti_in_chat* crea_elemento(char* utente, char* password){
    struct utenti_in_chat* newNode = (struct utenti_in_chat*)malloc(sizeof(struct utenti_in_chat));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->utente,utente);
    strcpy(newNode->password,password);
    
    return newNode;
}
void inserisci_persona(struct utenti_in_chat** prova, char* utente,char* password){
    struct utenti_in_chat* newNode = crea_elemento(utente,password);
    if (*prova == NULL) {
        *prova = newNode;
        return;
    }
    struct utenti_in_chat* temp = *prova;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}
struct utenti_registrati{
    char utente[1024];
    char password[1024];
    struct utenti_registrati* pun;
};
struct utenti_registrati* principio;
struct utenti_registrati* aggiungi(char* utente,char* password){
    struct utenti_registrati* newNode = (struct utenti_registrati*)malloc(sizeof(struct utenti_registrati));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->utente,utente);
    strcpy(newNode->password,password);
    
    return newNode;
}
void nuovo_utente(char* utente,char* password){
    struct utenti_registrati* newNode = aggiungi(utente,password);
    if (principio == NULL) {
        principio = newNode;
        return;
    }
    struct utenti_registrati* temp = principio;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}
struct sessione_di_gioco* createNodeServer(int sd, char* utente, char* password) {
    struct sessione_di_gioco* newNode = (struct sessione_di_gioco*)malloc(sizeof(struct sessione_di_gioco));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->utente,utente);
    strcpy(newNode->password,password);
    newNode->sd = sd;
    newNode->token_rimanenti = 5;
    newNode->iniziata = 0;
    newNode->in_chat = 0;
    newNode->terminata = 0;
    newNode->ora_inizio = (time_t)-1;
    
    newNode->oggetti = NULL;
    
    strcpy(newNode->ultimo_ricevuto, "");
    strcpy(newNode->oggetto_in_mano,"");
    newNode->pun = NULL;
    return newNode;
}
void insertAtEndServer(int sd,char* utente,char* password) {
    struct sessione_di_gioco* newNode = createNodeServer(sd,utente,password);
    if (inizio == NULL) {
        inizio = newNode;
        return;
    }
    struct sessione_di_gioco* temp = inizio;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}


struct oggetti_raccolti* createNodeServerOggetto(char* oggetto) {
    struct oggetti_raccolti* newNode = (struct oggetti_raccolti*)malloc(sizeof(struct oggetti_raccolti));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->oggetto, oggetto);
    newNode->pun = NULL;
    return newNode;
}

void insertAtEndServerOggetto(struct oggetti_raccolti** prova,char* oggetto) {
    struct oggetti_raccolti* newNode = createNodeServerOggetto(oggetto);
    if (*prova == NULL) {
        *prova = newNode;
        return;
    }
    struct oggetti_raccolti* temp = *prova;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}


int raccolto(struct oggetti_raccolti* prova,char* oggetto){
    int ret = 1;
    struct oggetti_raccolti* p = prova;
    while(p!=NULL){
        if(strcmp(p->oggetto,oggetto)==0){
            return ret;
        }
        p=p->pun;
    }
    ret = 0;
    return ret;
}




int gia_registrato(char* utente, char* password){
    
    int val = 0;
    struct utenti_registrati* p = principio;
    while(p!=NULL){
                            if(strcmp(utente,p->utente) == 0 && strcmp(password,p->password) == 0){
                              
                            val = 1;
                            return val;
                        
                        }
                        p=p->pun;
}
return val;
}
 int main(){
    char comando_server[1024];
    int server_attivo = 0;
    int ret, newfd, listener, i, len,lunghezza;
    socklen_t addrlen;
    int partite_in_corso = 0;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    uint16_t lmsg;
    
    struct sockaddr_in my_addr, cl_addr;
    char buffer[1024];
    char ausiliario[1024];
    char ausiliario1[1024];

    //time_t rawtime;
    //struct tm * timeinfo;
    struct chat* chat = inizializza_chat();
    /* Creazione socket */
    listener = socket(AF_INET, SOCK_STREAM, 0);

    /* Creazione indirizzo di bind */
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4242);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(listener, (struct sockaddr*)&my_addr, sizeof(my_addr) );

    if( ret < 0 ){
        perror("Bind non riuscita\n");
        exit(0);
    }

     listen(listener, 10);

    // Reset FDs
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // Aggiungo il socket di ascolto (listener), creato dalla socket()
    // all'insieme dei descrittori da monitorare (master)
    FD_SET(listener, &master);

    // Aggiorno il massimo
    fdmax = listener;

    //main loop

    while(server_attivo == 0){
        printf("digitare un comando\n");
        scanf("%s",comando_server);
        if(strcmp(comando_server,"start")==0){
            server_attivo = 1;
            break;
        }
        else if(strcmp(comando_server,"stop")==0){
            //printf("%il server non è ancora attivo\n");
            printf("il server è stato chiuso\n");
            ret=close(listener);
            exit(1);
        }
        else{
            printf("comando insensato\n");
        }
    }
    while(server_attivo==1){
        
        // Inizializzo il set read_fds, manipolato dalla select()
        read_fds = master;

        // Mi blocco in attesa di descrottori pronti in lettura
        // imposto il timeout a infinito
        // Quando select() si sblocca, in &read_fds ci sono solo
        // i descrittori pronti in lettura!
        ret = select(fdmax+1, &read_fds, NULL, NULL, NULL);
        
        if(ret < 0){
            perror("ERRORE SELECT:");
            exit(1);
        }


           for(i = 0; i <= fdmax; i++) {

            // controllo se i è pronto
            if(FD_ISSET(i, &read_fds)) {

                // se i è il listener, ho ricevuto una richiesta di connessione
                // (un client ha invocato connect())
                if(i == listener) {
                    printf("Nuovo client rilevato!\n");
                    fflush(stdout);
                    addrlen = sizeof(cl_addr);
                    // faccio accept() e creo il socket connesso 'newfd'


                     newfd = accept(listener,
                        (struct sockaddr *)&cl_addr, &addrlen);

                    // Aggiungo il descrittore al set dei socket monitorati
                    FD_SET(newfd, &master);

                    // Aggiorno l'ID del massimo descrittore
                    if(newfd > fdmax){
                         fdmax = newfd;
                     }
                } 


                // se non è il listener, 'i'' è un descrittore di socket
                // connesso che ha fatto la richiesta di connessione, e va servito
                // ***senza poi chiudere il socket*** perché l'orario
                // potrebbe essere chiesto nuovamente al server
                else {

                     // Metto la richiesta nel buffer (pacchetto "REQ\0")

                    ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
                    lunghezza = ntohs(lmsg);
                    ret = recv(i, (void*)&buffer, lunghezza, 0);

                    
                    
                    if(ret == 0){
                        printf("CHIUSURA client rilevata!\n");
                        fflush(stdout);

                        // il client ha chiuso il socket, quindi
                        // chiudo il socket connesso sul server
                        close(i);
                        // rimuovo il descrittore newfd da quelli da monitorare
                        FD_CLR(i, &master);
                    } 

                     else if(ret < 0){
                        strcpy(buffer, "no");
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        len = send(i, (void*)&buffer,lunghezza, 0);
                        // si è verificato un errore
                        close(i);
                        // rimuovo il descrittore newfd da quelli da monitorare
                        FD_CLR(i, &master);
                    } 

                     else {
                        printf("REQ client rilevata!\n");
                        fflush(stdout);
                        printf("%s\n", buffer);
                        //gestisco le risposte a tutti i possibili messaggi da parte del client
                        if(strcmp("CIAO",buffer)==0){
                        strcpy(buffer, "inserisci nome utente e password una a capo all'altra");
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                     }
                     else if(strcmp("faccio_login",buffer) == 0){
                        struct sessione_di_gioco* p = inizio;
                        ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
                        lunghezza = ntohs(lmsg);
                        ret = recv(i, (void*)&ausiliario, lunghezza, 0);

                        ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
                        lunghezza = ntohs(lmsg);
                        ret = recv(i, (void*)&ausiliario1, lunghezza, 0);
                        
                       
                        if(gia_registrato(ausiliario,ausiliario1)==1){
                            insertAtEndServer(i,ausiliario, ausiliario1);
                            while(p!=NULL){
                                if(strcmp(ausiliario,p->utente) == 0 && strcmp(ausiliario1,p->password) == 0){
                                    p->iniziata = 0;
                                    p->terminata = 0;
                                    break;
                                }
                                p=p->pun;
                            }
                            
                            strcpy(buffer,"benvenuto nell'escape room, digitare il comando start 1 per avviare il gioco sullo scenario 1");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                            }
                        else{
                        strcpy(buffer, "utente non registrato, per registrarsi inserire nome utente e password uno a capo all'altra");
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                     }
                     else if(strcmp("chat",buffer)==0){
                              
                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                    
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata==0){
                                time(&tempo);
                            tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                               
                               inserisci_persona(&chat->capo,temp->utente,temp->password);
                               temp->in_chat = 1;
                               strcpy(temp->ultimo_ricevuto,"chat");
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            strcpy(buffer,"ora sei dentro la chat globale, tutti i messaggi inviati che termineranno con un * andranno in chat globale");
                            strcat(buffer,"\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            printf("%s\n",buffer);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }

                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }

                            }
                            else{
                                    temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                                      strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                             
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }


                     }
                     else if(strcmp("ricevi",buffer)==0){
                            

                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                    
                                }
                                p=p->pun;
                            }

                            if(temp->iniziata==1 && temp->terminata==0){
                                time(&tempo);
                            tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                                if(temp->in_chat==1){
                                    strcpy(temp->ultimo_ricevuto,"chat");
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            strcpy(buffer,chat->messaggio);
                            strcat(buffer,"\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            printf("%s\n",buffer);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }

                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                                   strcpy(buffer,"non fai parte della chat globale");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                                                temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                                             strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else if(strcmp("start 1",buffer) == 0){
                         
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                         
                         char token[50];
                         
                         
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                    
                                    break;
                                }
                                p=p->pun;
                            }
                            strcpy(temp->ultimo_ricevuto,"start_1");
                            time(&temp->ora_inizio);
                        
                        if(temp->terminata == 0 && temp->iniziata == 0){
                        
                        temp->iniziata = 1;
                        partite_in_corso+=1;
                        sprintf(token,"%d token rimanenti", p->token_rimanenti);
                        strcpy(buffer,"benvenuto all'interno dell' economato, hai 1800 secondi per risolvere gli enigmi, prendere gli oggetti ed i relativi token, quindi uscire");
                        
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                     }
                     else{
                               strcpy(buffer,"partita gia iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                     }
                     }
                     else if(strcmp("mi_registro",buffer) == 0){
                        
                        ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
                        lunghezza = ntohs(lmsg);
                        ret = recv(i, (void*)&ausiliario, lunghezza, 0);

                        ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
                        lunghezza = ntohs(lmsg);
                        ret = recv(i, (void*)&ausiliario1, lunghezza, 0);

                        
                        if(gia_registrato(ausiliario,ausiliario1)==1){
                              strcpy(buffer,"utente gia registrato, ripetere nuovamente la registrazione inserendo nome utente e password uno a capo all'altra");
                              lunghezza = strlen(buffer) +1;
                              lmsg = htons(lunghezza);
                              len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                              if(len == -1){
                              perror("errore");
                              exit(1);
                              }
                              
                              ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                              if(ret < 0){
                              perror("Errore in fase di comunicazione \n");

                              }
                        }
                        else{
                              
                              nuovo_utente(ausiliario,ausiliario1);
                              strcpy(buffer,"registrazione avvenuta, per fare login inserire nome utente e password uno a capo all'altra");
                              lunghezza = strlen(buffer) +1;
                              lmsg = htons(lunghezza);
                              len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                              if(len == -1){
                              perror("errore");
                              exit(1);
                              }
                              
                              ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                              if(ret < 0){
                              perror("Errore in fase di comunicazione \n");

                              }
                        }
                        
                        
                     }
                     else if(strcmp("take bandiera",buffer)==0){
                         
                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                    
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata==0){
                                time(&tempo);
                            tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                            
                            if(raccolto(temp->oggetti,"bandiera")==0){
                                
                            strcpy(temp->oggetto_in_mano,"bandiera");
                            strcpy(temp->ultimo_ricevuto,"take_bandiera");
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            strcpy(buffer,"l'oggetto ++bandiera++ è chiuso da un enigma: Citare l'ultimo fantino vittorioso della contrada dell'istrice");
                            strcat(buffer,"\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            printf("%s\n",buffer);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }

                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                     
                                     strcpy(temp->ultimo_ricevuto,"take_bandiera");
                                     sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                     sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                    strcpy(buffer,"oggetto gia raccolto");
                                    strcat(buffer,"\n");
                                    strcat(buffer,stringa);
                                    strcat(buffer, "\n");
                                    strcat(buffer, token);
                                          strcpy(temp->ultimo_ricevuto,"");
                                          strcpy(temp->oggetto_in_mano,"");
                                          printf("%s\n",buffer);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                            }
                            }
                            else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else if(strcmp("look",buffer) == 0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata==0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look");
                       
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        strcpy(buffer, "benvenuto all'interno dell'economato della contrada sovrana dell'istrice, sul --tavolo-- giace una ++bandiera++ ed un ++bandierone++ ,sulla --scaffalatura-- giace un ++tamburo++ ed una ++chiave++, nella stanza si trova anche una ++porta++");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("objs",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                   
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata==1 && temp->terminata==0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                            strcpy(temp->ultimo_ricevuto,"objs");
                            char virgola[] = ",";
                            if(temp->oggetti == NULL){
                                 sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                 sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                 strcpy(buffer, "nessun oggetto raccolto");
                                 strcat(buffer, "\n");
                                 strcat(buffer,stringa);
                                 strcat(buffer,"\n");
                                 strcat(buffer,token);
                                 lunghezza = strlen(buffer)+1;
                                 lmsg = htons(lunghezza);
                                 len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                 if(len == -1){
                                 perror("errore");
                                 exit(1);
                                 }
                                
                                 ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                 if(ret < 0){
                                 perror("Errore in fase di comunicazione \n");

                                 }
                            }else{
                                sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                strcpy(buffer,temp->oggetti->oggetto);
                                struct oggetti_raccolti* p = temp->oggetti->pun;
                                while(p!=NULL){
                                    strcat(buffer,virgola);
                                    strcat(buffer,p->oggetto);
                                    p=p->pun;
                                }
                                
                                 strcat(buffer, "\n");
                                 strcat(buffer,stringa);
                                 strcat(buffer, "\n");
                                 strcat(buffer,token);
                                 lunghezza = strlen(buffer)+1;
                                 lmsg = htons(lunghezza);
                                 len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                 if(len == -1){
                                 perror("errore");
                                 exit(1);
                                 }
                                 
                                 ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                 if(ret < 0){
                                 perror("Errore in fase di comunicazione \n");

                                 }
                            }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look chiave",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_chiave");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                       
                        strcpy(buffer, "una piccola ++chiave++ con su scritto: per potermi prendere occorre usare su di me tutti gli oggetti presenti, eccetto uno, nell'ordine in cui si vedono apparire all'interno di una comparsa");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look tamburo",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_tamburo");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        
                        strcpy(buffer, "un ++tamburo++ vecchio con lo stemma della contrada dell istrice");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look tavolo",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_tavolo");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        
                        strcpy(buffer, "sul --tavolo-- c'e una ++bandiera++ semiarrotolata ed un ++bandierone++ disteso");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                       
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look scaffalatura",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_scaffalatura");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                      
                        strcpy(buffer, "sulla --scaffalatura-- c'e un ++tamburo++ dipinto a monticini con il cerchio di ferro");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look bandiera",buffer)==0){
                           time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_bandiera");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        
                        strcpy(buffer, "una ++bandiera++ di stoffa con asta piombata");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look bandierone",buffer)==0){
                           time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_bandierone");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        
                        strcpy(buffer, "un ++bandierone++ di seta ricamato con asta piombata ");
                        strcat(buffer,"\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                             
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("look porta",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                             struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1){
                        time(&tempo);
                        tempo_trascorso=difftime(tempo,temp->ora_inizio);
                        if(tempo_trascorso<1800){
                        strcpy(temp->ultimo_ricevuto,"look_porta");
                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                        sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                        
                        strcpy(buffer, "una ++porta++ bloccata");
                        strcat(buffer, "\n");
                        strcat(buffer,stringa);
                        strcat(buffer, "\n");
                        strcat(buffer,token);
                        lunghezza = strlen(buffer)+1;
                        lmsg = htons(lunghezza);
                        len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                        if(len == -1){
                        perror("errore");
                        exit(1);
                          }
                        
                        ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                        if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                                      }
                        }
                        else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                             
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else if(strcmp("take chiave",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                            if(raccolto(temp->oggetti,"chiave")==0){
                            strcpy(temp->oggetto_in_mano,"chiave");
                            strcpy(temp->ultimo_ricevuto,"take_chiave");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            strcpy(buffer,"l'oggetto ++chiave++ è bloccato");
                            strcat(buffer, "\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                    strcpy(buffer,"oggetto gia raccolto");
                                    sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                    sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcpy(temp->ultimo_ricevuto,"");
                                          strcpy(temp->oggetto_in_mano,"");
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                            }
                            }
                            else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                            strcpy(buffer,"partita non iniziata opppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else if(strcmp("take porta",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                            if(raccolto(temp->oggetti,"chiave")==0){
                            strcpy(temp->oggetto_in_mano,"chiave");
                            strcpy(temp->ultimo_ricevuto,"take_porta");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            strcpy(buffer,"l'oggetto ++porta++ è bloccato");
                            strcat(buffer, "\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                    strcpy(buffer,"oggetto gia raccolto");
                                    sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                    sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcpy(temp->ultimo_ricevuto,"");
                                          strcpy(temp->oggetto_in_mano,"");
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                            }
                            }
                            else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                            strcpy(buffer,"partita non iniziata opppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else if(strcmp("use chiave porta",buffer)==0){
                         time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                             struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }

                            if(temp->iniziata == 1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                                if(raccolto(temp->oggetti,"chiave")==1 && raccolto(temp->oggetti,"porta")==0){
                                       temp->terminata = 1;
                                       temp->token_rimanenti -=1;
                                       insertAtEndServerOggetto(&temp->oggetti,"porta");
                                       printf("%s\n",temp->oggetti->oggetto);
                                       strcpy(temp->ultimo_ricevuto,"use_chiave_porta");
                                       strcpy(temp->oggetto_in_mano,"");
                                       strcpy(buffer, "la ++porta++ e ora aperta. Vittoria");
                                       sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                       sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                       strcat(buffer,"\n");
                                       strcat(buffer,stringa);
                                       strcat(buffer, "\n");
                                       strcat(buffer,token);
                                       lunghezza = strlen(buffer)+1;
                                       lmsg = htons(lunghezza);
                                       len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                       if(len == -1){
                                       perror("errore");
                                       exit(1);
                                       }
                                       
                                       ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                       if(ret < 0){
                                       perror("Errore in fase di comunicazione \n");

                                        }
                                       }
                                       else{
                                          strcpy(buffer,"non tutti gli oggetti sono stati raccolti oppure porta gia aperta");
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcpy(temp->ultimo_ricevuto,"use_chiave_porta");
                                          strcpy(temp->oggetto_in_mano,"");
                                          strcat(buffer, "\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                       }
                            }
                            else{
                            temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                                    strcpy(buffer,"partita non iniziata oppure gia terminata");
                                    lunghezza = strlen(buffer) +1;
                                    lmsg = htons(lunghezza);
                                    len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                    if(len == -1){
                                    perror("errore");
                                    exit(1);
                                     }
                                    
                                     ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                    if(ret < 0){
                                    perror("Errore in fase di comunicazione \n");

                                    }
                            }
                     }
                     else if(strcmp("prendi da cassa comune",buffer)==0){
                         time_t tempo;
                         char stringa[50];
                         char token[50];
                         double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                         while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                         if(temp->iniziata == 1 && temp->terminata == 0){
                             time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                                char* oggetto = prendiCassaComune(&chiorba);
                                if(oggetto){
                                if(raccolto(temp->oggetti,oggetto)){
                                    strcpy(buffer,"utente gia in possesso di tale oggetto");
                                          strcpy(temp->ultimo_ricevuto,"prendi_da_cassa_comune");
                                          inserisciInCoda(&chiorba,oggetto);
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                          
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                }
                                else{
                                    strcpy(buffer,"preso oggetto da cassa comune");
                                    temp->token_rimanenti-=1;
                                          strcpy(temp->ultimo_ricevuto,"prendi_da_cassa_comune");
                                          insertAtEndServerOggetto(&temp->oggetti,oggetto);
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                          
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                }
                            }
                            else{
                                 strcpy(buffer,"nessun oggetto in cassa comune");
                                          strcpy(temp->ultimo_ricevuto,"prendi_da_cassa_comune");
                                          
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                            }
                            }
                            else{
                                    temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                         }
                         else{
                                    strcpy(buffer,"partita non iniziata oppure gia terminata");
                                    lunghezza = strlen(buffer) +1;
                                    lmsg = htons(lunghezza);
                                    len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                    if(len == -1){
                                    perror("errore");
                                    exit(1);
                                     }
                                    // Invio la risposta (e il terminatore di stringa)
                                     ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                    if(ret < 0){
                                    perror("Errore in fase di comunicazione \n");

                                    }
                            }
                     }
                     else if(strcmp("metti in cassa comune",buffer)==0){
                         time_t tempo;
                         char stringa[50];
                         char token[50];
                         double tempo_trascorso;
                         struct sessione_di_gioco* p = inizio;
                         struct sessione_di_gioco* temp = inizio;
                         while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                         
                         if(temp->iniziata == 1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                                    if(!temp->oggetti){
                                          strcpy(buffer,"nessun oggetto raccolto");
                                          strcpy(temp->ultimo_ricevuto,"metti_in_cassa_comune");
                                          
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           // Invio la risposta (e il terminatore di stringa)
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                    }else{
                                        temp->token_rimanenti+=1;
                                        char* oggetto = removeHead(&temp->oggetti);
                                        inserisciInCoda(&chiorba,oggetto);
                                        strcpy(buffer,"oggetto messo in cassa comune");
                                        strcpy(temp->ultimo_ricevuto,"metti_in_cassa_comune");
                                        sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           // Invio la risposta (e il terminatore di stringa)
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                    }
                            }
                            else{
                                    temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                         }
                         else{
                                    strcpy(buffer,"partita non iniziata oppure gia terminata");
                                    lunghezza = strlen(buffer) +1;
                                    lmsg = htons(lunghezza);
                                    len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                    if(len == -1){
                                    perror("errore");
                                    exit(1);
                                     }
                                    // Invio la risposta (e il terminatore di stringa)
                                     ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                    if(ret < 0){
                                    perror("Errore in fase di comunicazione \n");

                                    }
                            }
                     }
                     else if(strcmp("use tamburo bandiera bandierone chiave",buffer)==0){
                            
                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata == 1 && temp->terminata == 0){
                                  time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                                if(raccolto(temp->oggetti,"tamburo")==1 && raccolto(temp->oggetti,"bandiera") == 1 && raccolto(temp->oggetti,"bandierone") == 1 && raccolto(temp->oggetti,"chiave")==0){
                                       insertAtEndServerOggetto(&temp->oggetti,"chiave");
                                       temp->token_rimanenti-=1;
                                       printf("%s\n",temp->oggetti->oggetto);
                                       strcpy(temp->ultimo_ricevuto,"use_tamburo_bandiera_bandierone_chiave");
                                       strcpy(temp->oggetto_in_mano,"");
                                       strcpy(buffer, "oggetto ++chiave++ sbloccato");
                                       sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                       sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                       strcat(buffer,"\n");
                                       strcat(buffer,stringa);
                                       strcat(buffer, "\n");
                                       strcat(buffer, token);
                                       lunghezza = strlen(buffer)+1;
                                       lmsg = htons(lunghezza);
                                       len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                       if(len == -1){
                                       perror("errore");
                                       exit(1);
                                       }
                                       // Invio la risposta (e il terminatore di stringa)
                                       ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                       if(ret < 0){
                                       perror("Errore in fase di comunicazione \n");

                                        }
                                       }
                                       else{
                                          strcpy(buffer,"non tutti gli oggetti sono stati raccolti oppure chiave gia raccolta");
                                          strcpy(temp->ultimo_ricevuto,"use_tamburo_bandiera_bandierone_chiave");
                                          strcpy(temp->oggetto_in_mano,"");
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           // Invio la risposta (e il terminatore di stringa)
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                                       }
                            }
                            else{
                                    temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                                    strcpy(buffer,"partita non iniziata oppure gia terminata");
                                    lunghezza = strlen(buffer) +1;
                                    lmsg = htons(lunghezza);
                                    len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                    if(len == -1){
                                    perror("errore");
                                    exit(1);
                                     }
                                    // Invio la risposta (e il terminatore di stringa)
                                     ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                    if(ret < 0){
                                    perror("Errore in fase di comunicazione \n");

                                    }
                            }
                            
                     }
                     else if(strcmp("end",buffer)==0){
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1){
                                         temp->terminata = 1;
                                         strcpy(buffer,"disconnessione client");
                                         lunghezza = strlen(buffer) +1;
                                         lmsg = htons(lunghezza);
                                         len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                         if(len == -1){
                                         perror("errore");
                                         exit(1);
                                         }
                                         // Invio la risposta (e il terminatore di stringa)
                                         ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                         if(ret < 0){
                                         perror("Errore in fase di comunicazione \n");

                                         }
                                         partite_in_corso-=1;
                            }
                            else{
                                        strcpy(buffer,"partita non iniziata oppure gia terminata");
                                         lunghezza = strlen(buffer) +1;
                                         lmsg = htons(lunghezza);
                                         len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                         if(len == -1){
                                         perror("errore");
                                         exit(1);
                                         }
                                         // Invio la risposta (e il terminatore di stringa)
                                         ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                         if(ret < 0){
                                         perror("Errore in fase di comunicazione \n");

                                         }
                            }
                            
                            if(partite_in_corso == 0){
     printf("disconnessione server\n");
     close(listener);
     exit(1);
 }
                     }
                     else if(strcmp("take tamburo",buffer)==0){
                            
                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                            if(raccolto(temp->oggetti,"tamburo")==0){
                            strcpy(temp->oggetto_in_mano,"tamburo");
                            strcpy(temp->ultimo_ricevuto,"take_tamburo");
                            strcpy(buffer,"l'oggetto ++tamburo++ è chiuso da un enigma: Citare l'ultimo capitano vittorioso della contrada dell'istrice");
                            printf("%s\n",buffer);
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            strcat(buffer,"\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer,token);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                    strcpy(buffer,"oggetto gia raccolto");
                                          strcpy(temp->ultimo_ricevuto,"");
                                          strcpy(temp->oggetto_in_mano,"");
                                          printf("%s\n",buffer);
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           // Invio la risposta (e il terminatore di stringa)
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }
                            }
                            }
                            else{
                                               temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else if(strcmp("take bandierone",buffer)==0){
                         
                            time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                            struct sessione_di_gioco* p = inizio;
                            struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp = p;
                                }
                                p=p->pun;
                            }
                            if(temp->iniziata==1 && temp->terminata == 0){
                                time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                            if(raccolto(temp->oggetti,"bandierone")==0){
                            strcpy(temp->oggetto_in_mano,"bandierone");
                            strcpy(temp->ultimo_ricevuto,"take_bandierone");
                            strcpy(buffer,"l'oggetto ++bandierone++ è chiuso da un enigma: Citare il cavallo che ha vinto l'ultimo palio della contrada dell'istrice");
                            printf("%s\n",buffer);
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                            strcat(buffer,"\n");
                            strcat(buffer,stringa);
                            strcat(buffer, "\n");
                            strcat(buffer, token);
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            else{
                                   strcpy(buffer,"oggetto gia raccolto");
                                          strcpy(temp->ultimo_ricevuto,"");
                                          strcpy(temp->oggetto_in_mano,"");
                                          printf("%s\n",buffer);
                                          sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                                          sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer,"\n");
                                          strcat(buffer,token);
                                          lunghezza = strlen(buffer) +1;
                                          lmsg = htons(lunghezza);
                                          len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                                          if(len == -1){
                                              perror("errore");
                                              exit(1);
                                           }
                                           // Invio la risposta (e il terminatore di stringa)
                                          ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                                          if(ret < 0){
                                           perror("Errore in fase di comunicazione \n");

                                          }

                            }
                            }
                            else{
                                                 temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                            }
                            else{
                            strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                            }
                     }
                     else{
                         //printf("messaggio diverso\n");
                        //qui gestisco le risposte agli enigmi, se arrivo qui, confronto l'ultimo messaggio inviato dal server con l'ultimo ricevuto
                        // se la combinazione mi da una risposta plausibile sblocco l'oggetto
                        time_t tempo;
                            char stringa[50];
                            char token[50];
                            double tempo_trascorso;
                        struct sessione_di_gioco* p = inizio;
                        struct sessione_di_gioco* temp = inizio;
                            while(p!=NULL){
                                if(p->sd==i){
                                    
                                    temp=p;
                                }
                                p=p->pun;
                            }
                        if(temp->iniziata == 1 && temp->terminata == 0){
                            time(&tempo);
                                tempo_trascorso=difftime(tempo,temp->ora_inizio);
                            if(tempo_trascorso<1800){
                        
                        if(strcmp(temp->ultimo_ricevuto,"take_bandiera")==0 && strcmp(buffer,"trecciolino")==0){
                            temp->token_rimanenti-=1;
                            insertAtEndServerOggetto(&temp->oggetti,"bandiera");
                            printf("%s\n",temp->oggetti->oggetto);
                            strcpy(temp->ultimo_ricevuto,"trecciolino");
                            strcpy(temp->oggetto_in_mano,"");
                            strcpy(buffer, "risposta esatta, l'oggetto ++bandiera++ è stato sbloccato");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else if(strcmp(temp->ultimo_ricevuto,"take_tamburo")==0 && strcmp(buffer,"franchi")==0){
                            insertAtEndServerOggetto(&temp->oggetti,"tamburo");
                            temp->token_rimanenti-=1;
                            printf("%s\n",temp->oggetti->oggetto);
                            strcpy(temp->ultimo_ricevuto,"franchi");
                            strcpy(temp->oggetto_in_mano,"");
                            strcpy(buffer, "risposta esatta, l'oggetto ++tamburo++ è stato sbloccato");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else if(strcmp(temp->ultimo_ricevuto,"take_bandierone")==0 && strcmp(buffer,"gia del menhir")==0){
                            insertAtEndServerOggetto(&temp->oggetti,"bandierone");
                            temp->token_rimanenti-=1;
                            printf("%s\n",temp->oggetti->oggetto);
                            strcpy(temp->ultimo_ricevuto,"gia_del_menhir");
                            strcpy(temp->oggetto_in_mano,"");
                            strcpy(buffer, "risposta esatta, l'oggetto ++bandierone++ è stato sbloccato");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else if(strcmp(temp->ultimo_ricevuto,"take_bandierone")==0 && strcmp(buffer,"gia del menhir")!=0){
                            strcpy(buffer, "risposta errata");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer, token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else if(strcmp(temp->ultimo_ricevuto,"take_tamburo")==0 && strcmp(buffer,"franchi")!=0){
                            strcpy(buffer, "risposta errata");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer,"\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else if(strcmp(temp->ultimo_ricevuto,"take_bandiera")==0 && strcmp(buffer,"trecciolino")!=0){
                            strcpy(buffer, "risposta errata");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        else{
                            if(buffer[strlen(buffer)-1]!='*'){
                                
                                printf("sono_qui\n");
                                fflush(stdout);
                            if(strcmp(buffer,"ciao")==0){
                                printf("match\n");
                            }
                            else{
                                printf("unmatch\n");
                            }
                            strcpy(temp->oggetto_in_mano,"");
                            strcpy(temp->ultimo_ricevuto,buffer);
                            strcpy(buffer, "comando insensato");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }else{
                            strcpy(chat->messaggio,buffer);
                             strcpy(temp->oggetto_in_mano,"");
                            strcpy(temp->ultimo_ricevuto,buffer);
                            strcpy(buffer, "messaggio fissato in chat");
                            sprintf(stringa, "%.2f seconds.", 1800-tempo_trascorso );
                            sprintf(token, "%d token rimanenti", temp->token_rimanenti);
                                          strcat(buffer,"\n");
                                          strcat(buffer,stringa);
                                          strcat(buffer, "\n");
                                          strcat(buffer,token);
                            lunghezza = strlen(buffer)+1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                            // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                            perror("Errore in fase di comunicazione \n");

                            }
                        }
                        }
                        }
                        else{
                                  temp->terminata = 1;
                            strcpy(buffer,"tempo scaduto, sconfitta");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                        }
                     }
                     else{
                         strcpy(buffer,"partita non iniziata oppure gia terminata");
                            lunghezza = strlen(buffer) +1;
                            lmsg = htons(lunghezza);
                            len = send(i,(void*)&lmsg,sizeof(uint16_t),0);
                            if(len == -1){
                            perror("errore");
                            exit(1);
                            }
                              // Invio la risposta (e il terminatore di stringa)
                            ret = send(i, (void*)&buffer,lunghezza, 0);
                          
                            if(ret < 0){
                             perror("Errore in fase di comunicazione \n");

                            }
                     }
                    }
                    
                }
                
            }
        }
 }
 
 }
 return 0;
 }