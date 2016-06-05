//
// Created by lishunan on 4/19/16.
//

#ifndef HOSTSVC_RPCCHANNEL_H
#define HOSTSVC_RPCCHANNEL_H

#include "HostSvcCommon.hpp"
#include <unordered_map>

using namespace google::protobuf;

namespace GkcHostSvc {
	using messageHandler = function<void(RPCResponse)>;
    class Connection : public protobuf::RpcChannel {
    private:
		std::condition_variable cv;
		std::mutex m;
		std::atomic<bool> connected{ false };
		std::unordered_map<string, messageHandler> handlerMap;
		RPCResponse response1;
        int _clientID = -1;
		WebsocketClient _WSclient;
		shared_ptr < WebsocketConnecton> con;
		shared_ptr<thread> pthread;
		char pSend[1000];
    public:
		static shared_ptr<Connection> create(const string &server) {
			return make_shared<Connection>(server);
		}


		void registerHandler(const string& s,messageHandler h)
		{
			handlerMap.insert({ s,h });
		}

		Connection(const string &server) {
			_WSclient.init_asio();
			_WSclient.start_perpetual();
			_WSclient.set_access_channels(websocketpp::log::alevel::none);
			_WSclient.set_open_handler([this](websocketpp::connection_hdl hdl) {
				this->connected = true;
				cout << "connected" << endl;

			});
			_WSclient.set_message_handler([this](websocketpp::connection_hdl hdl,WebsocketClient::message_ptr msg)
			{
				auto _respondString = msg->get_payload();
				//cout << msg->get_opcode() << "opcode" << "received:" << _respondString.size() << endl;
				RPCResponse r;
				r.ParseFromString(_respondString);
				if(r.ispushmessage())
				{
					cout << "received:"<<_respondString.size() << endl;
					auto tt = handlerMap.at(r.pushtype());
					tt(r);
				}
				else
				{
					response1 = r;
					_clientID = r.clientid();
					print(_clientID);
					println(" receive response");
					cv.notify_one();
				}
			});
#ifdef GKC_SSL
			_WSclient.set_tls_init_handler([this](websocketpp::connection_hdl hdl)->shared_ptr<asio::ssl::context>
			{
				auto ctx = make_shared<asio::ssl::context>(asio::ssl::context::sslv23_client);

				try {
					ctx->set_options(asio::ssl::context::default_workarounds |
						asio::ssl::context::no_sslv2 |
						asio::ssl::context::no_sslv3 |
						asio::ssl::context::single_dh_use);
					ctx->set_verify_mode(0);
				}
				catch (std::exception& e) {
					std::cout << e.what() << std::endl;
				}
				return ctx;
			});
			string protocal = "wss://";
#else
			string protocal = "ws://";
#endif
			pthread=make_shared<thread>([this]()
			{
				cout << "running" << endl;
				_WSclient.run();
			});

			error_code ec;
			con = _WSclient.get_connection(protocal+server,ec);
			if(ec)
			{
				cout << "connection fail "+ec.message()<< endl;
				exit(1);
			}
			if (_WSclient.connect(con) != nullptr) {

			}
			else
			{
				cout << "error connection" << endl;
			}

		}

        Connection(const Connection &) = delete;

        Connection(Connection &&) = default;

        Connection &operator=(const Connection &) = delete;

        Connection &operator=(Connection &&) = default;

        int getClientID() {
            return _clientID;
        }

        ~Connection()
        {
			
        }

		void close()
        {
			con->close(websocketpp::close::status::normal,"quit");
	        _WSclient.stop_perpetual();
			pthread->join();
			pthread.reset();
        }

        void CallMethod(const MethodDescriptor *method, RpcController *controller, const Message *request,
                                Message *response, Closure *done) override{
			std::unique_lock<std::mutex> lk(m);
            auto p = dynamic_cast<const RPCRequest *> (request);
			while (!connected)
				;
			//cout << p->DebugString() << endl << p->ByteSize() << endl;
			p->SerializeToArray(pSend, sizeof pSend);
			auto messageSize = static_cast<size_t > (p->ByteSize());
			auto ec = con->send(pSend, messageSize);
			
			if (ec) {
				cerr << "?" << ec.message() << endl;
				return;
			}

			cv.wait(lk);

			if(controller->Failed())
			{
				cerr << controller->ErrorText();
			}
			auto pLocalResponse = static_cast<RPCResponse *>(response);
			*pLocalResponse = response1;
			
			
            if (done != nullptr)
                done->Run();

			lk.unlock();
        }
    };

    using pConnection=shared_ptr<Connection>;
}

#endif //HOSTSVC_RPCCHANNEL_H
