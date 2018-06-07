//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ExtTimeSync.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>

#define NAME "/tmp/opp-sock"

namespace inet {

Define_Module(ExtTimeSync);

void ExtTimeSync::initialize()
{
    if (auto scheduler = dynamic_cast<RealTimeScheduler *>(getSimulation()->getScheduler())) {
        rtScheduler = scheduler;

        struct sockaddr_un server;


        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0) {
            //perror("opening stream socket");
            //exit(1);
        }
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, NAME);
        if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
            //perror("binding stream socket");
            //exit(1);
        }
        printf("Socket has name %s\n", server.sun_path);
        listen(sock, 5);

        rtScheduler->addCallback(sock, this);

    }
    else {
        // throw?
    }

   scheduleAt(simTime() + 1, new cMessage("tick"));
}

void ExtTimeSync::handleMessage(cMessage *msg)
{
    scheduleAt(simTime() + 1, msg);
}


bool ExtTimeSync::notify(int fd)
{
    if (msgsock == fd) {
        int rval;
        char buf[1024];
        if ((rval = read(fd, buf, 1024)) < 0)
            perror("reading stream message");
        else if (rval == 0) {
            printf("Ending connection\n");
            rtScheduler->removeCallback(msgsock, this);
        } else {
            buf[50] = 0;
            printf("-->%s\n", buf);
        }

        return true;
    }
    else if (fd == sock) {
        std::cout << "A NEW CONNECTION" << std::endl;
        rtScheduler->removeCallback(msgsock, this);
        msgsock = accept(sock, 0, 0);
        std::cout << "NEW CONNECTION ACCEPTED" << std::endl;
        rtScheduler->addCallback(msgsock, this);
        return true;
    }
    else
        return false;
}


ExtTimeSync::~ExtTimeSync()
{
    close(sock);
    close(msgsock);
    unlink(NAME);
}

} //namespace
