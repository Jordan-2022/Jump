// **************** DO NOT CHANGE THIS FILE ****************
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int sock = 0;
  struct addrinfo hints, *result;
  char *msg = "abcdefghij";

  if (argc > 1) {
    msg = "i";
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;

  if (getaddrinfo("host.docker.internal", "2346", &hints, &result)) {
    return -1;
  }

  if ((sock = socket(result->ai_family, result->ai_socktype,
                     result->ai_protocol)) < 0) {
    return -1;
  }

  if (connect(sock, (struct sockaddr *)result->ai_addr, result->ai_addrlen) <
      0) {
    return -1;
  }

  send(sock, msg, strlen(msg), 0);
  close(sock);
  freeaddrinfo(result);
  return 0;
}
