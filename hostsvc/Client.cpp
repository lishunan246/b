//
// Created by lishunan on 16-4-2.
//
#define SDL_MAIN_HANDLED

#include "HostSvcCommon.hpp"
#include "Connection.hpp"
#include "TestObject.h"
#include "ProxyRemoteScreen.h"
#include "RPCClient.h"

using namespace GkcHostSvc;

//const static std::string server("127.0.0.1");
//const static std::string server("192.168.78.131");
//const static std::string server("192.168.78.128");
//const static std::string server("192.168.78.136");
void test() {

    RPCClient client("127.0.0.1:10000");


    ProxyStringObject string(client.newConnection());
    ProxyObject counter(client.newConnection());
    int a;
    cout << "counter1: add" << endl;
    cin >> a;
    counter.add(a);

    std::cout << "counter1: ";
    std::cout << counter.getCount() << std::endl;
    cout << "counter1: sub" << endl;
    cin >> a;
    counter.sub(a);
    std::cout << "counter1: ";
    std::cout << counter.getCount() << std::endl;
    std::string x = "";
    cout << "string append:" << endl;
    cin >> x;
    string.append(x);
    cout << "string append:" << endl;
    cin >> x;
    string.append(x);
    ProxyObject counter2(client.newConnection());
    counter2.sub(133);
    std::cout << "Counter 2: ";
    std::cout << counter2.getCount() << std::endl;

    std::cout << "string: " << string.get() << endl;
#ifdef WIN32
    system("pause");
#endif
}


void test2() {

    //auto pConnection = Connection::create("192.168.78.1:10000");
	auto pConnection = Connection::create("127.0.0.1:10000");

    if (pConnection == nullptr)
        return;

    ProxyRemoteScreen p(pConnection);

    p.activate();
	bool a = true;
	while (1)
	{
		system("pause");
		if(a)
		{
			p.deactivate();
			
		}
		else
		{
			p.activate();
		}
		a = !a;
	}
	
	
#ifdef WIN32
	system("pause");
#else
	sleep(4);
#endif
       

    p.deactivate();


    pConnection->close();
}

int main() {
    test2();

    return 0;
}
