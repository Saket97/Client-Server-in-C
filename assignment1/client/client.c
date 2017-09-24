#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#define SERVER_PORT 8080

void parse_string(char* authenticate, char* u_name, char* from)
{
    int l = strlen(from);
    char c = from[0];
    int i = 0;
    while (c != '@')
    {
        authenticate[i] = c;
        i++;
        c = from[i];
    }
    authenticate[i] = '\0';
    i = 0;
    c = from[i];
    while (c != ':')
    {
        u_name[i] = from[i];
        i++;
        c = from[i];
    }
    u_name[i] = '\0';
}
int check_format(char* s)
{
    int l = strlen(s);
    int i = 0;
    int count1 = 0, count2 = 0;
    while(i < l)
    {
        if (s[i] == ':')
            count1++;
        if (s[i] == '@')
            count2++;
        ++i;
    }
    if (count1 == 1 && count2 == 1)
        return 1;
    else
        return 0;
}
int main(int argc, char *argv[])
{
    struct sockaddr_in cli_addr, serv_addr;
    struct hostent *server;
    char authenticate[1000], recd_data[1000], u_name[20];
    char filename[20];
    printf("Please enter the filename (less than 20 bytes)\n");
    fflush(stdout);
    scanf("%s",filename);
    int s;
    if (argc != 2)
    {
        printf("ERROR");
        exit(1);
    }
    if (check_format(argv[1]) == 0)
    {
        perror("Argument Format Wrong\n");
        exit(1);
    }
    parse_string(authenticate, u_name, argv[1]);
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    bzero(&cli_addr, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port = htons(0);

    if (bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    char *ret = strchr(argv[1], '@');
    if ((server=gethostbyname(ret+1))==NULL)
    {
        perror("gethostbyname");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = *((ulong*)server->h_addr);
    serv_addr.sin_port = htons(SERVER_PORT);
    if (connect(s, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("connect");
        exit(1);
    }
    // printf("Authenticate inside client, %s", authenticate);
    fflush(stdout);
    if (write(s, authenticate, strlen(authenticate)+1) == -1)
    {
        perror("Client: Error in sending the username and password\n");
        exit(1);
    }

    if (read(s, recd_data, 1024) == -1 )
    {
        perror("Client: Error in receiving authentication response\n");
        exit(1);
    }
    char buffer[30] = "Hello ";
    strncat(buffer, u_name, 20);
    printf("Client: Message received from server after authentication request, %s\n", recd_data);
    fflush(stdout);

    if (strncmp(recd_data, "Authentication Failure!!!", 30) != 0)
    {
        printf("Client: Authenticated :-)\n");
        printf("Client: Requesting for the file: %s\n", filename);
        fflush(stdout);
        if (write(s, filename, strlen(filename)+1) == -1)
        {
            perror("Error in communicating the filename\n");
            exit(1);
        }
        char read_file[1000];
        int t;
        if ((t = read(s, read_file, 1000)) == -1)
        {
            perror("Error receiving the file\n");
            exit(1);
        }
        char* r = "File Not Found";
        if (strcmp(r, read_file) == 0)
        {
            printf("Client: %s\n", read_file);
        }
        else
        {
            FILE *fp = fopen(filename, "w");
            fwrite(read_file, 1, t-1, fp);
            fclose(fp);
            printf("Client: Read file: \n%s\n", read_file);
        }

    }
    close(s);
    return 0;
}
