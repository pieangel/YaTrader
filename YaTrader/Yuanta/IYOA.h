#ifndef _IYOA_H_
#define _IYOA_H_

#include "CommDef.h"

//------------------------------------------------------------------------------
#pragma pack( push, 1 )

enum DATA_TYPE {
	TYPE_STRING,
	TYPE_LONG,
	TYPE_LONG64,
	TYPE_DOUBLE,
	TYPE_FLOAT,
	TYPE_SHORT,
	TYPE_WORD,
	TYPE_BYTE
};

// 조회TR 수신 Data
typedef struct _RECV_DATA
{
	_RECV_DATA() { ZeroMemory( this, sizeof(_RECV_DATA) ); }

	char*				lpData;
	int					nRqID;						// Request ID
	int					nDataLength;				// 받은 데이터 크기
	char				szTrCode	[24+1];			// TR Code
	
} RECV_DATA, *LPRECV_DATA;

class IYOAMethod  
{
public:
	IYOAMethod () { ZeroMemory( this, sizeof( IYOAMethod ) ); }
	~IYOAMethod() { }

protected:
	// 초기화
	typedef long	( __stdcall *FP_INITIAL					) ( LPCTSTR, HWND, LPCTSTR, long, BOOL );
	typedef long	( __stdcall *FP_UNINITIAL				) ();
	typedef long	( __stdcall *FP_SETHWND					) ( HWND );
	typedef void	( __stdcall *FP_SETCONFIG				) ( long, long );

	// 서버 연결
	typedef BOOL	( __stdcall *FP_ISCONNECTED				) ( );
	typedef BOOL	( __stdcall *FP_DISCONNECT				) ( );

	// 로그인
	typedef long	( __stdcall *FP_LOGIN					) ( HWND, LPCTSTR, LPCTSTR, LPCTSTR );
	typedef long	( __stdcall *FP_LOGOUT					) ( HWND );

	// 조회
	typedef long	( __stdcall *FP_REQUEST					) ( HWND, LPCTSTR, BOOL, long );
	typedef long	( __stdcall *FP_REQUESTDATA				) ( HWND, LPCTSTR, void*, long, BOOL, long );
	typedef long	( __stdcall *FP_RESET					) ( LPCTSTR );

	typedef long	( __stdcall *FP_SETTRFIELDSTRING		) ( LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, long );
	typedef long	( __stdcall *FP_SETTRFIELDLONG			) ( LPCTSTR, LPCTSTR, LPCTSTR, long, long );
	typedef long	( __stdcall *FP_SETTRFIELDLONG64		) ( LPCTSTR, LPCTSTR, LPCTSTR, INT64, long );
	typedef long	( __stdcall *FP_SETTRFIELDDOUBLE		) ( LPCTSTR, LPCTSTR, LPCTSTR, double, long );
	typedef long	( __stdcall *FP_SETTRFIELDFLOAT			) ( LPCTSTR, LPCTSTR, LPCTSTR, float, long );
	typedef long	( __stdcall *FP_SETTRFIELDWORD			) ( LPCTSTR, LPCTSTR, LPCTSTR, WORD, long );
	typedef long	( __stdcall *FP_SETTRFIELDBYTE			) ( LPCTSTR, LPCTSTR, LPCTSTR, byte, long );
	typedef long	( __stdcall *FP_SETTRINFO				) ( LPCTSTR, LPCTSTR );
	typedef long	( __stdcall *FP_SETFIELDSTRING			) ( LPCTSTR, LPCTSTR, long );
	typedef long	( __stdcall *FP_SETFIELDLONG			) ( LPCTSTR, long, long );
	typedef long	( __stdcall *FP_SETFIELDLONG64			) ( LPCTSTR, INT64, long );
	typedef long	( __stdcall *FP_SETFIELDDOUBLE			) ( LPCTSTR, double, long );
	typedef long	( __stdcall *FP_SETFIELDFLOAT			) ( LPCTSTR, float, long );
	typedef long	( __stdcall *FP_SETFIELDWORD			) ( LPCTSTR, WORD, long );
	typedef long	( __stdcall *FP_SETFIELDBYTE			) ( LPCTSTR, byte, long );

	typedef long	( __stdcall *FP_GETROWCOUNT				) ( LPCTSTR, LPCTSTR );
	typedef long	( __stdcall *FP_GETTRFIELDATTR			) ( LPCTSTR, LPCTSTR, LPCTSTR, byte*, long );
	typedef long	( __stdcall *FP_GETTRFIELDSTRING		) ( LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, long, long );
	typedef long	( __stdcall *FP_GETTRFIELDLONG			) ( LPCTSTR, LPCTSTR, LPCTSTR, long*, long );
	typedef long	( __stdcall *FP_GETTRFIELDLONG64		) ( LPCTSTR, LPCTSTR, LPCTSTR, INT64*, long );
	typedef long	( __stdcall *FP_GETTRFIELDDOUBLE		) ( LPCTSTR, LPCTSTR, LPCTSTR, double*, long );
	typedef long	( __stdcall *FP_GETTRFIELDFLOAT			) ( LPCTSTR, LPCTSTR, LPCTSTR, float*, long );
	typedef long	( __stdcall *FP_GETTRFIELDWORD			) ( LPCTSTR, LPCTSTR, LPCTSTR, WORD*, long );
	typedef long	( __stdcall *FP_GETTRFIELDBYTE			) ( LPCTSTR, LPCTSTR, LPCTSTR, byte*, long );
	typedef long	( __stdcall *FP_GETROWCOUNTBYREQID		) ( long, LPCTSTR );
	typedef long	( __stdcall *FP_GETFIELDATTR			) ( LPCTSTR, byte*, long );
	typedef long	( __stdcall *FP_GETFIELDSTRING			) ( LPCTSTR, LPCTSTR, long, long );
	typedef long	( __stdcall *FP_GETFIELDLONG			) ( LPCTSTR, long*, long );
	typedef long	( __stdcall *FP_GETFIELDLONG64			) ( LPCTSTR, INT64*, long );
	typedef long	( __stdcall *FP_GETFIELDDOUBLE			) ( LPCTSTR, double*, long );
	typedef long	( __stdcall *FP_GETFIELDFLOAT			) ( LPCTSTR, float*, long );
	typedef long	( __stdcall *FP_GETFIELDWORD			) ( LPCTSTR, WORD*, long );
	typedef long	( __stdcall *FP_GETFIELDBYTE			) ( LPCTSTR, byte*, long );

	typedef long	( __stdcall *FP_GETFIELDSTRINGBYREQID	) ( long, LPCTSTR, LPCTSTR, LPCTSTR, long, long );
	typedef long	( __stdcall *FP_RELEASEDATA				) ( long );

