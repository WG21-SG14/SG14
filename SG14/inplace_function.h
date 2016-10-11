#pragma once

#include <type_traits>
#include <functional>

namespace stdext
{

constexpr size_t InplaceFunctionDefaultCapacity = 32;
constexpr size_t InplaceFunctionDefaultAlignment = 16;

enum class inplace_function_operation
{
	Destroy,
	Copy,
	Move
};

template <typename SignatureT, size_t CapacityT = InplaceFunctionDefaultCapacity, size_t AlignmentT = InplaceFunctionDefaultAlignment>
class inplace_function;

template <typename RetT, typename... ArgsT, size_t CapacityT, size_t AlignmentT>
class inplace_function<RetT(ArgsT...), CapacityT, AlignmentT>
{
public:
	template <typename SignatureT2, std::size_t CapacityT2, std::size_t AlignmentT2>
	friend class inplace_function;
	
	// TODO static_assert for misalignment
	// TODO create free operator overloads, to handle switched arguments

	// Creates and empty inplace_function
	inplace_function() : m_InvokeFctPtr(&DefaultFunction), m_ManagerFctPtr(nullptr)
	{
	}

	// Destroys the inplace_function. If the stored callable is valid, it is destroyed also
	~inplace_function()
	{
		this->clear();
	}

	// Creates an implace function, copying the target of other within the internal buffer
	// If the callable is larger than the internal buffer, a compile-time error is issued
	// May throw any exception encountered by the constructor when copying the target object
	template<typename CallableT>
	inplace_function(const CallableT& c)
	{
		this->set(c);
	}

	// Moves the target of an implace function, storing the callable within the internal buffer
	// If the callable is larger than the internal buffer, a compile-time error is issued
	// May throw any exception encountered by the constructor when moving the target object
	template<typename CallableT, class = typename std::enable_if<!std::is_lvalue_reference<CallableT>::value>::type>
	inplace_function(CallableT&& c)
	{
		this->set(std::move(c));
	}

	// Copy construct an implace_function, storing a copy of other’s target internally
	// May throw any exception encountered by the constructor when copying the target object
	inplace_function(const inplace_function& other)
	{
		this->copy(other);
	}

	// Move construct an implace_function, moving the other’s target to this inplace_function’s internal buffer
	// May throw any exception encountered by the constructor when moving the target object
	inplace_function(inplace_function&& other)
	{
		this->move(std::move(other));
	}

	// Allows for copying from inplace_function object of the same type, but with a smaller buffer
	// May throw any exception encountered by the constructor when copying the target object
	// If OtherCapacity is greater than Capacity, a compile-time error is issued
	template<size_t OtherCapacityT>
	inplace_function(const inplace_function<RetT(ArgsT...), OtherCapacityT>& other)
	{
		this->copy(other);
	}

	// Allows for moving an inplace_function object of the same type, but with a smaller buffer
	// May throw any exception encountered by the constructor when moving the target object
	// If OtherCapacity is greater than Capacity, a compile-time error is issued
	template<size_t OtherCapacity>
	inplace_function(inplace_function<RetT(ArgsT...), OtherCapacity>&& other)
	{
		this->move(other);
	}

	// Assigns a copy of other’s target
	// May throw any exception encountered by the assignment operator when copying the target object
	inplace_function& operator=(const inplace_function& other)
	{
		this->clear();
		this->copy(other);
		return *this;
	}

	// Assigns the other’s target by way of moving
	// May throw any exception encountered by the assignment operator when moving the target object
	inplace_function& operator=(inplace_function&& other)
	{
		this->clear();
		this->move(std::move(other));
		return *this;
	}

	// Allows for copy assignment of an inplace_function object of the same type, but with a smaller buffer
	// If the copy constructor of target object throws, this is left in uninitialized state
	// If OtherCapacity is greater than Capacity, a compile-time error is issued
	template<size_t OtherCapacityT>
	inplace_function& operator=(const inplace_function<RetT(ArgsT...), OtherCapacityT>& other)
	{
		this->clear();
		this->copy(other);
		return *this;
	}

