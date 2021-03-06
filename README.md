

# RcppArmadilloNLoptExample

I wrote two simple packages trying to access respectively the `C` and `C++` nlopt API via the *nloptr* R package.
These packages also link to *RcppArmadillo* and *Rcpp* (apart from nloptr), since the goal is to write some C/C++ code using the features of both Rcpp(Armadillo) and nlopt, to eventually interface this code with R.

These packages require the development version of *nloptr* in order to access the header via the file "nloptrAPI.h".

```r
devtools::install_github("jyypma/nloptr")
```

# nloptC

Interface with the C API of nlopt works just fine, using the example provided by jyypma in [https://github.com/jyypma/nloptr/blob/master/tests/testthat/test-C-api.R](https://github.com/jyypma/nloptr/blob/master/tests/testthat/test-C-api.R). I just slightly modify the export to R SEXP object in order to use some Rcpp wrapper (see code below).


```r
devtools::install_github("jchiquet/RcppArmadilloNLoptExample/nloptC", force = TRUE)
```

```
## Downloading GitHub repo jchiquet/RcppArmadilloNLoptExample@master
## from URL https://api.github.com/repos/jchiquet/RcppArmadilloNLoptExample/zipball/master
```

```
## Installing nloptC
```

```
## '/usr/lib/R/bin/R' --no-site-file --no-environ --no-save --no-restore  \
##   --quiet CMD INSTALL  \
##   '/tmp/RtmpdAJCW2/devtools7c4632f8e110/jchiquet-RcppArmadilloNLoptExample-f71dd4a/nloptC'  \
##   --library='/home/jchiquet/R/x86_64-pc-linux-gnu-library/3.4'  \
##   --install-tests
```

```
## 
```

```
## Reloading installed nloptC
```


```r
library(nloptC)
test_nlopt_c()
```

```
## [1] 0.3333333 0.2962963
```

## Source of src/nlopt_c.cpp



```r
#include "RcppArmadillo.h"

// [[Rcpp::depends(RcppArmadillo)]]

#include <vector>
#include <nloptrAPI.h>

double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)
{
    if (grad) {
        grad[0] = 0.0;
        grad[1] = 0.5 / sqrt(x[1]);
    }
    return sqrt(x[1]);
}
typedef struct {
    double a, b;
} my_constraint_data;

double myconstraint(unsigned n, const double *x, double *grad, void *data)
{
    my_constraint_data *d = (my_constraint_data *) data;
    double a = d->a, b = d->b;
    if (grad) {
        grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
        grad[1] = -1.0;
    }
    return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

// [[Rcpp::export]]
// #' @export
Rcpp::NumericVector test_nlopt_c()
 {
  double lb[2] = { -HUGE_VAL, 0 }; /* lower bounds */
  nlopt_opt opt;

  opt = nlopt_create(NLOPT_LD_MMA, 2); /* algorithm and dimensionality */
  nlopt_set_lower_bounds(opt, lb);
  nlopt_set_min_objective(opt, myfunc, NULL);
  my_constraint_data data[2] = { {2,0}, {-1,1} };

  nlopt_add_inequality_constraint(opt, myconstraint, &data[0], 1e-8);
  nlopt_add_inequality_constraint(opt, myconstraint, &data[1], 1e-8);
  nlopt_set_xtol_rel(opt, 1e-4);
  double x[2] = { 1.234, 5.678 };  /* some initial guess */
  double minf; /* the minimum objective value, upon return */
  if (nlopt_optimize(opt, x, &minf) < 0) {
    //Rprintf("nlopt failed!\\n");
}
else {
    //Rprintf("found minimum at f(%g,%g) = %0.10g\\n", x[0], x[1], minf);
}
  nlopt_destroy(opt);
  std::vector<double> result(x, x + 2) ;
  return Rcpp::wrap(result) ;
}
```

# nloptCpp

Unfortunatly, using a similar approach with the C++ nlopt API does not work. The build works just fine but the loading fails:

```
Error: package or namespace load failed for ‘nloptCpp’ in dyn.load(file, DLLpath = DLLpath, ...):
 unable to load shared object '/home/jchiquet/R/x86_64-pc-linux-gnu-library/3.4/nloptCpp/libs/nloptCpp.so':
  /home/jchiquet/R/x86_64-pc-linux-gnu-library/3.4/nloptCpp/libs/nloptCpp.so: undefined symbol: nlopt_set_munge
Error: loading failed
```


```r
devtools::install_github("jchiquet/RcppArmadilloNLoptExample/nloptCpp", force = TRUE)
```

```
## Downloading GitHub repo jchiquet/RcppArmadilloNLoptExample@master
## from URL https://api.github.com/repos/jchiquet/RcppArmadilloNLoptExample/zipball/master
```

```
## Installing nloptCpp
```

```
## '/usr/lib/R/bin/R' --no-site-file --no-environ --no-save --no-restore  \
##   --quiet CMD INSTALL  \
##   '/tmp/RtmpdAJCW2/devtools7c461e9baa7a/jchiquet-RcppArmadilloNLoptExample-f71dd4a/nloptCpp'  \
##   --library='/home/jchiquet/R/x86_64-pc-linux-gnu-library/3.4'  \
##   --install-tests
```

```
## 
```

```
## Installation failed: Command failed (1)
```

## Source of src/nlopt_cpp.cpp


```r
#include "RcppArmadillo.h"

// [[Rcpp::depends(RcppArmadillo)]]

#include <nloptrAPI.h>
#include <nlopt.hpp>

using namespace Rcpp;

struct my_constraint_data
{
  double a;
  double b;
};

double myvfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
  if (!grad.empty()) {
    grad[0] = 0.0;
    grad[1] = 0.5 / std::sqrt(x[1]);
  }
  return std::sqrt(x[1]);
}

double myvconstraint(const std::vector<double> &x, std::vector<double> &grad, void *data)
{
  my_constraint_data *d = reinterpret_cast<my_constraint_data*>(data);
  double a = d->a, b = d->b;
  if (!grad.empty()) {
    grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
    grad[1] = -1.0;
  }
  return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

// [[Rcpp::export]]
double test_nlopt_cpp()
{
  nlopt::opt opt(nlopt::LD_MMA, 2);
  std::vector<double> lb(2);
  lb[0] = R_NegInf;
  lb[1] = 0.0;
  opt.set_lower_bounds(lb);
  opt.set_min_objective(myvfunc, NULL);
  my_constraint_data data[2] = { {2,0}, {-1,1} };
  opt.add_inequality_constraint(myvconstraint, &data[0], 1e-8);
  opt.add_inequality_constraint(myvconstraint, &data[1], 1e-8);

  opt.set_xtol_rel(1e-4);
  std::vector<double> x(2);
  x[0] = 1.234;
  x[1] = 5.678;
  double minf;
  nlopt::result result = opt.optimize(x, minf);
  Rcpp::Rcout << int(result) << "\n";
  Rcpp::Rcout << x[0] << "," << x[1] << "\n";
  return minf;
}
```

