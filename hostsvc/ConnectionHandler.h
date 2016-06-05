//
// Created by lishunan on 16-5-23.
//

#ifndef DESKTOP_CONNECTIONHANDLER_H
#define DESKTOP_CONNECTIONHANDLER_H

#include "HostSvcCommon.hpp"

namespace GkcHostSvc {
    class ConnectionHandler {
        WebsocketServer *server;
        websocketpp::connection_hdl hdl;
    public:
        ConnectionHandler(WebsocketServer *server, websocketpp::connection_hdl hdl) : server(server), hdl(hdl) { }

        void send(const void *payload, size_t len) {
			asio::error_code ec;
			try {
				server->send(hdl, payload, len, websocketpp::frame::opcode::binary, ec);
			}
			catch (std::exception e)
			{
				cerr << "send error" << e.what() << endl;
			}

		}
		void send(const string& s)
        {
			server->send(hdl, s, websocketpp::frame::opcode::BINARY);
        }
    };

    using pConnectionHandler=shared_ptr<ConnectionHandler>;
}


#endif //DESKTOP_CONNECTIONHANDLER_H
