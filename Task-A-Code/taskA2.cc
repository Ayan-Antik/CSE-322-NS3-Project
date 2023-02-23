
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv6-flow-classifier.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TaskA2");

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
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
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
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
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
  // Time curr = Simulator :: Now();
  // if(curr.GetSeconds() < simulationTime){
      ScheduleTx();
  // }  
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


int main(int argc, char *argv[]){



    uint32_t tcp_adu_size = 32;
    // uint64_t data_mbytes = 0;
    bool sack = true;
    std::string recovery = "ns3::TcpClassicRecovery";
    Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName (recovery)));

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));

    Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));
    Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
    Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));


    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));
    Packet::EnablePrinting ();

    std::string filename = "f";

    uint nNodes = 20;
    uint nFlow = 10;
    uint nPacketsPerSecond = 500;
    uint speed = 5;
    // uint payloadSize = 1024;
    // uint nPackets = 2000;
    // std::string dataRate = std::to_string(nPacketsPerSecond/10) + "Kbps";/* Application layer datarate. */


    CommandLine cmd(__FILE__);
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("nFlow", "Number of Flow", nFlow);
    cmd.AddValue("nPacketsPerSecond", "Number of packets per second", nPacketsPerSecond);
    cmd.AddValue("speed", "Speed of nodes", speed);
    cmd.AddValue("filename", "Filename", filename);

    cmd.Parse(argc, argv);
    uint halfFlow = nFlow/2;
    
    
    std::string dataRate = std::to_string(nPacketsPerSecond*tcp_adu_size*8/1024) + "Kbps";/* Application layer datarate. */

    NodeContainer WsNodes;
    WsNodes.Create(nNodes);

    MobilityHelper mobility;
    uint width = (sqrt(nNodes));
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (.5),
                                 "DeltaY", DoubleValue (1.0),
                                 "GridWidth", UintegerValue (width),
                                 "LayoutType", StringValue ("RowFirst"));

    std::stringstream speedstr;
    speedstr << "ns3::UniformRandomVariable[Min=0.0|Max=" << speed << "]";
    mobility.SetMobilityModel("ns3::GaussMarkovMobilityModel",
                                "MeanVelocity", StringValue(speedstr.str()),
                                "Bounds", BoxValue(Box(0, 25, 0, 25, 0, 25)),
                                "TimeStep", TimeValue(Seconds(5)));
    mobility.Install(WsNodes);

    LrWpanHelper lrwpanhelper;
    NetDeviceContainer wsDevices = lrwpanhelper.Install(WsNodes);

    lrwpanhelper.AssociateToPan(wsDevices, 0);

    InternetStackHelper inetV6;
    inetV6.Install(WsNodes);

    SixLowPanHelper sixLowPanHelper;
    NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install(wsDevices);


    //*ip address
    Ipv6AddressHelper ipv6addr;

    ipv6addr.SetBase(Ipv6Address("2001:f00d::"), Ipv6Prefix(64));
    Ipv6InterfaceContainer wsInterfaces;
    wsInterfaces = ipv6addr.Assign(sixLowPanDevices);
    wsInterfaces.SetForwarding (0, true);
    wsInterfaces.SetDefaultRouteInAllNodes (0);

    for (uint32_t i = 0; i < sixLowPanDevices.GetN (); i++) {
      Ptr<NetDevice> dev = sixLowPanDevices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }
    
    uint16_t i = 1;
     for( uint32_t f=0; f<halfFlow; f++ ) {
       
        uint16_t port = 8080+f;
      
        PacketSinkHelper sinkApp ("ns3::TcpSocketFactory",
        Inet6SocketAddress (Ipv6Address::GetAny (), port));
        sinkApp.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
        ApplicationContainer sinkApps = sinkApp.Install (WsNodes.Get(i-1));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (simulationTime));


        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (WsNodes.Get (i), TcpSocketFactory::GetTypeId ());
        Ptr<MyApp> app = CreateObject<MyApp> ();
        app->Setup (ns3TcpSocket, Inet6SocketAddress(wsInterfaces.GetAddress(i-1, 1), port), tcp_adu_size, DataRate (dataRate));
        
        WsNodes.Get (i)->AddApplication (app);
        app->SetStartTime (Seconds (1.));
        app->SetStopTime (Seconds (simulationTime));
        i++;
        if(i%(nNodes) == 0)i=1;
        
  } 



    // NS_LOG_UNCOND("NodeCount: " << ns3::NodeList::GetNNodes() << std::endl);
    std::ofstream out;

    FlowMonitorHelper fmhelp;
    Ptr<FlowMonitor> monitor = fmhelp.InstallAll();


     Simulator::Stop (Seconds (simulationTime));
    Simulator::Run ();
    


    Ptr<Ipv6FlowClassifier> classifier = DynamicCast <Ipv6FlowClassifier> (fmhelp.GetClassifier6());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    uint32_t PacketsSent = 0;
    uint32_t PacketsReceived = 0;
    uint32_t PacketsDrop = 0;
    double AvgThroughput = 0;
    double AvgSendThroughput = 0;
    double CurrThroughput = 0;
    double Avgdelay;
    
    uint32_t j = 0;
    uint32_t total = 0;
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { total++;}
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { 
      
      Ipv6FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
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
      NS_LOG_UNCOND("Average Throughput: " << CurrThroughput << "Kbps");


      PacketsSent += iter->second.txPackets;
      PacketsReceived += iter->second.rxPackets ;
      PacketsDrop +=  iter->second.lostPackets;
      AvgThroughput += CurrThroughput;
      Avgdelay += iter->second.delaySum.GetDouble();
      j++;
    } 
    AvgThroughput/=(simulationTime);
    Avgdelay=Avgdelay/PacketsReceived;
    AvgSendThroughput = AvgSendThroughput/(simulationTime/2);

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
      out << speed << " " << AvgThroughput << " " << Avgdelay << " " << PacketsReceived*100.0/PacketsSent 
      << " " << PacketsDrop*100.0/PacketsSent << std::endl; 

    Simulator::Destroy ();
 





}