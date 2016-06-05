//
// Created by lishunan on 4/30/16.
//

#ifndef DESKTOP_REMOTESCREEN_H
#define DESKTOP_REMOTESCREEN_H

#include <fstream>

#define SDL_MAIN_HANDLED

#include "SDL.h"
#include "ProxyObjectBase.hpp"
#include "RealObjectBase.hpp"
#include "RepeatWork.h"

using namespace GkcHostSvc;


class RemoteScreen {
protected:
    string _type = "RemoteScreen";
public:
    virtual ~RemoteScreen() {
    }

    virtual string get_screen() = 0;

    virtual void activate() = 0;

    virtual void deactivate() = 0;
};


#endif //DESKTOP_REMOTESCREEN_H