	// Allows for move assignment of an inplace_function object of the same type, but with a smaller buffer
	// If the move constructor of target object throws, this is left in uninitialized state
	// If OtherCapacity is greater than Capacity, a compile-time error is issued
	template<size_t OtherCapacity>
	inplace_function& operator=(inplace_function<RetT(ArgsT...), OtherCapacity>&& other)
	{
		this->clear();
		this->move(std::move(other));
		return *this;
	}

	// Assign a new target
	// If the copy constructor of target object throws, this is left in uninitialized state
	template<typename CallableT, class = typename std::enable_if<!std::is_lvalue_reference<CallableT>::value>::type>
	inplace_function& operator=(const CallableT& target)
	{
		this->clear();
		this->set(target);
		return *this;
	}

	// Assign a new target by way of moving
	// If the move constructor of target object throws, this is left in uninitialized state
	template<typename Callable>
	inplace_function& operator=(Callable&& target)
	{
		this->clear();
		this->set(std::move(target));
		return *this;
	}

	// Compares this inplace function with a null pointer
	// Empty functions compare equal, non-empty functions compare unequal
	bool operator ==(std::nullptr_t)
	{
		return !operator bool();
	}

	// Compares this inplace function with a null pointer
	// Empty functions compare equal, non-empty functions compare unequal
	bool operator !=(std::nullptr_t)
	{
		return operator bool();
	}

	// Converts to 'true' if assigned
	explicit operator bool() const throw()
	{
		return m_InvokeFctPtr != &DefaultFunction;
	}

	// Invokes the target
	// Throws std::bad_function_call if not assigned
	RetT operator () (ArgsT... args) const
	{
		return m_InvokeFctPtr(std::forward<ArgsT>(args)..., data());
	}

	// Swaps two targets
	void swap(inplace_function& other)
	{
		BufferType tempData;
		this->move(m_Data, tempData);
		other.move(other.m_Data, m_Data);
		this->move(tempData, other.m_Data);
		std::swap(m_InvokeFctPtr, other.m_InvokeFctPtr);
		std::swap(m_ManagerFctPtr, other.m_ManagerFctPtr);
	}

private:
	using BufferType = typename std::aligned_storage<CapacityT, AlignmentT>::type;
	void clear()
	{
		m_InvokeFctPtr = &DefaultFunction;
		if (m_ManagerFctPtr)
			m_ManagerFctPtr(data(), nullptr, Operation::Destroy);
		m_ManagerFctPtr = nullptr;
	}

	template<size_t OtherCapacityT>
	void copy(const inplace_function<RetT(ArgsT...), OtherCapacityT>& other)
	{
		static_assert(OtherCapacityT <= CapacityT, "Can't squeeze larger inplace_function into a smaller one");

		if (other.m_ManagerFctPtr)
			other.m_ManagerFctPtr(data(), other.data(), Operation::Copy);

		m_InvokeFctPtr = other.m_InvokeFctPtr;
		m_ManagerFctPtr = other.m_ManagerFctPtr;
	}

	void move(BufferType& from, BufferType& to)
	{
		if (m_ManagerFctPtr)
			m_ManagerFctPtr(&from, &to, Operation::Move);
		else
			to = from;
	}

	template<size_t OtherCapacityT>
	void move(inplace_function<RetT(ArgsT...), OtherCapacityT>&& other)
	{
		static_assert(OtherCapacityT <= CapacityT, "Can't squeeze larger inplace_function into a smaller one");

		if (other.m_ManagerFctPtr)
			other.m_ManagerFctPtr(data(), other.data(), Operation::Move);

		m_InvokeFctPtr = other.m_InvokeFctPtr;
		m_ManagerFctPtr = other.m_ManagerFctPtr;
	}

