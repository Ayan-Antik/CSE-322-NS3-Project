/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FifthScriptExample");

// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off 
// application is not created until Application Start time, so we wouldn't be 
// able to hook the socket (now) at configuration time.  Second, even if we 
// could arrange a call after start time, the socket is not public so we 
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass 
// this socket into the constructor of our simple application which we then 
// install in the source node.
// ===========================================================================
//
class MyApp : public Application 
{
public:

  MyApp ();
  virtual ~MyApp();

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

MyApp::~MyApp()
{
  m_socket = 0;
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
  m_socket->Bind ();
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

double simulationTime = 10;
void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);
  // Time curr = Simulator :: Now();
  // if(curr.GetSeconds() < simulationTime){
  //     ScheduleTx();
  // }  
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

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << oldCwnd << " " << newCwnd << std::endl;

}

static void
RxDrop (Ptr<const Packet> p)
{
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
}

static void
SSThresh (Ptr<OutputStreamWrapper> stream2, uint32_t oldValue, uint32_t newValue)
{
  NS_LOG_UNCOND ("SSThresh updated at " << Simulator::Now ().GetSeconds () << " Old val: " << oldValue << " New val: " << newValue);
  // *stream2->GetStream () << Simulator::Now ().GetSeconds () << " " << oldValue << " " << newValue << std::endl;

}

int 
main (int argc, char *argv[])
{ 

  uint nPacketsPerSecond = 500;
  uint payloadSize = 1024;

  CommandLine cmd (__FILE__);
  cmd.AddValue("nPacketsPerSecond", "Number of packets per second", nPacketsPerSecond);
  cmd.Parse (argc, argv);

  std::string dataRate = std::to_string(nPacketsPerSecond*payloadSize*8/1024) + "Kbps";/* Application layer datarate. */

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
    Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName ("ns3::TcpClassicRecovery")));
    // LogComponentEnable("TcpFrGfr", LOG_LEVEL_ALL);
    // LogComponentEnable("TcpNewReno", LOG_LEVEL_ALL);
    // LogComponentEnable("TcpCongestionOps", LOG_LEVEL_FUNCTION);
    // LogComponentEnable("TcpRecoveryOps", LOG_LEVEL_FUNCTION);
    // LogComponentEnable("TcpSocketBase", LOG_INFO);
    // LogComponentEnable("TcpVegas", LOG_FUNCTION);

  
  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.0001));
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (interfaces.GetAddress (1), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (simulationTime));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("TaskB/Congstats.txt");
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("TaskB/SSthreshstats.txt");



  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (nodes.Get (0), TcpSocketFactory::GetTypeId ());
  ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
  ns3TcpSocket->TraceConnectWithoutContext ("SlowStartThreshold", MakeBoundCallback (&SSThresh, stream2));

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, payloadSize, 2000, DataRate (dataRate));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.));
  app->SetStopTime (Seconds (simulationTime-1.0));

  devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));
  // devices.Get (0)->TraceConnectWithoutContext ("UpdateSsThresh", MakeCallback (&SSThresh));

    // std::ofstream outDeliv;
    // outDeliv.open("TaskB/statsDrop.txt", std::ios_base::app);

    FlowMonitorHelper fmhelp;
    Ptr<FlowMonitor> monitor = fmhelp.InstallAll();
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
    uint32_t total_rx_Bytes = 0;
    double delay = 0;
    uint32_t j = 0;
    uint32_t total = 0;
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { total++;}
    for (auto iter = stats.begin(); iter != stats.end(); iter++)
    { 
      
      // Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
      CurrThroughput = (iter->second.rxBytes*8.0/(iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds())/1024);
      total_rx_Bytes += iter->second.rxBytes;
      if(j<=total/2){
        AvgSendThroughput += CurrThroughput;
      }
      

      PacketsSent += iter->second.txPackets;
      PacketsReceived += iter->second.rxPackets ;
      PacketsLost +=  iter->second.lostPackets;
      AvgThroughput += CurrThroughput;
      delay += iter->second.delaySum.GetSeconds();
      j++;
    } 
      // AvgThroughput/=j;
      delay/=PacketsReceived;
      // AvgSendThroughput = AvgSendThroughput/(j/2);

      NS_LOG_UNCOND("\n\nTOTAL SIMULATION: ");
      NS_LOG_UNCOND("Sent Packets: " << PacketsSent);
      NS_LOG_UNCOND("Received Packets: " << PacketsReceived);
      NS_LOG_UNCOND("Lost Packets: " << PacketsLost );
      NS_LOG_UNCOND("Packet Delivery Ratio: " << PacketsReceived*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Packet Drop Ratio: " << PacketsLost*100.0/PacketsSent << "%");
      NS_LOG_UNCOND("Delay: " << delay << " seconds");
      NS_LOG_UNCOND("Average Throughput: " <<AvgThroughput<< "Kbps");
      NS_LOG_UNCOND("Average Throughput without ACK: " <<AvgSendThroughput<< "Kbps");

      // outDeliv << nPacketsPerSecond << " " << AvgThroughput << std::endl;

  Simulator::Destroy ();

  return 0;
}

