#ifndef RAND_C
#define RAND_C

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

#endif /* RAND_C */
