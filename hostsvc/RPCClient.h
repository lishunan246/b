//
// Created by lishunan on 16-5-9.
//

#ifndef DESKTOP_RPCCLIENT_H
#define DESKTOP_RPCCLIENT_H

#include <string>
#include "Connection.hpp"

namespace GkcHostSvc {
    using namespace std;

    class RPCClient {
    private:
        string _address;
    public:
        RPCClient(string address) {
            _address = address;
        }

        pConnection newConnection() {
            return Connection::create(_address);
        }
    };
}
#endif //DESKTOP_RPCCLIENT_H