	// 실시간
	typedef long	( __stdcall *FP_REGISTAUTO				) ( HWND, LPCTSTR );
	typedef long	( __stdcall *FP_UNREGISTAUTO			) ( HWND, LPCTSTR );
	typedef long	( __stdcall *FP_UNREGISTAUTOWITHKEY		) ( HWND, LPCTSTR, LPCTSTR );
	typedef long	( __stdcall *FP_UNREGISTAUTOWITHREQID	) ( long );
	typedef long	( __stdcall *FP_UNREGISTAUTOWITHWINDOW	) ( HWND );

	// 계좌
	typedef long	( __stdcall *FP_GETACCOUNTCOUNT			) ( );
	typedef long	( __stdcall *FP_GETACCOUNT				) ( long, LPCTSTR, long );
	typedef long	( __stdcall *FP_GETACCOUNTINFO			) ( long, LPCTSTR, LPCTSTR, long );

	// 계좌
	typedef void	( __stdcall *FP_GETSERVERNAME			) ( LPSTR, long );
	typedef long	( __stdcall *FP_GETCODECOUNT			) ( long );
	typedef long	( __stdcall *FP_GETCODEINFOBYINDEX		) ( long, long, long, LPCTSTR, long );
	typedef long	( __stdcall *FP_GETCODEINFO				) ( long, long, LPCTSTR, LPCTSTR, long );
	typedef long	( __stdcall *FP_GETLASTERROR			) ( );
	typedef long	( __stdcall *FP_GETERRORMESSAGE			) ( long, LPCTSTR, long );
	typedef long	( __stdcall *FP_SETTIMEOUT				) ( long );

	FP_INITIAL					m_fpInitial;
	FP_UNINITIAL				m_fpUnInitial;
	FP_ISCONNECTED				m_fpIsConnected;
	FP_DISCONNECT				m_fpDisconnect;
	FP_SETHWND					m_fpSetHWnd;
	FP_SETCONFIG				m_fpSetConfig;

	FP_LOGIN					m_fpLogin;
	FP_LOGOUT					m_fpLogout;	

	FP_REQUEST					m_fpRequest;
	FP_REQUESTDATA				m_fpRequestData;
	FP_RESET					m_fpReset;

	FP_SETTRFIELDSTRING			m_fpSetTRFieldString;
	FP_SETTRFIELDLONG			m_fpSetTRFieldLong;
	FP_SETTRFIELDLONG64			m_fpSetTRFieldLong64;
	FP_SETTRFIELDDOUBLE			m_fpSetTRFieldDouble;
	FP_SETTRFIELDFLOAT			m_fpSetTRFieldFloat;
	FP_SETTRFIELDWORD			m_fpSetTRFieldWord;
	FP_SETTRFIELDBYTE			m_fpSetTRFieldByte;
	FP_SETTRINFO				m_fpSetTRInfo;
	FP_SETFIELDSTRING			m_fpSetFieldString;
	FP_SETFIELDLONG				m_fpSetFieldLong;
	FP_SETFIELDLONG64			m_fpSetFieldLong64;
	FP_SETFIELDDOUBLE			m_fpSetFieldDouble;
	FP_SETFIELDFLOAT			m_fpSetFieldFloat;
	FP_SETFIELDWORD				m_fpSetFieldWord;
	FP_SETFIELDBYTE				m_fpSetFieldByte;

	FP_GETROWCOUNT				m_fpGetRowCount;
	FP_GETTRFIELDATTR			m_fpGetTRFieldAttr;
	FP_GETTRFIELDSTRING			m_fpGetTRFieldString;
	FP_GETTRFIELDLONG			m_fpGetTRFieldLong;
	FP_GETTRFIELDLONG64			m_fpGetTRFieldLong64;
	FP_GETTRFIELDDOUBLE			m_fpGetTRFieldDouble;
	FP_GETTRFIELDFLOAT			m_fpGetTRFieldFloat;
	FP_GETTRFIELDWORD			m_fpGetTRFieldWord;
	FP_GETTRFIELDBYTE			m_fpGetTRFieldByte;
	FP_GETFIELDATTR				m_fpGetFieldAttr;
	FP_GETFIELDSTRING			m_fpGetFieldString;
	FP_GETFIELDLONG				m_fpGetFieldLong;
	FP_GETFIELDLONG64			m_fpGetFieldLong64;
	FP_GETFIELDDOUBLE			m_fpGetFieldDouble;
	FP_GETFIELDFLOAT			m_fpGetFieldFloat;
	FP_GETFIELDWORD				m_fpGetFieldWord;
	FP_GETFIELDBYTE				m_fpGetFieldByte;
	FP_GETROWCOUNTBYREQID		m_fpGetRowCountByReqID;
	FP_GETFIELDSTRINGBYREQID	m_fpGetFieldStringByReqID;
	FP_RELEASEDATA				m_fpReleaseData;

	FP_REGISTAUTO				m_fpRegistAuto;
	FP_UNREGISTAUTO				m_fpUnregistAuto;
	FP_UNREGISTAUTOWITHKEY		m_fpUnregistAutoWithKey;
	FP_UNREGISTAUTOWITHREQID	m_fpUnregistAutoWithReqID;
	FP_UNREGISTAUTOWITHWINDOW	m_fpUnregistAutoWithWindow;

	FP_GETACCOUNTCOUNT			m_fpGetAccountCount;
	FP_GETACCOUNT				m_fpGetAccount;
	FP_GETACCOUNTINFO			m_fpGetAccountInfo;

	FP_GETSERVERNAME			m_fpGetServerName;
	FP_GETLASTERROR				m_fpGetLastError;
	FP_GETERRORMESSAGE			m_fpGetErrorMessage;
	FP_GETCODECOUNT				m_fpGetCodeCount;
	FP_GETCODEINFOBYINDEX		m_fpGetCodeInfoByIndex;
	FP_GETCODEINFO				m_fpGetCodeInfo;
	FP_SETTIMEOUT				m_fpSetTimeout;
};

class IYOA : public IYOAMethod
{
public:
	IYOA () { 
		IYOAMethod ();
		m_hModule	= NULL;
		m_hWnd		= NULL;
		m_strPath	= _T("");
	}
	~IYOA() { }

protected:
	HMODULE			m_hModule;
	HWND			m_hWnd;
	CString			m_strPath;

protected:
	inline	int		LoadLibHelper		( LPCTSTR szPath );

public:
	// 초기화 관련
	inline	long	YOA_Initial					( LPCTSTR szURL, HWND hWnd, LPCTSTR szPath=NULL, long nStartMsgID=WM_USER, BOOL bUnicode=TRUE );
	inline	long	YOA_UnInitial				();
	inline	BOOL	YOA_IsInit					();
	inline	long	YOA_SetHWnd					( HWND hWnd );
	inline	void	YOA_SetConfig				( long nKey, long nValue );

	// 서버연결
	inline	BOOL	YOA_IsConnected				();
	inline	BOOL	YOA_Disconnect				();

