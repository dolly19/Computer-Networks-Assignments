#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
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
    int usage;
};

void findProcesses(int,int);
void sort(struct process[],int);
void swap(struct process*,struct process*);
void findTime(int,struct process[],int);



void *func(void* fd)
{
    
    int sockfd = *(int*)fd;
    char top[10];
    bzero(top,10);
    recv(sockfd,top,10,0);
    //printf("hi%s\n",limit);
    int N = atoi(top);
    //printf("%d\n",N);

    char data[1000];
    recv(sockfd,data,1000,0);
    printf("Client's top process is:%s",data);

    FILE *f;
    findProcesses(sockfd,N);
    char filename[100];
    //printf("outside\n");
    sprintf(filename,"serverProcessClientID%d.txt",sockfd);
    
    f = fopen(filename,"r");
    if(f == NULL){
        printf("Could not open file\n");
        exit(1);
    }
    bzero(data,1000);
    while(fgets(data,1000,f) != NULL){
        //printf("%s",data);
        if(send(sockfd,data,sizeof(data),0) < 0){
            printf("File send failed!\n");
            exit(1);
        }
        bzero(data,1000);
    }
    bzero(data,1000);
    sleep(10);
    printf("Terminated connection with client ID%d\n",sockfd);
    close(sockfd);
}

void findProcesses(int sockfd,int N){
    
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
    sprintf(fileName,"serverProcessClientID%d.txt",sockfd);
    
    FILE *f = fopen(fileName,"w");
    for(i=0;i<N;i++){
        fprintf(f,"%d %s %d\n",arr[i].id,arr[i].name,arr[i].usage);
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
    
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    int fd = open(filename,O_RDONLY);
    if(!fd){
        perror("couldn't open file");
        exit(0);
    }
    
    char name[100];
    int utime;
    int stime;

    char buf[10000];
    read(fd,buf,10000);
    char *pch = NULL;
    pch = strtok(buf," ");
    pch = strtok(NULL," ");
    strcpy(name,pch);
    int i=0;
    while(i!=12){
        pch = strtok(NULL," ");
        i++;
    }
    utime = atoi(pch);
    pch = strtok(NULL," ");
    stime = atoi(pch);
    long total=utime+stime;
    
    struct process proc;
    proc.id = pid;
    proc.usage = total;
    strcpy(proc.name,name);
    arr[size] = proc;

}


int main()
{
    struct sockaddr_in servaddr;
    struct sockaddr sockaddr;
    struct sockaddr_in clientaddr;

    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    puts("Socket created");


    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

   
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("socket binding failed");
        exit(EXIT_FAILURE);
    }
    puts("bind done");


    
    if (listen(sockfd, 5)!= 0)
    {
        perror("server listening failed");
        exit(EXIT_FAILURE);
    }
    puts("server listening");



    int len = sizeof(clientaddr);
    pthread_t thread_id;
    int accepted;
    while (accepted = accept(sockfd, (struct sockaddr *)&clientaddr, &len))
    {
       printf("server accepted connection with client ID%d\n",accepted);
       if (pthread_create(&thread_id, NULL, func, (void *)&accepted) < 0)
       {
           perror("thread creation failed");
           exit(EXIT_FAILURE);
       }
    }

    if (accepted != 0)
    {
        perror("server acceptance failed\n");
        exit(EXIT_FAILURE);
    }
         
    close(sockfd);
    }