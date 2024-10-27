#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <time.h>


/*struct lista{
    char messaggio[1024];
    struct lista* pun;
};
struct lista* testa;

struct lista* createNode(char* message) {
    struct lista* newNode = (struct lista*)malloc(sizeof(struct lista));
    if (newNode == NULL) {
        printf("Errore: memoria insufficiente\n");
        exit(1);
    }
    strcpy(newNode->messaggio,message);
    newNode->pun = NULL;
    return newNode;
}
void insertAtEnd(char* message) {
    struct lista* newNode = createNode(message);
    if (testa == NULL) {
        testa = newNode;
        return;
    }
    struct lista* temp = testa;
    while (temp->pun != NULL) {
        temp = temp->pun;
    }
    temp->pun = newNode;
}
char* stampa_ultimo(){
    struct lista* p = testa;
    while(p->pun != NULL){
        p = p->pun;
    }
    return p->messaggio;
}*/
//loop infinito nel quale il client manda i comandi di gioco al server e riceve le opportune risposte
void gioca(int sd){
    int ret,lunghezza;
    char messaggio[1024];
    uint16_t lmsg;
    fflush(stdin);
    while(1){
    
    memset((void*)&messaggio,0,sizeof(messaggio));
    fgets(messaggio,sizeof(messaggio),stdin);
    messaggio[strcspn(messaggio, "\n")] = '\0';
    fflush(stdin);
    
    lunghezza = strlen(messaggio)+1;
    lmsg = htons(lunghezza);
    ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = send(sd,(void*)&messaggio,lunghezza,0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = recv(sd, (void*)&lmsg, sizeof(uint16_t), 0);
    lunghezza = ntohs(lmsg);
    ret = recv(sd, (void*)&messaggio, lunghezza, 0);
    //printf("%s\n",messaggio);
    //quasto messaggio viene ricevuto solo quando il client digita end, in tal caso esco dal ciclo di gioco
    if(strcmp(messaggio,"disconnessione client")==0){
        return;
    }
    }
}
//funzione del login
char* login(int sd){
    int ret, lunghezza;
    char utente[1024];
    char password[1024];
    //char risposta[1024];
    char* risposta = (char*)malloc(1024 * sizeof(char));
    if (risposta == NULL) {
        perror("Errore di allocazione memoria");
        exit(1);
    }
    char log[1024];
    uint16_t lmsg;

    strcpy(log,"faccio_login");
    lunghezza = strlen(log)+1;
    lmsg = htons(lunghezza);
    ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = send(sd,(void*)&log,lunghezza,0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }

    scanf("%s", utente);
    //printf("%s", "inserisci la password: \n");
    scanf("%s", password);
    
    lunghezza = strlen(utente)+1;
    lmsg = htons(lunghezza);
    ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = send(sd,(void*)&utente,lunghezza,0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    
    lunghezza = strlen(password)+1;
    lmsg = htons(lunghezza);
    ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = send(sd,(void*)&password,lunghezza,0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    //sleep(3);
    ret = recv(sd, (void*)&lmsg, sizeof(uint16_t), 0);
    lunghezza = ntohs(lmsg);
    ret = recv(sd,(void*)risposta,lunghezza,0);
    return risposta;
}
//funzione della registrazione
char* registrazione(int sd){
       
       int ret, lunghezza;
       char utente[1024];
       char password[1024];
       //char risposta[1024];
       char* risposta = (char*)malloc(1024 * sizeof(char));
    if (risposta == NULL) {
        perror("Errore di allocazione memoria");
        exit(1);
    }
       char log[1024];
       uint16_t lmsg;
       strcpy(log,"mi_registro");     
       lunghezza = strlen(log)+1;
       lmsg = htons(lunghezza);
       ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
       if(ret == -1){
        perror("errore");
        exit(1);
       }
       ret = send(sd,(void*)&log,lunghezza,0);
       if(ret == -1){
        perror("errore");
        exit(1);
        }
       scanf("%s", utente);
       scanf("%s", password);
       lunghezza = strlen(utente)+1;
       lmsg = htons(lunghezza);
       ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
       if(ret == -1){
        perror("errore");
        exit(1);
        }
       ret = send(sd,(void*)&utente,lunghezza,0);
       if(ret == -1){
        perror("errore");
        exit(1);
        }
    
        lunghezza = strlen(password)+1;
        lmsg = htons(lunghezza);
        ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
        if(ret == -1){
        perror("errore");
        exit(1);
        }
        ret = send(sd,(void*)&password,lunghezza,0);
        if(ret == -1){
        perror("errore");
        exit(1);
        }
        //sleep(3);
        ret = recv(sd, (void*)&lmsg, sizeof(uint16_t), 0);
        lunghezza = ntohs(lmsg);
        ret = recv(sd,(void*)risposta,lunghezza,0);
        return risposta;
}
int main(){
    char connessione[5] = "CIAO\0";
    char risposta[1024];
    
    char aux[1024];
    
    int sd,ret,lunghezza;
    uint16_t lmsg;
    struct sockaddr_in server_addr;


    /*creo il socket*/

    sd = socket(AF_INET,SOCK_STREAM,0);
    
    /*creo gli indirizzi del server*/
    
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    /* mi connetto al server*/
    ret = connect(sd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret<0){
        perror("error di connessione: \n");
        exit(1);
    }
    lunghezza = strlen(connessione)+1;
    lmsg = htons(lunghezza);
    ret = send(sd,(void*)&lmsg,sizeof(uint16_t),0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    ret = send(sd,(void*)&connessione,lunghezza,0);
    if(ret == -1){
        perror("errore");
        exit(1);
    }
    /* se ricevo risposta di connessione negativa, esco*/
    ret = recv(sd, (void*)&lmsg, sizeof(uint16_t), 0);
    lunghezza = ntohs(lmsg);
    ret = recv(sd,(void*)&risposta,lunghezza,0);
    printf("%s\n",risposta);

    if(strcmp(risposta,"no")==0){
        close(sd);
        exit(1);
    }
    //insertAtEnd(risposta);
    
    
    strcpy(risposta,login(sd));
    printf("%s\n",risposta);
    while(strcmp("utente non registrato, per registrarsi inserire nome utente e password uno a capo all'altra",risposta)==0){
        
       strcpy(aux,registrazione(sd));
       printf("%s\n",aux);
       if(strcmp(aux,"registrazione avvenuta, per fare login inserire nome utente e password uno a capo all'altra")==0)
  
        strcpy(risposta,login(sd));
        printf("%s\n",risposta);
     }  
        
        gioca(sd);
    /*vado qui quando il client invia il comando end ed eseguo una close */
    close(sd);
    return 0;
}