#include "customheader.h"
using namespace std;
struct clientchunk{
    int client,chunk;
    char ptr[BUFF_SIZE];
    FILE * fp1;
    char conip[BUFF_SIZE];
    char conport[BUFF_SIZE];
    char cpath[TRANSFER_SIZE];
    char downpath[BUFF_SIZE];
};
void * kthclient(void *tmp){
    char Buff[chunksize];   
    if(!tmp) {pthread_exit(0);}   
    clientchunk * cc = (clientchunk *)tmp;   
    int k,i;
    k=cc->client;i=cc->chunk;
    long val=0;
    // FILE * fp = cc->fp1;
    // FILE * res = fopen("/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/result.jpg", "r+"); 
    // if(res==NULL){cout << "Problem";}

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    string inetaddr(cc->conip);
    address.sin_addr.s_addr = inet_addr(inetaddr.c_str());
    address.sin_port = htons(atoi(cc->conport));  
    //cout <<"Receiver";
    if(connect(sockfd, (struct sockaddr *)&address, sizeof(address))<0){
        cout << "Please reconnect to some other port "<< cc->conport <<endl;
    }
    sendstring(Buff, cc->conport, sockfd); //port
    sendstring(Buff, cc->conip, sockfd);  //ip
    sendstring(Buff, cc->ptr, sockfd); //filename
    cout << cc->cpath << endl;
    sendstring(Buff, cc->cpath, sockfd); //filepath
    send(sockfd, &cc->chunk, sizeof(cc->chunk), 0);//chunk number
    
    //send(sockfd, cc->conport, sizeof(cc->conport), 0); 
    //send(sockfd, cc->ptr, sizeof(cc->ptr), 0);    
         
    val = i * chunksize;

    //FILE * res = fopen("/media/soumalya/New\ Volume/Ebooks_Mtech/sem1/Operating\ System/Assignment2/Assignment2_VS/result.jpg", "r+"); 
    FILE * res = fopen(cc->downpath, "r+");
    // fseek(fp, val, SEEK_SET);
    fseek(res, val, SEEK_SET);
    char Bufft[TRANSFER_SIZE];
    int size=chunksize; int n;
    while((n=recv(sockfd, Bufft, sizeof(Bufft), 0))>0 && size > 0){
        fwrite(Bufft, sizeof(char), n, res);        
        bzero(Bufft, sizeof(Bufft));
        size=size-n;
    }
    fclose(res);

    // cout << "chunk number " << i << " Position of fp " << val << " "<< ftell(fp) << " "<<ftell(res) << endl;
    // //cout << "actual position of fp before copy " << ftell(res)<<" "<<fp<<endl; 
    // bzero(Buff, sizeof(Buff));
    // int n=fread(Buff, sizeof(char), chunksize, fp);
    // //cout<<Buff<<endl;
    // fwrite(Buff, sizeof(char), n, res);
    // // cout << "Test"<<endl;
    // cout << "chunksize read " << n << endl;
    // cout << "Actual position of fp after copy "<< ftell(fp)<<" "<<fp<< endl;    
    // cout << endl;
    // // for(int x=0; x< fpv.size(); x++){
    // //     //cerr<< "Err" << x <<endl;
    // //     fclose(res[x]);
    // // } 
    // fclose(res);
    // fclose(fp);  
}