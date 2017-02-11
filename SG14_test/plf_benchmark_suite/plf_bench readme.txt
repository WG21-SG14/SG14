The benchmarking suite includes a range of functions, some of which may be important to you, some of which may not.

They are all things I've used in the course of testing, so apologies for the clutter.

The individual_tests folder contains the files I used to generate the graphs in the plf::colony and plf::stack documentation.

To build these individual benchmarks, simply load one of the individual tests, add the folder below this one (and your plf::colony include folder) to your include path, and compile. These by default, spit out .csv files, which can be read into any excel-equivalent spreadsheet like libreoffice calc. From there you can generate your own graphs.

Tests are compatible with c++11 and c++03 for the most part.

www.plflib.org

Matt Bentley