	// 로그인
	inline	long	YOA_Login					( HWND hWnd, LPCTSTR pszUserID, LPCTSTR pszUserPW, LPCTSTR pszCertPW );
	inline	long	YOA_Logout					( HWND hWnd );

	// 조회
	inline	long	YOA_Request					( HWND hWnd, LPCTSTR pszDSOID, BOOL bReleaseData=TRUE, long nNextReqID=-1 );
	inline	long	YOA_RequestData				( HWND hWnd, LPCTSTR pszDSOID, void* lpData, long nDataSize, BOOL bReleaseData=TRUE, long nNextReqID=-1 );
	inline	long	YOA_Reset					( LPCTSTR pszDSOID );

	inline	long	YOA_SetTRFieldString		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldLong			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, long nValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldLong64		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, INT64 nValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldDouble		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, double dValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldFloat			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, float fValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldWord			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, WORD wValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRFieldByte			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte byteValue, long nOccursIndex=0 );
	inline	long	YOA_SetTRInfo				( LPCTSTR pszDSOID, LPCTSTR pszBlockName );
	inline	long	YOA_SetFieldString			( LPCTSTR pszFieldName, LPCTSTR pszValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldLong			( LPCTSTR pszFieldName, long nValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldLong64			( LPCTSTR pszFieldName, INT64 nValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldDouble			( LPCTSTR pszFieldName, double dValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldFloat			( LPCTSTR pszFieldName, float fValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldWord			( LPCTSTR pszFieldName, WORD wValue, long nOccursIndex=0 );
	inline	long	YOA_SetFieldByte			( LPCTSTR pszFieldName, byte byteValue, long nOccursIndex=0 );

	inline	long	YOA_GetRowCount				( LPCTSTR pszDSOID, LPCTSTR pszBlockName );
	inline	long	YOA_GetTRFieldAttr			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldString		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldLong			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, long* nValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldLong64		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, INT64* nValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldDouble		( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, double* dValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldFloat			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, float* fValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldWord			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, WORD* wValue, long nOccursIndex=0 );
	inline	long	YOA_GetTRFieldByte			( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldAttr			( LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldString			( LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex=0 );
	inline	long	YOA_GetFieldLong			( LPCTSTR pszFieldName, long* nValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldLong64			( LPCTSTR pszFieldName, INT64* nValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldDouble			( LPCTSTR pszFieldName, double* dValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldFloat			( LPCTSTR pszFieldName, float* fValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldWord			( LPCTSTR pszFieldName, WORD* wValue, long nOccursIndex=0 );
	inline	long	YOA_GetFieldByte			( LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex=0 );
	inline	long	YOA_GetRowCountByReqID		( long nReqID, LPCTSTR pszBlockName );
	inline	long	YOA_GetFieldStringByReqID	( long nReqID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex=0 );
	inline	long	YOA_ReleaseData				( long nRequestID );

	// 실시간
	inline	long	YOA_RegistAuto				( HWND hWnd, LPCTSTR pszAutoID );
	inline	long	YOA_UnregistAuto			( HWND hWnd, LPCTSTR pszAutoID );
	inline	long	YOA_UnregistAutoWithKey		( HWND hWnd, LPCTSTR pszAutoID, LPCTSTR pszKeys );
	inline	long	YOA_UnregistAutoWithReqID	( long nReqID );
	inline	long	YOA_UnregistAutoWithWindow	( HWND hWnd );

	// 계좌
	inline	long	YOA_GetAccountCount			();
	inline	long	YOA_GetAccount				( long nIndex, LPCTSTR pszAcc, long nAccSize );
	inline	long	YOA_GetAccountInfo			( long nInfoType, LPCTSTR pszAccount, LPCTSTR pszAccountInfoBuf, long nAccountInfoBufSize );

	// 정보
	inline	long	YOA_GetServerName			( LPSTR pszServerNameBuf, long nServerNameBufSize );
	inline	long	YOA_GetLastError			();
	inline	long	YOA_GetErrorMessage			( long nErrorCode, LPCTSTR pszErrorMessageBuf, long nErrorMessageBufSize );
	inline	long	YOA_GetCodeCount			( long nMerketType );
	inline	long	YOA_GetCodeInfoByIndex		( long nMerketType, long nInfoType, long nIndex, LPCTSTR pszCodeInfoBuf, long nCodeInfoBufSize );
	inline	long	YOA_GetCodeInfo				( long nMerketType, long nInfoType, LPCTSTR pszCode, LPCTSTR pszCodeInfoBuf, long nCodeInfoBufSize );
	inline	long	YOA_SetTimeout				( long nInterval );
	inline	void	YOA_ReStart					();

	inline  void	SetPacketString				( char* pData, int nSize, int nType, CString strValue );
	inline  void	SetPacketLong				( char* pData, int nSize, int nType, long nValue );
	inline  void	SetPacketLong64				( char* pData, int nSize, int nType, INT64 nLValue );
	inline  void	SetPacketDouble				( char* pData, int nSize, int nType, double dValue );
	inline  void	SetPacketFloat				( char* pData, int nSize, int nType, float fValue );
	inline  void	SetPacketWord				( char* pData, int nSize, int nType, WORD wValue );
	inline  void	SetPacketByte				( char* pData, int nSize, int nType, byte byteValue );

	inline	CString GetPacketString				( TCHAR* pData, int nSize, int nType );
	inline	long	GetPacketLong				( char* pData, int nSize, int nType );
	inline	INT64	GetPacketLong64				( char* pData, int nSize, int nType );
	inline	double	GetPacketDouble				( char* pData, int nSize, int nType );
	inline	float	GetPacketFloat				( char* pData, int nSize, int nType );
	inline	WORD	GetPacketWord				( char* pData, int nSize, int nType );
	inline	byte	GetPacketByte				( char* pData, int nSize, int nType );
	inline	CString GetPacketDBLString			( char* pData, int nSize, int nType, int nDotPos );
	
};

long IYOA::YOA_Initial( LPCTSTR szURL, HWND hWnd, LPCTSTR szPath/*=NULL*/, long nStartMsgID/*=WM_USER*/, BOOL bUnicode/*=TRUE*/ )
{
	if( YOA_IsInit() ) 
		return RESULT_SUCCESS;

	m_hWnd = hWnd;
	if ( NULL != szPath )
		m_strPath.Format( _T("%s"), szPath );

	BOOL bResult = LoadLibHelper( szPath );

	if ( bResult )
	{
		if( NULL == m_fpInitial ) 
			m_fpInitial = (FP_INITIAL)GetProcAddress(m_hModule, "YOA_Initial");

		ASSERT( m_fpInitial );

		if( NULL == m_fpInitial ) 
			return ERROR_FUNCTION_NOT_FOUND;

		return m_fpInitial( szURL, hWnd, szPath, nStartMsgID, bUnicode );
	}
	else
		return ERROR_MODULE_NOT_FOUND;
}


long IYOA::YOA_UnInitial()
{
	if( NULL == m_hModule ) 
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpUnInitial ) 
		m_fpUnInitial = (FP_UNINITIAL)GetProcAddress(m_hModule, "YOA_UnInitial");

	ASSERT( m_fpUnInitial );

	if( NULL == m_fpUnInitial ) 
		return ERROR_FUNCTION_NOT_FOUND;

	long nResult = m_fpUnInitial();

	::FreeLibrary( m_hModule );

	m_hModule = NULL;

	return nResult;
}

BOOL IYOA::LoadLibHelper( LPCTSTR szPath )
{
	TCHAR szCommLib[MAX_PATH] = { 0 };

	if( NULL == szPath || 0 == lstrlen( szPath ) )
	{
		lstrcpy( szCommLib, _T( "YuantaAPI.dll" ) );
	}
	else
	{
		_stprintf_s( szCommLib, sizeof(szCommLib)/sizeof(TCHAR), _T( "%s\\YuantaAPI.dll" ), szPath );
	}

	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetCurrentDirectory( MAX_PATH, szCurrentDir );
	SetCurrentDirectory( szPath );
	m_hModule = ::LoadLibrary( szCommLib );
	SetCurrentDirectory( szCurrentDir );

	return NULL == m_hModule ? FALSE : TRUE;
}

long IYOA::YOA_SetHWnd( HWND hWnd )
{
	if( NULL == m_hModule ) 
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetHWnd ) 
		m_fpSetHWnd = (FP_SETHWND)GetProcAddress(m_hModule, "YOA_SetHWnd");

	ASSERT( m_fpSetHWnd );

	if( NULL == m_fpSetHWnd ) 
		return ERROR_FUNCTION_NOT_FOUND;
	
	m_hWnd = hWnd;

	return m_fpSetHWnd( hWnd );
}

void IYOA::YOA_SetConfig( long nKey, long nValue )
{
	if( NULL == m_hModule ) 
		return;

	if( NULL == m_fpSetConfig ) 
		m_fpSetConfig = (FP_SETCONFIG)GetProcAddress(m_hModule, "YOA_SetConfig");

	ASSERT( m_fpSetConfig );

	if( NULL == m_fpSetConfig ) 
		return;

	return m_fpSetConfig( nKey, nValue );
}

BOOL IYOA::YOA_IsInit()
{ 
	return m_hModule != NULL; 
}

BOOL IYOA::YOA_IsConnected()
{
	ASSERT( m_hModule );

	if( NULL == m_hModule ) 
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpIsConnected ) 
		m_fpIsConnected = (FP_ISCONNECTED)GetProcAddress(m_hModule, "YOA_IsConnected");

	ASSERT( m_fpIsConnected );

	if( NULL == m_fpIsConnected ) 
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpIsConnected();
}

BOOL IYOA::YOA_Disconnect()
{
	ASSERT( m_hModule );

	if( NULL == m_hModule ) 
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpDisconnect ) 
		m_fpDisconnect = (FP_DISCONNECT)GetProcAddress(m_hModule, "YOA_Disconnect");

	ASSERT( m_fpDisconnect );

	if( NULL == m_fpDisconnect ) 
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpDisconnect();
}

long IYOA::YOA_Login( HWND hWnd, LPCTSTR pszUserID, LPCTSTR pszUserPW, LPCTSTR pszCertPW )
{
	ASSERT( m_hModule );

	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpLogin )	
		m_fpLogin = (FP_LOGIN)GetProcAddress(m_hModule, "YOA_Login");

	ASSERT( m_fpLogin );

	if( NULL == m_fpLogin )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpLogin( hWnd, pszUserID, pszUserPW, pszCertPW );
}

long IYOA::YOA_Logout( HWND hWnd )
{
	ASSERT( m_hModule );

	if( NULL == m_hModule ) 
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpLogout ) 
		m_fpLogout = (FP_LOGOUT)GetProcAddress(m_hModule, "YOA_Logout");

	ASSERT( m_fpLogout );

	if( NULL == m_fpLogout ) 
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpLogout( hWnd );
}

long IYOA::YOA_Request( HWND hWnd, LPCTSTR pszDSOID, BOOL bReleaseData/* =TRUE */, long nNextReqID /* =-1 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpRequest )	
		m_fpRequest = (FP_REQUEST)GetProcAddress(m_hModule, "YOA_Request");

	if( NULL == m_fpRequest )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpRequest(hWnd, pszDSOID, bReleaseData, nNextReqID);
}

long IYOA::YOA_RequestData( HWND hWnd, LPCTSTR pszDSOID, void* lpData, long nDataSize, BOOL bReleaseData/* =TRUE */, long nNextReqID /* =-1 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpRequestData )	
		m_fpRequestData = (FP_REQUESTDATA)GetProcAddress(m_hModule, "YOA_RequestData");

	if( NULL == m_fpRequestData )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpRequestData(hWnd, pszDSOID, lpData, nDataSize, bReleaseData, nNextReqID);
}

long IYOA::YOA_Reset( LPCTSTR pszDSOID )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpReset )	
		m_fpReset = (FP_RESET)GetProcAddress(m_hModule, "YOA_Reset");

	if( NULL == m_fpReset )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpReset(pszDSOID);
}

long IYOA::YOA_SetTRFieldString( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldString )	
		m_fpSetTRFieldString = (FP_SETTRFIELDSTRING)GetProcAddress(m_hModule, "YOA_SetTRFieldString");

	if( NULL == m_fpSetTRFieldString )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldString(pszDSOID, pszBlockName, pszFieldName, pszValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldLong( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, long nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldLong )	
		m_fpSetTRFieldLong = (FP_SETTRFIELDLONG)GetProcAddress(m_hModule, "YOA_SetTRFieldLong");

	if( NULL == m_fpSetTRFieldLong )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldLong(pszDSOID, pszBlockName, pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldLong64( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, INT64 nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldLong64 )	
		m_fpSetTRFieldLong64 = (FP_SETTRFIELDLONG64)GetProcAddress(m_hModule, "YOA_SetTRFieldLong64");

	if( NULL == m_fpSetTRFieldLong64 )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldLong64(pszDSOID, pszBlockName, pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldDouble( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, double dValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldDouble )	
		m_fpSetTRFieldDouble = (FP_SETTRFIELDDOUBLE)GetProcAddress(m_hModule, "YOA_SetTRFieldDouble");

	if( NULL == m_fpSetTRFieldDouble )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldDouble(pszDSOID, pszBlockName, pszFieldName, dValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldFloat( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, float fValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldFloat )	
		m_fpSetTRFieldFloat = (FP_SETTRFIELDFLOAT)GetProcAddress(m_hModule, "YOA_SetTRFieldFloat");

	if( NULL == m_fpSetTRFieldFloat )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldFloat(pszDSOID, pszBlockName, pszFieldName, fValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldWord( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, WORD wValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldWord )	
		m_fpSetTRFieldWord = (FP_SETTRFIELDWORD)GetProcAddress(m_hModule, "YOA_SetTRFieldWord");

	if( NULL == m_fpSetTRFieldWord )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldWord(pszDSOID, pszBlockName, pszFieldName, wValue, nOccursIndex);
}

long IYOA::YOA_SetTRFieldByte( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRFieldByte )	
		m_fpSetTRFieldByte = (FP_SETTRFIELDBYTE)GetProcAddress(m_hModule, "YOA_SetTRFieldByte");

	if( NULL == m_fpSetTRFieldByte )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRFieldByte(pszDSOID, pszBlockName, pszFieldName, byteValue, nOccursIndex);
}

long IYOA::YOA_SetTRInfo( LPCTSTR pszDSOID, LPCTSTR pszBlockName )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTRInfo )	
		m_fpSetTRInfo = (FP_SETTRINFO)GetProcAddress(m_hModule, "YOA_SetTRInfo");

	if( NULL == m_fpSetTRInfo )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTRInfo(pszDSOID, pszBlockName);
}

long IYOA::YOA_SetFieldString( LPCTSTR pszFieldName, LPCTSTR pszValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldString )	
		m_fpSetFieldString = (FP_SETFIELDSTRING)GetProcAddress(m_hModule, "YOA_SetFieldString");

	if( NULL == m_fpSetFieldString )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldString(pszFieldName, pszValue, nOccursIndex);
}

long IYOA::YOA_SetFieldLong( LPCTSTR pszFieldName, long nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldLong )	
		m_fpSetFieldLong = (FP_SETFIELDLONG)GetProcAddress(m_hModule, "YOA_SetFieldLong");

	if( NULL == m_fpSetFieldLong )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldLong(pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_SetFieldLong64( LPCTSTR pszFieldName, INT64 nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldLong64 )	
		m_fpSetFieldLong64 = (FP_SETFIELDLONG64)GetProcAddress(m_hModule, "YOA_SetFieldLong64");

	if( NULL == m_fpSetFieldLong64 )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldLong64(pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_SetFieldDouble( LPCTSTR pszFieldName, double dValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldDouble )	
		m_fpSetFieldDouble = (FP_SETFIELDDOUBLE)GetProcAddress(m_hModule, "YOA_SetFieldDouble");

	if( NULL == m_fpSetFieldDouble )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldDouble(pszFieldName, dValue, nOccursIndex);
}

long IYOA::YOA_SetFieldFloat( LPCTSTR pszFieldName, float fValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldFloat )	
		m_fpSetFieldFloat = (FP_SETFIELDFLOAT)GetProcAddress(m_hModule, "YOA_SetFieldFloat");

	if( NULL == m_fpSetFieldFloat )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldFloat(pszFieldName, fValue, nOccursIndex);
}

long IYOA::YOA_SetFieldWord( LPCTSTR pszFieldName, WORD wValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldWord )	
		m_fpSetFieldWord = (FP_SETFIELDWORD)GetProcAddress(m_hModule, "YOA_SetFieldWord");

	if( NULL == m_fpSetFieldWord )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldWord(pszFieldName, wValue, nOccursIndex);
}

long IYOA::YOA_SetFieldByte( LPCTSTR pszFieldName, byte byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetFieldByte )	
		m_fpSetFieldByte = (FP_SETFIELDBYTE)GetProcAddress(m_hModule, "YOA_SetFieldByte");

	if( NULL == m_fpSetFieldByte )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetFieldByte(pszFieldName, byteValue, nOccursIndex);
}


long IYOA::YOA_GetRowCount( LPCTSTR pszDSOID, LPCTSTR pszBlockName )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetRowCount )	
		m_fpGetRowCount = (FP_GETROWCOUNT)GetProcAddress(m_hModule, "YOA_GetRowCount");

	if( NULL == m_fpGetRowCount )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetRowCount(pszDSOID, pszBlockName);
}

long IYOA::YOA_GetTRFieldAttr( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldAttr )	
		m_fpGetTRFieldAttr = (FP_GETTRFIELDATTR)GetProcAddress(m_hModule, "YOA_GetTRFieldAttr");

	if( NULL == m_fpGetTRFieldAttr )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldAttr(pszDSOID, pszBlockName, pszFieldName, byteValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldString( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldString )	
		m_fpGetTRFieldString = (FP_GETTRFIELDSTRING)GetProcAddress(m_hModule, "YOA_GetTRFieldString");

	if( NULL == m_fpGetTRFieldString )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldString(pszDSOID, pszBlockName, pszFieldName, pszValueBuf, nValueBufSize, nOccursIndex);

}

long IYOA::YOA_GetTRFieldLong( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, long* nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldLong )	
		m_fpGetTRFieldLong = (FP_GETTRFIELDLONG)GetProcAddress(m_hModule, "YOA_GetTRFieldLong");

	if( NULL == m_fpGetTRFieldLong )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldLong(pszDSOID, pszBlockName, pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldLong64( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, INT64* nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldLong64 )	
		m_fpGetTRFieldLong64 = (FP_GETTRFIELDLONG64)GetProcAddress(m_hModule, "YOA_GetTRFieldLong64");

	if( NULL == m_fpGetTRFieldLong64 )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldLong64(pszDSOID, pszBlockName, pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldDouble( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, double* dValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldDouble )	
		m_fpGetTRFieldDouble = (FP_GETTRFIELDDOUBLE)GetProcAddress(m_hModule, "YOA_GetTRFieldDouble");

	if( NULL == m_fpGetTRFieldDouble )
		return -1;

	return m_fpGetTRFieldDouble(pszDSOID, pszBlockName, pszFieldName, dValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldFloat( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, float* fValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldFloat )	
		m_fpGetTRFieldFloat = (FP_GETTRFIELDFLOAT)GetProcAddress(m_hModule, "YOA_GetTRFieldFloat");

	if( NULL == m_fpGetTRFieldFloat )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldFloat(pszDSOID, pszBlockName, pszFieldName, fValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldWord( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, WORD* wValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldWord )	
		m_fpGetTRFieldWord = (FP_GETTRFIELDWORD)GetProcAddress(m_hModule, "YOA_GetTRFieldWord");

	if( NULL == m_fpGetTRFieldWord )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldWord(pszDSOID, pszBlockName, pszFieldName, wValue, nOccursIndex);
}

long IYOA::YOA_GetTRFieldByte( LPCTSTR pszDSOID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetTRFieldByte )	
		m_fpGetTRFieldByte = (FP_GETTRFIELDBYTE)GetProcAddress(m_hModule, "YOA_GetTRFieldByte");

	if( NULL == m_fpGetTRFieldByte )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetTRFieldByte(pszDSOID, pszBlockName, pszFieldName, byteValue, nOccursIndex);
}

long IYOA::YOA_GetFieldAttr( LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldAttr )	
		m_fpGetFieldAttr = (FP_GETFIELDATTR)GetProcAddress(m_hModule, "YOA_GetFieldAttr");

	if( NULL == m_fpGetFieldAttr )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldAttr(pszFieldName, byteValue, nOccursIndex);
}

long IYOA::YOA_GetFieldString( LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldString )	
		m_fpGetFieldString = (FP_GETFIELDSTRING)GetProcAddress(m_hModule, "YOA_GetFieldString");

	if( NULL == m_fpGetFieldString )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldString(pszFieldName, pszValueBuf, nValueBufSize, nOccursIndex);

}

long IYOA::YOA_GetFieldLong( LPCTSTR pszFieldName, long* nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldLong )	
		m_fpGetFieldLong = (FP_GETFIELDLONG)GetProcAddress(m_hModule, "YOA_GetFieldLong");

	if( NULL == m_fpGetFieldLong )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldLong(pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_GetFieldLong64( LPCTSTR pszFieldName, INT64* nValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldLong64 )	
		m_fpGetFieldLong64 = (FP_GETFIELDLONG64)GetProcAddress(m_hModule, "YOA_GetFieldLong64");

	if( NULL == m_fpGetFieldLong64 )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldLong64(pszFieldName, nValue, nOccursIndex);
}

long IYOA::YOA_GetFieldDouble( LPCTSTR pszFieldName, double* dValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldDouble )	
		m_fpGetFieldDouble = (FP_GETFIELDDOUBLE)GetProcAddress(m_hModule, "YOA_GetFieldDouble");

	if( NULL == m_fpGetFieldDouble )
		return -1;

	return m_fpGetFieldDouble(pszFieldName, dValue, nOccursIndex);
}

long IYOA::YOA_GetFieldFloat( LPCTSTR pszFieldName, float* fValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldFloat )	
		m_fpGetFieldFloat = (FP_GETFIELDFLOAT)GetProcAddress(m_hModule, "YOA_GetFieldFloat");

	if( NULL == m_fpGetFieldFloat )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldFloat(pszFieldName, fValue, nOccursIndex);
}

long IYOA::YOA_GetFieldWord( LPCTSTR pszFieldName, WORD* wValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldWord )	
		m_fpGetFieldWord = (FP_GETFIELDWORD)GetProcAddress(m_hModule, "YOA_GetFieldWord");

	if( NULL == m_fpGetFieldWord )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldWord(pszFieldName, wValue, nOccursIndex);
}

long IYOA::YOA_GetFieldByte( LPCTSTR pszFieldName, byte* byteValue, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldByte )	
		m_fpGetFieldByte = (FP_GETFIELDBYTE)GetProcAddress(m_hModule, "YOA_GetFieldByte");

	if( NULL == m_fpGetFieldByte )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldByte(pszFieldName, byteValue, nOccursIndex);
}

long IYOA::YOA_GetRowCountByReqID( long nReqID, LPCTSTR pszBlockName )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetRowCountByReqID )	
		m_fpGetRowCountByReqID = (FP_GETROWCOUNTBYREQID)GetProcAddress(m_hModule, "YOA_GetRowCountByReqID");

	if( NULL == m_fpGetRowCountByReqID )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetRowCountByReqID(nReqID, pszBlockName);
}

long IYOA::YOA_GetFieldStringByReqID( long nReqID, LPCTSTR pszBlockName, LPCTSTR pszFieldName, LPCTSTR pszValueBuf, long nValueBufSize, long nOccursIndex /* = 0 */ )
{
	if( NULL == m_hModule )
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetFieldStringByReqID )	
		m_fpGetFieldStringByReqID = (FP_GETFIELDSTRINGBYREQID)GetProcAddress(m_hModule, "YOA_GetFieldStringByReqID");

	if( NULL == m_fpGetFieldStringByReqID )
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetFieldStringByReqID(nReqID, pszBlockName, pszFieldName, pszValueBuf, nValueBufSize, nOccursIndex);

}

long IYOA::YOA_ReleaseData( long nRequestID )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpReleaseData )	
		m_fpReleaseData = (FP_RELEASEDATA)GetProcAddress(m_hModule, "YOA_ReleaseData");

	if( NULL == m_fpReleaseData )	
		return ERROR_FUNCTION_NOT_FOUND;

	m_fpReleaseData(nRequestID);

	return TRUE;
}

long IYOA::YOA_RegistAuto( HWND hWnd, LPCTSTR pszAutoID )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpRegistAuto )	
		m_fpRegistAuto = (FP_REGISTAUTO)GetProcAddress(m_hModule, "YOA_RegistAuto");

	if( NULL == m_fpRegistAuto )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpRegistAuto( hWnd, pszAutoID );
}

