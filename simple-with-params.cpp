#include <stdio.h>
#include <math.h>
#include <stdio.h> // for file output
#include <stdlib.h>  /* For exit() function */

#include <gsl/gsl_vector.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_blas.h> // to compute the norm
#include <gsl/gsl_multimin.h> // gsl multidimensional minimization


#include <iostream>

using namespace std;

#include <adolc/adolc.h>             /* use of ALL ADOL-C interfaces */
#include <adolc/adouble.h>             /* use of ALL ADOL-C interfaces */
#include <adolc/drivers/drivers.h>    // use of "Easy to Use" drivers,
                                      // grants the gradient(.)
#include <adolc/taping.h>             // use of taping
#include <cstdlib>


// in this file we keep the x fixed
// and then we make a values ai random


// To use this file we need to first create the tape and
// then call the functions my_anything.
// This is because all the my_anything functions
// use the tape.

// adolc related stuff
int tape_id = 1; // to id the tape in adolc
size_t tape_stats[STAT_SIZE];





// The number of neurons on each network
const int number_of_neurons = 10;

const int number_of_xi = 10;

const int number_of_ai = 2;

double min_a = 100;
double max_a = -1;

double x_min = 0.0;
double x_max = 3.0;

double a_min = 0.5;
double a_max = 10;

int max_number_of_iterations =  500000; // 2000000;

int print_after_this_number_of_iterations =  5000;

int adam_write_data_after_these_iterations =  50000;

double learning_rate = 0.001;

double grad_tolerance = 1e-5;

// configuration parameters for adam

double adam_stepsize = 0.001;
double beta1   = 0.9;
double beta2   = 0.999;
double epsilon = 1e-8;


// the initial values
double Y0 = 1;
double X0 = 0;

// the right hand side of the EDO
adouble f (double x, adouble y, double a) {
  return a*(sin(x)-y);
}



// the number of weights each network has
// this is 3 + number of parameters
const int number_of_parameters = 1;

const int weights_per_neuron = 3 + number_of_parameters;

// the number of functions in the system
const int number_of_functions = 1;

// the total number of variables
// in all the networks
const int number_of_variables =
  weights_per_neuron*
  number_of_functions*
  number_of_neurons;




// // points xi
double xi[number_of_xi];
double ai[number_of_ai];

// vectors for adam
double m[number_of_variables];
double v[number_of_variables];

// a function to write the results to a file
void write_results_to_file(double * p) {
   
  // let's write the output to a file
  FILE * f = fopen ("weights_data.dat", "w");
  // the first line is the number of variables
  fprintf (f, "%d\n", number_of_variables); 
  // the second line is the number of neurons
  fprintf (f, "%d\n", number_of_neurons); 
  // next, x_min
  fprintf (f, "%16.16e\n", x_min); 
  // and then, x_max
  fprintf (f, "%16.16e\n", x_max);
  // next, a_min
  fprintf (f, "%16.16e\n", a_min); 
  // and then, a_max
  fprintf (f, "%16.16e\n", a_max);
  // x0 
  fprintf (f, "%16.16e\n", X0);
  // y0 
  fprintf (f, "%16.16e\n", Y0);
  // finally, the weights
  for (int i = 0; i < number_of_variables; i++){
    fprintf (f, "%16.16e\n", p[i]);
  }
  fclose (f);
  // end of writing to file
}

// a function to write the gsl results to a file
void write_gsl_results_to_file(gsl_vector * p) {

  // let's write the output to a file
  FILE * f = fopen ("weights_data.dat", "w");
  // the first line is the number of variables
  fprintf (f, "%d\n", number_of_variables); 
  // the second line is the number of neurons
  fprintf (f, "%d\n", number_of_neurons); 
  // next, x_min
  fprintf (f, "%16.16e\n", x_min); 
  // and then, x_max
  fprintf (f, "%16.16e\n", x_max);
  // next, a_min
  fprintf (f, "%16.16e\n", a_min); 
  // and then, a_max
  fprintf (f, "%16.16e\n", a_max);
  // x0 
  fprintf (f, "%16.16e\n", X0);
  // y0 
  fprintf (f, "%16.16e\n", Y0);
  // finally, the weights
  for (int i = 0; i < number_of_variables; i++){
    fprintf (f, "%16.16e\n", gsl_vector_get(p, i));
  }
  fclose (f);
  // end of writing to file
}

