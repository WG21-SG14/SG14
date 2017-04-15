#pragma once

#ifndef PLF_PACKED_DEQUE_H
#define PLF_PACKED_DEQUE_H

#include <cstring> // for size_t
#include <deque>

#if (defined(_MSC_VER) && (_MSC_VER > 1600)) || (defined(__cplusplus) && __cplusplus >= 201103L)
	#define PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif



namespace plf
{


template <class element_type, class allocator_type = std::allocator<element_type> >
class packed_deque
{
private:

	struct package;
	typedef package * package_pointer_type;
	typedef package_pointer_type * pointer_to_package_pointer_type;


	struct package
	{
		element_type element;
		pointer_to_package_pointer_type ref;

		package(const element_type &el, const pointer_to_package_pointer_type rf): element(el), ref(rf) {};
	};


	typedef std::deque<package, typename allocator_type::template rebind<package>::other> package_deque;
	typedef std::deque<package_pointer_type, typename allocator_type::template rebind<package_pointer_type>::other> package_pointer_deque;

	package_deque packages;
	package_pointer_deque pointers_to_packages;
	pointer_to_package_pointer_type free_list_head;
	size_t current_size, largest_size;


public:

	typedef typename package_deque::iterator iterator;


	class handle
	{
	private:
		pointer_to_package_pointer_type pointer_to_package_pointer;
		friend class packed_deque;

	public:
		handle(): pointer_to_package_pointer(NULL) {}

		handle(const pointer_to_package_pointer_type p): pointer_to_package_pointer(p) {}

		handle(const handle &source): pointer_to_package_pointer(source.pointer_to_package_pointer) {}



		inline handle & operator = (const handle &source)
		{
			pointer_to_package_pointer = source.pointer_to_package_pointer;
			return *this;
		}



		#ifdef PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
			handle(handle &&source): pointer_to_package_pointer(std::move(source.pointer_to_package_pointer)) {}


			inline handle & operator = (handle &&source)
			{
				pointer_to_package_pointer = std::move(source.pointer_to_package_pointer);
				return *this;
			}
		#endif



		inline element_type & operator * () const
		{
			return (*(*pointer_to_package_pointer)).element;
		}



		inline element_type * operator -> () const
		{
			return &((*(*pointer_to_package_pointer)).element);
		}
	};



	packed_deque(): free_list_head(NULL), current_size(0), largest_size(0) {}



	handle insert(const element_type &element)
	{
		pointer_to_package_pointer_type loc;

		if (free_list_head == NULL)
		{
			pointers_to_packages.push_back(NULL);
			loc = &(pointers_to_packages.back());
		}
		else
		{
			loc = free_list_head;
			free_list_head = reinterpret_cast<pointer_to_package_pointer_type>(*free_list_head);
		}

		packages.push_back(package(element, loc));
		*loc = &(packages.back());
		
		++current_size;
		largest_size = (largest_size < current_size) ? current_size : largest_size;

		return handle(loc);
	}



	iterator erase(const iterator &it)
	{
		iterator return_iterator = it + 1;

		if (return_iterator == packages.end())
		{
			return_iterator = it;
		}
		
		erase(handle(it->ref));
		
		return return_iterator;
	}

	

	void erase(const handle &loc)
	{
		package &location = *(*(loc.pointer_to_package_pointer));

		// Either add handle pointer location to free list (if not back) or pop from back
		if (location.ref != &(pointers_to_packages.back()))
		{
			*(location.ref) = reinterpret_cast<package_pointer_type>(free_list_head);
			free_list_head = location.ref;
		}
		else
		{
			pointers_to_packages.pop_back();
		}

		// Move/copy back element to erasure location if erasure location is not back
		if (&location != &(packages.back()))
		{
			#ifdef PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
				location = std::move(packages.back());
			#else
				location = packages.back();
			#endif

			*(location.ref) = &location;
		}

		packages.pop_back();
		--current_size;
	}



	inline iterator begin() { return packages.begin(); };
	inline iterator end() { return packages.end(); };
	inline const iterator & begin() const { return packages.begin(); };
	inline const iterator & end() const { return packages.end(); };



#ifdef PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
	packed_deque(packed_deque &&source): packages(std::move(source.packages)), pointers_to_packages(std::move(source.pointers_to_packages)), free_list_head(source.free_list_head), current_size(source.current_size), largest_size(source.largest_size) {}



	inline packed_deque & operator = (packed_deque &&source)
	{
		packages = std::move(source.packages);
		pointers_to_packages = std::move(source.pointers_to_packages);
		free_list_head = source.free_list_head;

		return *this;
	}
#endif



	inline size_t size() const
	{
		return static_cast<size_t>(current_size);
	}



    inline size_t capacity() const
	{
		return static_cast<size_t>(
		((((largest_size * sizeof(package)) / 512) + 1) * 512) / sizeof(package)
		); // this approximation based on GCC (libstdc++) deque implementation only
	}



	inline size_t approximate_memory_use() const
	{
		return static_cast<size_t>(
			64 + // node map
			((((largest_size * sizeof(package)) / 512) + 1) * 512) + 
			((((largest_size * sizeof(package_pointer_type)) / 512) + 1) * 512) + 
			sizeof(*this)); // this approximation based on GCC (libstdc++) deque implementation only
	}
	
	

	inline void clear()
	{
		packages.clear();
		pointers_to_packages.clear();
		free_list_head = NULL;
	}


};


}


#ifdef PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
#undef PLF_PACKED_DEQUE_MOVE_SEMANTICS_SUPPORT
#endif


#endif // PLF_PACKED_DEQUE_H
