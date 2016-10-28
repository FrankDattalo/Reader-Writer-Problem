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
#include "rand.c"
#include "atomic_integer.c"

typedef pthread_mutex_t mutex;
typedef sem_t semaphore;

/* locking mechanisms */
static mutex            writerMutex;
static semaphore        readerEmptySempahore;
static semaphore        writerEmptySemaphore;

/* implementation of atom integer variables */
static atomic_integer   lineCount;
static atomic_integer   readerCount;
static atomic_integer   writerCount;
static atomic_integer   shouldContinue;

/* buffer variable */
static FILE*            buffer;

/* initialization code */
void init() {
  randInit();

  /* opens buffer for writing */
  buffer = fopen(FILENAME, "w+");

  /* mutex lock setup */
  mutex_init(&writerMutex, NULL);

  /* atomic integers setup */
  atomic_init_and_set(&shouldContinue, 1);
  atomic_init(&lineCount);
  atomic_init(&readerCount);
  atomic_init(&writerCount);

  /* semaphore setup */
  semaphore_init(&readerEmptySempahore, 0, 1);
  semaphore_init(&writerEmptySemaphore, 0, 1);
}

/* de initialization code */
void deInit() {
  fclose(buffer);
}

/* utility method to signal threads to end */
void signalEnd() {
  atomic_set(&shouldContinue, 0);
}

static int writeItem(int item) {

  atomic_increment(&writerCount); /* writerCount++ */

  if (atomic_get(&readerCount) > 0) { /* if readerCount > 0 */
    sempahore_wait(&readerEmptySempahore); /* wait until no readers */
  }

  mutex_lock(&writerMutex); /* get write lock */

  /* write begins here*/
  fseek(buffer, 0, SEEK_END);
  fprintf(buffer, "%d\n", item);
  fflush(buffer);

  printf("Writer: [ %2d ] %11d\n", atomic_increment_and_get(&lineCount), item);
  /* write ends here */

  mutex_unlock(&writerMutex); /* release write lock */

  if (atomic_decrement_and_get(&writerCount) == 0) { /* if --writerCount == 0 */
    semaphore_signal(&writerEmptySemaphore); /* signal readers that there are no writers */
  }

  return 0;
}

static int readItem() {

  atomic_increment(&readerCount); /* readerCount++ */

  if(atomic_get(&writerCount) > 0) { /* if writerCount > 0 */
    sempahore_wait(&writerEmptySemaphore); /* wait until there are no writers */
  }

  /* read begins here */
  FILE* localBuffer = fopen(FILENAME, "r+");
  rewind(localBuffer);

  int lineToRead = nextRand() % atomic_get(&lineCount);
  int currentLine = 0;
  int val;

  /* walk the file until you reach the proper line */
  while(currentLine < lineToRead) {
    fscanf(localBuffer, "%d\n", &val);
    currentLine++;
  }

  fscanf(localBuffer, "%d\n", &val);

  fclose(localBuffer);

  lineToRead++;

  printf("Reader: [ %2d ] %11d\n", lineToRead, val);
  /* read ends here */

  if(atomic_decrement_and_get(&readerCount) == 0) { /* if --readerCount == 0 */
    semaphore_signal(&readerEmptySempahore); /* signal writers that there are no readers */
  }

  return 0;
}

/* writer thread */
void* writer(void* params) {

  /* local variable */
  int sleepTime = nextRand() % 10 + 1;

  printf("Creating a writer that sleeps for %d seconds\n", sleepTime);
  sleep(1);

  while(atomic_get(&shouldContinue)) { /* while should continue is true */

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

  while(atomic_get(&shouldContinue)) { /* while should continue is true */

    /* read from buffer */
    if(atomic_get(&lineCount) > 0) { /* if linecount > 0 */

      /* read item, and assert that the return was not an error */
      assertTrue(!readItem(),
        "An error occurred while trying to read a number");
    }

    sleep(sleepTime); /* sleep for random amount */
  }
}

#endif /* READER_WRITER_C */
