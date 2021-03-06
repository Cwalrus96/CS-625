#include "ParSet.h"


void initializeParSet(ParSet * p, int id) {
  p->c = randomizeParameter(30611);
  p->d = randomizeParameter(113.064);
  p->h = randomizeParameter(0.5289);
  p->beta = randomizeParameter(0.0426);
  p->n = randomizeParameter(0.5916);
  p->lambda2 = randomizeParameter(1.3484);
  p->b = randomizeParameter(80.5297);
  p->r = randomizeParameter(2.85);
  p->s = randomizeParameter(3.25);
  p->lambda1 = randomizeParameter(2.9699);
  p->a = randomizeParameter(1017.1);
  p->id = id;
  p->error = -1.0;
  p->mutate = 0.0;
  p-> selected = 0.0;
  if(p->s > p->r) //s should be less than r - swap
  {
        float temp = p->s;
        p->s = p->r;
        p->r = temp;
  }
}

float randomizeParameter(float p) {
  float seed = rand();
  float sign = (((int)seed) % 2 == 0) ? -1.0 : 1.0;
  seed = seed / RAND_MAX;
  float change = seed * 0.25 * sign;
  change = p * change;
  return (p + change);
}
