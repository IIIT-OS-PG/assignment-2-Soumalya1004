#include "customheader.h"
#include "clienttracker.h"
#include "chunktransfer.h"
#define PORT 10100
using namespace std;
//int port;
map < string, int > ump;
char clientip[BUFF_SIZE];
struct client_details{
    int sock;
    struct sockaddr address;
    int addr_len;
};
// struct clientchunk{
//     int client,chunk;
//     char ptr[BUFF_SIZE];//fname
//     FILE * fp1;
//     char conip[BUFF_SIZE];
//     char conport[BUFF_SIZE];
// };
void receiver(int p){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(p);    
    cout <<"Receiver";
    if(connect(sockfd, (struct sockaddr *)&address, sizeof(address))<0){
        cout << "Please reconnect to some other port" <<endl;
    }
    // cout << "Client paused enter any number" << endl;
    // cout << "Multithreading checkpoint" << endl;
    // int x;
    // cin>>x;    
    char Buff[BUFF_SIZE];
    int file_size;int n;
    recv(sockfd, &file_size, sizeof(file_size), 0);    
    cout<< "File size " <<file_size << " " <<endl;
    FILE *fp = fopen("/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/image.jpg", "w+");
    while((n=recv(sockfd, Buff, BUFF_SIZE, 0))>0 && file_size>0){
        fwrite(Buff, sizeof(char), n, fp);        
        memset(Buff, '\0', BUFF_SIZE);
        file_size -=n;
    }
    fclose(fp);
    close(sockfd);
}
void * sender(void *tmp){    
    if(!tmp) {pthread_exit(0);} 
    int n; 
    char Buff[BUFF_SIZE];
    client_details * con = (client_details *)tmp;
    int sockfd = con->sock; 
    // n=recv(sockfd, Buff, BUFF_SIZE, 0);
    receivestring(Buff, sockfd); //port
    cout << Buff << endl;
    bzero(Buff, sizeof(Buff));
    receivestring(Buff, sockfd);  //ip
    cout << Buff << endl;
    bzero(Buff, sizeof(Buff));
    receivestring(Buff, sockfd);  //filename
    cout << Buff << endl;
    //FILE *fp = fopen(Buff, "r");
    bzero(Buff, sizeof(Buff));
    bzero(Buff, sizeof(Buff));
    receivestring(Buff, sockfd);  //filepath
    cout<<"E" << Buff << "E"<<endl;
    string sf(Buff);
    Buff[sf.length()]='\0';
    FILE *fp = fopen(Buff, "r");
    if(fp==NULL){cout<<"Could not open file "<<endl;}
    //bzero(Buff, sizeof(Buff));
    long val;int i;
    recv(sockfd, &i, sizeof(i), 0);  //chunk number  
    cout << "Cnumer " << i <<endl;  
    val = i * chunksize;
    fseek(fp, val, SEEK_SET);
    int size=chunksize;
    char Bufft[TRANSFER_SIZE];
    while((n = fread(Bufft, sizeof(char), sizeof(Bufft), fp)) > 0 && size > 0){
        send(sockfd, Bufft, n, 0);
        bzero(Bufft, sizeof(Bufft));
        size=size-n;
    }
    cout << endl << "Transfer Complete" << endl;
    // FILE *fp = fopen("/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/image.jpg", "r");
    // fseek(fp, 0, SEEK_END);
    // int size = ftell(fp);
    // rewind(fp);    
    // send(sockfd, &size, sizeof(size), 0);
    // cout << size << endl;   
    // int n;
    // while((n = fread(Buff, sizeof(char), BUFF_SIZE, fp)) > 0 && size > 0){
    //     send(sockfd, Buff, n, 0);
    //     memset(Buff, '\0', BUFF_SIZE);
    //     size-=n;
    // }
    // fclose(fp);
    fclose(fp);
    close(sockfd);
}
void * server(void *tmp){
    pthread_t sthread;
    if(!tmp) {pthread_exit(0);}
    int * port = (int *)tmp;  
    client_details * cli;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    cout << "Server Port "<< *port << endl;
    if(server_fd <= 0){
        cerr << "Cannot create socket";
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(*port);
    address.sin_addr.s_addr = inet_addr(clientip); 
    int addrlen = sizeof(address);
    int b = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(b<0){
        cerr << "Could not bind";
        exit(EXIT_FAILURE);
    }    
    int l = listen(server_fd, 5);
    if(l < 0){
        cerr << "Failed to listen";
        exit(EXIT_FAILURE);
    } 
    while(1){  
        cli = (client_details *)malloc(sizeof(client_details));         
        cli->sock = accept(server_fd, &cli->address, (socklen_t*)&cli->addr_len);         
        cout << "Connection request accepted1"<<endl;      
        pthread_create(&sthread, NULL, sender, (void *)cli);
        pthread_detach(sthread);
    }    
    close(server_fd);
}
void setcommandinfo(){
    char Buff[BUFF_SIZE];    
    char* k, *v;
    FILE *ci = fopen("commandinfo.txt", "r");
    while(fgets(Buff, sizeof(Buff), ci)!=NULL){
        k = strtok(Buff, " ");
        v = strtok(0, "\n");
        if(!k || !v){cout << "Error iitializing data";return;}
        //cout << k << " " << v;
        ump[k]=atoi(v);
    }
    cout << "Command List \n";
    for(auto index : ump){
        cout << index.first << " " << index.second << endl; 
    } 
    fclose(ci);     
}
void checkSHA(FILE *fp, char Buffer[chunksize]){
    unsigned char Buff[chunksize];
    char s[chunksize];
    int count =0,n,i;
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    int numberofchunks = ceil((size*1.0 / chunksize));
    cout << (size / chunksize) << endl;
    while((n=fread(Buff, sizeof(char), chunksize, fp))>0 && size>0){        
        unsigned char obuf[10];
        SHA1(Buff, sizeof(Buff), obuf);
        count+=1;        
        for(i=0; i<10; i++) {
        //printf("%02x ", obuf[i]);
        char buf[100];
        sprintf((char*)&buf,"%02x", obuf[i]);
        strcat(s,buf);   
        memset(buf, '\0', 100);
        }      
        memset(Buff, '\0', sizeof(Buff));
        cout << endl;        
    }
    //fclose(fp);
    cout << endl << count << endl;
    //printf("\n%s", s);
    string st(s);
    string sst(Buffer);
    if(strcmp(st.c_str(), sst.c_str()) == 0){
        cout << "Download Successful " << endl;
    }
}
int main(int argc, char *argv[]){
    setcommandinfo();
    // unordered_map < string, unsigned int > tip;
    strcpy(clientip, argv[1]);
    vector < pair < char[BUFF_SIZE], unsigned int > > tip(2);
    pthread_t serverthread;
    int th;
    int prt = atoi(argv[2]);
    char uid1[BUFF_SIZE],pass1[BUFF_SIZE];
    //cout << "Server port " << port << endl;   
    th=pthread_create(&serverthread, NULL, server, (void *)&prt);
    if(th){
        cout << "Error creating thread";
    }
    pthread_detach(serverthread);
    int p;   
    this_thread::sleep_for(3s);
    // while(1){
    //     cout << "Enter port to connect";
    //     cin >> p;
    //     if(p==0)break;
    //     receiver(p);        
    // }    
    //loaduserinfo();
    char Buff[BUFF_SIZE];
    int n;  
    string argu=argv[3]; 
    int groupid=1;
    // cout << "Enter Tracker port to connect ";
    // cin >> p;
    char *k, *kv;char ip[BUFF_SIZE];unsigned int port = p;
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
    while(1){
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        //char ip[] = "127.0.0.1";
        char ip[BUFF_SIZE];
        strcpy(ip, tip[0].first);
        p=tip[0].second;
        unsigned int port = p;
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip);
        address.sin_port = htons(port);
        cout << "Connecting to "<< ip << " " << port <<endl;
        if(connect(sockfd, (struct sockaddr *)&address, sizeof(address))<0){
            cout << "Reconnecting to backup tracker" <<endl;
            this_thread::sleep_for(3s);
            strcpy(ip, tip[1].first);
            port=tip[1].second;           
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr(ip);
            address.sin_port = htons(port);                          
            if(connect(sockfd, (struct sockaddr *)&address, sizeof(address))<0){
                cout << "Please try again later" <<endl;
            }
            else{
                cout << "Backup tracker up" << endl;  
                cout << "Connected to "<< ip << " " << port <<endl;
            }            
        }
        else{
            cout << "Connected to "<< ip << " " << port <<endl;
        }
        // cout << endl << "1 for Upload 2 for download test";
        // cout << endl << "3 for Registration 4 for Login";
        // cout << endl << "5 for Logout" << endl;

        string command;
        cout << "Enter command ";
        getline(cin, command);
        cout << command << endl;
        char commandarr[BUFF_SIZE];
        strcpy(commandarr, command.c_str());
        char *argument[20];int index=0;
        //bzero(argument, sizeof(argument));
        char *t = strtok(commandarr, " ");
        while (t != NULL){
            argument[index] = t;
            //cout << index << " " << argument[index] << endl;            
            t=strtok(NULL, " ");
            index++;
        }
        int x=ump[argument[0]];
        //cout << x << " ";         
        //cin >> x;        
        send(sockfd, &x, sizeof(x), 0);
        if(x==1){
            string logg(uid1);
            if(logg.length()==0){
                cout << "Please login to upload file " << endl;
                bzero(argument, sizeof(argument));
                continue;
            }
            // send(sockfd, &x, sizeof(x), 0);
            //Upload
            // printf("\nEnter file name ");            
            // cin >> argu;
            //cin >> ws;            
            //string str = "/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/"+argu;            char fname[fnamesize];
            //cout << "Enter file path ";
            char filepath[BUFF_SIZE];
            char fname[fnamesize];
            string line;
            strcpy(filepath, argument[1]);
            //cin >> filepath;
            // strcpy(filepath,argv[3]);
            //cout << filepath <<endl;
            //cin >> line;
            // gets(line);            
            // bzero(Buff, sizeof(Buff));
            // strcpy(Buff, line.c_str());
            // cout << Buff << endl;
            // bzero(Buff, sizeof(Buff));
            //fgets(filepath, BUFF_SIZE, stdin); 
            // scanf("%[^\n]%*c", filepath);
            //cout << line <<endl;

            char *arguments[20];int ind=0;
            char filepath1[BUFF_SIZE];
            strcpy(filepath1, filepath);
            char *t = strtok(filepath1, "/");
            while (t != NULL){
                arguments[ind] = t;
                //cout << ind << " " << arguments[ind] << endl;
                
                t=strtok(NULL, "/");
                ind++;
            }
            cout<< arguments[ind-1]<<endl;
            // char *t = strtok(arr, "|");
            // //argu[0] = token;
            // while (t != NULL)
            // {   argu[i] = t;
            //     t = strtok(NULL, "|");
            //     i++;
            // }
            //fname = arguments[ind-1];
            strcpy(fname, arguments[ind-1]);
            cout << fname << endl;
            //strcpy(fname, argu.c_str());
            FILE *fp = fopen(filepath, "r");
            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);
            rewind(fp);
            cout << size;    
            int numberofchunks = ceil(size*1.0/chunksize);
            sendclientdetails(sockfd, ip, prt, groupid, fname, Buff, numberofchunks);             
            sendSHAdetails(sockfd, fp);
            sendstring(Buff, filepath, sockfd);
            sendstring(Buff, uid1, sockfd);  
            cout << "Sent" << endl;
            fclose(fp);
        }
        else if(x==2){
            string logg(uid1);
            if(logg.length()==0){
                cout << "Please login to download file " << endl;
                continue;
            }
            // send(sockfd, &x, sizeof(x), 0);            
            //Download
            // printf("\nEnter file name ");
            // cin >> argu;
            v.clear();
            char fname[fnamesize];
            char downloadpath[BUFF_SIZE];
            cout << argument[1]<<" ";
            strcpy(fname, argument[1]);
            strcpy(downloadpath, argument[2]);
            //strcpy(fname, argu.c_str());
            cout << fname <<endl;
            getpeerdetails(fname, sockfd, ip, port, groupid, Buff);
            //FILE *res1= fopen("result.jpg", "w+"); 
            FILE *res1= fopen(downloadpath, "w+"); 
            fclose(res1); 
            vector < string > vec;  
            vector < peerdetail * > vecpeers;  
            for (auto it = v.begin(); it != v.end(); ++it){
                struct peerdetail *pd1=*it;
                // printf("%s%s%s", pd1->ip, pd1->port, pd1->group);
                // printf("%s%s%s", pd1->fn, pd1->noc, pd1->filesha);
                // printf("%sE \n", pd1->cpatt);
                printf("%s \n", pd1->cpath);
                string s3 = regex_replace(pd1->cpatt, std::regex("^ +| +$|( ) +"), "$1");
                strcpy(pd1->cpatt, s3.c_str());
                string stri(pd1->cpatt);
                vec.push_back(stri);
                vecpeers.push_back(pd1);
                // receiver(atoi(pd1->port));
                // break;                
            }
            vector <vector <int > > mat;
            int noc,rows;
            for (auto it = vec.begin(); it != vec.end(); ++it){
                string stri(*it);
                noc=stri.length();                
                vector < int > rw(stri.length());
                for(int x=0; x<rw.size(); x++){
                    rw[x]=stri[x]-'0';
                }
                mat.push_back(rw);                
                //cout << stri <<endl;
            }
            for(int xx=0; xx<mat.size(); xx++){
                for(int yy=0; yy<mat[xx].size(); yy++){
                    cout << mat[xx][yy] << " ";
                }
                cout << endl;
            } 
            rows=mat.size();int i,j,k=0;
            clientchunk * ki;
            pthread_t cthread[noc];
            for(i=0; i<noc; i++){
                j=0;
                while(j<rows){
                    if(mat[k][i]==1){
                        cout << "Client "  << k << endl;
                        ki = (clientchunk *)malloc(sizeof(clientchunk));                        
                        ki->client=k;
                        ki->chunk=i;
                        //cout << vecpeers[k]->ip << " " << vecpeers[k]->port << endl;
                        strcpy(ki->conip, vecpeers[k]->ip);
                        strcpy(ki->conport, vecpeers[k]->port);
                        strcpy(ki->ptr, vecpeers[k]->fn);
                        //cout << vecpeers[k]->cpath;
                        strcpy(ki->cpath, vecpeers[k]->cpath);
                        strcpy(ki->downpath, downloadpath);
                        //ki->fp1 = fopen(ki->ptr, "r");
                        // // ki->fp1=fp;
                        // //cout << "PKB: BT " << ki->client << " " << ki->chunk << "\n";
                        pthread_create(&cthread[i], NULL, kthclient, (void *)ki);                        
                        //kthclient((void *)ki);                                        
                        k=(k+1)%rows;                
                        break;                
                    }
                    k=(k+1)%rows;               
                    j++;
                }        
            }
            for(i=0;i<noc;i++){
                pthread_join(cthread[i], NULL);
            }    
            FILE *res2= fopen(downloadpath, "r+"); 
            checkSHA(res2, vecpeers[0]->filesha);    
            fclose(res2); 
        }
        else if(x==3){
            //Register
            char uid[BUFF_SIZE],pass[BUFF_SIZE];
            // cout << "Please enter userid ";
            // cin >> uid;
            // cout << "Please enter password ";
            // cin >> pass;
            strcpy(uid, argument[1]);
            strcpy(pass, argument[2]);
            cout<< "\n Userid " << uid << " Password " << pass << endl;
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, uid, sockfd); //senduserid
            bzero(Buff, sizeof(Buff));
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, pass, sockfd); //sendpassword
            bzero(Buff, sizeof(Buff));             
        } 
        else if(x==4){
            //Login            
            // cout << "Please enter userid ";
            // cin >> uid1;
            // cout << "Please enter password ";
            // cin >> pass1;
            strcpy(uid1, argument[1]);
            strcpy(pass1, argument[2]);
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, uid1, sockfd); //senduserid
            bzero(Buff, sizeof(Buff));
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, pass1, sockfd); //sendpassword
            bzero(Buff, sizeof(Buff));
            receivestring(Buff, sockfd); //ack
            cout << Buff << endl;                     
        }      
        else if(x==5){
            //Logout
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, uid1, sockfd); //senduserid
            bzero(Buff, sizeof(Buff));
            bzero(Buff, sizeof(Buff));
            sendstring(Buff, pass1, sockfd); //sendpassword
            bzero(Buff, sizeof(Buff));
            receivestring(Buff, sockfd); //ack
            bzero(uid1, sizeof(uid1));
            bzero(pass1, sizeof(pass1));
            cout << Buff << endl;   
        } 
        close(sockfd);
    }
    return 0;
}