// the random generation functions
double frand() {
  // returns a random number between 0 and 1
  return (1.0d*rand())/(1.0d*RAND_MAX);
}

double random(double min, double max) {
  // returns a random number between min and max
  return min + frand() * (max - min);
}



// the sigmoid function
adouble SGM (adouble t) {
  return 1/(1+exp(-t));
}

// the derivative of the sigmoid function
adouble dSGMdx (adouble t) {
  adouble S = SGM(t);
  return S*(1-S);
}



// this is the neural network
// with one parameters
adouble N(double x, adouble p[], double a) {
  adouble r = 0;
  for (int i = 0; i < number_of_neurons; i++) {
    r = r + p[i+2*number_of_neurons]*SGM(p[i]*x + 
                                         p[i+number_of_neurons] +
                                         a*p[i+3*number_of_neurons]);
  }
  return r;
}


// // this is the derivative of the neural network
// // with respect to the variable x
adouble dNdx(double x, adouble p[], double a) { 
  adouble r = 0;
  for (int i = 0; i < number_of_neurons; i++) {
    r = r + p[i+2*number_of_neurons]*p[i]*dSGMdx(p[i]*x + 
                                                 p[i+number_of_neurons] +
                                                 a*p[i+3*number_of_neurons]);
  }
  return r;
}



// here we create the trial solutions
adouble TS(double x, adouble p[], double a) {
  return Y0 + (x - X0)*N(x,p,a);
}


// and here we write the derivative of the
// trial solutions with respect to x
adouble dTSdx (double x, adouble p[], double a) {
  return N(x,p,a) + (x - X0)*dNdx(x,p,a);
}


// And this is the error function we want
// to minimize
adouble E(adouble p[]) {

  adouble er = 0;

  adouble e1;

  // let's iterate through the x_i
  for (int i = 0; i < number_of_xi; i++) {

    for (int j = 0; j < number_of_ai; j++) {

      // first, the error corresponding to f1
      e1 = (dTSdx(xi[i], p, ai[j]) - // first the derivative
            // then the right hand side
            f(xi[i], TS(xi[i], p, ai[j]), ai[j]));

      er = er + e1*e1;

    }
  }
  return er;
}


// And this is the error function we want
// to minimize with random xi :-o
adouble noisy_E(adouble p[]) {

  adouble er = 0;

  adouble e1;

  // let's generate the points ai
  double ai[number_of_ai];

  // create the random ai
  for (int i = 0; i < number_of_ai; i++) {
    ai[i] = a_min + frand() * (a_max - a_min);
    if (ai[i] < min_a) {
      min_a = ai[i];
    }
    if (ai[i] > max_a) {
      max_a = ai[i];
    }

  }
  
  // let's iterate through the x_i
  for (int i = 0; i < number_of_xi; i++) {

    for (int j = 0; j < number_of_ai; j++) {
      
      // first, the error corresponding to f1
      e1 = (dTSdx(xi[i], p, ai[j]) - // first the derivative
            // then the right hand side
            f(xi[i], TS(xi[i], p, ai[j]), ai[j]));

      er = er + e1*e1;

    }
    
  }

  return er;
}



double eval_E_and_create_tape_no_gsl (double *v) {
  // the first thing is to create the tape
  // evaluating the function.

  adouble ap[number_of_variables];

  adouble result;

  double value_to_return;

  double yy[1];

  trace_on(1);
  
  for (int i = 0; i < number_of_variables; i++) {
    ap[i] <<= v[i];
  }

  result = noisy_E(ap);

  result >>= value_to_return;

  trace_off();


  return value_to_return;
  
}



