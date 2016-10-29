#ifndef READER_WRITER_C
#define READER_WRITER_C

#define FILENAME "output.txt"

#define mutex_init pthread_mutex_init
#define mutex_lock pthread_mutex_lock
#define mutex_unlock pthread_mutex_unlock

#define semaphore_init sem_init
#define semaphore_signal sem_post
#define sempahore_wait sem_wait

#include "helper_functions.c"

typedef pthread_mutex_t mutex;
typedef sem_t semaphore;

/* locking mechanisms */
static mutex            readerCountMutex;
static semaphore        noWriters;

/* accounting varibles */
static int shouldContinue;
static int readerCount;
static int lineCount;

/* buffer variable */
static FILE*            buffer;

/* initialization code */
void init() {
  randInit();

  /* opens buffer for writing */
  buffer = fopen(FILENAME, "w+");

  /* mutex lock setup */
  mutex_init(&readerCountMutex, NULL);

  /* accounting integers setup */
  shouldContinue = 1;
  lineCount = 0;
  readerCount = 0;

  /* semaphore setup */
  semaphore_init(&noWriters, 0, 1);
}

/* de initialization code */
void deInit() {
  fclose(buffer);
}

/* utility method to signal threads to end */
void signalEnd() {
  shouldContinue = 0;
}

static int writeItem(int item) {

  sempahore_wait(&noWriters); /* get write lock */

  /* write begins here*/
  fseek(buffer, 0, SEEK_END);
  fprintf(buffer, "%d\n", item);
  fflush(buffer);

  printf("Writer: [ %2d ] %11d\n", lineCount, item);

  lineCount++;
  /* write ends here */

  semaphore_signal(&noWriters); /* release write lock */

  return 0;
}

static int readItem() {

  mutex_lock(&readerCountMutex);
  readerCount++;
  if(readerCount == 1) {
    sempahore_wait(&noWriters);
  }
  mutex_unlock(&readerCountMutex);

  /* read begins here */
  FILE* localBuffer = fopen(FILENAME, "r+");
  rewind(localBuffer);

  int lineToRead = nextRand() % lineCount;
  int currentLine = 0;
  int val;

  /* walk the file until you reach the proper line */
  while(currentLine < lineToRead) {
    fscanf(localBuffer, "%d\n", &val);
    currentLine++;
  }

  fscanf(localBuffer, "%d\n", &val);

  fclose(localBuffer);

  printf("Reader: [ %2d ] %11d\n", lineToRead, val);

  /* read ends here */

  mutex_lock(&readerCountMutex);
  readerCount--;
  if(readerCount == 0) {
    semaphore_signal(&noWriters);
  }
  mutex_unlock(&readerCountMutex);

  return 0;
}

/* writer thread */
void* writer(void* params) {

  /* local variable */
  int sleepTime = nextRand() % 10 + 1;

  printf("Creating a writer that sleeps for %d seconds\n", sleepTime);
  sleep(1);

  while(shouldContinue) { /* while should continue is true */

    /* write to buffer and assert that return was not an error */
    assertTrue(!writeItem(nextRand()),
      "An error occurred while trying to write a number");

    sleep(sleepTime); /* sleep for random amount */
  }
}

/* reader thread */
void* reader(void* params) {

  /* local variables */
  int sleepTime = nextRand() % 10 + 1;

  printf("Creating a reader that sleeps for %d seconds\n", sleepTime);
  sleep(1);

  while(shouldContinue) { /* while should continue is true */

    /* read from buffer */
    if(lineCount > 0) { /* if linecount > 0 */

      /* read item, and assert that the return was not an error */
      assertTrue(!readItem(),
        "An error occurred while trying to read a number");
    }

    sleep(sleepTime); /* sleep for random amount */
  }
}

#endif /* READER_WRITER_C */
