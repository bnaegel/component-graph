#include "utils.h"
#include <cmath>
#include <iostream>
/*
 **  CUBIC.C - Solve a cubic polynomial
 **  public domain by Ross Cottrell
 */

using namespace std;
#define PI (4*atan(1.0))

void SolveCubic(double  a,
                double  b,
                double  c,
                double  d,
                int    *solutions,
                double *x)
{
    long double    a1 = b/a, a2 = c/a, a3 = d/a;
    long double    Q = (a1*a1 - 3.0*a2)/9.0;
    long double R = (2.0*a1*a1*a1 - 9.0*a1*a2 + 27.0*a3)/54.0;
    double    R2_Q3 = R*R - Q*Q*Q;

    double    theta;
    double epsilon = 1e-7;
    if (fabs(Q) < epsilon) //Q = 0
    {
	/* critical case where the inflexion point has zero slope */
	/* simplified solution */
	if (R2_Q3 <= 0.0){
		*solutions = 3;
		x[0] = x[1] = x[2] = - a1/3.0;
	}
	else{
		*solutions = 1;
        x[0] = std::pow((double)(sqrt(R2_Q3)+fabs(R)), (double) (1/3.0) );
		x[0] += Q/x[0];
		x[0] *= (R < 0.0) ? 1 : -1;
		x[0] -= a1/3.0;
	}
    }
    else
    {
        if (R2_Q3 <= 0.0)
        {
            *solutions = 3;
            double Q3 = Q*Q*Q;
            if (Q3<=0)
            {
                std::cerr<<"Q3<=0"<<std::endl;
            }
            double ratio = R/sqrt(Q3);
            //std::cout<<"ratio= "<<ratio<<std::endl;
            if ((ratio>1) || (ratio<-1))
                std::cerr<<"ratio not an acos"<<std::endl;
            theta = acos(ratio);
            //std::cout<<"Q="<<Q<<" theta="<<theta<<std::endl;

            x[0] = -2.0*sqrt(Q)*cos(theta/3.0) - a1/3.0;
            x[1] = -2.0*sqrt(Q)*cos((theta+2.0*PI)/3.0) - a1/3.0;
            x[2] = -2.0*sqrt(Q)*cos((theta+4.0*PI)/3.0) - a1/3.0;
        }
        else
        {
            *solutions = 1;
            x[0] = pow((double)(sqrt(R2_Q3)+fabs(R)), (double)(1/3.0));
            x[0] += Q/x[0];
            x[0] *= (R < 0.0) ? 1 : -1;
            x[0] -= a1/3.0;
        }
    }
}
