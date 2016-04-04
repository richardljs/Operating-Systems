#include "cs537.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

int *buf;
int buffers;
int threads;
int fill = 0;
int use = 0;
int count = 0;
int i;
int clientlen;
int listenfd;
struct sockaddr_in clientaddr;
/* Initializing the mutex and both conditional variables */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_t master_thread;
pthread_t *worker_thread;

/* Final Put version (from textbook) */
void put(int value) {
  buf[fill] = value;
  fill = (fill + 1) % buffers;
  count ++;
}

/* Final Get version (from textbook) */
int get() {
  int tmp = buf[use];
  use = (use + 1) % buffers;
  count --;
  return tmp;
}

/* Producer code with cond variable (from textbook) */
void *producer(void *arg) {
  clientlen = sizeof(clientaddr);
  int connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
  pthread_mutex_lock(&mutex);
  while (count == buffers) pthread_cond_wait(&empty, &mutex);
  put(connfd);
  pthread_cond_signal(&full);
  pthread_mutex_unlock(&mutex);
}

/* Consumer code with cond variable (from textbook) */
void *consumer(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    while (count == 0) pthread_cond_wait(&full, &mutex);
    int tmp = get();
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    requestHandle(tmp);
    Close(tmp);
  }
}

void getargs(int *port, int *threads, int *buffers, int argc, char *argv[])
{
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <portnum> <threads> <buffers>\n", argv[0]);
    exit(1);
  } else {
    *port = atoi(argv[1]);
    if(*port < 2000) {
      fprintf(stderr, "Choose a port number that is higher than 2000!\n");
      exit(1);
    }
    *threads = atoi(argv[2]);
    if(*threads < 1) {
      fprintf(stderr, "Number of threads specified invalid!\n");
      exit(1);
    }
    *buffers = atoi(argv[3]);
    if(*buffers < 1) {
      fprintf(stderr, "Number of buffers specified invalid!\n");
      exit(1);
    }
    /* Initialize size of the buffer array */
    buf = (int *)malloc(sizeof(int) * (*buffers));
    /* Initialize worker threads */
    worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * (*threads));
    while(i < (*threads)) {
      (void) pthread_create(&(worker_thread[i]), NULL, consumer, NULL);
      i++;
    }
  }
}

int main(int argc, char *argv[])
{
  int port;
  /* Get the arguments specified to the webserver */
  getargs(&port, &threads, &buffers, argc, argv);
  listenfd = Open_listenfd(port);
  while(1) {
    (void) pthread_create(&master_thread, NULL, producer, NULL);
    (void) pthread_join(master_thread, NULL);
  }
  return 0;
}
