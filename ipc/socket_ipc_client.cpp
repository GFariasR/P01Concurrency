#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#define ADDRESS     "mysocket"  /* addr to connect */

#include <string>
#include <vector>

main()
{
    char c;
    FILE *fp;
    int s;

    /*
     * Get a socket to work with.  This socket will
     * be in the UNIX domain, and will be a
     * stream socket.
     */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("client: socket");
        return 1;
    }

    /*
     * Create the address we will be connecting to.
     */
    sockaddr_un saun;
    (void *)memset((void *)&saun, 0, sizeof(saun));
    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, ADDRESS);

    /*
     * Try to connect to the address.  For this to
     * succeed, the server must already have bound
     * this address, and must have issued a listen()
     * request.
     *
     * The third argument indicates the "length" of
     * the structure, not just the length of the
     * socket name.
     */
    socklen_t len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (connect(s, (sockaddr *)&saun, len) < 0) {
        perror("client: connect");
        return 1;
    }

    /*
     * We'll use stdio for reading
     * the socket.
     */
    fp = fdopen(s, "r");
    std::vector<std::string> strs;

    /*
     * First we read some strings from the server
     * print them out, saving them into strs vector.
     */
    while (true) {
      std::string tmp_str("Client::");
      while (( (c = fgetc(fp)) != '\n') and (c != EOF)) {
        putchar(c);   // print it
	tmp_str.push_back(c);   // save it on temporal
      }
      if (!tmp_str.empty() ) {
        putchar('\n');   // print End Of Line
        tmp_str.push_back('\n');   // save it on temporal
        strs.push_back(tmp_str);   // push the temporal to vector
      }
      if (c == EOF) {
	break;
      }
    }
    // close the socket
    close(s);

    /*
     * Now we send some strings to the server.
     * We need to open a socket first
     */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("client: socket2");
        return 1;
    }

    if (connect(s, (sockaddr *)&saun, len) < 0) {
        perror("client: connect2");
        return 1;
    }

    for (auto st : strs) {
      send(s, st.c_str(), st.length(), 0);
    }
    
    /*
     * We can simply use close() to terminate the
     * connection, to send EOF to the server
     */
    close(s);

    return 0;
}
