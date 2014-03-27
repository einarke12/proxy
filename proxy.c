/*
 * proxy.c - CS:APP Web proxy
 *
 * TEAM MEMBERS:
 *     Einar Karl Einarsson, einarke12@ru.is 
 *     Unnar Kristjánsson, unnar12@ru.is
 *     Aron Bachmann Árnason, arona12@ru.is
 * 
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */ 

#include "csapp.h"

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port);
void proxy(int fd);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);
void read_requesthdrs(rio_t *rp);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
            char *shortmsg, char *longmsg);

/*
 * Proxy server core
 */
void prox(int fd) 
{
    int size 10;
    int serverfd;
    struct stat sbuf;  
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], pathname[MAXLINE], logstring[MAXLINE];
    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);

    format_log_entry(logstring, struct sockaddr_in *sockaddr, 
              uri, int size)
    
    char parse_hostname[MAXLINE];
    char parse_port[MAXLINE];
    if (parse_uri(uri, parse_hostname, pathname, parse_port) == -1){ 
        printf("Unable to parse the URI!\n");
        return 0;
    }
    
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }
    
    if ( (serverfd = open_clientfd(parse_hostname, parse_port)) < 0){
            printf("Unable to connect to web server! \n");
            Close(serverfd);
        return 0;
    }
    
    if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found",
                    "Proxy couldn’t find this file");
    return;
    }

}

void read_requesthdrs(rio_t *rp)
{  
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}

/* 
 * main - Main routine for the proxy program 
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port, clientlen;
    // Client port
    struct sockaddr_in clientaddr;
 
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    //Open and listen top argument port
    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        
        prox(connfd);
        Close(connfd);
    }
}

/*
 * parse_uri - URI parser
 * 
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */

int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) 
    {
    	hostname[0] = '\0';        
    	return -1;
    }
       
    // Extract the host name 
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';
    
    // Extract the port number 
    *port = 80;  default 
    if (*hostend == ':')   
	*port = atoi(hostend + 1);
    
     Extract the path 
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
	pathname[0] = '\0';
    }
    else {
	pathbegin++;	
	strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    // creating a new filepointer to the log file 
    FILE *file; 
    file = fopen("proxy.log", "a+"); 
    fprintf(file, '\n',  logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri); 
    fclose(file); 

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}


