//
// Created by lishunan on 16-5-25.
//

#ifndef DESKTOP_PROXYREMOTESCREEN_H
#define DESKTOP_PROXYREMOTESCREEN_H

#include "RemoteScreen.hpp"
#include "ProxyRemoteScreen_Export.h"
class ProxyRemoteScreen_EXPORT ProxyRemoteScreen : public RemoteScreen, public ProxyObjectBase {
    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;
    string _screen;
    SDL_Window *_window;
    SDL_Surface *_screenSurface, *g;
public:
    explicit ProxyRemoteScreen(pConnection connection) : ProxyObjectBase(connection, RemoteScreen::_type) {
        connection->registerHandler(RemoteScreen::_type, get_message_handler());
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            cerr << "SDL_Init() Failed: " << SDL_GetError() << endl;
            exit(1);
        }
        _window = SDL_CreateWindow("App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
        if (_window == nullptr) {
            cerr << "can't create window " << SDL_GetError() << endl;
        }
    }

    void display(const string &gs) {
		//return;
		auto x = SDL_RWFromConstMem(gs.c_str(), (int)gs.size());
//        auto now = std::time(nullptr);
//        auto name = std::to_string(now) + ".bmp";
//
//        ofstream outfile(name, ofstream::binary | ofstream::trunc);
//
//        outfile << gs;
//        outfile.close();
//
//        g = SDL_LoadBMP(name.c_str());
		g = SDL_LoadBMP_RW(x, 1);
        if (g == nullptr)
            cerr << "load fail" << SDL_GetError() << endl;

        _screenSurface = SDL_GetWindowSurface(_window);
        //SDL_FillRect(_screenSurface, nullptr, SDL_MapRGB(_screenSurface->format, 0xff, 0xff, 0xff));
        SDL_BlitSurface(g, nullptr, _screenSurface, nullptr);
        SDL_UpdateWindowSurface(_window);
        SDL_FreeSurface(g);
        g = nullptr;
        //std::remove(name.c_str());
    }

    void activate() override {
        call("activate");
    }

    void deactivate() override {
        call("deactivate");
    }

    string get_screen() override {
        call("get_screen");
        _screen = _response.resultbytes();
        cout << _response.ByteSize() << "transferred" << endl;
        return _screen;
    }

    void close() {
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

    messageHandler get_message_handler() {
        return [this](RPCResponse response) {
            //auto p2 = static_pointer_cast<ProxyRemoteScreen>(p);
            auto x = response.resultbytes();
			cout<<("get frame ");
			cout << response.resultint() << endl;
            this->display(x);
        };
    }
};


#endif //DESKTOP_PROXYREMOTESCREEN_H
