% tinyrox says don't edit this manually, but it can't stop you!
\name{testRosenbrock}
\alias{testRosenbrock}
\title{NLopt Call Example with n-dimensional Rosenbrock}
\usage{
testRosenbrock(n = 4L, verbose = TRUE)
}
\arguments{
\item{n}{A integer denoting the dimensionality of the problem (in order to make it
expensive enough to measure run-time as n=2 is too fast)}

\item{verbose}{A logical flag indicating whether results are printed on convergence}
}
\value{
A numeric vector with n elements
}
\description{
An n-dimensional Rosenbrock function (i.e. the 'banana' function in 2-d)
is optimized to provide another example for NLopt (more suitable for timing).
}
\examples{
testRosenbrock(4)
}
