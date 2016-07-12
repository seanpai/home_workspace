#pragma once
// smart-pointer implementation for c++98
class RC
{
private:
	int count; // Reference count

public:
	RC() : count(0)
	{
	}

	void AddRef()
	{
		// Increment the reference count
		count++;
	}

	int Release()
	{
		// Decrement the reference count and
		// return the reference count.
		return --count;
	}
};

class deleter_base
{
public:
	deleter_base(){};
	virtual ~deleter_base() {};
private:
	deleter_base& operator=(const deleter_base&);
	deleter_base(const deleter_base&);
};

template<typename P, typename D>
class deleter_impl : public deleter_base
{
private:
	D del;
	P ptr;
	deleter_impl& operator=(const deleter_impl &);
	deleter_impl(const deleter_impl&);

public:

	explicit deleter_impl(P p, D& _px) : ptr(p), del(_px) {};
	//deleter_impl(P p) : ptr(p), del() {};

	virtual ~deleter_impl()
	{
		if (NULL != ptr)
		{
			del(ptr);
		}
	}

};

enum : unsigned int
{
	NON_ARRAY_PTR = 1,
	ARRAY_PTR = 2
};

template < typename T> 
class SmartPtr
{
private:
	T*    pData;       // pointer
	RC* reference; // Reference count
	unsigned int m_nArrayIndex;
	deleter_base* m_pDeleter;

public:

	SmartPtr() : 
		pData(NULL), 
		reference(NULL), 
		m_nArrayIndex(NON_ARRAY_PTR), 
		m_pDeleter(NULL)
	{
		// Create a new reference 
		reference = new RC();
		// Increment the reference count
		reference->AddRef();
	}

	SmartPtr(T* pValue, unsigned int nArrayIndex) :
		pData(pValue),
		reference(0), 
		m_nArrayIndex(nArrayIndex),
		m_pDeleter(NULL)
	{
		// Create a new reference 
		reference = new RC();
		// Increment the reference count
		reference->AddRef();
	}

	template<typename D>
	SmartPtr(T* pValue, D del) : 
		pData(pValue), 
		reference(0), 
		m_nArrayIndex(NON_ARRAY_PTR)
	{
		// Create a new reference 
		reference = new RC();
		// Increment the reference count
		reference->AddRef();
		m_pDeleter = new deleter_impl<T*, D>(pValue, del);
	}

	SmartPtr(const SmartPtr<T>& sp) : 
		pData(sp.pData), 
		reference(sp.reference), 
		m_nArrayIndex(sp.m_nArrayIndex),
		m_pDeleter(NULL)
	{
		// Copy constructor
		// Copy the data and reference pointer
		// and increment the reference count
		reference->AddRef();
	}

	~SmartPtr()
	{
		// Destructor
		// Decrement the reference count
		// if reference become zero delete the data
		if(0 == reference->Release())
		{
			if(NULL != pData && NULL == m_pDeleter)
			{
				if (ARRAY_PTR == m_nArrayIndex)
				{
					std::cout << "Array Deleter Called" << std::endl;
					delete[] pData;
				}
				else
					delete pData;	// For non-array pointer
			}
			if(NULL != reference)
				delete reference;

			if (NULL != m_pDeleter)
				delete m_pDeleter;
		}
	}

	void Reset(T* pValue, int nArrayIndex = NON_ARRAY_PTR);

	T& operator* ()
	{
		return *pData;
	}

	T* operator-> ()
	{
		return pData;
	}

	T* GetPtr()
	{
		return pData;
	}

	operator void*();

	SmartPtr<T>& operator = (const SmartPtr<T>& sp)
	{
		// Assignment operator
		if (this != &sp) // Avoid self assignment
		{
			// Decrement the old reference count
			// if reference become zero delete the old data
			if(reference->Release() == 0)
			{
				delete pData;
				delete reference;
			}

			// Copy the data and reference pointer
			// and increment the reference count
			pData = sp.pData;
			reference = sp.reference;
			reference->AddRef();
		}
		return *this;
	}
};

template < typename T >
void SmartPtr<T>::Reset(T* pValue, int nArrayIndex)
{
	if(pData != pValue)
	{
		if( 0 == reference->Release())
		{
			if (NULL != pData && NULL == m_pDeleter)
			{
				if (ARRAY_PTR == m_nArrayIndex)
					delete[] pData;
				else
					delete pData;	// For non-array pointer
			}
			if (NULL != reference)
				delete reference;

			if (NULL != m_pDeleter)
				delete m_pDeleter;
		}

		pData = pValue;
		m_nArrayIndex = nArrayIndex;
		reference = new RC();
		reference->AddRef();
	
	}
}

template < typename T >
SmartPtr<T>::operator void*()
{
	if(NULL != pData)
		return (void*)1;
	else
		return (void*)0;
}

template<typename T>
struct ArrayDeleter
{      
	void operator () (T* p)
	{
		std::cout << "ArrayDeleter" << std::endl;
		delete [] p;
	}
};
