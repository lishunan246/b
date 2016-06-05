#include "RPCServer.hpp"
#include "TestObject.h"
#include "RealRemoteScreen.h"
using namespace GkcHostSvc;
using namespace google::protobuf;

int main() {
    RPCServer server;
	
    RealObject o;
    RealStringObject s;
	RealRemoteScreen r;
    server.registerObject(&o);
    server.registerObject(&s);
	server.registerObject(&r);

	server.run(10000);

    return 0;
}
