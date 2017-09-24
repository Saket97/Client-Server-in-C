#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define SERVER_PORT 8080
char filedata[10000];
int fill (char filedata[], char filename[])
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        // perror("could not find file\n");
        char *fd = "File Not Found";
        strcpy(filedata, fd);
        return strlen(filedata)+1;
    }
    else
    {
        int n = fread(filedata, 1, 1000, fp);
        filedata[n] = '\0';
        fclose(fp);
        return n+1;
    }
}
int authenticate_user(char authenticate[])
{
    // printf("Authentication function called\n",authenticate);
    fflush(stdout);
    FILE *fp = fopen("users.txt", "r");
    char line[200];
    int l = strlen(authenticate);
    while( fgets(line, 200, fp) != NULL)
    {
        if (strstr(line, authenticate) && strlen(line)-1 == strlen(authenticate))
        {
            // printf("len(line):%d len(auth0:%d", strlen(line), strlen(authenticate));
            fflush(stdout);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
void find_uname(char* authenticate, char* u_name)
{
    int i=0;
    char c = authenticate[i];
    while(c != ':')
    {
        u_name[i] = c;
        ++i;
        c = authenticate[i];
    }
    u_name[i] = '\0';
}
int main (int argc, char** argv)
{
    struct sockaddr_in cli_addr, serv_addr;
    struct hostnet *server;
    int cli_addr_len, s, news;
    long data;
    char u_name[20];
    if ((s = socket(AF_INET, SOCK_STREAM,0)) == -1)
    {
        perror("Socket\n");
        exit(1);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    listen(s,5);
    while(1)
    {
        if ((news=accept(s, (struct sockaddr *)&cli_addr, &cli_addr_len)) == -1)
        {
            perror("accept\n");
            exit(1);
        }
        // authenticate
        char authenticate[1000];
        if (read(news, authenticate, 1000) == -1)
        {
            perror ("Server: Error in reading the username and password\n");
            close (news);
            exit(1);
        }
        else
        {
            // search in the file auth.txt
            int flag = authenticate_user(authenticate);
            find_uname(authenticate, u_name);
            // printf("\nfound username, %s\n",u_name);
            fflush(stdout);
            char buffer[30] = "Hello ";
            strncat(buffer, u_name, 20);
            if (flag == 0)
            {
                char r[30] = "Authentication Failure!!!";
                perror("Server: Authentication failed\n");
                write(news, r, strlen(r)+1);
                fflush(stdout);
                close(news);
               
            }
            else
                write(news, buffer, strlen(buffer)+1);
                
        }
        char filename[30];
        if (read(news, filename, 30) == -1)
        {
            perror("Server: Error in reading the filename\n");
            close(news);
            exit(1);
        }
        else
        {
            bzero(filedata, 10000);
            int n = fill(filedata, filename);
            if (write(news, filedata, n) == -1)
            {
                perror("Server: Error in sending the file data");
                close(news);
                exit(1);
            }
            close(news);
        }
    }
}