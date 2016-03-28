all:
	g++ -g ./src/msgq_manager.cpp ./src/reactor.cpp ./src/cache_manager.cpp ./src/netio.cpp ./src/netio_user_event_handler.cpp ./src/tcp_net_handler.cpp ./src/usock_udp_handler.cpp -o netio -I./include
	g++ -g -fPIC ./src/msgq_manager.cpp ./src/service_loader.cpp ./src/reactor.cpp ./src/container.cpp ./src/container_user_event_handler.cpp ./src/service_dispatcher.cpp ./src/intf_service.cpp -o container -I./include -ldl -lpth
	g++ -g -fPIC -shared ./src/service_dispatcher.cpp ./src/cache_manager.cpp ./src/intf_service.cpp ./src/svc_test.cpp -o ./so/svc_test.so -I./include -lpth
	g++ -g -fPIC -shared ./src/msgq_manager.cpp ./src/cache_manager.cpp ./src/service_dispatcher.cpp ./src/intf_service.cpp ./src/svc_test2.cpp -o ./so/svc_test2.so -I./include -lpth