#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <iostream>

#define ADDRESS     "mysocket"  /* addr to connect */

/*
 * Strings we send to the client.
 */
std::vector<std::string> strs {
    "This is the first string.\n",
    "This is the second string.\n",
    "This is the third string.\n",
    "This is the four string."};

main()
{
    char c;
    FILE *fp;
    int socketfd, newsocketfd;

    /*
     * Get a socket to work with.  This socket will
     * be in the UNIX domain, and will be a
     * stream socket.
     */
    if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("server: socket");
        return 1;
    }

    /*
     * Create the address we will be binding to.
     */
    sockaddr_un saun;
    (void *)memset((void *)&saun, 0, sizeof(saun));
    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, ADDRESS);

    
    /*
     * Try to bind the address to the socket.  We
     * unlink the name first so that the bind won't
     * fail.
     *
     * The third argument indicates the "length" of
     * the structure, not just the length of the
     * socket name.
     */
    unlink(ADDRESS);
    
    socklen_t len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (bind(socketfd, (const sockaddr *)&saun, len) < 0) {
        perror("server: bind");
        return 1;
    }

    /*
     * Listen on the socket.
     */
    if (listen(socketfd, 5) < 0) {
        perror("server: listen");
        return 1;
    }

    /*
     * Accept connections.  When we accept one, ns
     * will be connected to the client.
     */
    sockaddr_un fsaun;
    memset(&fsaun, 0, sizeof(fsaun));
    socklen_t fromlen = sizeof(sockaddr_un);
    if ((newsocketfd = accept(socketfd, (sockaddr *)&fsaun, &fromlen)) < 0) {
        perror("server: accept");
        exit(1);
    }

    /*
     * First we send some strings to the client.
     */
    for (auto st : strs) {
      send(newsocketfd, st.c_str(),st.length(), 0);
    }
    
    /*
     * terminate the connection (sends EOF)
     */
    close(newsocketfd);

    /*
     * Then we read some strings from the client and
     * print them out.
     */
    
    /*
     * Accept connections.  When we accept one, ns
     * will be connected to the client.
     */
    memset(&fsaun, 0, sizeof(fsaun));
    if ((newsocketfd = accept(socketfd, (sockaddr *)&fsaun, &fromlen)) < 0) {
        perror("server: accept2");
        exit(1);
    }

    /*
     * We'll use stdio for reading the socket.
     */
    fp = fdopen(newsocketfd, "r");

    while ((c = fgetc(fp)) != EOF) {
      putchar(c);
    }

    /*
     * terminate the connection. We received the EOF already
     */
    unlink(ADDRESS);
    close(socketfd);

    return 0;
}
