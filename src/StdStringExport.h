#ifndef STDSTRING_H
#define STDSTRING_H

typedef const char* PCSTR;
typedef char* PSTR;

/* In RageUtil: */
void MakeUpper( char *p, size_t iLen );
void MakeLower( char *p, size_t iLen );
void MakeUpper( wchar_t *p, size_t iLen );
void MakeLower( wchar_t *p, size_t iLen );

namespace StdString
{

	inline char sstoupper(char ch);
	inline char sstolower(char ch);


template<typename CT>
class CStdStr;


template<typename CT>
inline
CStdStr<CT> operator+(const  CStdStr<CT>& str1, const  CStdStr<CT>& str2);

template<typename CT>	
inline

CStdStr<CT> operator+(const  CStdStr<CT>& str, CT t);
/**
 * @brief Another way to concatenate two strings together.
 * @param str the original string.
 * @param pA the string to be added.
 * @return the longer string.
 */
template<typename CT>
inline
CStdStr<CT> operator+(const  CStdStr<CT>& str, PCSTR pA);

/**
 * @brief Another way to concatenate two strings together.
 * @param pA the original string.
 * @param str the string to be added.
 * @return the longer string.
 */	
template<typename CT>
inline
CStdStr<CT> operator+(PCSTR pA, const  CStdStr<CT>& str);

/** @brief Our wrapper for std::string. */
template<typename CT>
class CStdStr : public std::basic_string<CT>
{
	// Typedefs for shorter names.  Using these names also appears to help
	// us avoid some ambiguities that otherwise arise on some platforms

	typedef typename std::basic_string<CT>		MYBASE;	 // my base class
	typedef CStdStr<CT>				MYTYPE;	 // myself
	typedef typename MYBASE::const_pointer		PCMYSTR; // PCSTR 
	typedef typename MYBASE::pointer		PMYSTR;	 // PSTR
	typedef typename MYBASE::iterator		MYITER;  // my iterator type
	typedef typename MYBASE::const_iterator		MYCITER; // you get the idea...
	typedef typename MYBASE::reverse_iterator	MYRITER;
	typedef typename MYBASE::size_type		MYSIZE;   
	typedef typename MYBASE::value_type		MYVAL; 
	typedef typename MYBASE::allocator_type		MYALLOC;
	typedef typename MYBASE::traits_type		MYTRAITS;
	
public:

	bool track = false;

	// CStdStr inline constructors
	CStdStr();

	~CStdStr();


	CStdStr(const MYTYPE& str);
	CStdStr(const std::string& str);

	CStdStr(PCMYSTR pT, MYSIZE n);

	CStdStr(PCSTR pA);

	CStdStr(MYCITER first, MYCITER last);

	CStdStr(MYSIZE nSize, MYVAL ch, const MYALLOC& al = MYALLOC());

	// CStdStr inline assignment operators -- the ssasn function now takes care
	// of fixing  the MSVC assignment bug (see knowledge base article Q172398).
	MYTYPE& operator=(const MYTYPE& str);

	MYTYPE& operator=(const std::string& str);

	MYTYPE& operator=(PCSTR pA);

	MYTYPE& operator=(CT t);

	MYTYPE& operator+=(const MYTYPE& str);

	MYTYPE& operator+=(const std::string& str);

	MYTYPE& operator+=(PCSTR pA);

	MYTYPE& operator+=(CT t);


	// addition operators -- global friend functions.

#if defined(_MSC_VER) && _MSC_VER < 1300 /* VC6, not VC7 */
/* work around another stupid vc6 bug */
#define EMP_TEMP
#else
#define EMP_TEMP <>
#endif
	friend	MYTYPE	operator+ EMP_TEMP(const MYTYPE& str1,	const MYTYPE& str2);
	friend	MYTYPE	operator+ EMP_TEMP(const MYTYPE& str,	CT t);
	friend	MYTYPE	operator+ EMP_TEMP(const MYTYPE& str,	PCSTR sz);
	friend	MYTYPE	operator+ EMP_TEMP(PCSTR pA,				const MYTYPE& str);

	// -------------------------------------------------------------------------
	// Case changing functions
	// -------------------------------------------------------------------------
	MYTYPE& MakeUpper();

	MYTYPE& MakeLower();



	// -------------------------------------------------------------------------
	// CStdStr -- Direct access to character buffer.  In the MS' implementation,
	// the at() function that we use here also calls _Freeze() providing us some
	// protection from multithreading problems associated with ref-counting.
	// -------------------------------------------------------------------------
	CT* GetBuffer(int nMinLen = -1);

	void ReleaseBuffer(int nNewLen = -1);

	

	// -------------------------------------------------------------------------
	// RString Facade Functions:
	//
	// The following methods are intended to allow you to use this class as a
	// drop-in replacement for CString.
	// -------------------------------------------------------------------------
	int CompareNoCase(PCMYSTR szThat)	const;

	bool EqualsNoCase(PCMYSTR szThat)	const;

	MYTYPE Left(int nCount) const;

	int Replace(CT chOld, CT chNew);

	int Replace(PCMYSTR szOld, PCMYSTR szNew);

	MYTYPE Right(int nCount) const;

	CT& operator[](int nIdx);

	const CT& operator[](int nIdx) const;

	CT& operator[](unsigned int nIdx);

	const CT& operator[](unsigned int nIdx) const;

	CT& operator[](long unsigned int nIdx);

	const CT& operator[](long unsigned int nIdx) const;

	operator const CT* () const;
};


typedef CStdStr<char>		CStdStringA;
#define CStdString				CStdStringA

#define StdStringLessNoCase		SSLNCA
#define StdStringEqualsNoCase		SSENCA


struct StdStringLessNoCase;
struct StdStringEqualsNoCase;

}

#endif	// #ifndef STDSTRING_H
