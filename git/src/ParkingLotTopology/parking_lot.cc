#include <iostream>
#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h" 
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/config-store-module.h"
#include "ns3/node.h"
#include "ns3/netanim-module.h"


#define MAX_SOURCES 100;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TCPSCRIPT");
/**
 * -----------------------------------------------------------------------
 * Declaration of all variables required for tracing the given parameters
 * -----------------------------------------------------------------------
*/


uint64_t queueSize;
uint64_t queueSize_02;
Ptr<OutputStreamWrapper> qSize_stream;

uint64_t droppedPackets;
uint64_t droppedPackets_02;
uint64_t previously_droppedPackets;
uint64_t previously_droppedPackets_02;
Ptr<OutputStreamWrapper> dropped_stream;

uint32_t pkt_count = 0;
uint32_t prev_pkt_count = 0;
uint32_t pkt_count_02 = 0;
uint32_t prev_pkt_count_02 = 0;
uint32_t pkt_count_03 = 0;
uint32_t prev_pkt_count_03 = 0;
Time prevTime = Seconds (0);
Ptr<OutputStreamWrapper> bottleneckTransmittedStream;

uint64_t packetsTransmitted;
uint64_t previous_transmitted_packets = 0;
uint64_t packetsTransmitted_02;
uint64_t previous_transmitted_packets_02 = 0;
Time prevTime02 = Seconds (0);
Ptr<OutputStreamWrapper> utilization;

/**
 * --------------------------------------------------------------------- 
 *                  Functions for tracing Queue Size
 * --------------------------------------------------------------------- 
*/

static void plotQsizeChange (uint32_t oldQSize, uint32_t newQSize){
    queueSize = newQSize;
}

static void plotQsizeChange_02 (uint32_t oldQSize, uint32_t newQSize){
    queueSize_02 = newQSize;
}

static void TraceQueueSize(){
    *qSize_stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << queueSize << "\t" << queueSize_02 << std::endl;
}

static void StartTracingQueueSize(){
    Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/TxQueue/PacketsInQueue", MakeCallback(&plotQsizeChange));
    Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/TxQueue/PacketsInQueue", MakeCallback(&plotQsizeChange_02));
}

/**
 * --------------------------------------------------------------------- 
 *                  Functions for tracing Packet Loss 
 * --------------------------------------------------------------------- 
*/

static void RxDrop(Ptr<OutputStreamWrapper> stream,  Ptr<const Packet> p){
   droppedPackets++;
} 
static void RxDrop_02(Ptr<OutputStreamWrapper> stream,  Ptr<const Packet> p){
   droppedPackets_02++;
} 
static void TraceDroppedPacket(std::string droppedTrFileName){
    // tracing all the dropped packets in a seperate file
    Config::ConnectWithoutContext("/NodeList/0/DeviceList/*/TxQueue/Drop", MakeBoundCallback(&RxDrop, dropped_stream));
    Config::ConnectWithoutContext("/NodeList/1/DeviceList/*/TxQueue/Drop", MakeBoundCallback(&RxDrop_02, dropped_stream));

}

static void TraceDroppedPkts(){
    uint64_t packetLoss = (droppedPackets - previously_droppedPackets);
    uint64_t packetloss02 = (droppedPackets_02 - previously_droppedPackets_02);
    *dropped_stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << packetLoss + packetloss02 << std::endl;
    previously_droppedPackets = droppedPackets;
    previously_droppedPackets_02 = droppedPackets_02;
}

/**
 * --------------------------------------------------------------------- 
 *                  Functions for tracing Network Throughput
 * --------------------------------------------------------------------- 
*/

// For saving packets received at the sink at each time instant into the defined ascii stream
static void
TraceBottleneckTx(uint32_t pktSize){
    Time currrent_time = Now();
    // float btl_thr = (pkt_count*8*pktSize) /(1000*1000);
    float btl_thr = ((pkt_count-prev_pkt_count)*8*pktSize)/(1000 * 1000*(currrent_time.GetSeconds() - prevTime.GetSeconds()));
    float btl_thr_2 = ((pkt_count_02-prev_pkt_count_02)*8*pktSize)/(1000 * 1000*(currrent_time.GetSeconds() - prevTime.GetSeconds()));
    float btl_thr_3 = ((pkt_count_03-prev_pkt_count_03)*8*pktSize)/(1000 * 1000*(currrent_time.GetSeconds() - prevTime.GetSeconds()));
    *bottleneckTransmittedStream->GetStream() << Simulator::Now().GetMilliSeconds() << "\t" << btl_thr << "\t" << btl_thr_2 << "\t"<< btl_thr_3 <<std::endl;
    prev_pkt_count = pkt_count;
    prev_pkt_count_02 = pkt_count_02;
    prev_pkt_count_03 = pkt_count_03;
    prevTime = currrent_time;
}


