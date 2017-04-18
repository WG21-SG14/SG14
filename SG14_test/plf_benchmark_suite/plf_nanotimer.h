#pragma once

// Copyright (c) 2016, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

#ifndef PLF_NANOTIMER
#define PLF_NANOTIMER


// ~Nanosecond-precision cross-platform (linux/bsd/mac/windows, C++03/C++11) simple timer class:

// Mac OSX implementation:
#if defined(__MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h>

	namespace plf
	{

	class nanotimer
	{
	private:
		clock_serv_t system_clock;
		mach_timespec_t time1, time2;
	public:
		nanotimer()
		{
			host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &system_clock);
		}

		~nanotimer()
		{
			mach_port_deallocate(mach_task_self(), system_clock);
		}

		inline void start()
		{
			clock_get_time(system_clock, &time1);
		}

		inline double get_elapsed_ms()
		{
			return static_cast<double>(get_elapsed_ns()) / 1000000.0;
		}

		inline double get_elapsed_us()
		{
			return static_cast<double>(get_elapsed_ns()) / 1000.0;
		}

		double get_elapsed_ns()
		{
			clock_get_time(system_clock, &time2);
			return ((1000000000.0 * static_cast<double>(time2.tv_sec - time1.tv_sec)) + static_cast<double>(time2.tv_nsec - time1.tv_nsec));
		}
	};




// Linux/BSD implementation:
#elif (defined(linux) || defined(__linux__) || defined(__linux)) || (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
	#include <time.h>
	#include <sys/time.h>

	namespace plf
	{

	class nanotimer
	{
	private:
		struct timespec time1, time2;
	public:
		nanotimer() {}
		
		inline void start()
		{
			clock_gettime(CLOCK_MONOTONIC, &time1);
		}
		
		inline double get_elapsed_ms()
		{
			return get_elapsed_ns() / 1000000.0;
		}

		inline double get_elapsed_us()
		{
			return get_elapsed_ns() / 1000.0;
		}

		double get_elapsed_ns()
		{
			clock_gettime(CLOCK_MONOTONIC, &time2);
			return ((1000000000.0 * static_cast<double>(time2.tv_sec - time1.tv_sec)) + static_cast<double>(time2.tv_nsec - time1.tv_nsec));
		}
	};




// Windows implementation:
#elif defined(_WIN32)
	#if defined(_MSC_VER) && !defined(NOMINMAX)
		#define NOMINMAX // Otherwise MS compilers act like idiots when using std::numeric_limits<>::max() and including windows.h
	#endif
	
	#include <windows.h>
	
	namespace plf
	{

	class nanotimer
	{
	private:
		LARGE_INTEGER ticks1, ticks2;
		double frequency;
	public:
		nanotimer()
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			frequency = static_cast<double>(freq.QuadPart);
		}

		inline void start()
		{
			QueryPerformanceCounter(&ticks1);
		}

		double get_elapsed_ms()
		{
			QueryPerformanceCounter(&ticks2);
			return (static_cast<double>(ticks2.QuadPart - ticks1.QuadPart) * 1000.0) / frequency;
		}

		inline double get_elapsed_us()
		{
			return get_elapsed_ms() * 1000.0;
		}

		inline double get_elapsed_ns()
		{
			return get_elapsed_ms() * 1000000.0;
		}
	};
#endif
// Else: failure warning - your OS is not supported



#if defined(__MACH__) || (defined(linux) || defined(__linux__) || defined(__linux)) || (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)) || defined(_WIN32)
void nanosecond_delay(double delay_ns)
{
	nanotimer timer;
	timer.start();

	while(timer.get_elapsed_ns() < delay_ns)
	{};
}



inline void microsecond_delay(double delay_us)
{
	nanosecond_delay(delay_us * 1000.0);
}


inline void millisecond_delay(double delay_ms)
{
	nanosecond_delay(delay_ms * 1000000.0);
}


} // namespace
#endif

#endif // PLF_NANOTIMER
