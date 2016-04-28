the plf_bench.h benchmarking suite includes a range of functions, some of which may be important to you, some of which may not.

they are all things I've used in the course of testing, so apologies for the clutter.

if you want to know what is relevant to you, look at the individual_tests folder. these are the files I used to generate the graphs on the plf::colony and plf::stack websites.

to build these individual benchmarks, simply add the folder containing plf_bench.h and the rest to your include path, and compile.

the individual tests, by default, spit out .csv files, which can be read into any excel-equivalent spreadsheet like libreoffice calc. From there you can easily generate your own graphs using an X-Y scatter plot

tests are compatible with c++11 and c++03