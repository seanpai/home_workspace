
template<typename T>
int StringFormat(std::basic_string<T>& txt, const T* szFormat,...)
{
	const static unsigned int _MAX_CHARS = 512;
	std::vector<T> _buffer(_MAX_CHARS);
	va_list argList;
	va_start(argList,szFormat);

	int ret = 0;
	if(typeid(T) == typeid(wchar_t))
	{
		ret = _vsnwprintf_s(&_buffer[0],_MAX_CHARS,_TRUNCATE,szFormat,argList);
	}

	va_end(argList);
	txt.assign(&_buffer[0],ret);
	return ret;
}

extern CRITICAL_SECTION g_cs_FormatString;
template<typename T>
int StringFormat(std::basic_string<T>& txt, const T* szFormat,...)
{
	const static unsigned int _MAX_CHARS = 512;
#if 0 // Don't use it any more because it would cause the main loop to be in dead-lock state.
	LOCK_GUARD LockGuard(g_cs_FormatString);
#endif
	std::vector<T> _buffer(_MAX_CHARS);
	va_list argList;
	va_start(argList,szFormat);

	int ret = 0;
	if(typeid(T) == typeid(wchar_t))
	{
		ret = _vsnwprintf_s(&_buffer[0],_MAX_CHARS,_TRUNCATE,szFormat,argList);
	}

	va_end(argList);
	txt.assign(&_buffer[0],ret);
	return ret;
}

extern CRITICAL_SECTION g_cs_PutMsg;
int OutputDebugMsg(LPCWSTR lpszFormat, ...)
{
	static const unsigned long MAX_SIZE_MSG_BUFFER = 512;
	
#if 0 // Don't use it any more because it would cause the main loop to be in dead-lock state.
	LOCK_GUARD LockGuard(g_cs_PutMsg);
#endif	
	std::vector<wchar_t> _buffer(MAX_SIZE_MSG_BUFFER);
	std::wstring strMsg = L"";
	va_list ArgList;
	int ret = 0;

	va_start(ArgList, lpszFormat);
	ret = _vsnwprintf_s(&_buffer[0], MAX_SIZE_MSG_BUFFER,  _TRUNCATE, lpszFormat, ArgList);

#ifdef DEBUG
	TRACE(&_buffer[0]);
#else
	OutputDebugString(&_buffer[0]);
#endif

	va_end(ArgList);
	return ret;
}



