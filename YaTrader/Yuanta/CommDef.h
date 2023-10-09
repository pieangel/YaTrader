#ifndef _YOACOMMONDEF_H_
#define _YOACOMMONDEF_H_

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	로그타입코드
*/
#define		YOALOG_TRAACE									0
#define		YOALOG_ERROR									1
#define		YOALOG_DEBUG									2

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	반환/응답코드
*/
#define 	RESULT_FAIL										-1		// API 실패 반환 코드
#define 	RESULT_SUCCESS									1000	// API 성공 반환 코드
#define 	RESPONSE_LOGIN_FAIL								1		// 로그인 실패 코드
#define 	RESPONSE_LOGIN_SUCCESS							2		// 로그인 성공 코드

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	에러코드
*/
#define		ERROR_MODULE_NOT_FOUND							11		// Yuanta Open API 모듈을 찾을 수 없습니다.
#define		ERROR_FUNCTION_NOT_FOUND						12		// Yuanta Open API 함수를 찾을 수 없습니다.
#define		ERROR_NOT_INITIAL								13		// Yuanta Open API 초기화 상태가 아닙니다.

#define		ERROR_SYSTEM_START								19
#define		ERROR_SYSTEM_CERT_ERROR							20		// 인증오류입니다.
#define		ERROR_SYSTEM_MAX_CON							21		// 다중접속한도 초과입니다.
#define		ERROR_SYSTEM_FORCE_KILL							22		// 네트워크 연결이 서버에 의해 종료되었습니다.
#define		ERROR_SYSTEM_EMERGENCY							23		// 시스템 비상 상황입니다.
#define		ERROR_SYSTEM_INFINIT_CALL						24		// 이상 호출로 접속이 종료됩니다.
#define		ERROR_SYSTEM_SOCKET_CLOSE						25		// 네트웍 연결이 끊어졌습니다.
#define		ERROR_SYSTEM_END								99

#define		ERROR_NOT_LOGINED								101		// 로그인 상태가 아닙니다.
#define		ERROR_ALREADY_LOGINED							102		// 이미 로그인된 상태입니다.
#ifndef ERROR_INDEX_OUT_OF_BOUNDS
#define		ERROR_INDEX_OUT_OF_BOUNDS						103		// 인덱스가 가용범위를 넘었습니다.
#endif
#define		ERROR_TIMEOUT_DATA								104		// 타임아웃이 발생하였습니다.
#define		ERROR_USERINFO_NOT_FOUND						105		// 사용자 정보를 찾을 수 없습니다.
#define		ERROR_ACCOUNT_NOT_FOUND							106		// 계좌번호를 찾을 수 없습니다.
#define		ERROR_ACCOUNT_PASSWORD_INCORRECT				107		// 계좌 비밀번호를 잘못 입력하셨습니다.
#define		ERROR_TYPE_NOT_FOUND							108		// 요청한 타입을 찾을 수 없습니다.

#define		ERROR_CERT_PASSWORD_INCORRECT					110		// 공인인증 비밀번호가 일치하지 않습니다.
#define		ERROR_CERT_NOT_FOUND							111		// 공인인증서를 찾을 수 없습니다.
#define		ERROR_CETT_CANCEL_SELECT						112		// 공인인증서 선택을 취소했습니다.
#define		ERROR_NEED_TO_UPDATE							113		// 공인인증 업데이트가 필요합니다.
#define		ERROR_CERT_7_ERROR								114		// 공인인증 7회 오류입니다.
#define		ERROR_ID_PASSWORD_INCORRECT						115		// 로그인 비밀번호 비밀번호가 일치하지 않습니다.
#define		ERROR_CERT_PASSWORD_SHORT						116		// 공인인증서 비밀번호가 최소길이보다 짧습니다.
#define		ERROR_ID_SHORT									117		// 로그인 아이디가 최소길이보다 짧습니다.
#define		ERROR_ID_PASSWORD_SHORT							118		// 로그인 비밀번호가 최소길이보다 짧습니다.
#define		ERROR_CERT_DIFF_CERT_SELECT						119		// 공인인증서 DN이 일치하지 않습니다.

#define		ERROR_CERT_OLD									121		// 폐기된 인증서 입니다.
#define		ERROR_CERT_TIME_OVER							122		// 만료된 인증서 입니다.
#define		ERROR_CERT_STOP									123		// 정지된 인증서 입니다.
#define		ERROR_CERT_NOTMATCH_SN							124		// SN이 일치하지 않는 인증서입니다.
#define		ERROR_CERT_ETC									125		// 기타오류 인증서입니다.
#define		ERROR_CERT_TIME_OUT								126		// 타인증기관 발급 인증서 검증에서 타임아웃이 발생하였습니다. 다시 시도해 주십시오

