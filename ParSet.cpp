#include "ParSet.h" 
#include <cstdlib>

ParSet::ParSet() {
  c = RandomizeParameters(30611); 
  d = RandomizeParameters(113.064); 
  h = RandomizeParameters(0.5289); 
  beta = RandomizeParameters(0.0426); 
  n = RandomizeParameters(0.5916); 
  lambda2 = RandomizeParameters(1.3484); 
  b = RandomizeParameters(80.5297); 
  r = RandomizeParameters(2.85); 
  s = RandomizeParameters(3.25); 
  lambda1 = RandomizeParameters(2.9699); 
  a = RandomizeParameters(1017.1); 
}

ParSet::RandomizeParameters(float p) {
  float seed = rand(); 
  float sign = (seed % 2 == 0) ? -1.0 : 1.0; 
  seed = seed / RAND_MAX; 
  float change = seed * 0.25 * sign; 
  return (p + change); 
}
