#pragma once
// smart-pointer implementation for c++98
namespace hhi
{

enum
{
	NON_ARRAY_PTR = 1,
	ARRAY_PTR = 2
};

#ifndef NULL
#define NULL (0)
#endif

template< class T > struct sp_element
{
    typedef T type;
};

template<typename T>
struct default_delete
{
	void operator()(T* pValue)
	{
		delete pValue;
	}
};

class ReferenceCounter
{
private:
	int m_nRefCount; // Reference count
	ReferenceCounter& operator=(const ReferenceCounter&);
	ReferenceCounter(const ReferenceCounter&);

public:
	ReferenceCounter() : m_nRefCount(0)
	{
	}

	void AddRef()
	{
		// Increment the reference count
		m_nRefCount++;
		std::cout<<"Referrence Count: "<<m_nRefCount<<std::endl;
	}

	int Release()
	{
		// Decrement the reference count and
		// return the reference count.
		return --m_nRefCount;
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

//	deleter_impl(P p, D& _px) : ptr(p), del(_px) {};
//	//deleter_impl(P p) : ptr(p), del() {};

	deleter_impl( P p, D& d ): ptr( p ), del( d )
    {
    }

	deleter_impl( P p ): ptr( p ), del()
    {
    }

	virtual ~deleter_impl()
	{
		if (NULL != ptr)
		{
			del(ptr);
		}
	}

};

class SmartPtrDeleter
{
private:
	deleter_base* m_pDeleter;
public:
	SmartPtrDeleter() : m_pDeleter(NULL)
	{}

    template<class P, class D>
    SmartPtrDeleter( P p, D d ): m_pDeleter(0)
	{
    	try
    	{
        	m_pDeleter = new deleter_impl<P, D>(p, d);
    	}
    	catch(...)
    	{
    		d(p);
    	}
	}


};

template < typename T >
class shared_ptr
{
public:
	//typedef void (*UserDleter)(T*);
	typedef typename sp_element< T >::type element_type;

private:
	element_type*    m_pData;       // pointer
	ReferenceCounter* m_reference; // Reference count
	deleter_base* m_pDeleter;

	SmartPtrDeleter m_Deletor;

	//void (*m_UsrDeleter)(T*);

public:

	shared_ptr() :
		m_pData(NULL),
		m_reference(NULL),
		m_pDeleter(NULL),
		m_Deletor()
		//m_UsrDeleter(NULL)
	{
		// Create a new reference 
		m_reference = new ReferenceCounter();
		// Increment the reference count
		m_reference->AddRef();
	}

//	shared_ptr(T* pValue, UserDleter pDeleter = NULL) :
//		m_pData(pValue),
//		m_reference(0),
////		m_pDeleter(NULL),
//		m_UsrDeleter(pDeleter)
//	{
//		// Create a new reference
//		m_reference = new ReferenceCounter();
//		// Increment the reference count
//		m_reference->AddRef();
//	}

	template<typename Y>
	shared_ptr(Y* pValue) :
		m_pData(pValue),
		m_reference(NULL),
		m_pDeleter(NULL),
		m_Deletor()
	{
		// Create a new reference
		m_reference = new ReferenceCounter();
		// Increment the reference count
		m_reference->AddRef();
	}

	template<typename Y, typename D >
	shared_ptr(Y* pValue, D del) :
		m_pData(pValue),
		m_reference(0),
		m_Deletor(pValue, del)
	{
		// Create a new reference
		m_reference = new ReferenceCounter();
		// Increment the reference count
		m_reference->AddRef();
		m_pDeleter = new deleter_impl<Y*, D>(pValue, del);
	}

	template< typename Y >
	shared_ptr(const shared_ptr<Y>& sp) :
		m_pData(sp.m_pData),
		m_reference(sp.m_reference),
		m_pDeleter(sp.m_pDeleter)
		//m_UsrDeleter(sp.m_UsrDeleter)
	{
		// Copy constructor
		// Copy the data and reference pointer
		// and increment the reference count
		m_reference->AddRef();
	}

	void clearTargetObj()
	{
		if(NULL != m_pData )
		{
			//if (NULL != m_UsrDeleter)
			if(NULL != m_pDeleter)
			{
				//m_pDeleter(m_pData);
				delete m_pDeleter;
			}
			else
			{
				std::cout<< "Dtor called" << std::endl;
				delete m_pData;	// For non-array pointer
			}
		}

		if(NULL != m_reference)
			delete m_reference;

	}

	~shared_ptr()
	{
		// Destructor
		// Decrement the reference count
		// if reference become zero delete the data
		if(0 == m_reference->Release())
		{
			clearTargetObj();
		}
	}

//	template<typename D = default_delete<T> >
//	void reset(T* pValue, D deletor)
//	{
//		if(m_pData != pValue)
//		{
//			if( 0 == m_reference->Release())
//			{
//				clearTargetObj();
//			}
//
//			m_pData = pValue;
//			m_pDeleter = new deleter_impl<T*, D>(pValue, deletor);
//			m_reference = new ReferenceCounter();
//			m_reference->AddRef();
//		}
//	}

	T& operator* () const
	{
		return *m_pData;
	}

	T* operator-> () const
	{
		return m_pData;
	}

	T* get() const
	{
		return m_pData;
	}

	operator void*()
	{
		if(NULL != m_pData)
			return static_cast<void*>(1);
		else
			return static_cast<void*>(0);
	}

	shared_ptr<T>& operator = (const shared_ptr<T>& sp)
	{
		// Assignment operator
		if (this != &sp) // Avoid self assignment
		{
			// Decrement the old reference count
			// if reference become zero delete the old data
			if(m_reference->Release() == 0)
			{
				clearTargetObj();
			}

			// Copy the data and reference pointer
			// and increment the reference count
			m_pData = sp.m_pData;
			//m_UsrDeleter = sp.m_UsrDeleter;
			m_pDeleter = sp.m_pDeleter;
			m_reference = sp.m_reference;
			m_reference->AddRef();
		}
		return *this;
	}
};

template<typename T, typename D = default_delete<T> >
class unique_ptr
{
private:
	T* m_pData;
	D m_pUsrDeleter;
public:
	unique_ptr() :
		m_pData(NULL)
	{}

	unique_ptr(T* pData) :
		m_pData(pData)
	{}

	unique_ptr(T* pData, D pUsrDeleter) :
		m_pData(pData),
		m_pUsrDeleter(pUsrDeleter)
	{}

	unique_ptr(const unique_ptr& up)
	{
		this->m_pData = up.m_pData;
		up.m_pData = NULL;
	}

	~unique_ptr()
	{
		if(NULL!=m_pData)
		{
			m_pUsrDeleter(m_pData);
		}
	}
	const unique_ptr& operator=(const unique_ptr& up) // move
	{
		this->m_pData = up.m_pData;
		return (*this);
	}

	T* get() const
	{
		return m_pData;
	}

	void reset(T* pData)
	{
		if(m_pData == pData)
		{
			return;
		}

		if(NULL != m_pData)
		{
			m_pUsrDeleter(m_pData);
		}
		m_pData = pData;
	}

	void reset(T* pData, D pUsrDeleter)
	{
		if(m_pData == pData)
		{
			return;
		}

		if(NULL != m_pData)
		{
			m_pUsrDeleter(m_pData);
		}
		m_pData = pData;
		m_pUsrDeleter = pUsrDeleter;
	}

};

} // hhi

template<typename T>
struct ArrayDeleter
{      
	void operator () (T* p)
	{
		std::cout << "ArrayDeleter" << std::endl;
		delete [] p;
	}
};