	void* data() { return &m_Data; }
	const void* data() const { return &m_Data; }

	using CompatibleFunctionPointer = RetT(*)(ArgsT...);
	using InvokeFctPtrType = RetT(*)(ArgsT..., const void* thisPtr);
	using Operation = inplace_function_operation;
	using ManagerFctPtrType = void(*) (void* thisPtr, const void* fromPtr, Operation);

	InvokeFctPtrType m_InvokeFctPtr;
	ManagerFctPtrType m_ManagerFctPtr;

	BufferType m_Data;

	static RetT DefaultFunction(ArgsT..., const void*)
	{
		throw std::bad_function_call();
	}

	void set(std::nullptr_t)
	{
		m_ManagerFctPtr = nullptr;
		m_InvokeFctPtr = &DefaultFunction;
	}

	// For function pointers
	void set(CompatibleFunctionPointer ptr)
	{
		// this is dodgy, and - according to standard - undefined behaviour. But it works
		// see: http://stackoverflow.com/questions/559581/casting-a-function-pointer-to-another-type
		m_ManagerFctPtr = nullptr;
		m_InvokeFctPtr = reinterpret_cast<InvokeFctPtrType>(ptr);
	}

	// Set - for functors
	// enable_if makes sure this is excluded for function references and pointers.
	template<typename FunctorArgT>
	typename std::enable_if<!std::is_pointer<FunctorArgT>::value && !std::is_function<FunctorArgT>::value>::type
	set(const FunctorArgT& ftor)
	{
		using FunctorT = typename std::remove_reference<FunctorArgT>::type;
		static_assert(sizeof(FunctorT) <= CapacityT, "Functor too big to fit in the buffer");

		// copy functor into the mem buffer
		FunctorT* buffer = reinterpret_cast<FunctorT*>(&m_Data);
		new (buffer) FunctorT(ftor);

		// generate destructor, copy-constructor and move-constructor
		m_ManagerFctPtr = &manage<FunctorT>;

		// generate entry call
		m_InvokeFctPtr = &invoke<FunctorT>;
	}

	// Set - for functors
	// enable_if makes sure this is excluded for function references and pointers.
	template<typename FunctorArgT>
	typename std::enable_if<!std::is_pointer<FunctorArgT>::value && !std::is_function<FunctorArgT>::value>::type
		set(FunctorArgT&& ftor)
	{
		using FunctorT = typename std::remove_reference<FunctorArgT>::type;
		static_assert(sizeof(FunctorT) <= CapacityT, "Functor too big to fit in the buffer");

		// copy functor into the mem buffer
		FunctorT* buffer = reinterpret_cast<FunctorT*>(&m_Data);
		new (buffer) FunctorT(std::move(ftor));

		// generate destructor, copy-constructor and move-constructor
		m_ManagerFctPtr = &manage<FunctorT>;

		// generate entry call
		m_InvokeFctPtr = &invoke<FunctorT>;
	}

	template <typename FunctorT>
	static RetT invoke(ArgsT... args, const void* dataPtr)
	{
		FunctorT* functor = (FunctorT*)const_cast<void*>(dataPtr);
		return (*functor)(std::forward<ArgsT>(args)...);
	}

	template <typename FunctorT>
	static void manage(void* dataPtr, const void* fromPtr, Operation op)
	{
		FunctorT* thisFunctor = reinterpret_cast<FunctorT*>(dataPtr);
		switch (op)
		{
		case Operation::Destroy:
			thisFunctor->~FunctorT();
			break;
		case Operation::Copy:
			{
				const FunctorT* source = (const FunctorT*)const_cast<void*>(fromPtr);
				new (thisFunctor) FunctorT(*source);
				break;
			}
		case Operation::Move:
			{
				FunctorT* source = (FunctorT*)fromPtr;
				new (thisFunctor) FunctorT(std::move(*source));
				break;
			}
		}
	}
};

}
