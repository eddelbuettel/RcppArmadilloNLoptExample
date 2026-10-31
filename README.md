
#### What Is This About?

This is a mostly-local experimental repo where I once tried to offer help to
[another repository](https://github.com/jchiquet/RcppArmadilloNLoptExample)
which had been struggling with an integration between RcppArmadillo and
nloptr.

This repository now contains three working packages showing how to set up the
desired integration. Here one relies on an external nlopt installation and
links to it, and the other two take advantage of the API exporting header
`nloptrAPI.h` and do _not_ require an external library. (The difference
between two and three is additional use of a logging library.)

#### Author

For this fork: Dirk Eddelbuettel
