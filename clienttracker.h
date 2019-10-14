#include "customheader.h"
#define ll long long
using namespace std;
struct peerdetail{
    char ip[20];
    char port[20];
    char group[5];
    char fn[BUFF_SIZE];
    char noc[10];
    char filesha[chunksize];
    char cpatt[BUFF_SIZE];
};
vector<peerdetail *> v;
void fillfileinfo(char Buff[], FILE *fp, int n, int bufflen){    
    string clientip(Buff);
    Buff[bufflen]='\0';
    fwrite(Buff, sizeof(char), n, fp); //client ip    
    memset(Buff, '\0', BUFF_SIZE);
}
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
void sendSHAdetails(int sockfd, FILE *fp){
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
    printf("\n%s", s);
    string st(s);
    int bufflen = st.length();   
    send(sockfd, &bufflen, sizeof(int), 0);
    send(sockfd, s, bufflen, 0);     
    memset(s, '\0', sizeof(s)); 
    //rewind(fp);
    string cip=""; // chunk pattern
    char buf1[BUFF_SIZE];
    memset(buf1, '\0', sizeof(buf1)); 
    for(int jk=0; jk<numberofchunks; jk++){
        cip=cip + to_string(1);
    }    
    strcpy(buf1, cip.c_str());    
    sendstring(buf1, buf1, sockfd);
}
void sendclientdetails(int sockfd, char ip[], unsigned int port, int groupid, 
                       string argu, char Buff[], int numberofchunks){
    string  cip;
    int bufflen;
    cip = ip; // client ip    
    strcpy(Buff, cip.c_str());
    bufflen = cip.length();
    send(sockfd, &bufflen, sizeof(int), 0);
    send(sockfd, Buff, bufflen, 0);    
    memset(Buff, '\0', BUFF_SIZE);
    cip = to_string(port); // client port
    strcpy(Buff, cip.c_str());
    bufflen = cip.length();
    send(sockfd, &bufflen, sizeof(int), 0);
    send(sockfd, Buff, bufflen, 0);    
    memset(Buff, '\0', BUFF_SIZE);
    cip = to_string(groupid); // client groupid
    strcpy(Buff, cip.c_str());
    bufflen = cip.length();
    send(sockfd, &bufflen, sizeof(int), 0);
    send(sockfd, Buff, bufflen, 0);    
    memset(Buff, '\0', BUFF_SIZE);
    strcpy(Buff, argu.c_str()); // client filename
    bufflen = argu.length();
    send(sockfd, &bufflen, sizeof(int), 0);
    send(sockfd, Buff, bufflen, 0);    
    memset(Buff, '\0', BUFF_SIZE);  
    cip = to_string(numberofchunks); // number of chunks
    strcpy(Buff, cip.c_str());    
    sendstring(Buff, Buff, sockfd);
}

void getpeerdetails(char fname[], int sockfd, char ip[], 
                    unsigned int port, int groupid, char Buff[]){
    sendstring(Buff, fname, sockfd);
    int bufflen,n;
    char ack[BUFF_SIZE];
    string strr="Received";
    strcpy(ack,strr.c_str());
    ack[strr.length()] = '\0';         
    while (true){        
        receivestring(Buff, sockfd);
        if(strcmp(Buff, ack)==0){break;}  
        struct peerdetail *pd;
        pd = (peerdetail *)malloc(sizeof(peerdetail));  
        //strcpy(pd.ip, Buff);
        string s(Buff);
        strcpy(pd->ip, Buff);        
        // printf("%s ", Buff);
        receivestring(Buff, sockfd);
        strcpy(pd->port, Buff);        
        // printf("%s ", Buff);  
        receivestring(Buff, sockfd);        
        strcpy(pd->group, Buff);
        receivestring(Buff, sockfd);        
        strcpy(pd->fn, Buff);
        receivestring(Buff, sockfd);        
        strcpy(pd->noc, Buff);
        receivestring(Buff, sockfd);        
        strcpy(pd->filesha, Buff);
        receivestring(Buff, sockfd);        
        strcpy(pd->cpatt, Buff);
        // printf("%s\n", Buff);   
        v.push_back(pd);     
    }            
    // n=recv(sockfd, &bufflen, sizeof(bufflen), 0);
    // n=recv(sockfd, Buff, bufflen, 0); //ack
}

// int main(int argc, char *argv[]){
//     loaduserinfo();
//     char Buff[BUFF_SIZE];
//     int n;  
//     string argu=argv[2]; 
//     // string str = "/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/"+argu;
//     // char fname[fnamesize];
//     // strcpy(fname, str.c_str());
//     int groupid=1;
//     while(1){
//         int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//         char ip[] = "127.0.0.1";
//         unsigned int port = atoi(argv[1]);
//         struct sockaddr_in address;
//         address.sin_family = AF_INET;
//         address.sin_addr.s_addr = inet_addr(ip);
//         address.sin_port = htons(port);
//         if(connect(sockfd, (struct sockaddr *)&address, sizeof(address))<0){
//         cout << "Please reconnect to some other port" <<endl;
//         }
//         cout << endl << "1 for Upload 2 for download ";int x;cin >> x;        
        
//         if(x==1){
//             send(sockfd, &x, sizeof(x), 0);
//             //Upload
//             // printf("\nEnter file name ");            
//             // cin >> argu;
//             //cin >> ws;            
//             //string str = "/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/"+argu;            char fname[fnamesize];
//             char fname[fnamesize];
//             strcpy(fname, argu.c_str());
//             FILE *fp = fopen(fname, "r");
//             fseek(fp, 0, SEEK_END);
//             int size = ftell(fp);
//             rewind(fp);
//             cout << size;    
//             sendclientdetails(sockfd, ip, port, groupid, argu, Buff);    
//             sendSHAdetails(sockfd, fp);
//             fclose(fp);
//         }
//         else if(x==2){
//             send(sockfd, &x, sizeof(x), 0);            
//             //Download
//             // printf("\nEnter file name ");
//             // cin >> argu;
//             char fname[fnamesize];
//             strcpy(fname, argu.c_str());
//             cout << fname <<endl;
//             getpeerdetails(fname, sockfd, ip, port, groupid, Buff);                  
//         }
//         else if(x==3){
//             //Register
//             char uid[BUFF_SIZE],pass[BUFF_SIZE];
//             cout << "Please enter userid ";
//             cin >> uid;
//             cout << "Please enter password ";
//             cin >> pass;            
//             FILE *fp = fopen("userdetails.txt", "a+");
//             cout<<uid;
//             cout << pass;
//             fputs(uid, fp);
//             fputs(" ", fp);
//             fputs(pass, fp);
//             fputs("\n", fp);
//             fclose(fp);
//         }
//         // else if(x==4){
//         //     //Login
//         //     cout << argv[1] <<endl;
//         //     getpeerdetails(argv[1], sockfd, ip, port, groupid, Buff);                  
//         // }
//         close(sockfd);
//     }
//     return 0;
// }