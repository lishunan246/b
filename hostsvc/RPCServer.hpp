#include "HostSvcCommon.hpp"
#include "Client.hpp"
#include "ClientManager.hpp"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace GkcHostSvc
{
	using namespace websocketpp;
	using ClientMap = map<connection_hdl, Client::pClient, owner_less<connection_hdl>>;

	class RPCServer
	{
		ClientMap _clientMap;
		WebsocketServer _websocket_server;

		pCreatorMap _pCreatorMap = std::make_shared<CreatorMap>();
		ClientManager _cm;

		string _crt = PATH + "mycert.pem";
		string _dhkey = PATH + "dh.pem";
		string _privatekey = PATH + "example.key";
	public:
		explicit RPCServer()
		{
			_websocket_server.init_asio();
			
			_websocket_server.clear_access_channels(log::alevel::all);
			_websocket_server.set_open_handler([this](connection_hdl hdl)
				{
					print("clinet ");
					print(_cm.index.load());
					println(" connecting");
					auto newClient = Client::create(this->_cm.index, this->_pCreatorMap);
					_cm.newClient();
					_clientMap.insert({hdl,newClient});
				});

			_websocket_server.set_close_handler([this](connection_hdl hdl)
				{
					println("disconnecting");
					_cm.deleteClient();
					_clientMap.erase(hdl);
					
				});

			_websocket_server.set_message_handler([this](connection_hdl hdl, WebsocketServer::message_ptr msg)
				{
					auto x = _clientMap[hdl];
					if (x == nullptr)
					{
						cerr << "invalid hdl" << endl;
						exit(1);
					}

					x->handle(msg, &_websocket_server, hdl);
				});

#ifdef GKC_SSL
			_websocket_server.set_tls_init_handler([this](websocketpp::connection_hdl hdl)->shared_ptr<asio::ssl::context>
				{
					auto ctx = make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
					ctx->set_password_callback([](std::size_t size,
							asio::ssl::context_base::password_purpose purpose)
						{
							string s("test");
							return s;
						});
					asio::error_code ec;
					ctx->use_certificate_chain_file(_crt,ec);
					if(ec)
					{
						cerr << ec.message() << endl;
						exit(1);
					}
					ctx->use_private_key_file(_privatekey, asio::ssl::context::pem,ec);
					if (ec)
					{
						cerr << ec.message() << endl;
						exit(2);
					}
					ctx->use_tmp_dh_file(_dhkey,ec);
					if (ec)
					{
						cerr << ec.message() << endl;
						exit(3);
					}
					if (SSL_CTX_set_cipher_list(ctx->native_handle(), "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK") != 1)
					{
						std::cerr << "set cipher list error" << endl;
					}
					return ctx;
				});
#endif		
		}

		RPCServer(const RPCServer &) = delete;

		RPCServer(RPCServer &&) = default;

		RPCServer &operator=(const RPCServer &) = delete;

		RPCServer &operator=(RPCServer &&) = default;

		void run(uint16_t port)
		{
			_websocket_server.listen(port);
			_websocket_server.start_accept();
			_websocket_server.run();
		}

		void registerObject(RealObjectBase* r)
		{
			_pCreatorMap->insert({r->get_type(), r->get_creator()});
		}
	};
}
