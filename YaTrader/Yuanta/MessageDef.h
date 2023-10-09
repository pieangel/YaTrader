#ifndef _MESSAGEDEF_H_
#define _MESSAGEDEF_H_

//------------------------------------------------------------------------------
// 메시지 정의
// 메시지의 ID값은 Connect시에 설정한 nStartMsgID와 더하여 사용
#define 	WMU_STARTMSGID				WM_USER + 1000

#define 	WMU_RECEIVE_SYSTEM_MESSAGE	WMU_STARTMSGID + CMD_SYSTEM_MESSAGE		// System Message
#define 	WMU_RECEIVE_DATA			WMU_STARTMSGID + CMD_RECEIVE_DATA		// Request로 요청한 데이터를 받았을 때 발생
#define 	WMU_RECEIVE_REAL_DATA		WMU_STARTMSGID + CMD_RECEIVE_REAL_DATA	// RegistAuto로 드옥한 실시간을 받았을 때 발생
#define 	WMU_LOGIN					WMU_STARTMSGID + CMD_LOGIN				// 서버로부터 로그인 결과 받았을때 발생
#define 	WMU_LOGOUT					WMU_STARTMSGID + CMD_LOGOUT				// 서버로부터 로그아웃 결과 받았을때 발생
#define		WMU_RECEIVE_ERROR			WMU_STARTMSGID + CMD_RECEIVE_ERROR		// 서버로부터 오류를 받았을 때 발생

#endif // _MESSAGEDEF_H_