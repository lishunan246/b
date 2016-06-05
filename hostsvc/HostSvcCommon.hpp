//
// Created by lishunan on 16-4-2.
//

#ifndef HOSTSVC_HOSTSVCCOMMON_H
#define HOSTSVC_HOSTSVCCOMMON_H

#define GKC_SSL

#define ASIO_STANDALONE
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
#define ASIO_NO_TYPEID
#define ASIO_HAS_STD_CHRONO

#define  _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#include <iostream>
#include <asio.hpp>
#include <string>
#include <atomic>
#include <utility>

#include "PackedMessage.hpp"
#include "example.pb.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp> 
#include <websocketpp/config/asio.hpp>
namespace GkcHostSvc {
	using asio::ip::tcp;
	using namespace std;
	using namespace google;
    static const int PORT_INT(10000);
    static const std::string PORT("10000");

	template<typename T>
	void print(T t)
	{
#ifdef _DEBUG

		cout <<t;
#endif
	}

	template<typename T>
	void println(T t)
	{
#ifdef _DEBUG

		cout << t<<endl;
#endif
	}

	using atomicInt = std::atomic<int>;
	using pAtomicInt = std::shared_ptr<atomicInt>;
	using pSocket = std::shared_ptr<asio::ip::tcp::socket>;
#ifdef GKC_SSL
	using WebsocketServer = websocketpp::server< websocketpp::config::asio_tls>;
	using WebsocketClient = websocketpp::client<websocketpp::config::asio_tls_client>;
	using WebsocketConnecton = websocketpp::connection < websocketpp::config::asio_tls_client >;
#else
	using WebsocketServer = websocketpp::server< websocketpp::config::asio>;	
	using WebsocketClient = websocketpp::client<websocketpp::config::asio_client>;
	using WebsocketConnecton = websocketpp::connection < websocketpp::config::asio_client >;
#endif
	
#ifdef WIN32
	string PATH = "D:\\";
	string TMPPATH = ".\\temp\\";
#else
	string PATH = "/home/lishunan/";
	string TMPPATH= "./temp/";
#endif
}


#endif //HOSTSVC_HOSTSVCCOMMON_H
