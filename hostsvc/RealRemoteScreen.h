//
// Created by lishunan on 16-5-25.
//

#ifndef DESKTOP_REALREMOTESCREEN_H
#define DESKTOP_REALREMOTESCREEN_H

#include "RemoteScreen.hpp"
#include "RealRemoteScreen_Export.h"
class RealRemoteScreen_EXPORT RealRemoteScreen : public RemoteScreen, public RealObjectBase {
    shared_ptr<RepeatWork> sPW;
    std::mutex m, m2;
	const int SIZE = 5000000;
    //vector<string> files = {PATH + "4m.bmp", PATH + "1080.bmp"};
	vector<string> files;
	int loaded;
	vector< std::pair<int, char*>> vc;
	int count{0};
	int _index{ 0 };
public:
    RealRemoteScreen();

	~RealRemoteScreen();

    void activate() override {
        sPW->activate();
    }

    void deactivate() override {
        cout << "deactivate" << endl;
        sPW->deactivate();
    }

	std::pair<int,char*> get_SDL_RWops(string filename)
    {

		auto file = SDL_RWFromFile(filename.c_str(), "rb");
		if(!file)
		{
			cerr << SDL_GetError()<<endl;
		}
		auto size = SDL_RWsize(file);
		char* c = new char[size];
		SDL_RWread(file, c, size, 1);
		return{ size,c };
    }

    string get_screen() override {
        static int i = 0;
        if (i == 0)
            i = 1;
        else
            i = 0;
        m2.lock();
        ifstream fin(files[i], ios::binary);
        if (!fin.is_open()) {
            cerr << "fail openfile" << endl;
            return "";
        }
        auto x = fin.rdbuf();
        ostringstream ostrm;
        ostrm << x;
        fin.close();
        m2.unlock();
        return ostrm.str();
    }

    ObjectService get_service() const override {
        return [](const RPCRequest *rpcRequest, RPCResponse *response, const shared_ptr<RealObjectBase> &p) {
            auto xp = dynamic_pointer_cast<RealRemoteScreen>(p);
            auto method = rpcRequest->method();
            if (method == "activate") {
                xp->activate();

                response->set_success(true);
            }
            else if (method == "deactivate") {
                xp->deactivate();

                response->set_success(true);
            }
            else if (method == "get_screen") {
                auto x = xp->get_screen();

                response->set_success(true);
                response->set_resultbytes(x);
            }
            else {
                cerr << "unknow method" << endl;
            }
        };
    }

    const string &get_type() override {
        return _type;
    }

    Creator get_creator() override {
        return []() {
            return make_shared<RealRemoteScreen>();
        };
    }
};


#endif //DESKTOP_REALREMOTESCREEN_H
