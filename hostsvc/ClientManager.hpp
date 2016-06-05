//
// Created by lishunan on 5/7/16.
//

#ifndef DESKTOP_CLIENTMANAGER_H
#define DESKTOP_CLIENTMANAGER_H

#include "HostSvcCommon.hpp"

namespace GkcHostSvc
{
    class ClientManager {
    public:
        atomicInt count{0};
        atomicInt index{0};

        void newClient() {
	        ++count;
	        ++index;
        }

        void deleteClient() {
	        --count;
        }
    };
}


#endif //DESKTOP_CLIENTMANAGER_H
