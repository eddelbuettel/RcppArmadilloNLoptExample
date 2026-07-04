#include <RcppArmadillo/Lightest> 	// for Rcpp (and RcppArmadillo)
#include <nlopt.hpp>            	// for NLopt via C++
#include <tl.h>                     // for tiny logging via rspdlite

// Objective function for the N-dimensional Rosenbrock function
double rosenbrock(const std::vector<double> &x, std::vector<double> &grad, void *data) {
    int n = x.size();

    // Compute gradient if requested
    if (!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        for (int i = 0; i < n - 1; ++i) {
            double term1 = 400.0 * (x[i+1] - x[i]*x[i]) * x[i];
            double term2 = 2.0 * (x[i] - 1.0);
            grad[i] += term1 + term2;
            grad[i+1] -= 200.0 * (x[i+1] - x[i]*x[i]);
        }
    }

    // Compute function value
    double sum = 0.0;
    for (int i = 0; i < n - 1; ++i) {
        double t1 = (x[i+1] - x[i]*x[i]);
        double t2 = (x[i] - 1.0);
        sum += 100.0 * t1 * t1 + t2 * t2;
    }
    tl::trace("[rosenbrock] objective function now {:0.8f}", sum);
    return sum;
}

//' @title NLopt Call Example with n-dimensional Rosenbrock
//'
//' @description An n-dimensional Rosenbrock function (i.e. the 'banana' function in 2-d)
//' is optimized to provide another example for NLopt (more suitable for timing).
//'
//' @param n A integer denoting the dimensionality of the problem (in order to make it
//' expensive enough to measure run-time as n=2 is too fast)
//' @param verbose A logical flag indicating whether results are printed on convergence
//'
//' @return A numeric vector with n elements
//'
//' @examples
//' testRosenbrock(4)
// [[Rcpp::export]]
std::vector<double> testRosenbrock(int n = 4, bool verbose = true) {
    // Initialize NLopt optimizer with COBYLA algorithm, no derivatives
    nlopt::opt opt(nlopt::LN_COBYLA, n);

    // Set the objective function to minimize
    opt.set_min_objective(rosenbrock, nullptr);

    // Set optional boundaries (a common domain for Rosenbrock)
    std::vector<double> lb(n, -5.0);
    std::vector<double> ub(n, 10.0);
    opt.set_lower_bounds(lb);
    opt.set_upper_bounds(ub);

    // Set stopping criteria
    opt.set_xtol_rel(1e-6);

    // Initial guess
    std::vector<double> x(n, -1.2);
    double minf; 				// Variable to store optimal function value

    nlopt::result result = opt.optimize(x, minf);
    if (result < 0) {
        tl::warn("nlopt failed: {}", (int)result);
    } else {
        tl::info("[testRosenbrock] Minimum {:0.4f} reached", minf);
    }
    if (verbose) {
        Rcpp::Rcout << "Optimization successful: " << result << "\n"
                    << "Optimal value: " << minf << "\n"
                    << "Optimal variables (should be close to 1.0):\n";
        for(int i = 0; i < n; ++i) {
            Rcpp::Rcout << "  x[" << i << "] = " << x[i] << "\n";
        }
    }

    return x;
}