long IYOA::YOA_UnregistAuto( HWND hWnd, LPCTSTR pszAutoID )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpUnregistAuto )	
		m_fpUnregistAuto = (FP_UNREGISTAUTO)GetProcAddress(m_hModule, "YOA_UnregistAuto");

	if( NULL == m_fpUnregistAuto )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpUnregistAuto( hWnd, pszAutoID );
}

long IYOA::YOA_UnregistAutoWithKey( HWND hWnd, LPCTSTR pszAutoID, LPCTSTR pszKeys )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpUnregistAutoWithKey )	
		m_fpUnregistAutoWithKey = (FP_UNREGISTAUTOWITHKEY)GetProcAddress(m_hModule, "YOA_UnregistAutoWithKey");

	if( NULL == m_fpUnregistAutoWithKey )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpUnregistAutoWithKey( hWnd, pszAutoID, pszKeys );
}

long IYOA::YOA_UnregistAutoWithReqID( long nReqID )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpUnregistAutoWithReqID )	
		m_fpUnregistAutoWithReqID = (FP_UNREGISTAUTOWITHREQID)GetProcAddress(m_hModule, "YOA_UnregistAutoWithReqID");

	if( NULL == m_fpUnregistAutoWithReqID )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpUnregistAutoWithReqID( nReqID );
}

