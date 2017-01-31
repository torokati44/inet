package inet.examples.smoltcp;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import inet.node.smoltcp.SmolTcpNode;
import ned.DatarateChannel;

network SmolTcpServerNet
{
    parameters:
        double per = default(0);
        @display("bgb=232,193");
    types:
        channel C extends DatarateChannel
        {
            datarate = 10Mbps;
            delay = 0.1us;
            per = per;
        }
    submodules:
        client1: StandardHost {
            parameters:
                @display("p=53,67;i=device/pc3");
        }
        server: SmolTcpNode {
            parameters:
                @display("p=181,67;i=device/pc2");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=53,134");
        }
    connections:
        client1.ethg++ <--> C <--> server.ethg++;
}

