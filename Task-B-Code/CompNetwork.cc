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

NS_LOG_COMPONENT_DEFINE ("CustomNetworkExample");

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

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
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

// static void
// RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
// {   
//   std::cout << "Packet Dropped\n";
//   NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
//   file->Write (Simulator::Now (), p);
// }




int main(int argc, char *argv[]){
    uint nWifi = 4;
    uint nPacketsPerSecond = 500;
    std::string tcpvariant = "ns3::TcpNewReno";             /* TCP variant type. */
    // uint nFlow = 2;
    CommandLine cmd(__FILE__);
    cmd.AddValue("nPacketsPerSecond", "Number of packets per second", nPacketsPerSecond);
    cmd.AddValue("tcpvariant", "TCP Variant", tcpvariant);
    cmd.Parse(argc, argv);

    uint payloadSize = 1024;                       /* Transport layer payload size in bytes. */
    // std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
    double simulationTime = 25;                        /* Simulation time in seconds. */
    // bool pcapTracing = false;     
    uint nPackets = 20000;

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpvariant));
    Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName ("ns3::TcpClassicRecovery")));
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



    
    std::string dataRate = std::to_string(nPacketsPerSecond*payloadSize*8/1024) + "Kbps";/* Application layer datarate. */

    //try and create nodes
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

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


    // std::cout << "Hello\n";
    YansWifiChannelHelper LwifiChannel = YansWifiChannelHelper::Default ();
    YansWifiChannelHelper RwifiChannel = YansWifiChannelHelper::Default ();
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

    Ssid ssid = Ssid ("ns-3-ssid");
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
 
    //set mobility of sta devices
    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (.5),
                                    "DeltaY", DoubleValue (1.0),
                                    "GridWidth", UintegerValue (3),
                                    "LayoutType", StringValue ("RowFirst"));

    //  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
    //                              "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodesL);
    mobility.Install (wifiStaNodesR);

    mobility.Install (wifiApNodeL);    
    mobility.Install (wifiApNodeR);    

    //IP Layer

    
    InternetStackHelper stack;   
    stack.Install(wifiApNodeR);
    stack.Install(wifiApNodeL);
    stack.Install(wifiStaNodesL);

    stack.Install(wifiStaNodesR);

    //set address
    Ipv4AddressHelper address;

    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign (p2pDevices); //Address between Routers

    address.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfacesL = address.Assign (apDevicesL);
    Ipv4InterfaceContainer staInterfacesL = address.Assign (staDevicesL);

    address.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterfacesR = address.Assign (apDevicesR);
    Ipv4InterfaceContainer staInterfacesR = address.Assign (staDevicesR);

    
    for(uint i = 0; i< nWifi; i++){

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




    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


    //Add Flow Monitor
    FlowMonitorHelper fmhelp;
    Ptr<FlowMonitor> monitor = fmhelp.InstallAll();

    std::ofstream outTpt;
    outTpt.open("TaskB/statsTpt2.txt", std::ios_base::app);

     std::ofstream outDel;
    outDel.open("TaskB/statsDelay2.txt", std::ios_base::app);

     std::ofstream outDeliv;
    outDeliv.open("TaskB/statsDrop.txt", std::ios_base::app);


    // p2pDevices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, file));
    
    Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast <Ipv4FlowClassifier> (fmhelp.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    uint32_t PacketsSent = 0;
    uint32_t PacketsReceived = 0;
    uint32_t PacketsLost = 0;
    uint32_t AvgThroughput = 0;
    uint32_t AvgSendThroughput = 0;
    uint32_t CurrThroughput = 0;
    double delay = 0;
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
      
      NS_LOG_UNCOND("\n------------Flow ID: " << iter->first);
      NS_LOG_UNCOND("Src Addr: " << t.sourceAddress << "\t Destination Addr: " << t.destinationAddress);
      NS_LOG_UNCOND("Sent Packets: " << iter->second.txPackets );
      NS_LOG_UNCOND("Received Packets: " << iter->second.rxPackets );
      NS_LOG_UNCOND("Lost Packets: " << iter->second.lostPackets);
      NS_LOG_UNCOND("Packet Delivery Ratio: " << iter->second.rxPackets*100.0/iter->second.txPackets << "%");
      NS_LOG_UNCOND("Packet Drop Ratio: " << (iter->second.lostPackets)*100.0/iter->second.txPackets << "%");
      NS_LOG_UNCOND("Delay: " << iter->second.delaySum);
      NS_LOG_UNCOND("Average Throughput: " << CurrThroughput << "Kbps");


      PacketsSent += iter->second.txPackets;
      PacketsReceived += iter->second.rxPackets ;
      PacketsLost +=  iter->second.lostPackets;
      delay += iter->second.delaySum.GetSeconds();
      AvgThroughput += CurrThroughput;
      j++;
    } 
      AvgThroughput /= simulationTime;
      AvgSendThroughput /= simulationTime;
      delay /= PacketsReceived;
      NS_LOG_UNCOND("\n\nTOTAL SIMULATION: ");
      NS_LOG_UNCOND("Sent Packets: " << PacketsSent);
      NS_LOG_UNCOND("Received Packets: " << PacketsReceived);
      NS_LOG_UNCOND("Lost Packets: " << PacketsLost );
      NS_LOG_UNCOND("Delay: " << delay << "s");
      NS_LOG_UNCOND("Packet Delivery Ratio: " << PacketsReceived*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Packet Drop Ratio: " << PacketsLost*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Average Throughput: " <<AvgThroughput<< "Kbps");
      NS_LOG_UNCOND("Average Throughput without ACK: " <<AvgSendThroughput<< "Kbps");

      // outTpt << nPacketsPerSecond << " " << AvgThroughput << std::endl;
      // outDel << nPacketsPerSecond << " " << delay << std::endl;
      outDeliv << nPacketsPerSecond << " " << PacketsLost*100.0/PacketsSent << std::endl;
    Simulator::Destroy ();
    

    return 0;


}

