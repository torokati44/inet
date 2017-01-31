package inet.node.smoltcp;

import inet.linklayer.ethernet.EtherQoSQueue;
import inet.linklayer.ethernet.EtherMAC;
import inet.node.base.NodeBase;
import inet.networklayer.common.InterfaceTable;

module SmolTcpNode
{
    @networkNode;
    @display("bgb=200,300");
    gates:
	    inout ethg[];
    submodules:
        interfaceTable: InterfaceTable;
        stack: SmolTcpStack { @display("p=125,100"); };
        queue: EtherQoSQueue { @display("p=75,150"); };
        mac: EtherMAC { @display("p=100,250"); };
    connections:
        stack.ethOut --> queue.in;
        queue.out --> { @display("m=s"); } --> mac.upperLayerIn;
        mac.upperLayerOut --> stack.ethIn;
        ethg++ <--> { @display("m=s"); } <--> mac.phys;
}