#define		ERROR_REQUEST_FAIL								201		// DSO 요청이 실패하였습니다.
#define		ERROR_DSO_NOT_FOUND								202		// DSO를 찾을 수 없습니다.
#define		ERROR_BLOCK_NOT_FOUND							203		// 블록을 찾을 수 없습니다..
#define		ERROR_FIELD_NOT_FOUND							204		// 필드를 찾을 수 없습니다.
#define		ERROR_REQUEST_NOT_FOUND							205		// 요청 정보를 찾을 수 없습니다.
#define		ERROR_ATTR_NOT_FOUND							206		// 필드의 속성을 찾을 수 없습니다.
#define		ERROR_REGIST_FAIL								207		// AUTO 등록이 실패하였습니다.
#define		ERROR_AUTO_NOT_FOUND							208		// AUTO를 찾을 수 없습니다.
#define		ERROR_KEY_NOT_FOUND								209		// 요청한 키를 찾을 수 없습니다.
#define		ERROR_VALUE_NOT_FOUND							210		// 요청한 값을 찾을 수 없습니다.
#define		ERROR_REQUEST_SEC_LIMIT_OVER					211		// 요청 카운트 5초당 제한을 초과하였습니다.
#define		ERROR_REQUEST_MIN_LIMIT_OVER					212		// 요청 카운트 분당 제한을 초과하였습니다.
#define		ERROR_REQUEST_HOUR_LIMIT_OVER					213		// 요청 카운트 시간당 제한을 초과하였습니다.

#define		ERROR_MAX_CODE									RESULT_SUCCESS

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	시스템 통보 코드
*/
#define		NOTIFY_SYSTEM_NEED_TO_RESTART					26		// 모듈 변경에 따른 재시작 필요
#define		NOTIFY_SYSTEM_LOGIN_START						27		// 로그인을 시작합니다.
#define		NOTIFY_SYSTEM_LOGIN_REQ_USERINFO				28		// 사용자 정보를 요청합니다.
#define		NOTIFY_SYSTEM_LOGIN_RCV_USERINFO				29		// 사용자 정보를 수신했습니다.
#define		NOTIFY_SYSTEM_LOGIN_FILE_DWN_START				30		// 파일 다운로드를 시작합니다.
#define		NOTIFY_SYSTEM_LOGIN_FILE_DWN_END				31		// 파일 다운로드가 완료되었습니다.
#define		NOTIFY_SYSTEM_TRAY_APP_CHECK					32		// 트레이 App 체크
#define		NOTIFY_SYSTEM_INFO_DUP_CONNECT					33		// 다중접속
#define		NOTIFY_SYSTEM_INFO_DETECT_CLOSET				34		// 소켓 종료 감지, 재연결 시도중입니다.
#define		NOTIFY_SYSTEM_INFO_RECONNECT					35		// 재연결되었습니다.

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	계좌정보코드
*/
#define		ACCOUNT_INFO_NAME								1		// 계좌명
#define		ACCOUNT_INFO_NICKNAME							2		// 계좌별명
#define		ACCOUNT_INFO_TYPE								3		// 상품구분

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	종목정보코드
*/
#define		MARKET_TYPE_INTERNAL							0		// 국내(주식,선물옵션)
#define		MARKET_TYPE_GLOBAL_STOCK						1		// 해외주식
#define		MARKET_TYPE_GLOBAL_DERIVATIVE					2		// 해외선물옵션
#define		MARKET_TYPE_INTERNAL_STOCK						3		// 국내 주식
#define		MARKET_TYPE_INTERNAL_KOSPIFUTURE				4		// 국내 선물
#define		MARKET_TYPE_INTERNAL_KOSPIOPTION				5		// 국내 옵션
#define		MARKET_TYPE_GLOBAL_FUTURE						6		// 해외선물
#define		MARKET_TYPE_GLOBAL_OPTION						7		// 해외옵션

#define		CODE_INFO_CODE									0		// 코드
#define		CODE_INFO_STANDARD_CODE							1		// 표준코드
#define		CODE_INFO_NAME									2		// 한글 종목명
#define		CODE_INFO_ENG_NAME								3		// 영문 종목명
#define		CODE_INFO_JANG_GUBUN							4		// 장구분

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	환경설정코드
*/
#define		CONFIG_STRUCT_MODE								0		// 조회 및 실시간 TR에서 struct 사용을 위한 데이터 포인터 반환되도록 설정
#define		CONFIG_REALDATA_BYPASS_MODE						1		// 실시간 struct 사용시, Key-Value 가공처리 

//-------------------------------------------------------------------------------------------------------------------
/**	@brief	메시지
*/
// 메시지의 ID값은 Connect시에 설정한 nStartMsgID와 더하여 사용
#define 	CMD_YOA_FAIL									-1		// API Call 실패
#define 	CMD_YOA_SUCCESS									1		// API Call 성공
#define 	CMD_SYSTEM_MESSAGE								2		// System Message
#define 	CMD_LOGIN										3		// 서버로부터 로그인 결과 받았을때 발생
#define 	CMD_LOGOUT										4		// 서버로부터 로그아웃 결과 받았을때 발생
#define 	CMD_RECEIVE_ERROR								5		// 서버로부터 오류를 받았을 때 발생
#define 	CMD_RECEIVE_DATA								6		// Request로 요청한 데이터를 받았을 때 발생
#define 	CMD_RECEIVE_REAL_DATA							7		// RegistAuto로 드옥한 실시간을 받았을 때 발생

#endif // _YOACOMMONDEF_H_
