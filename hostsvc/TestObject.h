//
// Created by lishunan on 4/19/16.
//

#ifndef HOSTSVC_TESTOBJECT_H
#define HOSTSVC_TESTOBJECT_H

#include "Connection.hpp"
#include "RealObjectBase.hpp"
#include "ProxyObjectBase.hpp"

namespace GkcHostSvc {


    class TestStringObject {
    protected:
        static const string _type;
    public:
	    virtual ~TestStringObject()
	    {
	    }

	    virtual void append(const std::string &s) = 0;

        virtual void clear() = 0;

        virtual const std::string &get() = 0;
    };

    const string  TestStringObject::_type = "string";

    class RealStringObject : public TestStringObject, public RealObjectBase {
    private:
        std::string _string;

    public:
        RealStringObject() { }

        const string &get_type() override {
            return TestStringObject::_type;
        }

        ObjectService get_service() const override {
            return [](const RPCRequest *rpcRequest, RPCResponse *response, const shared_ptr<RealObjectBase> &p) {
                auto xp = dynamic_pointer_cast<RealStringObject>(p);
                auto method = rpcRequest->method();
                if (method == "append") {
                    xp->append(rpcRequest->paramstring());
                    response->set_success(true);
                }
                if (method == "clear") {
                    xp->clear();
                    response->set_success(true);
                }
                if (method == "get") {
                    auto s = xp->get();
                    response->set_resultstring(s);
                    response->set_success(true);
                }
            };
        }

        std::function<shared_ptr<RealObjectBase>(void)> get_creator()  override{
            return []() {
                return make_shared<RealStringObject>();
            };
        }


        void append(const string &s) override {
            cout << "append " << s << endl;
            _string += s;
        }

        void clear() override {
            cout << "clear " << endl;
            _string = "";
        }

        const string &get() override {
            cout << "get " << _string << endl;
            return _string;
        }
    };

    class ProxyStringObject : public TestStringObject, public ProxyObjectBase {
    public:
        ProxyStringObject(pConnection connection) : ProxyObjectBase(connection,
                                                                    TestStringObject::_type) {}

        void append(const string &s) override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("append");
            _request.set_paramstring(s);

            _service.RPC(&_controller, &_request, &_response, nullptr);

        }

        void clear() override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("clear");
            _service.RPC(&_controller, &_request, &_response, nullptr);

        }

        const string &get() override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("get");
            _request.set_paramint(0);

            _service.RPC(&_controller, &_request, &_response, nullptr);

            return _response.resultstring();
        }
    };


    class TestObject {
    protected:
        const static string _type;
    public:
	    virtual ~TestObject()
	    {
	    }

	    virtual void add(int n) = 0;

        virtual void sub(int n) = 0;

        virtual int getCount() = 0;
    };

    const string TestObject::_type = "counter";

    class RealObject : public TestObject, public RealObjectBase {
    private:
        int _count = 0;
    public:
        RealObject() { }

        const string &get_type() override {
            return TestObject::_type;
        }

        ObjectService get_service() const override {
            return [](const RPCRequest *rpcRequest, RPCResponse *response, const shared_ptr<RealObjectBase> &p) {
                auto xp = dynamic_pointer_cast<RealObject>(p);
                if (rpcRequest->method() == "add") {
                    xp->add(rpcRequest->paramint());
                    response->set_success(true);
                }
                else if (rpcRequest->method() == "sub") {
                    xp->sub(rpcRequest->paramint());
                    response->set_success(true);
                }
                else if (rpcRequest->method() == "getCount") {
                    auto x = xp->getCount();
                    response->set_success(true);
                    response->set_resultint(x);
                }
            };
        }

        std::function<shared_ptr<RealObjectBase>(void)> get_creator() {
            return []() {
                return make_shared<RealObject>();
            };
        }

        void add(int n) override {
            cout << "add " << n << endl;
            _count += n;
        }

        void sub(int n) override {
            cout << "sub " << n << endl;
            _count -= n;
        }

        int getCount() override {
            cout << "count " << _count << endl;
            return _count;
        }
    };

    class ProxyObject : public TestObject, public ProxyObjectBase {

    public:
        ProxyObject(pConnection rpcChannel) : ProxyObjectBase(rpcChannel, TestObject::_type) { }

        void add(int n) override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("add");
            _request.set_paramint(n);

            _service.RPC(&_controller, &_request, &_response, nullptr);

        }

        void sub(int n) override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("sub");
            _request.set_paramint(n);

            _service.RPC(&_controller, &_request, &_response, nullptr);

        }

        int getCount() override {
            _request.set_clientid(_pConnection->getClientID());
            _request.set_object(_objectID);
            _request.set_method("getCount");
            _request.set_paramint(0);

            _service.RPC(&_controller, &_request, &_response, nullptr);

            return _response.resultint();
        }


    };

}


#endif //HOSTSVC_TESTOBJECT_H
