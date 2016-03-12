#ifndef __RAY_MATH_TOOLKIT_H
#define __RAY_MATH_TOOLKIT_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
//#include <pmmintrin.h>
#include <immintrin.h>
#define MM256_SHUFFLE(x3, x2, x1, x0) \
    ((x3 << 3) | (x2 << 2) | (x1 << 1) | (x0))
static inline
void normalize(double *v)
{
    double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    assert(d != 0.0 && "Error calculating normal");

    v[0] /= d;
    v[1] /= d;
    v[2] /= d;
}

static inline
double length(const double *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static inline
void add_vector(const double *a, const double *b, double *out)
{  
   // / printf("a0:%lf\n",a[0],a[1],a[2] );
        out[0] = a[0] + b[0];
        out[1] = a[1] + b[1];
        out[2] = a[2] + b[2];               
}        
static inline
void subtract_vector(const double *a, const double *b, double *out)
{
        out[0] = a[0] - b[0];
        out[1] = a[1] - b[1];
        out[2] = a[2] - b[2];
}

static inline
void multiply_vectors(const double *a, const double *b, double *out)
{
    //for (int i = 0; i < 3; i++)
  //  __m128d   vsum0 = _mm_add_pd(  _mm_load_pd(&a[0]), _mm_load_pd(&b[0]));
        out[0] = a[0] * b[0];
        out[1] = a[1] * b[1];
        out[2] = a[2] * b[2];
}

static inline
void multiply_vector(const double *a, double b, double *out)
{
 //   for (int i = 0; i < 3; i++)
        out[0] = a[0] * b;
        out[1] = a[1] * b;
        out[2] = a[2] * b;
}

static inline
void cross_product(const double *v1, const double *v2, double *out)
{
    out[0] = v1[1] * v2[2] - v1[2] * v2[1];
    out[1] = v1[2] * v2[0] - v1[0] * v2[2];
    out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static inline
 void testfunction(const double *v1,const double *v2){
     double sum = 0.0;
     double *_256sum;
     double *_256sum2;
    
    __m256d  testvsum = _mm256_setzero_pd();
     
     __m256d vector2 =_mm256_set_pd(v1[0],v1[1],v1[2],9.0);
     __m256d vector3 =_mm256_set_pd(v2[0],v2[1],v2[2],9.0);
      testvsum = _mm256_add_pd(vector2,vector3);//256 
  //__m256d test256sum=_mm256_permute2f128_pd(testvsum, testvsum,1 );
   //test256sum = _mm256_permute_pd(test256sum,5);
     __m256d test256sum = _mm256_shuffle_pd(testvsum, testvsum, MM256_SHUFFLE(0, 1,0, 1));
     testvsum = _mm256_add_pd(testvsum,test256sum);
         test256sum     =_mm256_permute2f128_pd(testvsum, testvsum, 0x01);
           test256sum = _mm256_add_pd(testvsum,test256sum);
_256sum=  ( double*)&testvsum;
_256sum2= ( double*)&test256sum;

printf("v1:%lf   | %lf   | %lf   |%lf \n",v1[0],v1[1],v1[2],0.0);
printf("v2:%lf   | %lf   | %lf   |%lf \n",v2[0],v2[1],v2[2],0.0);
printf("eachadd:%lf   | %lf   | %lf   |%lf \n",_256sum[3],_256sum[2],_256sum[1],_256sum[0]);
 printf("256sum2:%lf   | %lf   | %lf   |%lf \n",_256sum2[3],_256sum2[2],_256sum2[1],_256sum2[0]);


}
static inline
double dot_product(const double *v1, const double *v2)
{
   //const double a[3] = {1.5,2.5,3.5};
   //const double b[3] ={4.5,5.5,6.5};
   __m256d  temp1 =_mm256_mul_pd(_mm256_set_pd(v1[0],v1[1],v1[2],0.0),_mm256_set_pd(v2[0],v2[1],v2[2],0.0));
    __m256d temp2 = _mm256_shuffle_pd(temp1, temp1, MM256_SHUFFLE(0, 1,0, 1));
 temp1 = _mm256_add_pd(temp1,temp2);
    temp2     =_mm256_permute2f128_pd(temp1, temp1, 0x01);
    temp2 = _mm256_add_pd(temp1,temp2);
    double * test;
    test = (double*)&temp2;
    //double dp = 0.0;
   // dp = v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
    //testfunction(a,b);
    //printf("check///////\ntest::%lf\ndp::%lf\n",test[0],dp );
    return test[0];
}

static inline
void scalar_triple_product(const double *u, const double *v, const double *w,
                           double *out)
{
    cross_product(v, w, out);
    multiply_vectors(u, out, out);
}

static inline
double scalar_triple(const double *u, const double *v, const double *w)
{
    double tmp[3];
    cross_product(w, u, tmp);
    return dot_product(v, tmp);
}

#endif
