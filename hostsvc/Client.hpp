#include "HostSvcCommon.hpp"
#include "example.pb.h"
#include "RPCController.hpp"
#include "RPCService.hpp"

namespace GkcHostSvc {
	class Client {
        pService _service;
        int _id;
    public:
        using pClient = std::shared_ptr<Client>;

        Client &operator=(const Client &) = delete;

        Client &operator=(Client &&) = default;

		explicit Client(int c, pCreatorMap pCreatorMap1)
				: _id(c) {
            _service = make_shared<RPCService>(pCreatorMap1);
        }

        Client(const Client &) = delete;

        Client(Client &&) = default;

		~Client()
		{
			print("clinet ");
			print(_id);
			println(" disconnecting");
			_service->setConnectionHandler(nullptr);
		}

		static pClient create(int p, pCreatorMap pCreatorMap1) {
            return std::make_shared<Client>( p, pCreatorMap1);
        }


		void handle(WebsocketServer::message_ptr msg,WebsocketServer* server, websocketpp::connection_hdl hdl)
        {
			RPCRequest request;
			auto s = msg->get_raw_payload();
			auto t = request.ParseFromString(s);
			if(!t)
			{
				cerr << "parse fail" << endl;
				return;
			}
			RPCResponse response;
			response.set_clientid(_id);
			RPCController controller;
			_service->setConnectionHandler(make_shared<ConnectionHandler>(server, hdl));
			_service->RPC(&controller, &request, &response, nullptr);
			string r;
			response.SerializeToString(&r);
			server->send(hdl, r,msg->get_opcode());

        }
    };
}