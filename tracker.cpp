#include "customheader.h"
using namespace std;
struct client_details{
    int sock;
    struct sockaddr address;
    int addr_len;    
};
struct peerdetailtracker{
    char ip[BUFF_SIZE];
    char port[BUFF_SIZE];
    char group[BUFF_SIZE];
    char fn[BUFF_SIZE];
    char noc[BUFF_SIZE];
    char filesha[chunksize];
    char cpattern[BUFF_SIZE];
};
struct userdetail{
    char uid[BUFF_SIZE],password[BUFF_SIZE];
    bool status;
};
vector<userdetail *> udv;
vector<peerdetailtracker *> vpdt;
void sendstring(char Buff[], char fname[], int sockfd){
    string s(fname);
    int bufflen=s.length();
    strcpy(Buff, s.c_str());      
    send(sockfd, &bufflen, sizeof(int), 0); 
    send(sockfd, Buff, bufflen, 0);
    //printf("%s %d", Buff, bufflen);
    memset(Buff, '\0', BUFF_SIZE);
}
void receivestring(char Buff[], int sockfd){
    int n;int bufflen;memset(Buff, '\0', BUFF_SIZE);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //filename s   
    Buff[bufflen]='\0'; 
}
void filltrackerinfo(char Buff[], FILE *fp, int n, int bufflen){    
    string clientip(Buff);
    Buff[bufflen]='\0';
    fwrite(Buff, sizeof(char), n, fp); //client ip    
    memset(Buff, '\0', BUFF_SIZE);
}
void saveclientdetails(int sockfd, FILE *fp, char *Buff){
    int bufflen;
    int n;    
    fseek(fp, 0, SEEK_END);
    //peerdetailtracker * cc = (peerdetailtracker *)malloc(sizeof(peerdetailtracker));  
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //client ip
    //strcpy(cc->ip, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //client port
    //strcpy(cc->port, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //client group
    //strcpy(cc->group, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //filename
    //strcpy(cc->fn, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //number of chunks
    //strcpy(cc->noc, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);    
    char buffer[chunksize];
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, buffer, bufflen, 0); //SHA Details
    //strcpy(cc->noc, buffer);
    filltrackerinfo(buffer, fp, n, bufflen); 
    memset(buffer, '\0', chunksize);
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //chunk pattern
    //strcpy(cc->cpattern, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    bzero(Buff, sizeof(Buff));    
    fputs(" ",fp);
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //filelocation
    // // receivestring(Buff, sockfd);
    cout << "Received";
    cout << Buff << endl;
    filltrackerinfo(Buff, fp, n, bufflen);
    fputs(" ",fp);
    bzero(Buff, sizeof(Buff));
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //user info
    //strcpy(cc->cpattern, Buff);
    filltrackerinfo(Buff, fp, n, bufflen);
    bzero(Buff, sizeof(Buff));   
    fputs("\n",fp);
    //vpdt.push_back(cc);    
}
bool isonline(char buff[BUFF_SIZE]){
    for (auto it=udv.begin(); it!=udv.end(); it++){
        struct userdetail *ud=*it;
        if(strcmp(ud->uid, buff)==0 && ud->status){
            return true;
        }
        cout << ud->uid << " " << ud->password << " " << ud->status << endl;        
    }
    return false;
}
void sendpeerdetails(int sockfd, FILE *fp){
    int n,bufflen;
    char Buff[BUFF_SIZE];
    char c[BUFF_SIZE];
    n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    n=recv(sockfd, Buff, bufflen, 0); //filename
    Buff[bufflen] = '\0';
    //printf("%s %d", Buff, bufflen);
    if(fp == NULL){
        printf("File can\'t be accessed");
    }   
    char Buf1[BUFF_SIZE]; 
    while(n=fgets(c,sizeof(c),fp)!=NULL){            
        //printf("%s\n", c);
        string strr(c);
        // cout << strr.length()<<endl;        
        //cout << strr.length()<<" " <<n << endl;
        //if(n==0){break;}
        if(strr.length() <= 6 ){continue;}  
        char * cip = strtok(c, " ");        
        char *  cport = strtok(0, " ");        
        char *  cgrp = strtok(0, " ");       
        char *  cfname = strtok(0, " ");
        char *  cnoc = strtok(0, " ");
        char *  cSHA = strtok(0, " ");
        char *  cpatt = strtok(0, " ");        
        char *  cpath = strtok(0, " "); 
        char *  cuser = strtok(0, "\n");        
        if(strcmp(Buff, cfname) == 0 && isonline(cuser)){
            //strcpy(Buff,s.c_str());            
            sendstring(Buf1, cip, sockfd);
            sendstring(Buf1, cport, sockfd);
            sendstring(Buf1, cgrp, sockfd);
            sendstring(Buf1, cfname, sockfd);
            sendstring(Buf1, cnoc, sockfd);
            sendstring(Buf1, cSHA, sockfd);
            sendstring(Buf1, cpatt, sockfd);
            sendstring(Buf1, cpath, sockfd);
            // cout <<"File path " <<cpath << endl;
            //cout << cip << " " << cport << " "<<cpatt <<endl;
        }    
         memset(c, '\0', sizeof(c));      
    }    
    cout << "Sent";
    rewind(fp);    
    string s="Received";
    strcpy(Buff,s.c_str());
    //bufflen=s.length();
    //Buff[bufflen]='\0';
    sendstring(Buff, Buff, sockfd);
    strcpy(Buff,s.c_str());
    sendstring(Buff, Buff, sockfd);
    // send(sockfd, &bufflen, sizeof(int), 0); 
    // send(sockfd, Buff, bufflen, 0);
}
void showusers(){
    for (auto it=udv.begin(); it!=udv.end(); it++){
        struct userdetail *ud=*it;
        //cout << ud->uid << " " << ud->password << " " << ud->status << endl;        
        cout << ud->uid << " " << ud->status << endl;        
    }
}
void * sockconnect(void *tmp){     
    if(!tmp) {pthread_exit(0);}   
    client_details * con = (client_details *)tmp;
    int sockfd = con->sock;    
    char Buff[BUFF_SIZE];
    //FILE *fp = fopen("/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/image.jpg", "wb");
    int x,n;
    n=recv(sockfd, &x, sizeof(x), 0);
    cout << endl << x << endl ; 
    if(x==1){
        // Upload
        FILE *fp = fopen("clientinfo.txt", "a+");
        saveclientdetails(sockfd, fp, Buff);
        fclose(fp);
    }
    else if(x==2){
        //Download
        FILE *fp = fopen("clientinfo.txt", "r+");
        sendpeerdetails(sockfd, fp);
        printf("%s\n", Buff); 
        fclose(fp);
    }
    else if(x==3){
        //Registration
        struct userdetail * ud=(userdetail *)malloc(sizeof(userdetail)); 
        FILE *fp = fopen("userdetails.txt", "a+");
        bzero(Buff, sizeof(Buff));
        receivestring(Buff, sockfd);
        strcpy(ud->uid, Buff);
        fputs(Buff, fp);
        fputs(" ", fp);
        //cout << Buff << endl; 
        bzero(Buff, sizeof(Buff));
        receivestring(Buff, sockfd);        
        strcpy(ud->password, Buff);
        fputs(Buff, fp);
        fputs("\n", fp);
        //cout << Buff << endl; 
        bzero(Buff, sizeof(Buff));
        fclose(fp);
        udv.push_back(ud);       
    }
    else if(x==4){
        //Login
        char uid[BUFF_SIZE],pass[BUFF_SIZE];
        bzero(Buff, sizeof(Buff));
        receivestring(Buff, sockfd); // receive user id
        strcpy(uid, Buff);
        bzero(Buff, sizeof(Buff)); 
        receivestring(Buff, sockfd); // receive password
        strcpy(pass, Buff);
        bzero(Buff, sizeof(Buff));
        int c=0,index=0;
        for (auto it=udv.begin(); it!=udv.end(); it++){
            struct userdetail *ud=*it;
            if(strcmp(uid, ud->uid)==0 && strcmp(pass, ud->password)==0){
                udv[index]->status=true;
                c=1;
                break;
            }
            index++;
            //printf("%s %s\n", ud->uid, ud->password);
        }
        if(c==0){
            //Invalid credentials
            string ss = "Invalid Credentials";
            bzero(Buff, sizeof(Buff));
            strcpy(Buff, ss.c_str());
            sendstring(Buff, Buff, sockfd);
            bzero(Buff, sizeof(Buff));            
        }
        else if(c==1){
            //Login successful
            string ss = "Login successful";
            bzero(Buff, sizeof(Buff));
            strcpy(Buff, ss.c_str());
            sendstring(Buff, Buff, sockfd);
            bzero(Buff, sizeof(Buff));
            showusers();
        }        
    }
    else if(x==5){
        //Logout
        char uid[BUFF_SIZE],pass[BUFF_SIZE];
        bzero(Buff, sizeof(Buff));
        receivestring(Buff, sockfd); // receive user id
        strcpy(uid, Buff);
        bzero(Buff, sizeof(Buff)); 
        receivestring(Buff, sockfd); // receive password
        strcpy(pass, Buff);
        bzero(Buff, sizeof(Buff));
        int c=0,index=0;
        for (auto it=udv.begin(); it!=udv.end(); it++){
            struct userdetail *ud=*it;
            if(strcmp(uid, ud->uid)==0 && strcmp(pass, ud->password)==0){
                udv[index]->status=false;
                c=1;
                break;
            }
            index++;
            //printf("%s %s\n", ud->uid, ud->password);
        }
        string ss = "Logout successful";
        bzero(Buff, sizeof(Buff));
        strcpy(Buff, ss.c_str());
        sendstring(Buff, Buff, sockfd);
        bzero(Buff, sizeof(Buff));
        showusers();
    }    
    close(sockfd);
}
void loaduserinfo(){
    FILE *fp = fopen("userdetails.txt", "r");
    char c[fnamesize];
    char *k, *v;
    cout << "Users "<< endl;
    struct userdetail * ud;
    while(fgets(c,fnamesize,fp)!=NULL){
        ud = (userdetail *)malloc(sizeof(userdetail));  
        k = strtok(c, " ");
        v = strtok(0, "\n");
        string s1(k);
        if(!k || !v){cout << "Error iitializing data";return;}
        //cout << k << " " << v;
        strcpy(ud->uid, s1.c_str());
        string s2(v);
        strcpy(ud->password, s2.c_str());
        ud->status=false;
        //cout << ud->uid << " " << ud->password << " " << ud->state << endl;
        udv.push_back(ud);
    }      
    rewind(fp);
    fclose(fp);
}