long IYOA::YOA_UnregistAutoWithWindow( HWND hWnd )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpUnregistAutoWithWindow )	
		m_fpUnregistAutoWithWindow = (FP_UNREGISTAUTOWITHWINDOW)GetProcAddress(m_hModule, "YOA_UnregistAutoWithWindow");

	if( NULL == m_fpUnregistAutoWithWindow )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpUnregistAutoWithWindow( hWnd );
}


long IYOA::YOA_GetAccountCount()
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetAccountCount )	
		m_fpGetAccountCount = (FP_GETACCOUNTCOUNT)GetProcAddress(m_hModule, "YOA_GetAccountCount");

	if( NULL == m_fpGetAccountCount )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetAccountCount();
}

long IYOA::YOA_GetAccount( long nIndex, LPCTSTR pszAccountBuf, long nAccountBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetAccount )	
		m_fpGetAccount = (FP_GETACCOUNT)GetProcAddress(m_hModule, "YOA_GetAccount");

	if( NULL == m_fpGetAccount )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetAccount( nIndex, pszAccountBuf, nAccountBufSize );
}

long IYOA::YOA_GetAccountInfo( long nInfoType, LPCTSTR pszAccount, LPCTSTR pszAccountInfoBuf, long nAccountInfoBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetAccountInfo )	
		m_fpGetAccountInfo = (FP_GETACCOUNTINFO)GetProcAddress(m_hModule, "YOA_GetAccountInfo");

	if( NULL == m_fpGetAccountInfo )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetAccountInfo( nInfoType, pszAccount, pszAccountInfoBuf, nAccountInfoBufSize );
}

