�ֹ� ó�� ����. 
���� ���� �ϴ� �Լ��� �� �Ѱ��� �����ϵ��� �Ѵ�. 
SI ������ �ֹ��� �����ϰų� ó���� �� �׻� ���ο� �ֹ� ��ȣ�� �´�. 
�׸��� ����Ȯ���̳� ���Ȯ�� ���� �ֹ� ��ȣ�� ���߿� �´�. 
�� ������ ���� �ֹ� ��ȣ�� ���� ���Ƿ� ������ �ֹ��̳� ��ҵ� �ֹ��� Ȯ���Ͽ� ����� ��� �Ѵ�. 
�׸��� ���߿� OrderUnfilled Event�� �� ���� ��ҳ� ������ ���� Ȯ�θ� �����Ѵ�. 

��ҳ� ������ �� �� ���� ȭ�鿡�� ���ְ� ���Ȯ���̳� ����Ȯ���� ���߿� ó���ϴ� ������ �Ѵ�. 
�ֹ��� �� �� order_request�� �ֹ�â�� ǥ���Ѵ�. �׸��� �ֹ��� �����Ѵ�. 
�ֹ��� ����Ȯ�εǰų� ü��Ǿ��� �� order_request�� �ֹ�â���� ���� �ش�. 

A. �ֹ� Ȯ�� ó�� ����.
1. ���ǻ翡�� �ֹ� ������ �޴´�.
2. �ֹ������� �Ľ��Ͽ� order_info json ��ü�� �����.
3. order_processor ť�� �ִ´�.
4. order_processor�� ť���� ������ �ֹ��� ó���ϱ� �����Ѵ�.
5. order_info�� ���� ���Ŀ� order_info�� custom ������ ���캸�� order_request_id�� �����Ѵ�.
6. order_request_id�� order_request_list�� �ִ��� ���� ����. 
7. order_request_id�� order_request_list�� �ִ� ��� 
  7.1. order_info�� �������� order��ü�� �����.
  7.2. order�� account_no, symbol_code, order_no, accepted_price, accepted_count, accepted_time, original_order_no, first_order_no�� �־��ش�. 
  7.3. order�� order_request_id�� �־��ش�. 
  7.3. order_account_type�� sub_account �� ��� order_account_type�� �־��ش�.  
8. order_request_list�� ���� ��� �� order_request_id ������ ���� ��� - ������ �ܺ� �ֹ�(���� ����ڰ� �ֹ��� ���� �ֹ�â �ܿ� �ٸ� process, �ٸ� ip�ּ� �� �ֹ�)���� ����
  8.1. order_info�� �������� order ��ü�� �����. 
  8.2. order�� account_no, symbol_code, order_no, accepted_price, accepted_count, accepted_time, original_order_no, first_order_no�� �־��ش�.
9. account_no�� ��� account_order_manager�� ã�´�.
10. account_order_manager���� symbol_order_manager�� ã�´�. 
11. symbol_order_manager accepted_order_list�� �߰��� �ش�. 
12. symbol_order_manager buy_order_manager/sell_order_manager�� order�� �߰��� �ش�. 
13. account_accepted_order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
14. order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
15. order_account_type�� sub_account �� ���
  15.1. order_request_id ������ main_account_no�� ��� account_order_manager�� ã�´�.
  15.2. account_order_manager���� symbol_order_manager�� ã�´�. 
  15.3. symbol_order_manager accepted_order_list�� �߰��� �ش�. 
  15.4. symbol_order_manager buy_order_manager/sell_order_manager�� order�� �߰��� �ش�. 
  15.5. order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
  15.6. account_accepted_order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 


B. �ֹ� ü�� ó�� ����.
1. ���ǻ翡�� �ֹ� ������ �޴´�.
2. �ֹ������� �Ľ��Ͽ� order_info json ��ü�� �����.
3. order_processor ť�� �ִ´�.
4. order_processor�� ť���� ������ �ֹ��� ó���ϱ� �����Ѵ�.
5. order_info�� ���� ���Ŀ� order_info�� custom ������ ���캸�� order_request_id�� �����Ѵ�.
6. order_request_id�� order_request_list�� �ִ��� ���� ����. 
7. order_request_id�� order_request_list�� �ִ� ��� 
  7.1. order_info�� �������� order��ü�� �����.
  7.2. order�� account_no, symbol_code, order_no, filled_price, filled_count, filled_time�� �־��ش�. 
  7.3. order�� order_request_id�� �־��ش�. 
  7.3. order_account_type�� sub_account �� ��� order_account_type�� �־��ش�.  
8. order_request_list�� ���� ��� �� order_request_id ������ ���� ��� - ������ �ܺ� �ֹ�(���� ����ڰ� �ֹ��� ���� �ֹ�â �ܿ� �ٸ� process, �ٸ� ip�ּ� �� �ֹ�)���� ����
  8.1. order_info�� �������� order ��ü�� �����. 
  8.2. order�� account_no, symbol_code, order_no, filled_price, filled_count, filled_time�� �־��ش�. 
9. account_no, symbol_code�� total_position_manager���� position�� ã�´�. 
10. order �ֹ��� �κ� ü���ΰ� �Ϻ� ü���ΰ��� �Ǵ��Ѵ�. 
11. order�� �ֹ� ������ ü�� ������ ��ġ���� ���� ���� �κ� ü��� ���¸� �����Ѵ�.
    �׸��� order�� �ֹ������� �����Ѵ�. 
12. order�� ü��Ǿ��� �� �ֹ� ������ �� �� ���� ���� �ֹ� ���¸� Unresolved_Filled�� �����Ѵ�. 
13. �ֹ����°� Unresolved_Filled�� ���� ����Ȯ�ο��� �� �κ��� Ȯ���Ͽ� ������ ó���Ѵ�. 
14. ã�� position�� filled_order�� ������ ������ ����Ѵ�. 
15. trade_count, open_count, average_price, open_price, settled_count�� ����Ѵ�. 
16. trade_price�� ����ϰ� position trade_profit_loss�� �����Ѵ�. 
17. position open_count, average_price, open_price�� �����Ѵ�. 
18. order�� settled_count�� ������� cut_stop_order�� �����Ѵ�. 
19. account_position_manager�� trade_profit_loss, open_profit_loss�� �����Ѵ�. 
20. order_view, account_position_view�� event�� ������. 
21. order_account_type�� sub_account �� ���
  21.1. order_request_id ������ main_account_no�� ��� account_order_manager�� ã�´�.
  21.2. account_order_manager���� symbol_order_manager�� ã�´�. 
  21.3. account_position_manager�� trade_profit_loss, open_profit_loss�� �����Ѵ�. 
  21.4. order_view, account_position_view�� event�� ������. 

�ֹ��� �ݵ�� �ѹ��� ó���Ѵ�.
�׷��� �ֹ� �̺�Ʈ�� ���õ� ��� �Ϳ� �˷��� �Ѵ�.
������� �ֹ��� �ڽ��� �����¿� �ڽ��� ���� �ݵ忡 �̺�Ʈ�� �˷��� �Ѵ�.
�̺�Ʈ�� ���� �����³� �ݵ�� �� �̺�Ʈ�� ó���Ѵ�.
�� ��Ʈ���� ���� ��ȣ�� �ݵ� �̸����� Ű�� ��´�. 
�̺�Ʈ�� �� �� �� Ű�� �̺�Ʈ�� ó���� ������ �� ������ �����Ѵ�.
�ٸ� StopOrder�� Window id �� Ű�� ��´�.
Stop Order�� ������ �� �ֹ��� �� â���� �ش�Ǳ� �����̴�. 