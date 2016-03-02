all:
	g++ ./src/msgq_manager.cpp ./src/service_dispatcher.cpp ./src/reactor.cpp ./src/netio.cpp -o netio -I./include
	g++ ./src/msgq_manager.cpp ./src/service_loader.cpp ./src/reactor.cpp ./src/service_dispatcher.cpp ./src/container.cpp -o container -I./include -ldl
	g++ -fPIC -shared ./src/svc_test.cpp -o ./so/svc_test.so -I./include
	g++ -fPIC -shared ./src/svc_test2.cpp -o ./so/svc_test2.so -I./include