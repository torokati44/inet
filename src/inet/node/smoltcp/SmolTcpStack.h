#ifndef __INET_SMOLTCPSTACK_H_
#define __INET_SMOLTCPSTACK_H_

#include <omnetpp.h>
#include <deque>

using namespace omnetpp;

struct Stack { /* opaque Rust struct */ };

namespace inet {

// wrapper and adapter to the Rust implementation
class SmolTcpStack : public cSimpleModule
{
    // opaque reference to the actual Rust Stack instance
    Stack *stack = nullptr;
    std::deque<std::vector<unsigned char>> rxBuffer;

  public:
    // the C API will forward the callback from Rust (from poll...) to these, in the appropriate module instance
    void sendEthernetFrame(unsigned const char *data, unsigned int size);
    unsigned int recvEthernetFrame(unsigned char *buffer);

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

} //namespace

#endif
