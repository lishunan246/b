//
// Created by lishunan on 4/23/16.
//

#ifndef HOSTSVC_REALOBJECTBASE_H
#define HOSTSVC_REALOBJECTBASE_H

#include <map>
#include <memory>
#include "HostSvcCommon.hpp"
#include <functional>
#include "ConnectionHandler.h"
namespace GkcHostSvc {
    class RealObjectBase {
        pConnectionHandler _hdl;

    public:
		using ObjectService = function<void(const RPCRequest *, RPCResponse *, const shared_ptr<RealObjectBase> &)>;
        RealObjectBase() { }

        RealObjectBase(const RealObjectBase &) = delete;

        RealObjectBase &operator=(const RealObjectBase &) = delete;

        RealObjectBase(RealObjectBase &&) = default;

        RealObjectBase &operator=(RealObjectBase &&) = default;

		virtual ~RealObjectBase() { }

        void SetConnectionHandler(pConnectionHandler p) {
            _hdl = p;
        }

        void send(const void *payload, size_t len) {
			if (_hdl)
			{
				_hdl->send(payload, len);
			}
        }
		void send(const string& s)
        {
			if(_hdl)
				_hdl->send(s);
        }

        virtual ObjectService get_service() const = 0;

        virtual const string &get_type() = 0;

        virtual std::function<shared_ptr<RealObjectBase>(void)> get_creator() = 0;
    };

	
	using ObjectMap = map<int, shared_ptr<RealObjectBase>>;
	using pObjectMap = std::shared_ptr<ObjectMap>;
	using Creator = std::function<shared_ptr<RealObjectBase>(void)>;
	using CreatorMap = map<string, Creator>;
	using pCreatorMap = std::shared_ptr<CreatorMap>;
}


#endif //HOSTSVC_REALOBJECTBASE_H
