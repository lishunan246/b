//
// Created by lishunan on 4/23/16.
//

#ifndef HOSTSVC_PROXYOBJECTBASE_H
#define HOSTSVC_PROXYOBJECTBASE_H

#include "HostSvcCommon.hpp"
#include "Connection.hpp"
#include "RPCController.hpp"

namespace GkcHostSvc {
    class ProxyObjectBase {
    protected:
        pConnection _pConnection;
        EchoService_Stub _service;
        RPCRequest _request;
        RPCResponse _response;
        RPCController _controller;
        int _objectID;
        string _type;
		void call(const string& functionName)
		{
			_request.set_clientid(_pConnection->getClientID());
			_request.set_object(_objectID);
			_request.set_method(functionName);

			_service.RPC(&_controller, &_request, &_response, nullptr);
		}

		void call(const string& functionName,const string& param)
		{
			_request.set_clientid(_pConnection->getClientID());
			_request.set_object(_objectID);
			_request.set_method(functionName);
			_request.set_paramstring(param);

			_service.RPC(&_controller, &_request, &_response, nullptr);
		}
		void call(const string& functionName, int param)
		{
			_request.set_clientid(_pConnection->getClientID());
			_request.set_object(_objectID);
			_request.set_method(functionName);
			_request.set_paramint(param);

			_service.RPC(&_controller, &_request, &_response, nullptr);
		}

		void call(const string& functionName, bool param)
		{
			_request.set_clientid(_pConnection->getClientID());
			_request.set_object(_objectID);
			_request.set_method(functionName);
			_request.set_parambool(param);

			_service.RPC(&_controller, &_request, &_response, nullptr);
		}

    public:
        ProxyObjectBase(pConnection Connection, const string &type) : _pConnection(Connection),
                                                                      _service(Connection.get()), _type(type)
        {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(0);
            _request.set_method("new");
            //_request.set_paramint(_name);
            _request.set_paramstring(_type);
            _service.RPC(&_controller, &_request, &_response, nullptr);
            _objectID = _response.resultint();
        }

        ProxyObjectBase(const ProxyObjectBase &) = delete;

        ProxyObjectBase &operator=(const ProxyObjectBase &) = delete;

        ProxyObjectBase(ProxyObjectBase &&) = default;

        ProxyObjectBase &operator=(ProxyObjectBase &&) = default;

        virtual ~ProxyObjectBase()
        {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(0);
            _request.set_method("delete");
            _request.set_paramint(_objectID);

            _service.RPC(&_controller, &_request, &_response, nullptr);
			if (_pConnection.use_count() == 1)
				_pConnection->close();
        }
    };
	using pProxyObjectBase = shared_ptr<ProxyObjectBase>;
	
}


#endif //HOSTSVC_PROXYOBJECTBASE_H