long IYOA::YOA_GetServerName( LPSTR pszServerNameBuf, long nServerNameBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetServerName )	
		m_fpGetServerName = (FP_GETSERVERNAME)GetProcAddress(m_hModule, "YOA_GetServerName");

	if( NULL == m_fpGetServerName )	
		return ERROR_FUNCTION_NOT_FOUND;

	m_fpGetServerName( pszServerNameBuf, nServerNameBufSize );
}

long IYOA::YOA_GetLastError()
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetLastError )	
		m_fpGetLastError = (FP_GETLASTERROR)GetProcAddress(m_hModule, "YOA_GetLastError");

	if( NULL == m_fpGetLastError )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetLastError();
}

long IYOA::YOA_GetErrorMessage( long nErrorCode, LPCTSTR pszErrorMessageBuf, long nErrorMessageBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetErrorMessage )	
		m_fpGetErrorMessage = (FP_GETERRORMESSAGE)GetProcAddress(m_hModule, "YOA_GetErrorMessage");

	if( NULL == m_fpGetErrorMessage )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetErrorMessage( nErrorCode, pszErrorMessageBuf, nErrorMessageBufSize );
}

long IYOA::YOA_GetCodeCount( long nMarketType )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetCodeCount )	
		m_fpGetCodeCount = (FP_GETCODECOUNT)GetProcAddress(m_hModule, "YOA_GetCodeCount");

	if( NULL == m_fpGetCodeCount )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetCodeCount( nMarketType );
}

