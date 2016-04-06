plf::nanotimer is a ~nanosecond-precision cross-platform (linux/bsd/mac/windows, C++03/C++11) simple timer class.

Usage is as follows:

{
	plf::nanotimer timer;


	timer.start()
	// Do something here
	double results = timer.get_elasped_ns();
	std::cout << "Timing: " << results << " nanoseconds." << std::endl;
	

	timer.start(); // "start" has the same semantics as "restart".
	// Do something else
	results = timer.get_elasped_ms();
	std::cout << "Timing: " << results << " milliseconds." << std::endl;

	
	timer.start()
	plf::microsecond_delay(15); // Delay program for 15 microseconds
	results = timer.get_elasped_us();
	std::cout << "Timing: " << results << " microseconds." << std::endl;
}	


Timer functions:
timer.start(): start or restart timer
double timer.get_elasped_ns(): get elapsed time in nanoseconds
double timer.get_elasped_us(): get elapsed time in microseconds
double timer.get_elasped_ms(): get elapsed time in milliseconds


Free-standing functions:
plf::millisecond_delay(double x): delay the program until x milliseconds have passed
plf::microseconds_delay(double x): delay the program until x microseconds have passed
plf::nanoseconds_delay(double x): delay the program until x nanoseconds have passed
