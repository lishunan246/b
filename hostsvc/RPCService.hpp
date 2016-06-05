//
// Created by lishunan on 4/20/16.
//

#ifndef HOSTSVC_SERVICEIMPL_H
#define HOSTSVC_SERVICEIMPL_H

#include "HostSvcCommon.hpp"
#include "RealObjectBase.hpp"
#include "RPCController.hpp"
#include <functional>
#include "ConnectionHandler.h"
namespace GkcHostSvc {
    class RPCService : public EchoService {
        ObjectMap _map;
        int _MapIndex=1;
        map<string, RealObjectBase::ObjectService> _serviceMap;
        pCreatorMap _pCreatorMap1;
        pConnectionHandler _hdl;
    public:

        RPCService(pCreatorMap pCreatorMap1) : _pCreatorMap1(pCreatorMap1) {
        }
        RPCService(const RPCService&)= delete;
        RPCService(RPCService&&)=default;
        RPCService&operator=(RPCService&&)= default;
        RPCService&operator=(const RPCService&)= delete;

        ~RPCService() {
            for_each(_map.begin(), _map.end(), [](std::pair<const int, shared_ptr<RealObjectBase>> &pair) {
                pair.second->SetConnectionHandler(nullptr);
            });
        }

        void setConnectionHandler(pConnectionHandler hdl) {
            _hdl = hdl;
        }

        virtual void RPC(RPCController *controller, const RPCRequest *rpcRequest, RPCResponse *response,
                         google::protobuf::Closure *done)
        {
            auto ob = rpcRequest->object();
            response->set_success(false);
            std::shared_ptr<RealObjectBase> p;
            if (ob == 0) {
                if (rpcRequest->method() == "new") {
                    auto t = _pCreatorMap1->at(rpcRequest->paramstring());

                    auto x = t();
					
                    x->SetConnectionHandler(_hdl);
                    _map.insert({_MapIndex, x});
                    auto type = x->get_type();
                    //if (!_serviceMap[type])
                        _serviceMap.insert({type, x->get_service()});
                    response->set_success(true);
                    response->set_resultint(_MapIndex);
                    ++_MapIndex;
                }
                else if (rpcRequest->method() == "delete") {
                    _map.erase(rpcRequest->paramint());
                    response->set_success(true);
                }
            }
            else if ((p = _map.at(ob))) {
                cout << p->get_type() << endl;
                auto t = _serviceMap[p->get_type()];
                if (t != nullptr) {
                    t(rpcRequest, response, p);
                }
                else
				{
					cerr << "invalid type: " << p->get_type() << endl;
					controller->SetFailed("invalid type");
				}
                    
            }
        }
    };
    using pService=shared_ptr<RPCService>;
}


#endif //HOSTSVC_SERVICEIMPL_H