long IYOA::YOA_GetCodeInfoByIndex( long nMarketType, long nInfoType, long nIndex, LPCTSTR pszCodeInfoBuf, long nCodeInfoBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetCodeInfoByIndex )	
		m_fpGetCodeInfoByIndex = (FP_GETCODEINFOBYINDEX)GetProcAddress(m_hModule, "YOA_GetCodeInfoByIndex");

	if( NULL == m_fpGetCodeInfoByIndex )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetCodeInfoByIndex( nMarketType, nInfoType, nIndex, pszCodeInfoBuf, nCodeInfoBufSize );
}

long IYOA::YOA_GetCodeInfo( long nMarketType, long nInfoType, LPCTSTR pszCode, LPCTSTR pszCodeInfoBuf, long nCodeInfoBufSize )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpGetCodeInfo )	
		m_fpGetCodeInfo = (FP_GETCODEINFO)GetProcAddress(m_hModule, "YOA_GetCodeInfo");

	if( NULL == m_fpGetCodeInfo )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpGetCodeInfo( nMarketType, nInfoType, pszCode, pszCodeInfoBuf, nCodeInfoBufSize );
}

long IYOA::YOA_SetTimeout( long nInterval )
{
	if( NULL == m_hModule )	
		return ERROR_NOT_INITIAL;

	if( NULL == m_fpSetTimeout )	
		m_fpSetTimeout = (FP_SETTIMEOUT)GetProcAddress(m_hModule, "YOA_SetTimeout");

	if( NULL == m_fpSetTimeout )	
		return ERROR_FUNCTION_NOT_FOUND;

	return m_fpSetTimeout( nInterval );
}

void IYOA::YOA_ReStart()
{
	YOA_UnInitial();

	TCHAR lpFileName[MAX_PATH + 1];
	GetModuleFileName( NULL, lpFileName, MAX_PATH );

	CString strTemp( lpFileName );
	CString strParam = _T("");
	CString strSyncPath = _T("");

	int nLastIndex = strTemp.ReverseFind('\\');
	if ( -1 != nLastIndex )
	{
		if ( 0 < m_strPath.GetLength() )
		{
			strSyncPath.Format( _T("%s\\YOASync.exe"), m_strPath );
			strParam.Format( _T("/exe=%s;"), strTemp);
		}
		else
		{
			strSyncPath.Format( _T("%s\\YOASync.exe"), strTemp.Left( nLastIndex ) );
			strParam.Format( _T("/exe=%s;"), strTemp.Mid( nLastIndex + 1 ));
		}
	}

	ShellExecute( NULL, _T("open"), strSyncPath, strParam, NULL, SW_SHOW );

	if ( NULL != m_hWnd )
		SendMessage(m_hWnd, WM_CLOSE, 0, 0);
}