//For counting the number of packets received at the sink
static void SinkRxCount(Ptr<const Packet> p, const Address &ad )
{  
  //std::cout << "One packet received" << std::endl;
  pkt_count++;
  //std::cout << pkt_count << std::endl; const Address &ad
}
static void SinkRxCount_02(Ptr<const Packet> p, const Address &ad )
{  
  //std::cout << "One packet received" << std::endl;
  pkt_count_02++;
  //std::cout << pkt_count << std::endl; const Address &ad
}
static void SinkRxCount_03(Ptr<const Packet> p, const Address &ad )
{  
  //std::cout << "One packet received" << std::endl;
  pkt_count_03++;
  //std::cout << pkt_count << std::endl; const Address &ad
}

// Call SinkRxCount function everytime a packet is received at the application layer of the sink node 
static void 
StartTracingSink(){
    for(int i = 93; i < 123; i++){
        std::string path = "/NodeList/" + std::to_string(i) + "/ApplicationList/*/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext(path, MakeCallback (&SinkRxCount));
    }
    for(int i = 123; i < 152; i++){
        std::string path = "/NodeList/" + std::to_string(i) + "/ApplicationList/*/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext(path, MakeCallback (&SinkRxCount_02));
    }
     for(int i = 153; i < 182; i++){
        std::string path = "/NodeList/" + std::to_string(i) + "/ApplicationList/*/$ns3::PacketSink/Rx";
        Config::ConnectWithoutContext(path, MakeCallback (&SinkRxCount_03));
    }
    //Config::ConnectWithoutContext("/NodeList/62/DeviceList/0/$ns3::PointToPointChannel/TxRxPointToPoint", MakeCallback(&SinkRxCount));
    //Config::ConnectWithoutContext("/NodeList/1/DeviceList/0/$ns3::PointToPointNetDevice/PhyTxEnd", MakeCallback(&SinkRxCount));
    //Config::ConnectWithoutContext("/NodeList/62/ApplicationList/0/$ns3::PacketSink/Rx", MakeCallback (&SinkRxCount));
}

/**
 * --------------------------------------------------------------------- 
 *                  Functions for tracing Link Utilization
 * --------------------------------------------------------------------- 
*/

static void TxxPacket(Ptr<const Packet> p ){
    packetsTransmitted++;
}
static void TxxPacket_02(Ptr<const Packet> p ){
    packetsTransmitted_02++;
}

static void TraceUtilization(uint32_t pktSize){
    Time currrent_time = Now();
    float btl_thr = ((packetsTransmitted-previous_transmitted_packets)*8*pktSize)/(1000 * 1000*(currrent_time.GetSeconds() - prevTime02.GetSeconds()));
    float btl_thr_02 = ((packetsTransmitted_02-previous_transmitted_packets_02)*8*pktSize)/(1000 * 1000*(currrent_time.GetSeconds() - prevTime02.GetSeconds()));
    *utilization->GetStream() << Simulator::Now().GetSeconds() << "\t" << btl_thr/100 << "\t" << btl_thr_02/100 <<std::endl;
    previous_transmitted_packets = packetsTransmitted ;
    previous_transmitted_packets_02 = packetsTransmitted_02 ;
    prevTime02 = currrent_time;
}

static void StartTracingUtilization(){
    packetsTransmitted = 0;
    previous_transmitted_packets = 0;
    packetsTransmitted_02 = 0;
    previous_transmitted_packets_02 = 0;
    Config::ConnectWithoutContext("/NodeList/0/DeviceList/0/PhyTxEnd", MakeCallback(&TxxPacket));
    Config::ConnectWithoutContext("/NodeList/1/DeviceList/0/PhyTxEnd", MakeCallback(&TxxPacket_02));
    Config::ConnectWithoutContext("/NodeList/1/DeviceList/1/PhyTxEnd", MakeCallback(&TxxPacket_02));
}