int main(int argc, char *argv[]){
    pthread_t sthread;
    client_details * cli;
    this_thread::sleep_for(10s);
    loaduserinfo();
    showusers();

    char Buff[BUFF_SIZE];
    vector < pair < char[BUFF_SIZE], unsigned int > > tip(2);
    string argu=argv[1]; 
    int groupid=1;
    // cout << "Enter Tracker port to connect ";
    // cin >> p;
    char *k, *kv;char ip[BUFF_SIZE];
    FILE *ti=fopen(argu.c_str(), "r");
    int tip_i=0;
    while(fgets(Buff, sizeof(Buff), ti)!=NULL){
        k = strtok(Buff, " ");
        kv = strtok(0, "\n");
        if(!k || !kv){cout << "Error iitializing data";return -1;}
        strcpy(tip[tip_i].first, k);
        tip[tip_i].second=atoi(kv);
        tip_i++;
        //break;
        //cout << k << " " << v;
        // ump[k]=atoi(v);
    }
    fclose(ti);

    int tracker_no=atoi(argv[2])-1;
    

    //cout << "Connected";    
    int port = tip[tracker_no].second;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd <= 0){
        cerr << "Cannot create socket";
        exit(EXIT_FAILURE);
    }    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    //address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_addr.s_addr = inet_addr(tip[tracker_no].first);
    cout << tip[tracker_no].first << " " << tip[tracker_no].second << endl;
    int addrlen = sizeof(address);
    int b = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(b<0){
        cerr << "Could not bind";
        exit(EXIT_FAILURE);
    }
    int l = listen(server_fd, 3);
    if(l < 0){
        cerr << "Failed to listen";
        exit(EXIT_FAILURE);
    }    
    while(1){
        // //int sockfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        // cli = (client_details *)malloc(sizeof(client_details));        
        // cli->sock = accept(server_fd, &cli->address, (socklen_t*)&cli->addr_len);        
        // pthread_create(&sthread, NULL, sockconnect, (void *)cli);
        // pthread_detach(sthread);
        // //sockconnect(sockfd);
        cli = (client_details *)malloc(sizeof(client_details));         
        cli->sock = accept(server_fd, &cli->address, (socklen_t*)&cli->addr_len);  
        cout << "Connection request accepted by tracker"<<endl;
        pthread_create(&sthread, NULL, sockconnect, (void *)cli);
        pthread_detach(sthread);
    }    
    close(server_fd);  
    return 0;
}