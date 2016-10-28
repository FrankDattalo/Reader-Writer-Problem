#ifndef MAIN_C
#define MAIN_C

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "helper_functions.c"
#include "reader_writer.c"

int main(int argc, char** argv) {

  int               sleepTime; /* the amount of time the main thread sleeps before ending the program */
  int               writerThreadCount; /* the count of writer threads */
  int               readerThreadCount; /* the count of reader threads */
  pthread_t*        writerThreadArray; /* the array of writer threads */
  pthread_t*        readerThreadArray; /* the array of reader threads */
  pthread_attr_t*   writerThreadAttrArray; /* the array of writer thread attributes */
  pthread_attr_t*   readerThreadAttrArray; /* the array of reader thread attributes */

  /* negative checking against argument count*/
  assertTrue(argc > 3,
    "Invalid use of program! <number of second to sleep> <number of writer threads> <number of reader threads>");

  /* parse input arguments */
  sleepTime         = safeConvert(argv[1], "Invalid sleep formatting");
  writerThreadCount = safeConvert(argv[2], "Invalid writer thread count formatting");
  readerThreadCount = safeConvert(argv[3], "Invalid reader thread count formatting");

  /* negative checking against invalid arg values */
  assertTrue(sleepTime > 0,         "Sleep time must be a number greater than zero");
  assertTrue(writerThreadCount > 0, "Writer thread count must be greater than zero");
  assertTrue(readerThreadCount > 0, "Reader thread count must be greater than zero");

  /* initialize threads here */
  writerThreadArray     = malloc(writerThreadCount * sizeof(pthread_t));
  readerThreadArray     = malloc(readerThreadCount * sizeof(pthread_t));
  writerThreadAttrArray = malloc(writerThreadCount * sizeof(pthread_attr_t));
  readerThreadAttrArray = malloc(readerThreadCount * sizeof(pthread_attr_t));

  /* more negative checking against failed malloc */
  assertTrue(!!writerThreadArray,      "Could not allocate memory for writer threads");
  assertTrue(!!readerThreadArray,      "Could not allocate memory for reader threads");
  assertTrue(!!writerThreadAttrArray,  "Could not allocate memory for writer thread attributes");
  assertTrue(!!readerThreadAttrArray,  "Could not allocate memory for reader thread attributes");

  printf("Running the program for at least %d seconds...\n", sleepTime);

  /* initialization code for reader writer program */
  init();

  /* start up threads */
  {
    int i;
    for(i = 0; i < writerThreadCount; i++) {
      pthread_attr_init(&writerThreadAttrArray[i]);

      pthread_create(&writerThreadArray[i], &writerThreadAttrArray[i],
        writer, NULL);
    }
  }

  {
    int i;
    for(i = 0; i < readerThreadCount; i++) {
      pthread_attr_init(&readerThreadAttrArray[i]);

      pthread_create(&readerThreadArray[i], &readerThreadAttrArray[i],
        reader, NULL);
    }
  }

  /* sleep for some time */
  sleep(sleepTime);

  printf("Signaling readers and writers to down...\n");

  /* signal threads to end */
  signalEnd();

  /* wait for threads to end */
  {
    int i;
    for(i = 0; i < writerThreadCount; i++) {
      pthread_join(writerThreadArray[i], NULL);
    }
  }

  {
    int i;
    for(i = 0; i < readerThreadCount; i++) {
      pthread_join(readerThreadArray[i], NULL);
    }
  }

  /* de initialize code for reader writer program */
  deInit();

  /* free allocated memory */
  free(writerThreadArray);
  free(readerThreadArray);
  free(writerThreadAttrArray);
  free(readerThreadAttrArray);

  printf("Ending the program...\n");

  return 0;
}

#endif /* MAIN_C */
