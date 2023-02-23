#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/yans-error-rate-model.h"


// Default Network Topology
//
//   Wifi 10.1.2.0
//                     AP
//  *    *    *    *   *
//  |    |    |    |   |    10.1.1.0
// n6   n7   n8   n9   n0 -------------- n1   n2   n3   n4   n5 
//                        point-to-point  |    |    |    |    |
//                                        *    *    *    *    * 
//                                       AP
//                                           Wifi 10.1.3.0
#define uint_32 uint
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TaskA");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}
double simulationTime = 20;                        /* Simulation time in seconds. */

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);
  Time curr = Simulator :: Now();
  if(curr.GetSeconds() < simulationTime){
      ScheduleTx();
  }  
//   if (++m_packetsSent < m_nPackets)
//     {
//       ScheduleTx ();
//     }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

// static void
// CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
// {
//   NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
//   *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
// }

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{   
  std::cout << "Packet Dropped\n";
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  file->Write (Simulator::Now (), p);
}


Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}



int main(int argc, char *argv[]){
    uint nWifi = 0;
    uint nFlow = 20;
    uint nNodes = 18;

    uint nPacketsPerSecond = 500;
    uint payloadSize = 1024;                       /* Transport layer payload size in bytes. */
    // std::string tcpVariant = "ns3::TcpNewReno";             /* TCP variant type. */
    // std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
    // bool pcapTracing = false;     
    uint nPackets = 2000;
    double txArea = 1.0;
    double range = 50.0; 
    std::string filename = "hi";


    // Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpVariant));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    CommandLine cmd(__FILE__);
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("nFlow", "Number of Flow", nFlow);
    cmd.AddValue("nPacketsPerSecond", "Number of packets per second", nPacketsPerSecond);
    cmd.AddValue("range", "Coverage Area", range);
    cmd.AddValue("filename", "Filename", filename);
    cmd.Parse(argc, argv);



    std::string dataRate = std::to_string(nPacketsPerSecond*payloadSize*8/1024) + "Kbps";/* Application layer datarate. */
    Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (range*txArea));
 

    nWifi = nNodes/2 - 1; //*Sta Node in each side of AP
    uint halfFlow = nFlow/2;

    //try and create nodes
    NodeContainer p2pNodes;
    p2pNodes.Create(2); //AP nodes

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install (p2pNodes);
    


    NodeContainer wifiStaNodesL, wifiStaNodesR;
    NodeContainer wifiApNodeL = p2pNodes.Get (0);
    wifiStaNodesL.Create (nWifi); 

    NodeContainer wifiApNodeR = p2pNodes.Get (1);
    wifiStaNodesR.Create (nWifi);


    YansWifiChannelHelper LwifiChannel = YansWifiChannelHelper::Default ();
    YansWifiChannelHelper RwifiChannel = YansWifiChannelHelper::Default ();

    LwifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel");
    RwifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel");
    YansWifiPhyHelper LwifiPhy;
    YansWifiPhyHelper RwifiPhy;


    LwifiPhy.SetChannel (LwifiChannel.Create ());
    RwifiPhy.SetChannel (RwifiChannel.Create ());
    

    WifiMacHelper LwifiMac;
    WifiMacHelper RwifiMac;

    WifiHelper LwifiHelper;
    WifiHelper RwifiHelper;
    
    
    LwifiHelper.SetRemoteStationManager ("ns3::AarfWifiManager");
    RwifiHelper.SetRemoteStationManager ("ns3::AarfWifiManager");

    Ssid ssid= Ssid ("ns-3-ssid");
    
    LwifiMac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

    RwifiMac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
    
    NetDeviceContainer staDevicesL;
    NetDeviceContainer staDevicesR;
    staDevicesL = LwifiHelper.Install(LwifiPhy, LwifiMac, wifiStaNodesL);
    staDevicesR = RwifiHelper.Install(RwifiPhy, RwifiMac, wifiStaNodesR);


    LwifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

    RwifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
    
    NetDeviceContainer apDevicesL;
    NetDeviceContainer apDevicesR;

    
    apDevicesL = LwifiHelper.Install(LwifiPhy, LwifiMac, wifiApNodeL);
    apDevicesR = RwifiHelper.Install(RwifiPhy, RwifiMac, wifiApNodeR);

    std::cout << "NodeCount: " << ns3::NodeList::GetNNodes() << std::endl;
    
    


    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (0.5),
                                    "DeltaY", DoubleValue (1.0),
                                    "GridWidth", UintegerValue (3),
                                    "LayoutType", StringValue ("RowFirst"));

    //  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    //                              "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodesL);
    mobility.Install (wifiApNodeL);
    
    mobility.Install (wifiStaNodesR);
    mobility.Install (wifiApNodeR);
    
    //*IP Layer
    
    InternetStackHelper stack;   
    stack.Install(wifiApNodeR);
    stack.Install(wifiApNodeL);
    stack.Install(wifiStaNodesL);
    stack.Install(wifiStaNodesR);

    //*set address
    Ipv4AddressHelper address;

    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign (p2pDevices); //Address of Bottleneck (p2p devices) //*L: 10.1.1.1, R: 10.1.1.2

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfacesL = address.Assign (apDevicesL);     //* Left AP Device address: 10.1.2.1 
    Ipv4InterfaceContainer staInterfacesL = address.Assign (staDevicesL);   //*10.1.2.2 - 10.1.2.(nWifi+1)

    address.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfacesR = address.Assign (apDevicesR);     //* Right AP Device address: 10.1.3.1
    Ipv4InterfaceContainer staInterfacesR = address.Assign (staDevicesR);   //*10.1.3.2 - 10.1.3.(nWifi+1)



    // for(NodeContainer::Iterator itr = wifiStaNodesR.Begin(); itr != wifiStaNodesR.End(); itr++){
    //   Ptr<Ipv4StaticRouting> sr = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting> ((*itr)->GetObject<Ipv4>()->GetRoutingProtocol());
    //   sr->SetDefaultRoute(apInterfacesR.GetAddress(0), 1);
    // }

    // for(NodeContainer::Iterator itr = wifiStaNodesL.Begin(); itr != wifiStaNodesL.End(); itr++){
    //   Ptr<Ipv4StaticRouting> sr = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting> ((*itr)->GetObject<Ipv4>()->GetRoutingProtocol());
    //   sr->SetDefaultRoute(apInterfacesL.GetAddress(0), 1);
    // }

    uint idx;
    if(halfFlow < nWifi)idx=halfFlow;
    else{
        idx = nWifi;
    }
    for(uint i = 0; i< idx; i++){

        uint16_t port = 8000+i;
        PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApp = sinkHelper.Install (wifiStaNodesR.Get(i));
        
        sinkApp.Start (Seconds (0.));
        sinkApp.Stop (Seconds(simulationTime));

        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (wifiStaNodesL.Get (i), TcpSocketFactory::GetTypeId ());
        Ptr<MyApp> app = CreateObject<MyApp> ();
        app->Setup (ns3TcpSocket, InetSocketAddress(staInterfacesR.GetAddress(i), port), payloadSize, nPackets, DataRate (dataRate));
        wifiApNodeL.Get (0)->AddApplication (app);
        app->SetStartTime (Seconds (1.));
        app->SetStopTime (Seconds (simulationTime-1.0));
        

    }

    uint flowLeft = halfFlow - nWifi; 
    bool flowDone = false;
    if(idx == nWifi){

        //*need more flows

        //* loop each node. (L1 -> R1 Flow has been established. Now create flows from L1 -> Rx, L2 -> Rx until flow is not left)
        for(uint i = 0; i < nWifi; i++){

            for(uint j = 0; j<nWifi; j++){
                if(j==i)continue;
                //* ADD FLOW

                uint16_t port = 8100+i+j;
                PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
                ApplicationContainer sinkApp = sinkHelper.Install (wifiStaNodesR.Get(j));
                
                sinkApp.Start (Seconds (0.));
                sinkApp.Stop (Seconds(simulationTime));

                Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (wifiStaNodesL.Get (i), TcpSocketFactory::GetTypeId ());
                Ptr<MyApp> app = CreateObject<MyApp> ();
                app->Setup (ns3TcpSocket, InetSocketAddress(staInterfacesR.GetAddress(j), port), payloadSize, nPackets, DataRate (dataRate));
                wifiApNodeL.Get (0)->AddApplication (app);
                app->SetStartTime (Seconds (1.));
                app->SetStopTime (Seconds (simulationTime-1.0));
                
                //*update flow
                flowLeft--;
                if(flowLeft == 0){
                    flowDone = true;
                    break;
                }
            }
            if(flowDone == true)break;
            
        }
            
        

    }



    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    std::ofstream out;

    //Add Flow Monitor
    FlowMonitorHelper fmhelp;
    Ptr<FlowMonitor> monitor = fmhelp.InstallAll();


    PcapHelper pcapHelper;
    Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("taskA.pcap", std::ios::out, PcapHelper::DLT_PPP);
    p2pDevices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));
    
    Simulator::Stop (Seconds (simulationTime+2.0));
    Simulator::Run ();
    


    Ptr<Ipv4FlowClassifier> classifier = DynamicCast <Ipv4FlowClassifier> (fmhelp.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    uint32_t PacketsSent = 0;
    uint32_t PacketsReceived = 0;
    uint32_t PacketsDrop = 0;
    double AvgThroughput = 0;
    double AvgSendThroughput = 0;
    double CurrThroughput = 0;
    Time Avgdelay;
    
    uint32_t j = 0;
    uint32_t total = 0;
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { total++;}
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { 
      
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
      CurrThroughput = (iter->second.rxBytes*8.0/1024);
      
      if(j<=total/2){

        AvgSendThroughput += CurrThroughput;

      }
      
      NS_LOG_UNCOND("\nFlow ID: " << iter->first);
      NS_LOG_UNCOND("Src Addr: " << t.sourceAddress << "\t Destination Addr: " << t.destinationAddress);
      NS_LOG_UNCOND("Sent Packets: " << iter->second.txPackets );
      NS_LOG_UNCOND("Received Packets: " << iter->second.rxPackets );
      NS_LOG_UNCOND("Lost Packets: " << iter->second.lostPackets );
    //   NS_LOG_UNCOND("Dropped Packets: " << iter->second.txPackets - iter->second.rxPackets);
      NS_LOG_UNCOND("Packet Delivery Ratio: " << iter->second.rxPackets*100.0/iter->second.txPackets << "%");
      NS_LOG_UNCOND("Packet Drop Ratio: " << (iter->second.lostPackets)*100.0/iter->second.txPackets << "%");
      NS_LOG_UNCOND("Delay: " << iter->second.delaySum);
      NS_LOG_UNCOND("Flow Throughput: " << CurrThroughput << "Kbps");


      PacketsSent += iter->second.txPackets;
      PacketsReceived += iter->second.rxPackets ;
      PacketsDrop +=  iter->second.lostPackets;
      AvgThroughput += CurrThroughput;
      Avgdelay += iter->second.delaySum;
      j++;
    } 
      AvgThroughput/=(simulationTime+2);
      Avgdelay=Avgdelay/PacketsReceived;
    //   Avgdelay /= 1e+9;
      AvgSendThroughput = AvgSendThroughput/(simulationTime+2/2);

      NS_LOG_UNCOND("\n\nTOTAL SIMULATION: ");
      NS_LOG_UNCOND("Sent Packets: " << PacketsSent);
      NS_LOG_UNCOND("Received Packets: " << PacketsReceived);
      NS_LOG_UNCOND("Dropped Packets: " << PacketsDrop );
      NS_LOG_UNCOND("Packet Delivery Ratio: " << PacketsReceived*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Packet Drop Ratio: " << PacketsDrop*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Average Throughput: " <<AvgThroughput<< "Kbps");
      NS_LOG_UNCOND("Average delay: " <<Avgdelay);
      NS_LOG_UNCOND("Average Throughput without ACK: " <<AvgSendThroughput<< "Kbps");

    out.open(filename, std::ios_base::app);
    out << nNodes << " " << AvgThroughput << " " << Avgdelay.GetDouble() << " " << PacketsReceived*100.0/PacketsSent 
    << " " << PacketsDrop*100.0/PacketsSent << std::endl; 

    Simulator::Destroy ();
    



    return 0;


}

