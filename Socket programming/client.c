#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#define PORT 1234

struct process{
    int id;
    char name[1000];
    unsigned long usage;
};

void findProcesses(int,int);
void sort(struct process[],int);
void swap(struct process*,struct process*);
void findTime(int,struct process[],int);

void func(int sockfd)
{
    int n;
    char N[10];
    bzero(N,10);
    sprintf(N,"15");
    printf("Connected to server\n");
    send(sockfd,N,sizeof(N),0);

    char fileName[100];
    char buffer[1000];
    findProcesses(sockfd,1);
    printf("Sending client's top Process to the server\n");
    FILE *clientFP;
    sprintf(fileName,"clientProcessID%d.txt",sockfd);
    clientFP = fopen(fileName,"r");
    if(clientFP == NULL){
        printf("Couldn't open file %s\n",fileName);
        exit(1);
    }
    bzero(buffer,1000);
    fgets(buffer,1000,clientFP);
    printf("%s",buffer);
    send(sockfd,buffer,1000,0);


    FILE *fp;
    
    
    sprintf(fileName,"receive%d.txt",sockfd);
    fp = fopen(fileName,"w");
    if(fp == NULL){
        printf("Couldn't open file");
        exit(1);
    }
    while(1){
        //printf("inside\n");
        n = recv(sockfd,buffer,1000,0);
        if(n<=0){
            break;
        }
        fprintf(fp,"%s",buffer);
        bzero(buffer,sizeof(buffer));
    }
    fclose(fp);
    printf("written server's top %s processes to file %s\n",N,fileName);
    
}

void findProcesses(int sockfd,int N){
    //printf("inside find process\n");
    struct process arr[1000];
    int size = 0;

    struct dirent *dirent;
    DIR *dir;
    int pid;

    if(!(dir = opendir("/proc"))){
        printf("Open Failed!\n");
        exit(0);
    }
    while(dirent = readdir(dir)){
        if(isdigit(*dirent -> d_name)){
            pid = atoi(dirent -> d_name);
            findTime(pid,arr,size);
            size = size + 1;
        }
    }
    
    int i;
    sort(arr,size);
    closedir(dir);
    char fileName[100];
    sprintf(fileName,"clientProcessID%d.txt",sockfd);
    FILE *f = fopen(fileName,"w");
    for(i=0;i<N;i++){
        fprintf(f,"%d %s %lu\n",arr[i].id,arr[i].name,arr[i].usage);
    }
    fclose(f);
    //return f;
}

void sort(struct process arr[],int size){
    int i,j,max_indx;
    for(i=0;i<size-1;i++){
        max_indx = i;
        for(j=i+1;j<size;j++){
            if(arr[j].usage > arr[max_indx].usage){
                max_indx = j;
            }
        }
        swap(&arr[i],&arr[max_indx]);
    }
}
void swap(struct process* p1,struct process* p2){
    struct process temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}
void findTime(int pid,struct process arr[],int size) {
    //printf("inside findTime\n");
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    int fd = open(filename,O_RDONLY);
    if(!fd){
        perror("couldn't open file");
        exit(0);
    }
    
    char comm[100];
    int utime;
    int stime;

    char buf[10000];
    read(fd,buf,10000);
    char *pch = NULL;
    pch = strtok(buf," ");
    pch = strtok(NULL," ");
    strcpy(comm,pch);
    int i=0;
    while(i!=12){
        pch = strtok(NULL," ");
        i++;
    }
    utime = atoi(pch);
    pch = strtok(NULL," ");
    stime = atoi(pch);
    long total=utime+stime;
    
    struct process p;
    p.id = pid;
    strcpy(p.name,comm);
    p.usage = total;
    arr[size] = p;

}


int main()
{

    struct sockaddr_in servaddr;
    struct sockaddr sockaddr;


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("socket creation failed\n");
        exit(0);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    
    int connectfd = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(connectfd !=0)
    {
        printf("Connection with the server failed\n");
        exit(0);
    }

    func(sockfd);
    close(sockfd);
}