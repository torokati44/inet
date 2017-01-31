package inet.node.smoltcp;

//
// Wraps a smoltcp stack.
// Currently that includes an application, therefore only ethernet gates are present.
//
simple SmolTcpStack
{
    gates:
        input ethIn;
        output ethOut;
}
