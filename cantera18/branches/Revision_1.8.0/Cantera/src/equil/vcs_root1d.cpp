/**
 * @file vcs_root1d.cpp
 *  Code for a one dimensional root finder program.
 */
/*
 *  $Id$
 */
/*
 * Copywrite (2006) Sandia Corporation. Under the terms of
 * Contract DE-AC04-94AL85000 with Sandia Corporation, the
 * U.S. Government retains certain rights in this software.
 */


#include "vcs_internal.h" 

#include <cstdio>
#include <cstdlib>
#include <cmath>

namespace VCSnonideal {

#define TOL_CONV 1.0E-5
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#ifdef DEBUG_MODE
static void print_funcEval(FILE *fp, double xval, double fval, int its)  
{
   fprintf(fp,"\n");
   fprintf(fp,"...............................................................\n");
   fprintf(fp,".................. vcs_root1d Function Evaluation .............\n");
   fprintf(fp,"..................  iteration = %5d ........................\n", its);
   fprintf(fp,"..................  value = %12.5g ......................\n", xval);
   fprintf(fp,"..................  funct = %12.5g ......................\n", fval);
   fprintf(fp,"...............................................................\n");  
   fprintf(fp,"\n");
}
#endif
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

  // One Dimensional Root Finder 
  /*
   *
   * vcs_root1d:
   *
   *
   *
   * Following is a nontrial example for vcs_root1d() where the buoyancy of a 
   * cylinder floating on water is calculated.
   *
   * @verbatim
   *   #include <cmath>
   *   #include <cstdlib>
   *
   *   #include "Cantera.h"
   *   #include "kernel/vcs_internal.h"
   *
   *   const double g_cgs = 980.;
   *   const double mass_cyl = 0.066;
   *   const double diam_cyl = 0.048;
   *   const double rad_cyl = diam_cyl / 2.0;
   *   const double len_cyl  = 5.46;
   *   const double vol_cyl  = Pi * diam_cyl * diam_cyl / 4 * len_cyl;
   *   const double rho_cyl = mass_cyl / vol_cyl;
   *   const double rho_gas = 0.0;
   *   const double rho_liq = 1.0;
   *   const double sigma = 72.88;
   *   // Contact angle in radians
   *   const double alpha1 = 40.0 / 180. * Pi;
   *  
   *   using namespace Cantera;
   *   using namespace VCSnonideal;
   *
   *   double func_vert(double theta1, double h_2, double rho_c) {
   *     double f_grav = - Pi * rad_cyl * rad_cyl * rho_c * g_cgs;
   *     double tmp = rad_cyl * rad_cyl * g_cgs;
   *     double tmp1 = theta1 + sin(theta1) * cos(theta1) - 2.0 * h_2 / rad_cyl * sin(theta1);
   *     double f_buoy = tmp * (Pi * rho_gas + (rho_liq - rho_gas) * tmp1);
   *     double f_sten = 2 * sigma * sin(theta1 + alpha1 - Pi);
   *     double f_net =  f_grav +  f_buoy +  f_sten;
   *     return f_net;
   *   }
   *   double calc_h2_farfield(double theta1) {
   *       double rhs = sigma * (1.0 + cos(alpha1 + theta1));
   *       rhs *= 2.0;
   *       rhs = rhs / (rho_liq - rho_gas) / g_cgs;
   *       double sign = -1.0;
   *       if (alpha1 + theta1 < Pi) sign = 1.0;
   *       double res = sign * sqrt(rhs);
   *       double h2 = res + rad_cyl * cos(theta1);
   *       return h2;
   *   }
   *   double funcZero(double xval, double Vtarget, int varID, void *fptrPassthrough, int *err) {
   *      double theta = xval;
   *      double h2 = calc_h2_farfield(theta);
   *      double fv = func_vert(theta, h2, rho_cyl);
   *      return fv;
   *   }
   *
   *  int main () {
   *
   *     double thetamax = Pi;
   *     double thetamin = 0.0;
   *     int maxit = 1000;
   *     int iconv;
   *     double thetaR = Pi/2.0;
   *     int printLvl = 4;
   *
   *     iconv =  VCSnonideal::vcsUtil_root1d(thetamin, thetamax, maxit, funcZero,
   *                                          (void *) 0, 0.0, 0, &thetaR, printLvl);
   *     printf("theta = %g\n", thetaR);
   *     double h2Final = calc_h2_farfield(thetaR);
   *     printf("h2Final = %g\n", h2Final);
   *     return 0;
   *  }
   * @endverbatim
   *
   */
  int vcsUtil_root1d(double xmin, double xmax, int itmax,
		     VCS_FUNC_PTR func, void *fptrPassthrough,
		     double FuncTargVal, int varID,
		     double *xbest, int printLvl) {
   static int callNum = 0;
   const char *stre = "vcs_root1d ERROR: ";
   const char *strw = "vcs_root1d WARNING: ";
   int converged = FALSE, err = FALSE;
#ifdef DEBUG_MODE
   char fileName[80];
   FILE *fp = 0;
#endif
   double x1, x2, xnew, f1, f2, fnew, slope;
   int its = 0;
   int posStraddle = 0;
   int retn = VCS_SUCCESS;
   int foundPosF = FALSE;
   int foundNegF = FALSE;
   int foundStraddle = FALSE;
   double xPosF = 0.0;
   double xNegF = 0.0;
   double fnorm;   /* A valid norm for the making the function value
		    * dimensionless */
   double c[9], f[3], xn1, xn2, x0 = 0.0, f0 = 0.0, root, theta, xquad;

   callNum++;
#ifdef DEBUG_MODE
   if (printLvl >= 3) {
     sprintf(fileName, "rootfd_%d.log", callNum);
     fp = fopen(fileName, "w");
     fprintf(fp, " Iter   TP_its  xval   Func_val  |  Reasoning\n");
     fprintf(fp, "-----------------------------------------------------"
	     "-------------------------------\n");
   }
#else
   if (printLvl >= 3) {
     plogf("WARNING: vcsUtil_root1d: printlvl >= 3, but debug mode not turned on\n");
   }
#endif
   if (xmax <= xmin) {
     plogf("%sxmin and xmax are bad: %g %g\n", stre, xmin, xmax);
     return VCS_PUB_BAD;
   }
   x1 = *xbest;
   if (x1 < xmin || x1 > xmax) {
    x1 = (xmin + xmax) / 2.0;     
   }
   f1 = func(x1, FuncTargVal, varID, fptrPassthrough, &err);
#ifdef DEBUG_MODE
   if (printLvl >= 3) {
     print_funcEval(fp, x1, f1, its); 
     fprintf(fp, "%-5d  %-5d  %-15.5E %-15.5E\n", -2, 0, x1, f1);
   }
#endif
   if (f1 == 0.0) {
      *xbest = x1;
      return VCS_SUCCESS; 
   }
   else if (f1 > 0.0) {
       foundPosF = TRUE;
       xPosF = x1;
   } else {
       foundNegF = TRUE;
       xNegF = x1;
   }
   x2 = x1 * 1.1;
   if (x2 > xmax)    x2 = x1 - (xmax - xmin) / 100.;
   f2 = func(x2, FuncTargVal, varID, fptrPassthrough, &err);
#ifdef DEBUG_MODE
   if (printLvl >= 3) {
     print_funcEval(fp, x2, f2, its);
     fprintf(fp, "%-5d  %-5d  %-15.5E %-15.5E", -1, 0, x2, f2);
   }
#endif
 
   if (FuncTargVal != 0.0) {
      fnorm = fabs(FuncTargVal) + 1.0E-13;
   } else {
      fnorm = 0.5*(fabs(f1) + fabs(f2)) + fabs(FuncTargVal);
   }

   if (f2 == 0.0)
      return retn;
   else if (f2 > 0.0) {
      if (!foundPosF) {
	 foundPosF = TRUE;
	 xPosF = x2;
      }
   } else {
      if (!foundNegF) {
	 foundNegF = TRUE;
	 xNegF = x2;
      }
   }
   foundStraddle = foundPosF && foundNegF;
   if (foundStraddle) {
      if (xPosF > xNegF) posStraddle = TRUE;
      else               posStraddle = FALSE;
   }
   
   do {
      /*
      *    Find an estimate of the next point to try based on
      *    a linear approximation.   
      */
      slope = (f2 - f1) / (x2 - x1);
      if (slope == 0.0) {
         plogf("%s functions evals produced the same result, %g, at %g and %g\n",
		strw, f2, x1, x2);
	 xnew = 2*x2 - x1 + 1.0E-3;
      } else {
	 xnew = x2 - f2 / slope; 
      } 
#ifdef DEBUG_MODE
      if (printLvl >= 3) {
	fprintf(fp, " | xlin = %-9.4g", xnew);
      }
#endif

      /*
      *  Do a quadratic fit -> Note this algorithm seems
      *  to work OK. The quadratic approximation doesn't kick in until
      *  the end of the run, when it becomes reliable.
      */
      if (its > 0) {
	 c[0] = 1.; c[1] = 1.; c[2] = 1.;
	 c[3] = x0; c[4] = x1; c[5] = x2;
	 c[6] = SQUARE(x0); c[7] = SQUARE(x1); c[8] = SQUARE(x2);
	 f[0] = - f0; f[1] = - f1; f[2] = - f2;
	 retn = vcsUtil_mlequ(c, 3, 3, f, 1);
	 if (retn == 1) goto QUAD_BAIL;
	 root = f[1]* f[1] - 4.0 * f[0] * f[2];
	 if (root >= 0.0) {
	    xn1 = (- f[1] + sqrt(root)) / (2.0 * f[2]);
	    xn2 = (- f[1] - sqrt(root)) / (2.0 * f[2]);	
	    if (fabs(xn2 - x2) < fabs(xn1 - x2) && xn2 > 0.0 ) xquad = xn2;
	    else                                               xquad = xn1;
	    theta = fabs(xquad - xnew) / fabs(xnew - x2);
	    theta = MIN(1.0, theta);
	    xnew = theta * xnew + (1.0 - theta) * xquad;
#ifdef DEBUG_MODE
	    if (printLvl >= 3) {
	      if (theta != 1.0) {
		fprintf(fp, " | xquad = %-9.4g", xnew);
	      }
	    }
#endif
	 } else {
	    /*
	    *   Pick out situations where the convergence may be
	    *   accelerated.
	    */
	    if ((DSIGN(xnew - x2) == DSIGN(x2 - x1)) &&
		(DSIGN(x2   - x1) == DSIGN(x1 - x0))    ) {
	       xnew += xnew - x2;
#ifdef DEBUG_MODE
	       if (printLvl >= 3) {
		 fprintf(fp, " | xquada = %-9.4g", xnew);
	       }
#endif
	    }
	 }
      }
      QUAD_BAIL: ;
      
      
      /*
      *
      *  Put heuristic bounds on the step jump
      */
      if ( (xnew > x1 && xnew < x2) || (xnew < x1 && xnew > x2)) {
	 /*
	 *
	 *   If we are doing a jump inbetween two points, make sure
	 *   the new trial is between 10% and 90% of the distance
	 *   between the old points.
	 */
	 slope = fabs(x2 - x1) / 10.;
	 if (fabs(xnew - x1) < slope) {
	    xnew = x1 + DSIGN(xnew-x1) * slope;
#ifdef DEBUG_MODE
	    if (printLvl >= 3) {
	      fprintf(fp, " | x10%% = %-9.4g", xnew);
	    }
#endif
	 }
	 if (fabs(xnew - x2) < slope) {
	    xnew = x2 + DSIGN(xnew-x2) * slope; 
#ifdef DEBUG_MODE
	    if (printLvl >= 3) {
	      fprintf(fp, " | x10%% = %-9.4g", xnew);
	    }
#endif
	 }
      } else {
	 /*
	 *   If we are venturing into new ground, only allow the step jump
	 *   to increase by 100% at each interation
	 */
	 slope = 2.0 * fabs(x2 - x1);
	 if (fabs(slope) < fabs(xnew - x2)) {
	    xnew = x2 + DSIGN(xnew-x2) * slope;
#ifdef DEBUG_MODE
	    if (printLvl >= 3) {
	      fprintf(fp, " | xlimitsize = %-9.4g", xnew);
	    }
#endif
	 }
      }
      
      
      if (xnew > xmax) {
        xnew = x2 + (xmax - x2) / 2.0;
#ifdef DEBUG_MODE
	if (printLvl >= 3) {
	  fprintf(fp, " | xlimitmax = %-9.4g", xnew);
	}
#endif
      }
      if (xnew < xmin) {
	 xnew = x2 + (x2 - xmin) / 2.0;
#ifdef DEBUG_MODE
	 if (printLvl >= 3) {
	   fprintf(fp, " | xlimitmin = %-9.4g", xnew);
	 }
#endif
      }
      if (foundStraddle) {
#ifdef DEBUG_MODE
         slope = xnew;	 
#endif
	 if (posStraddle) {
	   if (f2 > 0.0) {
	     if (xnew > x2)    xnew = (xNegF + x2)/2;
	     if (xnew < xNegF) xnew = (xNegF + x2)/2;
	   } else {
	     if (xnew < x2)    xnew = (xPosF + x2)/2;
	     if (xnew > xPosF) xnew = (xPosF + x2)/2;
	   }
	 } else {
	   if (f2 > 0.0) {
	     if (xnew < x2)    xnew = (xNegF + x2)/2;
	     if (xnew > xNegF) xnew = (xNegF + x2)/2;
	   } else {
	     if (xnew > x2)    xnew = (xPosF + x2)/2;
	     if (xnew < xPosF) xnew = (xPosF + x2)/2;
	   }
	 }
#ifdef DEBUG_MODE
	 if (printLvl >= 3) {
	   if (slope != xnew) {
	     fprintf(fp, " | xstraddle = %-9.4g", xnew);	    
	   }
	 }
#endif	
      }
      
      fnew = func(xnew, FuncTargVal, varID, fptrPassthrough, &err);
#ifdef DEBUG_MODE
      if (printLvl >= 3) {
	fprintf(fp,"\n");
	print_funcEval(fp, xnew, fnew, its);
	fprintf(fp, "%-5d  %-5d  %-15.5E %-15.5E", its, 0, xnew, fnew);
      }
#endif
      
      if (foundStraddle) {
	 if (posStraddle) {
	    if (fnew > 0.0) {
	       if (xnew < xPosF) xPosF = xnew;
	    } else {
	       if (xnew > xNegF) xNegF = xnew;	       
	    }
	 } else {
	    if (fnew > 0.0) {
	       if (xnew > xPosF) xPosF = xnew;	       
	    } else {
	       if (xnew < xNegF) xNegF = xnew;	  	       
	   }	   
	}
      }

      if (! foundStraddle) {
	 if (fnew > 0.0) {
	    if (!foundPosF) {
	       foundPosF = TRUE;
	       xPosF = xnew;
	       foundStraddle = TRUE;
	       if (xPosF > xNegF) posStraddle = TRUE;
	       else    	          posStraddle = FALSE;
	    }	    
	 } else {
	    if (!foundNegF) {
	       foundNegF = TRUE;
	       xNegF = xnew;
	       foundStraddle = TRUE;
	       if (xPosF > xNegF) posStraddle = TRUE;
	       else    	          posStraddle = FALSE;
	    }	   
	}
      }
      
      x0 = x1;
      f0 = f1;
      x1 = x2;
      f1 = f2;
      x2 = xnew; 
      f2 = fnew;
      if (fabs(fnew / fnorm) < 1.0E-5) {
        converged = TRUE;	 
      }
      its++;
   } while (! converged && its < itmax);
   if (converged) {
     if (printLvl >= 1) {
       plogf("vcs_root1d success: convergence achieved\n");
     }
#ifdef DEBUG_MODE
     if (printLvl >= 3) {
       fprintf(fp, " | vcs_root1d success in %d its, fnorm = %g\n", its, fnorm);
     }
#endif  
   } else {
      retn = VCS_FAILED_CONVERGENCE;
      if (printLvl >= 1) {
	plogf("vcs_root1d ERROR: maximum iterations exceeded without convergence\n");
      }
#ifdef DEBUG_MODE
      if (printLvl >= 3) {
	fprintf(fp, "\nvcs_root1d failure in %d its\n", its);
      }
#endif
   }
   *xbest = x2;
#ifdef DEBUG_MODE
   if (printLvl >= 3) {
     fclose(fp);
   }
#endif
   return retn;
}
/*****************************************************************************/
}

