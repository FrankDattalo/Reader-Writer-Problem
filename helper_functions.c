#ifndef HELPER_FUNCTIONS_C
#define HELPER_FUNCTIONS_C

/* seed for random number generator */
static unsigned int     randSeed;

/* initialization code for random number generator */
void randInit() {
  randSeed = (unsigned int) time(NULL);
}

/* random number generator function */
int nextRand() {
  return rand_r(&randSeed);
}

/* simple assert function that aborts execution on a false value */
void assertTrue(int value, char* errorString) {
  if(!value) {
    fprintf(stderr, "%s\n", errorString);
    exit(-1);
  }
}

/* safely converts a string to an integer, and
  aborts if the string cannot be converted */
int safeConvert(char* str, char* errorString) {
  if(str[0] == '0' && str[1] == '\0') {
    return 0;
  }

  int num = atoi(str);

  assertTrue(num, errorString);

  return num;
}

#endif /* HELPER_FUNCTIONS_C */
