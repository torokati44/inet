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




#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>


#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)


static
int * recv_fd(int socket, int n) {
        int *fds = (int*)malloc (n * sizeof(int));
        struct msghdr msg = {0};
        struct cmsghdr *cmsg;
        char buf[CMSG_SPACE(n * sizeof(int))], dup[256];
        memset(buf, '\0', sizeof(buf));
        struct iovec io = { .iov_base = &dup, .iov_len = sizeof(dup) };

        msg.msg_iov = &io;
        msg.msg_iovlen = 1;
        msg.msg_control = buf;
        msg.msg_controllen = sizeof(buf);

        if (recvmsg (socket, &msg, 0) < 0)
                handle_error ("Failed to receive message");

        cmsg = CMSG_FIRSTHDR(&msg);

        memcpy (fds, (int *) CMSG_DATA(cmsg), n * sizeof(int));

        return fds;
}




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

   scheduleAt(simTime(), new cMessage("tick"));
}

void ExtTimeSync::handleMessage(cMessage *msg)
{
    scheduleAt(simTime() + 0.1, msg);
}


bool ExtTimeSync::notify(int fd)
{
    if (msgsock == fd) {
        int rval;
        int opcode;
        if ((rval = read(fd, &opcode, 4)) < 0)
            perror("reading stream message");
        else if (rval == 0) {
            printf("Ending connection\n");
            rtScheduler->removeCallback(msgsock, this);
        } else {
            EV_INFO << "Read opcode: " << opcode << std::endl;

            int64 secs = simTime().inUnit(SimTimeUnit::SIMTIME_S);
            int64 usecs = simTime().inUnit(SimTimeUnit::SIMTIME_US) % 1000000;

            ::send(fd, &secs, 8, MSG_EOR);
            ::send(fd, &usecs, 8, MSG_EOR);

            //buf[50] = 0;
            //printf("-->%s\n", buf);
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