/**
 * --------------------------------------------------------------------- 
 *                  Main Function for ns3 Simulation
 * --------------------------------------------------------------------- 
*/

int 
main(int argc, char *argv[])
{   
    ConfigStore config;
    config.ConfigureDefaults ();
    config.ConfigureAttributes ();

    std::string bottleneckBandwidth = "100Mbps";  //bandwidth of the bottleneck link
    std::string bottleneckDelay = "1ms";          //bottleneck link has negligible propagation delay
    std::string accessBandwidth = "2Mbps";
    std::string RTT_one_hop = "8ms";   		//round-trip time of each TCP flow
    std::string RTT_two_hop = "6ms";
    std::string flavour = "TcpNewReno";		//TCP variant considered
    std::string queueType = "DropTail";       //DropTail or CoDel
    std::string queueSize = "2084p";      //in packets. Size of the buffer at the bottleck link
    uint32_t pktSize = 1446;        //in Bytes. 1458 to prevent fragments
    uint32_t enbBotTrace = 0;
    float startTime = 0; 	//start time of the simulation in seconds
    float simDuration = 250;        //in seconds. endtime = startTime + simDuration
    uint32_t cleanup_time = 2;
    uint32_t nSources = 30; //no of TCP sources
    float samplingRate = 0.1f;
    droppedPackets = 0;
    droppedPackets_02 = 0;
    previously_droppedPackets = 0;
    previously_droppedPackets_02 = 0;

    std::string root_dir;
    std::string qsizeTrFileName;
    std::string droppedTrFileName;
    std::string bottleneckTxFileName;
   
    qsizeTrFileName = "qsizeTrace";
    droppedTrFileName = "droppedPacketTrace";
    bottleneckTxFileName = "bottleneckTx";
    std::string iterator = "52";
    float stopTime = startTime + simDuration;
   
    std::string tcpModel ("ns3::"+flavour);
    
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue(tcpModel));
    // Configuring the packet size
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue (pktSize));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue (1));
    Config::SetDefault("ns3::TcpSocketBase::MaxWindowSize", UintegerValue (20*1000));

    // Defining the nodes 
    NodeContainer nodes;
    nodes.Create (3 + (nSources*6));
   
    // Source nodes
    NodeContainer src1_r1 [nSources];
    NodeContainer src2_r2 [nSources];
    NodeContainer src3_r1 [nSources];
    
    // Destination nodes
    NodeContainer des2_r3 [nSources];
    NodeContainer des3_r3 [nSources];
    NodeContainer des1_r2 [nSources];
    
    NodeContainer r1r2 = NodeContainer(nodes.Get(0), nodes.Get(1));
    NodeContainer r2r3 = NodeContainer(nodes.Get(1), nodes.Get(2));

    for( uint32_t i = 0; i< nSources ; i++){
        src1_r1[i] = NodeContainer(nodes.Get(i + 3 + (nSources *0)), nodes.Get(0));
        src2_r2[i] = NodeContainer(nodes.Get(i + 3 + (nSources *1)), nodes.Get(1));
        src3_r1[i] = NodeContainer(nodes.Get(i + 3 + (nSources *2)), nodes.Get(0));
        des2_r3[i] = NodeContainer(nodes.Get(i + 3 + (nSources *3)), nodes.Get(2));
        des3_r3[i] = NodeContainer(nodes.Get(i + 3 + (nSources *4)), nodes.Get(2));
        des1_r2[i] = NodeContainer(nodes.Get(i + 3 + (nSources *5)), nodes.Get(1));
    }
   

    // Defining the links to be used between nodes
    double range = (double) (stoi(RTT_one_hop.substr(0, RTT_one_hop.length()-2)));
    double min_01 = range - (0.1 * range);
    double max_01 = range + (0.1 * range);
    
    range = (double) (stoi(RTT_two_hop.substr(0, RTT_two_hop.length()-2)));
    double min_02 = range - (0.1 * range);
    double max_02 = range + (0.1 * range);
    
    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
    x->SetAttribute ("Min", DoubleValue (min_01));
    x->SetAttribute ("Max", DoubleValue (max_01));

    Ptr<UniformRandomVariable> y = CreateObject<UniformRandomVariable> ();
    y->SetAttribute ("Min", DoubleValue (min_02));
    y->SetAttribute ("Max", DoubleValue (max_02));

    PointToPointHelper bottleneck01;
    bottleneck01.SetDeviceAttribute("DataRate", StringValue(bottleneckBandwidth));
    bottleneck01.SetChannelAttribute("Delay", StringValue(bottleneckDelay));
    bottleneck01.SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (queueSize))); // p in 1000p stands for packets
    bottleneck01.DisableFlowControl();

    PointToPointHelper bottleneck02;
    bottleneck02.SetDeviceAttribute("DataRate", StringValue(bottleneckBandwidth));
    bottleneck02.SetChannelAttribute("Delay", StringValue(bottleneckDelay));
    bottleneck02.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (queueSize)));
    bottleneck02.DisableFlowControl();

    PointToPointHelper p2p_s1[nSources];
    PointToPointHelper p2p_s2[nSources];
    PointToPointHelper p2p_s3[nSources];
    PointToPointHelper p2p_d1[nSources];
    PointToPointHelper p2p_d2[nSources];
    PointToPointHelper p2p_d3[nSources];
    
    for (uint32_t i = 0; i < nSources; i++)
    {
        double delay = (y->GetValue())/4;
        //std::cout << delay*2 << std::endl;
        std::string delay_str = std::to_string(delay) + "ms";
        p2p_s1[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_s1[i].SetChannelAttribute ("Delay", StringValue(delay_str));
        p2p_s1[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_s1[i].DisableFlowControl();
        
        p2p_d3[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_d3[i].SetChannelAttribute ("Delay", StringValue(delay_str));
        p2p_d3[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_d3[i].DisableFlowControl();

        double delay02 = (x->GetValue())/4;
        //std::cout << delay*2 << std::endl;
        std::string delay_str_02 = std::to_string(delay02) + "ms";

        p2p_s2[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_s2[i].SetChannelAttribute ("Delay", StringValue(delay_str_02));
        p2p_s2[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_s2[i].DisableFlowControl();
        
        p2p_d2[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_d2[i].SetChannelAttribute ("Delay", StringValue(delay_str_02));
        p2p_d2[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_d2[i].DisableFlowControl();

        p2p_s3[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_s3[i].SetChannelAttribute ("Delay", StringValue(delay_str));
        p2p_s3[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_s3[i].DisableFlowControl();

        p2p_d1[i].SetDeviceAttribute ("DataRate", StringValue(accessBandwidth));
        p2p_d1[i].SetChannelAttribute ("Delay", StringValue(delay_str));
        p2p_d1[i].SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize (std::to_string(0/nSources)+"p"))); // p in 1000p stands for packets
        p2p_d1[i].DisableFlowControl();

    }
    
    
    // Consists of both the link and nodes
    NetDeviceContainer r1_r2 = bottleneck01.Install(r1r2);
    NetDeviceContainer r2_r3 = bottleneck02.Install(r2r3);
    
    NetDeviceContainer s1_r1[nSources];
    NetDeviceContainer s2_r2[nSources];
    NetDeviceContainer s3_r1[nSources];
    NetDeviceContainer d1_r2[nSources];
    NetDeviceContainer d2_r3[nSources];
    NetDeviceContainer d3_r3[nSources];

    
    for( uint32_t i = 0; i<nSources; i++){
        s1_r1[i] = p2p_s1[i].Install(src1_r1[i]);
        s2_r2[i] = p2p_s2[i].Install(src2_r2[i]);
        s3_r1[i] = p2p_s3[i].Install(src3_r1[i]);
        d1_r2[i] = p2p_d1[i].Install(des1_r2[i]);
        d2_r3[i] = p2p_d2[i].Install(des2_r3[i]);
        d3_r3[i] = p2p_d3[i].Install(des3_r3[i]);
    }

     

    //  Assigning ip address to each node.
    InternetStackHelper stack;
    stack.Install(nodes);
    Ipv4AddressHelper address;

    

    Ipv4InterfaceContainer ip_s1_r1 [nSources] ;
    Ipv4InterfaceContainer ip_s2_r2 [nSources] ;
    Ipv4InterfaceContainer ip_s3_r1 [nSources] ;
    Ipv4InterfaceContainer ip_d1_r2 [nSources] ;
    Ipv4InterfaceContainer ip_d2_r3 [nSources] ;
    Ipv4InterfaceContainer ip_d3_r3 [nSources] ;

    for (uint32_t i = 0; i < nSources; i++)
    {   
        
        std::string ip = "10.1."+std::to_string(i + 3 + (nSources *0))+".0";
        address.SetBase(ip.c_str(), "255.255.255.0");
        ip_s1_r1[i] = address.Assign(s1_r1[i]);

        std::string ip2 = "10.1."+std::to_string(i + 3 + (nSources *1))+".0";
        address.SetBase(ip2.c_str(), "255.255.255.0");
        ip_s2_r2[i] = address.Assign(s2_r2[i]);

        std::string ip3 = "10.1."+std::to_string(i + 3 + (nSources *2))+".0";
        address.SetBase(ip3.c_str(), "255.255.255.0");
        ip_d2_r3[i] = address.Assign(d2_r3[i]);


        std::string ip4 = "10.1."+std::to_string(i + 3 + (nSources *3))+".0";
        address.SetBase(ip4.c_str(), "255.255.255.0");
        ip_d3_r3[i] = address.Assign(d3_r3[i]);

        std::string ip5 = "10.2."+std::to_string(i + 3 + (nSources *0))+".0";
        address.SetBase(ip5.c_str(), "255.255.255.0");
        ip_s3_r1[i] = address.Assign(s3_r1[i]);

        std::string ip6 = "10.2."+std::to_string(i + 3 + (nSources *1))+".0";
        address.SetBase(ip6.c_str(), "255.255.255.0");
        ip_d1_r2[i] = address.Assign(d1_r2[i]);
    }
    
    address.SetBase("10.1.1.0","255.255.255.0");
    address.Assign(r1_r2);
    address.Assign(r2_r3);

    
    // Attaching sink to nodes
    uint16_t sinkPort = 8080;
    uint16_t sinkPort02 = 50001;
    uint16_t sinkPort03 = 49001;

    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress( Ipv4Address::GetAny(), sinkPort));
    PacketSinkHelper packetSinkHelper_02 ("ns3::TcpSocketFactory", InetSocketAddress( Ipv4Address::GetAny(), sinkPort02));
    PacketSinkHelper packetSinkHelper_03 ("ns3::TcpSocketFactory", InetSocketAddress( Ipv4Address::GetAny(), sinkPort03));

    Address sinkAddress[nSources];
    Address sinkAddress_02[nSources];
    Address sinkAddress_03[nSources];

    ApplicationContainer sinkApp[nSources];
    ApplicationContainer sinkApp_02[nSources];
    ApplicationContainer sinkApp_03[nSources];

    for (uint32_t i = 0; i < nSources; i++)
    {
        sinkAddress[i] = *(new Address(InetSocketAddress(ip_d3_r3[i].GetAddress(0), sinkPort)));
        sinkAddress_02[i] = *(new Address(InetSocketAddress(ip_d2_r3[i].GetAddress(0), sinkPort02)));
        sinkAddress_03[i] = *(new Address(InetSocketAddress(ip_d1_r2[i].GetAddress(0), sinkPort03)));
   
        sinkApp[i] = packetSinkHelper.Install(nodes.Get(i + 3 + (nSources * 4)));
        sinkApp[i].Start(Seconds(startTime));
        sinkApp[i].Stop(Seconds(stopTime));

        sinkApp_02[i] = packetSinkHelper_02.Install(nodes.Get(3 + (nSources * 3) + i));
        sinkApp_02[i].Start(Seconds(startTime));
        sinkApp_02[i].Stop(Seconds(stopTime));

        sinkApp_03[i] = packetSinkHelper_03.Install(nodes.Get(3 + (nSources * 5) + i));
        sinkApp_03[i].Start(Seconds(startTime));
        sinkApp_03[i].Stop(Seconds(stopTime));
    }
    
    ApplicationContainer sourceApps[nSources];
    ApplicationContainer sourceApps_02[nSources];
    ApplicationContainer sourceApps_03[nSources];

    double mean = 0.1;   // more like a ~ 0.06
    double bound = 1;
    Ptr<ExponentialRandomVariable> expRandomVariable = CreateObject<ExponentialRandomVariable> ();
    expRandomVariable->SetAttribute ("Mean", DoubleValue (mean));
    expRandomVariable->SetAttribute ("Bound", DoubleValue (bound));

    double stime = startTime;
    // Configuring the application at each source node.
    for (uint32_t i = 0; i < nSources; i++)
    {
        
        BulkSendHelper source_01("ns3::TcpSocketFactory",InetSocketAddress (ip_d3_r3[i].GetAddress (0), sinkPort));
        BulkSendHelper source02("ns3::TcpSocketFactory",InetSocketAddress (ip_d2_r3[i].GetAddress (0), sinkPort02));
        BulkSendHelper source03("ns3::TcpSocketFactory",InetSocketAddress (ip_d1_r2[i].GetAddress (0), sinkPort03));

        //Set the amount of data to send in bytes.  Zero is unlimited.
        source_01.SetAttribute ("MaxBytes", UintegerValue (0));
        source02.SetAttribute ("MaxBytes", UintegerValue(0));
        source03.SetAttribute ("MaxBytes", UintegerValue(0));

        sourceApps[i] = source_01.Install (nodes.Get (i + 3 + (nSources *0)));
        sourceApps[i].Start (Seconds (stime));
        sourceApps[i].Stop (Seconds (stopTime));

        sourceApps_02[i] = source02.Install (nodes.Get (3 + i + (nSources * 1)));
        sourceApps_02[i].Start (Seconds (stime));
        sourceApps_02[i].Stop (Seconds (stopTime));

        sourceApps_03[i] = source03.Install (nodes.Get (3 + i + (nSources * 2)));
        sourceApps_03[i].Start (Seconds (stime));
        sourceApps_03[i].Stop (Seconds (stopTime));

        double gap = expRandomVariable->GetValue();

        stime += gap;
        
        // std::cout << gap << std::endl;

    }
    
    
    // Configuring file stream to write the Qsize
    AsciiTraceHelper ascii_qsize;
    qSize_stream = ascii_qsize.CreateFileStream(qsizeTrFileName+ iterator + ".txt");



    AsciiTraceHelper ascii_dropped;
    dropped_stream = ascii_dropped.CreateFileStream (droppedTrFileName + iterator + ".txt");

    AsciiTraceHelper ascii_qsize_tx;
    bottleneckTransmittedStream = ascii_qsize_tx.CreateFileStream(bottleneckTxFileName+ iterator +".txt");

    AsciiTraceHelper ascii_tx;
    std::string name = "utilization" + iterator + ".txt";
    utilization = ascii_tx.CreateFileStream(name);
    // start tracing the congestion window size and qSize
    Simulator::Schedule( Seconds(stime), &StartTracingQueueSize);
    Simulator::Schedule( Seconds(stime), &StartTracingSink);
    Simulator::Schedule( Seconds(stime), &StartTracingUtilization);

    // start tracing Queue Size and Dropped Files
    Simulator::Schedule( Seconds(stime), &TraceDroppedPacket, droppedTrFileName);
    // writing the congestion windows size, queueSize, packetTx to files periodically ( 1 sec. )
    for (uint32_t time = stime; time < stopTime; time++)
    {   
        /*Simulator::Schedule( Seconds(time), &writeCwndToFile, nSources);*/
        Simulator::Schedule( Seconds(time), &TraceQueueSize);
        Simulator::Schedule( Seconds(time), &TraceDroppedPkts);
        Simulator::Schedule( Seconds(time), &TraceUtilization, pktSize);
    }
   for (float ttime = stime; ttime < stopTime; )
    {   
        Simulator::Schedule( Seconds(ttime), &TraceBottleneckTx, pktSize);
        ttime = ttime + samplingRate;
    }

    if ( enbBotTrace == 1 ){
        AsciiTraceHelper bottleneck_ascii;
        bottleneck01.EnableAsciiAll(bottleneck_ascii.CreateFileStream ("bottleneck.tr"));
        bottleneck01.EnablePcapAll("bottleneck", false);
        bottleneck02.EnableAsciiAll(bottleneck_ascii.CreateFileStream ("bottleneck02.tr"));
        bottleneck02.EnablePcapAll("bot", false);
    }


    Simulator::Stop (Seconds (stopTime+cleanup_time));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


    Simulator::Run ();

    Simulator::Destroy ();

    return 0;

}
