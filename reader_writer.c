#ifndef READER_WRITER_C
#define READER_WRITER_C

#define mutex_init pthread_mutex_init
#define mutex_lock pthread_mutex_lock
#define mutex_unlock pthread_mutex_unlock
#define mutex_destroy pthread_mutex_destroy

#define semaphore_init sem_init
#define semaphore_signal sem_post
#define semaphore_wait sem_wait
#define semaphore_destroy sem_destroy

#define FILENAME "output.txt"
#include "helper_functions.c"

typedef pthread_mutex_t mutex;
typedef sem_t semaphore;

/* locking mechanisms */
static mutex            readerCountMutex;
static mutex            writerCountMutex;

static semaphore        writerBlock;
static semaphore        readerBlock;
static semaphore        readerSerializer;

/* accounting variables for locking mechanisms */
static int              writerCount;
static int              readerCount;

/* accounting varibles */
static int              shouldContinue;
static int              lineCount;

/* buffer variable */
static FILE*            buffer;

/* initialization code */
void init() {
  randInit();

  /* opens buffer for writing */
  buffer = fopen(FILENAME, "w+");

  /* mutex lock setup */
  mutex_init(&readerCountMutex, NULL);
  mutex_init(&writerCountMutex, NULL);

  /* accounting variables for locking mechanisms setup */
  lineCount = 0;
  readerCount = 0;

  /* accounting integers setup */
  shouldContinue = 1;
  writerCount = 0;

  /* semaphore setup */
  semaphore_init(&readerSerializer, 0, 1);
  semaphore_init(&writerBlock, 0, 1);
  semaphore_init(&readerBlock, 0, 1);
}

/* de initialization code */
void deInit() {
  mutex_destroy(&readerCountMutex); /* kill all the things */
  mutex_destroy(&writerCountMutex);
  semaphore_destroy(&readerSerializer);
  semaphore_destroy(&writerBlock);
  semaphore_destroy(&readerBlock);
  fclose(buffer);
}

/* utility method to signal threads to end */
void signalEnd() {
  shouldContinue = 0;
}

static int writeItem(int item) {

  /* if first writer, then prevent readers from entering queue */
  mutex_lock(&writerCountMutex);
  if(++writerCount == 1) {
    semaphore_wait(&readerBlock);
  }
  mutex_unlock(&writerCountMutex);

  semaphore_wait(&writerBlock); /* mutual exclusion over writing */

  /* write begins here*/
  fseek(buffer, 0, SEEK_END);
  fprintf(buffer, "%d\n", item);
  fflush(buffer);

  printf("Writer: [ %4d ] %11d\n", lineCount + 1, item);

  lineCount++;
  /* write ends here */

  semaphore_signal(&writerBlock);

  mutex_lock(&writerCountMutex); /* if last writer allow readers to enter queue */
  if(--writerCount == 0) {
    semaphore_signal(&readerBlock);
  }
  mutex_unlock(&writerCountMutex);

  return 0;
}

static int readItem() {

  semaphore_wait(&readerSerializer); /* 1 reader at a time from here */
  semaphore_wait(&readerBlock); /* attempt to enter queue */

  mutex_lock(&readerCountMutex); /* if first reader then wait until writers are done then block */
  if(++readerCount == 1) {
    semaphore_wait(&writerBlock);
  }
  mutex_unlock(&readerCountMutex);

  semaphore_signal(&readerBlock); /* if we have reader block, let reader into queue or writers to write */
  semaphore_signal(&readerSerializer); /* let 1 reader into queueing section */

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

  printf("Reader: [ %4d ] %11d\n", lineToRead + 1, val);

  /* read ends here */

  /* if last reader let writers write */
  mutex_lock(&readerCountMutex);
  if(--readerCount == 0) {
    semaphore_signal(&writerBlock);
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
