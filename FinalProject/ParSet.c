#include "ParSet.h"


initializeParSet(*ParSet p, int id) {
  p->c = RandomizeParameter(30611); 
  p->d = RandomizeParameter(113.064); 
  p->h = RandomizeParameter(0.5289); 
  p->beta = RandomizeParameter(0.0426); 
  p->n = RandomizeParameter(0.5916); 
  p->lambda2 = RandomizeParameter(1.3484); 
  p->b = RandomizeParameter(80.5297); 
  p->r = RandomizeParameter(2.85); 
  p->s = RandomizeParameter(3.25); 
  p->lambda1 = RandomizeParameter(2.9699); 
  p->a = RandomizeParameter(1017.1); 
  p->id = id; 
  p->error = -1.0; 
}

randomizeParameter(float p) {
  float seed = rand(); 
  float sign = (seed % 2 == 0) ? -1.0 : 1.0; 
  seed = seed / RAND_MAX; 
  float change = seed * 0.25 * sign; 
  return (p + change); 
}