// this function evals the function E, returns
// that value, and its gradient.
double E_and_gradient(double * p, double * grad) {
  // the first thing is to create the tape
  // evaluating the function.
  // To do that, we need to convert the doubles in p
  // to input adoubles. Let's do that.

  adouble ap[number_of_variables];

  adouble result;

  double value_to_return;
  
  double yy[1];

  trace_on(1);
  
  for (int i = 0; i < number_of_variables; i++) {
    ap[i] <<= p[i];
  }

  result = noisy_E(ap);

  result >>= value_to_return;

  trace_off();

  gradient(1, number_of_variables, p, grad);
    
  return value_to_return;
  
}



// the following function implements
// the adam method for training a
// neural network.
// I'm following the pseudocode in
// the original paper
// the pseudocode is in the comments
int adam (int max_iter) {

  // the parameter's defintions are in
  // the global definition section.
  // int t = 0;

  double theta [number_of_variables];  // the network weights
  double g     [number_of_variables];  // the gradient
  double g2    [number_of_variables];  // the gradient
  double m_hat [number_of_variables];
  double v_hat [number_of_variables];
  double current_error;               // the current error
  double beta1_t;
  double beta2_t;

  // let's initialize theta_0 and the 1st and 2nd moments
  for (int i = 0; i < number_of_variables; i++) {
    theta[i] = random(-1.0, 1.0); // random weight
    m[i] = 0;                     // 1st moment
    v[i] = 0;                     // 2nd moment
  }

  //while theta_t not converged do
  for (int t = 0; t < max_iter; t++) {
    // let's get the gradient and the function value
    current_error = E_and_gradient(theta, g);

    
    if (t % print_after_this_number_of_iterations == 0) { 
      printf ("%7d:  %14.8f.\n", 
              t,
              current_error);
    }


    if ((t > 0) & (t % adam_write_data_after_these_iterations == 0)) {

      printf ("Writing data to file with data in  [%7.5f, %7.5f]\n",
              min_a, max_a);
      write_results_to_file(theta);
    }    

    // let's update the betai_t
    beta1_t = beta1_t * beta1_t;
    beta2_t = beta2_t * beta2_t;

    // m_t = beta1 * m1_t + (1 - beta1)*gt
    for (int i = 0; i < number_of_variables; i++) {
      // first m
      m[i] = beta1 * m[i] + (1 - beta1)*g[i];

      // then v
      v[i] = beta2 * v[i] + (1 - beta2)*g[i]*g[i];

      // m_hat
      m_hat[i] = m[i] / (1 - beta1_t);

      // v_hat
      v_hat[i] = v[i] / (1 - beta2_t);

      // now the weights

      theta[i] = theta[i] - adam_stepsize * m_hat[i] / (sqrt(v_hat[i]) + epsilon);

    }
    
  }

  
  printf("Finished with error: %10.8f in range [%7.5f, %7.5f]\n",
         current_error, min_a, max_a);

  write_results_to_file(theta);

  return 0;
  
}



int main (void) {
  // let's seed the random number generation
  srand(time(0));

  // gsl related initializations
  int iter = 0;

  // initial weights
  double v[number_of_variables];

  // let's define the xi
  for (int i = 1; i <= number_of_xi; i++){
    xi[i-1] = x_min + i*(x_max - x_min)/(1.0*number_of_xi);
  }

  // let's initialize the weights to a random value
  for (int i = 0; i < number_of_variables; i++){
    // v[i] = random(-1.0, 1.0);
    v[i] = 1;
  }

  double initial_result;
  initial_result = eval_E_and_create_tape_no_gsl(v);

  adam(max_number_of_iterations);

  return 1;
}
