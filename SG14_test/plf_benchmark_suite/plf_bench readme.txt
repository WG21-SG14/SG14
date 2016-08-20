the benchmarking suite includes a range of functions, some of which may be important to you, some of which may not.

they are all things I've used in the course of testing, so apologies for the clutter. Needs a cleanup

if you want to know what is relevant to you, look at the individual_tests folder. these are the files I used to generate the graphs in the plf::colony and plf::stack documentation.

to build these individual benchmarks, simply load one of the individual tests, add the folder below this one (and your plf::colony include folder) to your include path, and compile.

the individual tests, by default, spit out .csv files, which can be read into any excel-equivalent spreadsheet like libreoffice calc. From there you can generate your own graphs using an X-Y scatter plot

tests are compatible with c++11 and c++03 except for the ones involving indexed_vector, pointer_deque or booled_deque.

www.plflib.org

Matt Bentley
mattreecebentley@gmail.com