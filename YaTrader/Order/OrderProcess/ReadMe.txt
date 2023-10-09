주문 처리 순서. 
같은 일을 하는 함수는 단 한개만 존재하도록 한다. 
SI 증권은 주문을 변경하거나 처리할 때 항상 새로운 주문 번호가 온다. 
그리고 정정확인이나 취소확인 원래 주문 번호는 나중에 온다. 
그 이전에 원래 주문 번호가 먼저 오므로 정정된 주문이나 취소된 주문을 확인하여 취소해 줘야 한다. 
그리고 나중에 OrderUnfilled Event가 올 때는 취소나 정정에 대한 확인만 진행한다. 

취소나 정정을 할 때 먼저 화면에서 없애고 취소확인이나 정정확인은 나중에 처리하는 것으로 한다. 
주문을 낼 때 order_request는 주문창에 표시한다. 그리고 주문을 전송한다. 
주문이 접수확인되거나 체결되었을 때 order_request를 주문창에서 없애 준다. 

A. 주문 확인 처리 순서.
1. 증권사에서 주문 정보를 받는다.
2. 주문정보를 파싱하여 order_info json 객체를 만든다.
3. order_processor 큐에 넣는다.
4. order_processor는 큐에서 꺼내어 주문을 처리하기 시작한다.
5. order_info를 꺼낸 이후에 order_info에 custom 정보를 살펴보아 order_request_id를 추출한다.
6. order_request_id가 order_request_list에 있는지 살펴 본다. 
7. order_request_id가 order_request_list에 있는 경우 
  7.1. order_info를 바탕으로 order객체를 만든다.
  7.2. order에 account_no, symbol_code, order_no, accepted_price, accepted_count, accepted_time, original_order_no, first_order_no을 넣어준다. 
  7.3. order에 order_request_id를 넣어준다. 
  7.3. order_account_type이 sub_account 인 경우 order_account_type을 넣어준다.  
8. order_request_list에 없는 경우 나 order_request_id 정보가 없는 경우 - 무조건 외부 주문(현재 사용자가 주문을 내는 주문창 외에 다른 process, 다른 ip애서 온 주문)으로 간주
  8.1. order_info를 바탕으로 order 객체를 만든다. 
  8.2. order에 account_no, symbol_code, order_no, accepted_price, accepted_count, accepted_time, original_order_no, first_order_no을 넣어준다.
9. account_no를 얻어 account_order_manager를 찾는다.
10. account_order_manager에서 symbol_order_manager를 찾는다. 
11. symbol_order_manager accepted_order_list에 추가해 준다. 
12. symbol_order_manager buy_order_manager/sell_order_manager에 order를 추가해 준다. 
13. account_accepted_order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 
14. order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 
15. order_account_type이 sub_account 인 경우
  15.1. order_request_id 정보로 main_account_no를 얻어 account_order_manager를 찾는다.
  15.2. account_order_manager에서 symbol_order_manager를 찾는다. 
  15.3. symbol_order_manager accepted_order_list에 추가해 준다. 
  15.4. symbol_order_manager buy_order_manager/sell_order_manager에 order를 추가해 준다. 
  15.5. order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 
  15.6. account_accepted_order_view에 accepted_order_list가 갱신되었다는 event를 보낸다. 


B. 주문 체결 처리 순서.
1. 증권사에서 주문 정보를 받는다.
2. 주문정보를 파싱하여 order_info json 객체를 만든다.
3. order_processor 큐에 넣는다.
4. order_processor는 큐에서 꺼내어 주문을 처리하기 시작한다.
5. order_info를 꺼낸 이후에 order_info에 custom 정보를 살펴보아 order_request_id를 추출한다.
6. order_request_id가 order_request_list에 있는지 살펴 본다. 
7. order_request_id가 order_request_list에 있는 경우 
  7.1. order_info를 바탕으로 order객체를 만든다.
  7.2. order에 account_no, symbol_code, order_no, filled_price, filled_count, filled_time를 넣어준다. 
  7.3. order에 order_request_id를 넣어준다. 
  7.3. order_account_type이 sub_account 인 경우 order_account_type을 넣어준다.  
8. order_request_list에 없는 경우 나 order_request_id 정보가 없는 경우 - 무조건 외부 주문(현재 사용자가 주문을 내는 주문창 외에 다른 process, 다른 ip애서 온 주문)으로 간주
  8.1. order_info를 바탕으로 order 객체를 만든다. 
  8.2. order에 account_no, symbol_code, order_no, filled_price, filled_count, filled_time를 넣어준다. 
9. account_no, symbol_code로 total_position_manager에서 position을 찾는다. 
10. order 주문이 부분 체결인가 완벽 체결인가를 판단한다. 
11. order가 주문 수량과 체결 수량이 일치하지 않을 때는 부분 체결로 상태를 설정한다.
    그리고 order의 주문수량을 갱신한다. 
12. order가 체결되었을 때 주문 수량을 알 수 없을 때는 주문 상태를 Unresolved_Filled로 설정한다. 
13. 주문상태가 Unresolved_Filled일 때는 접수확인에서 이 부분을 확인하여 별도로 처리한다. 
14. 찾은 position과 filled_order로 다음의 값들을 계산한다. 
15. trade_count, open_count, average_price, open_price, settled_count를 계산한다. 
16. trade_price를 계산하고 position trade_profit_loss를 갱신한다. 
17. position open_count, average_price, open_price를 갱신한다. 
18. order의 settled_count를 기반으로 cut_stop_order를 설정한다. 
19. account_position_manager의 trade_profit_loss, open_profit_loss를 갱신한다. 
20. order_view, account_position_view에 event를 보낸다. 
21. order_account_type이 sub_account 인 경우
  21.1. order_request_id 정보로 main_account_no를 얻어 account_order_manager를 찾는다.
  21.2. account_order_manager에서 symbol_order_manager를 찾는다. 
  21.3. account_position_manager의 trade_profit_loss, open_profit_loss를 갱신한다. 
  21.4. order_view, account_position_view에 event를 보낸다. 

주문은 반드시 한번만 처리한다.
그러나 주문 이벤트는 관련된 모든 것에 알려야 한다.
서브계좌 주문은 자신의 본계좌와 자신이 속한 펀드에 이벤트로 알려야 한다.
이벤트를 받은 본계좌나 펀드는 그 이벤트를 처리한다.
각 컨트롤은 계좌 번호와 펀드 이름으로 키를 삼는다. 
이벤트가 올 때 이 키로 이벤트를 처리할 것인지 말 것인지 결정한다.
다만 StopOrder는 Window id 로 키를 삼는다.
Stop Order는 오로지 그 주문을 낸 창에만 해당되기 때문이다. 