void IYOA::SetPacketString( char* pData, int nSize, int nType, CString strValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		long nValue = atoi( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = _atoi64( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = atof( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)atof( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = atoi( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = atoi( (LPCSTR)(LPCTSTR)strValue );
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketLong( char* pData, int nSize, int nType, long nValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%d"), nValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = (INT64)nValue;
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = (double)nValue;
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)nValue;
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = (WORD)nValue;
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = (byte)nValue;
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketLong64( char* pData, int nSize, int nType, INT64 nLValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%Ld"), nLValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		int nValue = (long)nLValue;
		CopyMemory( pData, &nValue, nSize );		
	}
	else if ( TYPE_LONG64 == nType )
	{
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = (double)nLValue;
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)nLValue;
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = (WORD)nLValue;
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = (byte)nLValue;
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketDouble( char* pData, int nSize, int nType, double dValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%f"), dValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		int nValue = (long)dValue;
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = (INT64)dValue;
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)dValue;
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = (WORD)dValue;
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = (byte)dValue;
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketFloat( char* pData, int nSize, int nType, float fValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%f"), fValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		int nValue = (long)fValue;
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = (INT64)fValue;
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = (double)fValue;
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = (WORD)fValue;
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = (byte)fValue;
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketWord( char* pData, int nSize, int nType, WORD wValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%d"), wValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		int nValue = (long)wValue;
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = (INT64)wValue;
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = (double)wValue;
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)wValue;
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteValue = (byte)wValue;
		CopyMemory( pData, &byteValue, nSize );
	}
}

void IYOA::SetPacketByte( char* pData, int nSize, int nType, byte byteValue )
{
	FillMemory( pData, nSize, 0x00 );

	if ( TYPE_STRING == nType )
	{
		CString strValue;
		strValue.Format( _T("%d"), byteValue );
		CopyMemory( pData, strValue.GetBuffer(0), nSize );
	}
	else if ( TYPE_LONG == nType )
	{
		int nValue = (long)byteValue;
		CopyMemory( pData, &nValue, nSize );
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLValue = (INT64)byteValue;
		CopyMemory( pData, &nLValue, nSize );
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dValue = (double)byteValue;
		CopyMemory( pData, &dValue, nSize );
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fValue = (float)byteValue;
		CopyMemory( pData, &fValue, nSize );
	}
	else if ( TYPE_SHORT == nType || TYPE_WORD == nType )
	{
		WORD wValue = (WORD)byteValue;
		CopyMemory( pData, &wValue, nSize );
	}
	else if ( TYPE_BYTE == nType )
	{
		CopyMemory( pData, &byteValue, nSize );
	}
}

CString IYOA::GetPacketString( TCHAR* pData, int nSize, int nType )
{
	CString strData( _T("") );

	if ( TYPE_STRING == nType )
	{
		TCHAR* szData[2048] = {0,};
		memcpy( szData, pData, nSize );
		
		strData.Format( _T("%s"), szData );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData = 0;
		CopyMemory(&nData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%d"), nData );
		strData = strTemp;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%Ld"), nLData );
		strData = strTemp;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%f"), dData );
		strData = strTemp;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%f"), fData );
		strData = strTemp;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%d"), sData );
		strData = strTemp;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%d"), wData );
		strData = strTemp;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		CString strTemp;
		strTemp.Format( _T("%d"), byteData );
		strData = strTemp;
	}

	return strData;
}

long IYOA::GetPacketLong( char* pData, int nSize, int nType )
{
	long nData = 0;
	
	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		nData = atoi( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		CopyMemory(&nData, pData, nSize);
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		nData = (long)nLData;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		nData = (long)dData;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		nData = (long)fData;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		nData = (long)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		nData = (long)wData;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		nData = (long)byteData;
	}

	return nData;
}

INT64 IYOA::GetPacketLong64( char* pData, int nSize, int nType )
{
	INT64 nLData = 0;

	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		nLData = _atoi64( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData = 0;
		CopyMemory(&nData, pData, nSize);

		nLData = (INT64)nData;
	}
	else if ( TYPE_LONG64 == nType )
	{
		CopyMemory(&nLData, pData, nSize);
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		nLData = (INT64)dData;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		nLData = (INT64)fData;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		nLData = (INT64)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		nLData = (INT64)wData;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		nLData = (INT64)byteData;
	}

	return nLData;
}

double IYOA::GetPacketDouble( char* pData, int nSize, int nType )
{
	double dData = 0.0;

	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		dData = atof( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData;
		CopyMemory(&nData, pData, nSize);

		dData = (double)nData;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		dData = (double)nLData;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		CopyMemory(&dData, pData, nSize);
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		dData = (double)fData;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		dData = (double)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		dData = (double)wData;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		dData = (double)byteData;
	}

	return dData;
}

float IYOA::GetPacketFloat( char* pData, int nSize, int nType )
{
	float fData = 0.0;

	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		fData = (float)atof( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData;
		CopyMemory(&nData, pData, nSize);

		fData = (float)nData;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		fData = (float)nLData;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		fData = (float)dData;
	}
	else if ( TYPE_FLOAT == nType )
	{
		CopyMemory(&fData, pData, nSize);
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		fData = (float)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		fData = (float)wData;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		fData = (float)byteData;
	}

	return fData;
}

WORD IYOA::GetPacketWord( char* pData, int nSize, int nType )
{
	WORD wData = 0;

	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		wData = atoi( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData;
		CopyMemory(&nData, pData, nSize);

		wData = (WORD)nData;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		wData = (WORD)nLData;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		wData = (WORD)dData;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		wData = (WORD)fData;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		wData = (WORD)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		CopyMemory(&wData, pData, nSize);
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		wData = (WORD)byteData;
	}

	return wData;
}

byte IYOA::GetPacketByte( char* pData, int nSize, int nType )
{
	byte byteData = 0;

	if ( TYPE_STRING == nType )
	{
		CString strTemp(pData, nSize);
		byteData = atoi( (LPCSTR)(LPCTSTR)strTemp );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData;
		CopyMemory(&nData, pData, nSize);

		byteData = (byte)nData;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		byteData = (byte)nLData;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		byteData = (byte)dData;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		byteData = (byte)fData;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		byteData = (byte)sData;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		byteData = (byte)wData;
	}
	else if ( TYPE_BYTE == nType )
	{
		CopyMemory(&byteData, pData, nSize);
	}

	return byteData;
}

CString IYOA::GetPacketDBLString( char* pData, int nSize, int nType, int nDotPos )
{
	CString strData( _T("") );

	CString strFormat;
	strFormat.Format( _T("%%.%df"), nDotPos );

	if ( TYPE_STRING == nType )
	{
		CString strTemp( pData, nSize );

		strData.Format( strFormat, atof((LPCSTR)(LPCTSTR)strTemp) );
	}
	else if ( TYPE_LONG == nType )
	{
		long nData = 0;
		CopyMemory(&nData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, (double)nData );
		strData = strTemp;
	}
	else if ( TYPE_LONG64 == nType )
	{
		INT64 nLData = 0;
		CopyMemory(&nLData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, (double)nLData );
		strData = strTemp;
	}
	else if ( TYPE_DOUBLE == nType )
	{
		double dData = 0.0;
		CopyMemory(&dData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, dData );
		strData = strTemp;
	}
	else if ( TYPE_FLOAT == nType )
	{
		float fData = 0.0;
		CopyMemory(&fData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, fData );
		strData = strTemp;
	}
	else if ( TYPE_SHORT == nType )
	{
		short sData = 0;
		CopyMemory(&sData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, (double)sData );
		strData = strTemp;
	}
	else if ( TYPE_WORD == nType )
	{
		WORD wData = 0;
		CopyMemory(&wData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, (double)wData );
		strData = strTemp;
	}
	else if ( TYPE_BYTE == nType )
	{
		byte byteData = 0;
		CopyMemory(&byteData, pData, nSize);

		CString strTemp;
		strTemp.Format( strFormat, (double)byteData );
		strData = strTemp;
	}

	return strData;
}

#pragma pack( pop )

#endif // _IYOA_H_
