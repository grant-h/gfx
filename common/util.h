#ifndef UTIL_H
#define UTIL_H

// C doesnt have a modulus operator, just a remainder operator
static inline int mod(int n, int m)
{
	int r = n % m;
	return r < 0 ? r + m : r;
}

// Random floating point number in the range of [a,b]
static inline float randrange(float a, float b)
{
   if(a > b)
   {
      float tmp = a;
      a = b;
      b = tmp;
   }

   float rng = (float)rand()/RAND_MAX;
   
   return (b-a)*rng + a;
}

#endif
