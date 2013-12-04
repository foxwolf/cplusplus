#include <memory>
#include <string>
#include <sys/resource.h>

#include <ace/Signal.h>
#include <ildaemon/ActiveScheduler.h>
#include <ildaemon/BoundMethodRequestImpl.tcc>
#include <base/BaseCommon.h>
#include <boost/bind.hpp>
#include <vif/IfSetupCommon.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "HttpApplication.h"
#include "HttpServerRawStats.h"
#include "HttpClientRawStats.h"
#include "HttpVideoServerRawStats.h"
#include "HttpVideoClientRawStats.h"


USING_HTTP_NS;

///////////////////////////////////////////////////////////////////////////////

class TestHttpAbrApplication : public CPPUNIT_NS::TestCase
{
    CPPUNIT_TEST_SUITE(TestHttpAbrApplication);
    CPPUNIT_TEST(testAppleVodAdaptiveBitrate);
//    CPPUNIT_TEST(testAppleLiveAdaptiveBitrate);
//    CPPUNIT_TEST(testAppleVodProgressiveSimple);
//    CPPUNIT_TEST(testAppleVodProgressiveSegmented);
//    CPPUNIT_TEST(testLiveProgressive404);            // Needs to be enabled once client is ready
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void) { }
    
protected:
    static const size_t NUM_IO_THREADS = 4;
    static const rlim_t MAXIMUM_OPEN_FILE_HARD_LIMIT;
    static const unsigned short SERVER_PORT_NUMBER_BASE = 10009;
    unsigned short mServerPortNumber;
    
    void testAppleVodAdaptiveBitrate(void);
    void testAppleLiveAdaptiveBitrate(void);
    void testAppleVodProgressiveSimple(void);
    void testAppleVodProgressiveSegmented(void);
    void testLiveProgressive404(void);
};

///////////////////////////////////////////////////////////////////////////////

const rlim_t TestHttpAbrApplication::MAXIMUM_OPEN_FILE_HARD_LIMIT = (32 * 1024);

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::setUp(void)
{
    ACE_OS::signal(SIGPIPE, SIG_IGN);

    struct rlimit fileLimits;
    getrlimit(RLIMIT_NOFILE, &fileLimits);
    if (fileLimits.rlim_cur < MAXIMUM_OPEN_FILE_HARD_LIMIT || fileLimits.rlim_max < MAXIMUM_OPEN_FILE_HARD_LIMIT)
    {
        fileLimits.rlim_cur = std::max(fileLimits.rlim_cur, MAXIMUM_OPEN_FILE_HARD_LIMIT);
        fileLimits.rlim_max = std::max(fileLimits.rlim_max, MAXIMUM_OPEN_FILE_HARD_LIMIT);
        setrlimit(RLIMIT_NOFILE, &fileLimits);
    }

    mServerPortNumber = SERVER_PORT_NUMBER_BASE + (getpid() % 1023); 
}

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::testAppleVodAdaptiveBitrate(void)
{   
    IL_DAEMON_LIB_NS::ActiveScheduler scheduler;
    HttpApplication::handleList_t serverHandles;
    HttpApplication::handleList_t clientHandles;
	HttpApplication app(1);

    // Start application
    CPPUNIT_ASSERT(scheduler.Start(1, NUM_IO_THREADS) == 0);

    {
        ACE_Reactor *appReactor = scheduler.AppReactor();
        std::vector<ACE_Reactor *> ioReactorVec(1, scheduler.IOReactor(0));
        std::vector<ACE_Lock *> ioBarrierVec(1, scheduler.IOBarrier(0));
    
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Activate, &app, appReactor, boost::cref(ioReactorVec), boost::cref(ioBarrierVec));
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    // Configure server block
    HttpApplication::serverConfig_t serverConfig;

    serverConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    serverConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    serverConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    serverConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    serverConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    serverConfig.Common.Endpoint.SrcIfHandle = 0;
    serverConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    serverConfig.ProtocolProfile.HttpServerTypes = http_1_port_server::HttpServerType_APACHE;
    serverConfig.ProtocolProfile.ServerPortNum = mServerPortNumber;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.ResponseTimingType = L4L7_BASE_NS::ResponseTimingOptions_FIXED;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.FixedResponseLatency = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyMean = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyStdDeviation = 0;
    serverConfig.ResponseConfig.BodySizeType = L4L7_BASE_NS::BodySizeOptions_RANDOM;
    serverConfig.ResponseConfig.FixedBodySize = 0;
    serverConfig.ResponseConfig.RandomBodySizeMean = 1024;
    serverConfig.ResponseConfig.RandomBodySizeStdDeviation = 128;
    serverConfig.ResponseConfig.BodyContentType = L4L7_BASE_NS::BodyContentOptions_ASCII;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle = 1;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.ServerName = "TestHttpAbrApplication";
    serverConfig.ProtocolConfig.MaxSimultaneousClients = 0;
    serverConfig.ProtocolConfig.MaxRequestsPerClient = 0;

    // Configure Server ABR settings
    serverConfig.ProtocolProfile.EnableVideoServer = true;
    serverConfig.ProtocolProfile.ServerTargetDuration = 5;
    serverConfig.ProtocolProfile.ServerMediaSeqNum = 1;
    serverConfig.ProtocolProfile.ServerSlidingWindowPlaylistSize = 3;
    serverConfig.ProtocolProfile.ServerVideoLength = 20;
    serverConfig.ProtocolProfile.VideoServerVersion = http_1_port_server::HttpVideoServerVersionOptions_VERSION_1_0;
    serverConfig.ProtocolProfile.VideoServerStreamType = http_1_port_server::HttpVideoServerStreamTypeOptions_ADAPTIVE_BITRATE;
    serverConfig.ProtocolProfile.VideoServerType = http_1_port_server::HttpVideoServerTypeOptions_LIVE_STREAMING;

    // Add three bitrates
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_64);
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_150);
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_240);    

    {
        const HttpApplication::serverConfigList_t servers(1, serverConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigServers, &app, 0, boost::cref(servers), boost::ref(serverHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(serverHandles.size() == 1);
    CPPUNIT_ASSERT(serverHandles[0] == serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle);
    
    // Configure client block
    HttpApplication::clientConfig_t clientConfig;

    clientConfig.Common.Load.LoadProfile.LoadType = L4L7_BASE_NS::LoadTypes_CONNECTIONS;  // L4L7_BASE_NS::LoadTypes_CONNECTIONS_PER_TIME_UNIT
    clientConfig.Common.Load.LoadProfile.RandomizationSeed = 0;
    clientConfig.Common.Load.LoadProfile.MaxConnectionsAttempted = 0;
    clientConfig.Common.Load.LoadProfile.MaxOpenConnections = 0;
    clientConfig.Common.Load.LoadProfile.MaxTransactionsAttempted = 0;
    clientConfig.Common.Load.LoadPhases.resize(4);
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.Height = 1;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.SteadyTime = 60;
    /*
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;  
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.RampTime = 10;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.SteadyTime = 0;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.Height = 0;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.RampTime = 30;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.SteadyTime = 10;
*/
    clientConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    clientConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    clientConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    clientConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    clientConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    clientConfig.Common.Endpoint.SrcIfHandle = 0;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors.resize(1);
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].ifType = IFSETUP_NS::STC_DM_IFC_IPv4;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].indexInList = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList.resize(1);
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsRange = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsDirectlyConnected = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfCountPerLowerIf = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfRecycleCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.TotalCount = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.BllHandle = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.AffiliatedInterface = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[0] = 127;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[3] = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[0] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[3] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].SkipReserved = false;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrRepeatCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].PrefixLength = 8;
    clientConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    clientConfig.ProtocolProfile.EnableKeepAlive = false;//true;
    clientConfig.ProtocolProfile.UserAgentHeader = "Spirent Communications";
    clientConfig.ProtocolProfile.EnablePipelining = true;
    clientConfig.ProtocolProfile.MaxPipelineDepth = 8;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle = 2;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.ClientName = "TestHttpAbrApplication";
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.EndpointConnectionPattern = L4L7_BASE_NS::EndpointConnectionPatternOptions_BACKBONE_SRC_FIRST;
    clientConfig.ProtocolConfig.MaxTransactionsPerServer = 16;
    clientConfig.ServerPortInfo = mServerPortNumber;
//Config ABR settings
    clientConfig.ProtocolProfile.EnableVideoClient = true;      // temporary
    clientConfig.ProtocolProfile.VideoClientVideoType = http_1_port_server::HttpVideoClientVideoTypeOptions_VOD;
    clientConfig.ProtocolProfile.VideoClientViewTime = 20;
    clientConfig.ProtocolProfile.VideoClientStartBitrate = http_1_port_server::HttpVideoClientStartBitrateOptions_PREDEFINE;
    clientConfig.ProtocolProfile.VideoClientBitrateAlg = http_1_port_server::HttpVideoClientBitrateAlgOptions_SMART;
    clientConfig.ProtocolProfile.VideoClientPredefineMethod = http_1_port_server::HttpVideoClientPredefineMethodOptions_MINIMUM;
    clientConfig.ProtocolProfile.VideoClientType = http_1_port_server::HttpVideoClientTypeOptions_SPIRENT_HLS;

    {
        const HttpApplication::clientConfigList_t clients(1, clientConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigClients, &app, 0, boost::cref(clients), boost::ref(clientHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(clientHandles.size() == 1);
    CPPUNIT_ASSERT(clientHandles[0] == clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle);

    for (size_t run = 0; run < 1; run++)
    {
        // Start the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(serverHandles[0]);
            handles.push_back(clientHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StartProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }

        // Let the blocks run for a bit
        sleep(50);
        
        // Stop the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(clientHandles[0]);
            handles.push_back(serverHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StopProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }
    }
    
    std::vector<HttpServerRawStats> serverStats;
    std::vector<HttpClientRawStats> clientStats;
    std::vector<HttpVideoServerRawStats> videoServerStats;
    std::vector<HttpVideoClientRawStats> videoClientStats;


    app.GetHttpServerStats(0, serverHandles, serverStats);
    app.GetHttpClientStats(0, clientHandles, clientStats);

    CPPUNIT_ASSERT(serverStats.size() > 0);
    CPPUNIT_ASSERT(clientStats.size() > 0);

    CPPUNIT_ASSERT(serverStats[0].txResponseCode200 > 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode200 > 0);
    
    // Should not be any errors
    CPPUNIT_ASSERT( !(
                        serverStats[0].txResponseCode400 ||
                        serverStats[0].txResponseCode404 ||
                        serverStats[0].txResponseCode405 ||
                        clientStats[0].rxResponseCode400 ||
                        clientStats[0].rxResponseCode404 ||
                        clientStats[0].rxResponseCode405
                      )
                    );

    CPPUNIT_ASSERT(serverStats[0].unsuccessfulTransactions == 0);
    CPPUNIT_ASSERT(clientStats[0].unsuccessfulTransactions == 0);

    CPPUNIT_ASSERT(serverStats[0].successfulTransactions > 0);
    CPPUNIT_ASSERT(clientStats[0].successfulTransactions > 0);

    //TODO:  Add ABR STATS!

    // Shutdown the application
    {
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Deactivate, &app);
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    CPPUNIT_ASSERT(scheduler.Stop() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::testAppleLiveAdaptiveBitrate(void)
{
    IL_DAEMON_LIB_NS::ActiveScheduler scheduler;
    HttpApplication::handleList_t serverHandles;
    HttpApplication::handleList_t clientHandles;
	HttpApplication app(1);

    // Start application
    CPPUNIT_ASSERT(scheduler.Start(1, NUM_IO_THREADS) == 0);

    {
        ACE_Reactor *appReactor = scheduler.AppReactor();
        std::vector<ACE_Reactor *> ioReactorVec(1, scheduler.IOReactor(0));
        std::vector<ACE_Lock *> ioBarrierVec(1, scheduler.IOBarrier(0));
    
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Activate, &app, appReactor, boost::cref(ioReactorVec), boost::cref(ioBarrierVec));
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    // Configure server block
    HttpApplication::serverConfig_t serverConfig;

    serverConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    serverConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    serverConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    serverConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    serverConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    serverConfig.Common.Endpoint.SrcIfHandle = 0;
    serverConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    serverConfig.ProtocolProfile.HttpServerTypes = http_1_port_server::HttpServerType_APACHE;
    serverConfig.ProtocolProfile.ServerPortNum = mServerPortNumber;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.ResponseTimingType = L4L7_BASE_NS::ResponseTimingOptions_FIXED;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.FixedResponseLatency = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyMean = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyStdDeviation = 0;
    serverConfig.ResponseConfig.BodySizeType = L4L7_BASE_NS::BodySizeOptions_RANDOM;
    serverConfig.ResponseConfig.FixedBodySize = 0;
    serverConfig.ResponseConfig.RandomBodySizeMean = 1024;
    serverConfig.ResponseConfig.RandomBodySizeStdDeviation = 128;
    serverConfig.ResponseConfig.BodyContentType = L4L7_BASE_NS::BodyContentOptions_ASCII;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle = 1;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.ServerName = "TestHttpAbrApplication";
    serverConfig.ProtocolConfig.MaxSimultaneousClients = 0;
    serverConfig.ProtocolConfig.MaxRequestsPerClient = 0;

    // Configure Server ABR settings
    serverConfig.ProtocolProfile.ServerTargetDuration = 5;
    serverConfig.ProtocolProfile.ServerMediaSeqNum = 1;
    serverConfig.ProtocolProfile.ServerSlidingWindowPlaylistSize = 3;
    serverConfig.ProtocolProfile.ServerVideoLength = 20;
    serverConfig.ProtocolProfile.VideoServerVersion = http_1_port_server::HttpVideoServerVersionOptions_VERSION_1_0;
    serverConfig.ProtocolProfile.VideoServerStreamType = http_1_port_server::HttpVideoServerStreamTypeOptions_ADAPTIVE_BITRATE;
    serverConfig.ProtocolProfile.VideoServerType = http_1_port_server::HttpVideoServerTypeOptions_LIVE_STREAMING;

    // Add three bitrates
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_64);
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_150);
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_240);    

    {
        const HttpApplication::serverConfigList_t servers(1, serverConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigServers, &app, 0, boost::cref(servers), boost::ref(serverHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(serverHandles.size() == 1);
    CPPUNIT_ASSERT(serverHandles[0] == serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle);
    
    // Configure client block
    HttpApplication::clientConfig_t clientConfig;

    clientConfig.Common.Load.LoadProfile.LoadType = L4L7_BASE_NS::LoadTypes_CONNECTIONS;  // L4L7_BASE_NS::LoadTypes_CONNECTIONS_PER_TIME_UNIT
    clientConfig.Common.Load.LoadProfile.RandomizationSeed = 0;
    clientConfig.Common.Load.LoadProfile.MaxConnectionsAttempted = 0;
    clientConfig.Common.Load.LoadProfile.MaxOpenConnections = 0;
    clientConfig.Common.Load.LoadProfile.MaxTransactionsAttempted = 0;
    clientConfig.Common.Load.LoadPhases.resize(4);
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.Height = 1;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
/*    
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.RampTime = 10;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.SteadyTime = 0;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.Height = 0;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.RampTime = 30;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.SteadyTime = 10;
*/
    clientConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    clientConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    clientConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    clientConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    clientConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    clientConfig.Common.Endpoint.SrcIfHandle = 0;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors.resize(1);
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].ifType = IFSETUP_NS::STC_DM_IFC_IPv4;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].indexInList = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList.resize(1);
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsRange = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsDirectlyConnected = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfCountPerLowerIf = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfRecycleCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.TotalCount = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.BllHandle = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.AffiliatedInterface = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[0] = 127;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[3] = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[0] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[3] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].SkipReserved = false;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrRepeatCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].PrefixLength = 8;
    clientConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    clientConfig.ProtocolProfile.EnableKeepAlive = false; //true;
    clientConfig.ProtocolProfile.UserAgentHeader = "Spirent Communications";
    clientConfig.ProtocolProfile.EnablePipelining = true;
    clientConfig.ProtocolProfile.MaxPipelineDepth = 8;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle = 2;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.ClientName = "TestHttpAbrApplication";
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.EndpointConnectionPattern = L4L7_BASE_NS::EndpointConnectionPatternOptions_BACKBONE_SRC_FIRST;
    clientConfig.ProtocolConfig.MaxTransactionsPerServer = 16;
    clientConfig.ServerPortInfo = mServerPortNumber;
//ABR client
    clientConfig.ProtocolProfile.EnableVideoClient = false;      // temporary
    clientConfig.ProtocolProfile.VideoClientVideoType = http_1_port_server::HttpVideoClientVideoTypeOptions_LIVE;
    clientConfig.ProtocolProfile.VideoClientViewTime = 20;
    clientConfig.ProtocolProfile.VideoClientStartBitrate = http_1_port_server::HttpVideoClientStartBitrateOptions_PREDEFINE;
    clientConfig.ProtocolProfile.VideoClientBitrateAlg = http_1_port_server::HttpVideoClientBitrateAlgOptions_SMART;
    clientConfig.ProtocolProfile.VideoClientPredefineMethod = http_1_port_server::HttpVideoClientPredefineMethodOptions_MINIMUM;
    clientConfig.ProtocolProfile.VideoClientType = http_1_port_server::HttpVideoClientTypeOptions_SPIRENT_HLS;
    {
        const HttpApplication::clientConfigList_t clients(1, clientConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigClients, &app, 0, boost::cref(clients), boost::ref(clientHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(clientHandles.size() == 1);
    CPPUNIT_ASSERT(clientHandles[0] == clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle);

    for (size_t run = 0; run < 1; run++)
    {
        // Start the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(serverHandles[0]);
            handles.push_back(clientHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StartProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }

        // Let the blocks run for a bit
        sleep(50);

        // Stop the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(clientHandles[0]);
            handles.push_back(serverHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StopProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }
    }
    
    std::vector<HttpServerRawStats> serverStats;
    std::vector<HttpClientRawStats> clientStats;
    
    app.GetHttpServerStats(0, serverHandles, serverStats);
    app.GetHttpClientStats(0, clientHandles, clientStats);

    CPPUNIT_ASSERT(serverStats.size() > 0);
    CPPUNIT_ASSERT(clientStats.size() > 0);

    CPPUNIT_ASSERT(serverStats[0].txResponseCode200 > 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode200 > 0);
    
    // Should not be any errors
    CPPUNIT_ASSERT( !(
                        serverStats[0].txResponseCode400 ||
                        serverStats[0].txResponseCode404 ||
                        serverStats[0].txResponseCode405 ||
                        clientStats[0].rxResponseCode400 ||
                        clientStats[0].rxResponseCode404 ||
                        clientStats[0].rxResponseCode405
                      )
                    );

    CPPUNIT_ASSERT(serverStats[0].unsuccessfulTransactions == 0);
    CPPUNIT_ASSERT(clientStats[0].unsuccessfulTransactions == 0);

    CPPUNIT_ASSERT(serverStats[0].successfulTransactions > 0);
    CPPUNIT_ASSERT(clientStats[0].successfulTransactions > 0);

    //TODO:  Add ABR STATS!

    // Shutdown the application
    {
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Deactivate, &app);
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    CPPUNIT_ASSERT(scheduler.Stop() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::testAppleVodProgressiveSimple(void)
{  
    IL_DAEMON_LIB_NS::ActiveScheduler scheduler;
    HttpApplication::handleList_t serverHandles;
    HttpApplication::handleList_t clientHandles;
	HttpApplication app(1);

    // Start application
    CPPUNIT_ASSERT(scheduler.Start(1, NUM_IO_THREADS) == 0);

    {
        ACE_Reactor *appReactor = scheduler.AppReactor();
        std::vector<ACE_Reactor *> ioReactorVec(1, scheduler.IOReactor(0));
        std::vector<ACE_Lock *> ioBarrierVec(1, scheduler.IOBarrier(0));
    
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Activate, &app, appReactor, boost::cref(ioReactorVec), boost::cref(ioBarrierVec));
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    // Configure server block
    HttpApplication::serverConfig_t serverConfig;

    serverConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    serverConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    serverConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    serverConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    serverConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    serverConfig.Common.Endpoint.SrcIfHandle = 0;
    serverConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    serverConfig.ProtocolProfile.HttpServerTypes = http_1_port_server::HttpServerType_APACHE;
    serverConfig.ProtocolProfile.ServerPortNum = mServerPortNumber;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.ResponseTimingType = L4L7_BASE_NS::ResponseTimingOptions_FIXED;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.FixedResponseLatency = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyMean = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyStdDeviation = 0;
    serverConfig.ResponseConfig.BodySizeType = L4L7_BASE_NS::BodySizeOptions_RANDOM;
    serverConfig.ResponseConfig.FixedBodySize = 0;
    serverConfig.ResponseConfig.RandomBodySizeMean = 1024;
    serverConfig.ResponseConfig.RandomBodySizeStdDeviation = 128;
    serverConfig.ResponseConfig.BodyContentType = L4L7_BASE_NS::BodyContentOptions_ASCII;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle = 1;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.ServerName = "TestHttpAbrApplication";
    serverConfig.ProtocolConfig.MaxSimultaneousClients = 0;
    serverConfig.ProtocolConfig.MaxRequestsPerClient = 0;

    // Configure Server ABR settings
    serverConfig.ProtocolProfile.ServerTargetDuration = 0;
    serverConfig.ProtocolProfile.ServerMediaSeqNum = 1;
    serverConfig.ProtocolProfile.ServerSlidingWindowPlaylistSize = 0;
    serverConfig.ProtocolProfile.ServerVideoLength = 20;
    serverConfig.ProtocolProfile.VideoServerVersion = http_1_port_server::HttpVideoServerVersionOptions_VERSION_1_0;
    serverConfig.ProtocolProfile.VideoServerStreamType = http_1_port_server::HttpVideoServerStreamTypeOptions_PROGRESSIVE;
    serverConfig.ProtocolProfile.VideoServerType = http_1_port_server::HttpVideoServerTypeOptions_LIVE_STREAMING;

    // Add one bitrate
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_64);

    {
        const HttpApplication::serverConfigList_t servers(1, serverConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigServers, &app, 0, boost::cref(servers), boost::ref(serverHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(serverHandles.size() == 1);
    CPPUNIT_ASSERT(serverHandles[0] == serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle);
    
    // Configure client block
    HttpApplication::clientConfig_t clientConfig;

    clientConfig.Common.Load.LoadProfile.LoadType = L4L7_BASE_NS::LoadTypes_CONNECTIONS;  // L4L7_BASE_NS::LoadTypes_CONNECTIONS_PER_TIME_UNIT
    clientConfig.Common.Load.LoadProfile.RandomizationSeed = 0;
    clientConfig.Common.Load.LoadProfile.MaxConnectionsAttempted = 0;
    clientConfig.Common.Load.LoadProfile.MaxOpenConnections = 0;
    clientConfig.Common.Load.LoadProfile.MaxTransactionsAttempted = 0;
    clientConfig.Common.Load.LoadPhases.resize(4);
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.Height = 1;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.SteadyTime = 40;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
/*    
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.RampTime = 10;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.SteadyTime = 0;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.Height = 0;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.RampTime = 30;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.SteadyTime = 10;
*/
    clientConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    clientConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    clientConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    clientConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    clientConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    clientConfig.Common.Endpoint.SrcIfHandle = 0;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors.resize(1);
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].ifType = IFSETUP_NS::STC_DM_IFC_IPv4;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].indexInList = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList.resize(1);
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsRange = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsDirectlyConnected = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfCountPerLowerIf = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfRecycleCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.TotalCount = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.BllHandle = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.AffiliatedInterface = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[0] = 127;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[3] = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[0] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[3] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].SkipReserved = false;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrRepeatCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].PrefixLength = 8;
    clientConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    clientConfig.ProtocolProfile.EnableKeepAlive = false;// true;
    clientConfig.ProtocolProfile.UserAgentHeader = "Spirent Communications";
    clientConfig.ProtocolProfile.EnablePipelining = true;
    clientConfig.ProtocolProfile.MaxPipelineDepth = 8;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle = 2;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.ClientName = "TestHttpAbrApplication";
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.EndpointConnectionPattern = L4L7_BASE_NS::EndpointConnectionPatternOptions_BACKBONE_SRC_FIRST;
    clientConfig.ProtocolConfig.MaxTransactionsPerServer = 16;
    clientConfig.ServerPortInfo = mServerPortNumber;

    //ABR client
    clientConfig.ProtocolProfile.EnableVideoClient = false;      // temporary
    clientConfig.ProtocolProfile.VideoClientVideoType = http_1_port_server::HttpVideoClientVideoTypeOptions_VOD;
    clientConfig.ProtocolProfile.VideoClientViewTime = 20;
    clientConfig.ProtocolProfile.VideoClientStartBitrate = http_1_port_server::HttpVideoClientStartBitrateOptions_PREDEFINE;
    clientConfig.ProtocolProfile.VideoClientBitrateAlg = http_1_port_server::HttpVideoClientBitrateAlgOptions_SMART;
    clientConfig.ProtocolProfile.VideoClientPredefineMethod = http_1_port_server::HttpVideoClientPredefineMethodOptions_MINIMUM;
    clientConfig.ProtocolProfile.VideoClientType = http_1_port_server::HttpVideoClientTypeOptions_SPIRENT_HLS;

    {
        const HttpApplication::clientConfigList_t clients(1, clientConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigClients, &app, 0, boost::cref(clients), boost::ref(clientHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(clientHandles.size() == 1);
    CPPUNIT_ASSERT(clientHandles[0] == clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle);

    for (size_t run = 0; run < 1; run++)
    {
        // Start the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(serverHandles[0]);
            handles.push_back(clientHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StartProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }

        // Let the blocks run for a bit
        sleep(30);

        // Stop the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(clientHandles[0]);
            handles.push_back(serverHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StopProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }
    }
    
    std::vector<HttpServerRawStats> serverStats;
    std::vector<HttpClientRawStats> clientStats;
    
    app.GetHttpServerStats(0, serverHandles, serverStats);
    app.GetHttpClientStats(0, clientHandles, clientStats);

    CPPUNIT_ASSERT(serverStats.size() > 0);
    CPPUNIT_ASSERT(clientStats.size() > 0);

    CPPUNIT_ASSERT(serverStats[0].txResponseCode200 > 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode200 > 0);
    
    // Should not be any errors
    CPPUNIT_ASSERT( !(
                        serverStats[0].txResponseCode400 ||
                        serverStats[0].txResponseCode404 ||
                        serverStats[0].txResponseCode405 ||
                        clientStats[0].rxResponseCode400 ||
                        clientStats[0].rxResponseCode404 ||
                        clientStats[0].rxResponseCode405
                      )
                    );

    CPPUNIT_ASSERT(serverStats[0].unsuccessfulTransactions == 0);
    CPPUNIT_ASSERT(clientStats[0].unsuccessfulTransactions == 0);

    CPPUNIT_ASSERT(serverStats[0].successfulTransactions > 0);
    CPPUNIT_ASSERT(clientStats[0].successfulTransactions > 0);

    //TODO:  Add ABR STATS!

    // Shutdown the application
    {
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Deactivate, &app);
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    CPPUNIT_ASSERT(scheduler.Stop() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::testAppleVodProgressiveSegmented(void)
{    
    IL_DAEMON_LIB_NS::ActiveScheduler scheduler;
    HttpApplication::handleList_t serverHandles;
    HttpApplication::handleList_t clientHandles;
	HttpApplication app(1);

    // Start application
    CPPUNIT_ASSERT(scheduler.Start(1, NUM_IO_THREADS) == 0);

    {
        ACE_Reactor *appReactor = scheduler.AppReactor();
        std::vector<ACE_Reactor *> ioReactorVec(1, scheduler.IOReactor(0));
        std::vector<ACE_Lock *> ioBarrierVec(1, scheduler.IOBarrier(0));
    
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Activate, &app, appReactor, boost::cref(ioReactorVec), boost::cref(ioBarrierVec));
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    // Configure server block
    HttpApplication::serverConfig_t serverConfig;

    serverConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    serverConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    serverConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    serverConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    serverConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    serverConfig.Common.Endpoint.SrcIfHandle = 0;
    serverConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    serverConfig.ProtocolProfile.HttpServerTypes = http_1_port_server::HttpServerType_APACHE;
    serverConfig.ProtocolProfile.ServerPortNum = mServerPortNumber;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.ResponseTimingType = L4L7_BASE_NS::ResponseTimingOptions_FIXED;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.FixedResponseLatency = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyMean = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyStdDeviation = 0;
    serverConfig.ResponseConfig.BodySizeType = L4L7_BASE_NS::BodySizeOptions_RANDOM;
    serverConfig.ResponseConfig.FixedBodySize = 0;
    serverConfig.ResponseConfig.RandomBodySizeMean = 1024;
    serverConfig.ResponseConfig.RandomBodySizeStdDeviation = 128;
    serverConfig.ResponseConfig.BodyContentType = L4L7_BASE_NS::BodyContentOptions_ASCII;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle = 1;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.ServerName = "TestHttpAbrApplication";
    serverConfig.ProtocolConfig.MaxSimultaneousClients = 0;
    serverConfig.ProtocolConfig.MaxRequestsPerClient = 0;

    // Configure Server ABR settings
    serverConfig.ProtocolProfile.ServerTargetDuration = 5;
    serverConfig.ProtocolProfile.ServerMediaSeqNum = 1;
    serverConfig.ProtocolProfile.ServerSlidingWindowPlaylistSize = 3;
    serverConfig.ProtocolProfile.ServerVideoLength = 20;
    serverConfig.ProtocolProfile.VideoServerVersion = http_1_port_server::HttpVideoServerVersionOptions_VERSION_1_0;
    serverConfig.ProtocolProfile.VideoServerStreamType = http_1_port_server::HttpVideoServerStreamTypeOptions_PROGRESSIVE;
    serverConfig.ProtocolProfile.VideoServerType = http_1_port_server::HttpVideoServerTypeOptions_LIVE_STREAMING;

    // Add one bitrate
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_64);

    {
        const HttpApplication::serverConfigList_t servers(1, serverConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigServers, &app, 0, boost::cref(servers), boost::ref(serverHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(serverHandles.size() == 1);
    CPPUNIT_ASSERT(serverHandles[0] == serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle);
    
    // Configure client block
    HttpApplication::clientConfig_t clientConfig;

    clientConfig.Common.Load.LoadProfile.LoadType = L4L7_BASE_NS::LoadTypes_CONNECTIONS;  // L4L7_BASE_NS::LoadTypes_CONNECTIONS_PER_TIME_UNIT
    clientConfig.Common.Load.LoadProfile.RandomizationSeed = 0;
    clientConfig.Common.Load.LoadProfile.MaxConnectionsAttempted = 0;
    clientConfig.Common.Load.LoadProfile.MaxOpenConnections = 0;
    clientConfig.Common.Load.LoadProfile.MaxTransactionsAttempted = 0;
    clientConfig.Common.Load.LoadPhases.resize(4);
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.Height = 1;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
/*    
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.RampTime = 10;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.SteadyTime = 0;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.Height = 0;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.RampTime = 30;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.SteadyTime = 10;
*/
    clientConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    clientConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    clientConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    clientConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    clientConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    clientConfig.Common.Endpoint.SrcIfHandle = 0;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors.resize(1);
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].ifType = IFSETUP_NS::STC_DM_IFC_IPv4;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].indexInList = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList.resize(1);
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsRange = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsDirectlyConnected = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfCountPerLowerIf = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfRecycleCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.TotalCount = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.BllHandle = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.AffiliatedInterface = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[0] = 127;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[3] = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[0] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[3] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].SkipReserved = false;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrRepeatCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].PrefixLength = 8;
    clientConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    clientConfig.ProtocolProfile.EnableKeepAlive = false; //true;
    clientConfig.ProtocolProfile.UserAgentHeader = "Spirent Communications";
    clientConfig.ProtocolProfile.EnablePipelining = true;
    clientConfig.ProtocolProfile.MaxPipelineDepth = 8;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle = 2;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.ClientName = "TestHttpAbrApplication";
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.EndpointConnectionPattern = L4L7_BASE_NS::EndpointConnectionPatternOptions_BACKBONE_SRC_FIRST;
    clientConfig.ProtocolConfig.MaxTransactionsPerServer = 16;
    clientConfig.ServerPortInfo = mServerPortNumber;
    //ABR client
    clientConfig.ProtocolProfile.EnableVideoClient = false;      // temporary
    clientConfig.ProtocolProfile.VideoClientVideoType = http_1_port_server::HttpVideoClientVideoTypeOptions_VOD;
    clientConfig.ProtocolProfile.VideoClientViewTime = 20;
    clientConfig.ProtocolProfile.VideoClientStartBitrate = http_1_port_server::HttpVideoClientStartBitrateOptions_PREDEFINE;
    clientConfig.ProtocolProfile.VideoClientBitrateAlg = http_1_port_server::HttpVideoClientBitrateAlgOptions_SMART;
    clientConfig.ProtocolProfile.VideoClientPredefineMethod = http_1_port_server::HttpVideoClientPredefineMethodOptions_MINIMUM;
    clientConfig.ProtocolProfile.VideoClientType = http_1_port_server::HttpVideoClientTypeOptions_SPIRENT_HLS;

    {
        const HttpApplication::clientConfigList_t clients(1, clientConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigClients, &app, 0, boost::cref(clients), boost::ref(clientHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(clientHandles.size() == 1);
    CPPUNIT_ASSERT(clientHandles[0] == clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle);

    for (size_t run = 0; run < 1; run++)
    {
        // Start the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(serverHandles[0]);
            handles.push_back(clientHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StartProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }

        // Let the blocks run for a bit
        sleep(50);

        // Stop the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(clientHandles[0]);
            handles.push_back(serverHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StopProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }
    }
    
    std::vector<HttpServerRawStats> serverStats;
    std::vector<HttpClientRawStats> clientStats;
    
    app.GetHttpServerStats(0, serverHandles, serverStats);
    app.GetHttpClientStats(0, clientHandles, clientStats);

    CPPUNIT_ASSERT(serverStats.size() > 0);
    CPPUNIT_ASSERT(clientStats.size() > 0);

    CPPUNIT_ASSERT(serverStats[0].txResponseCode200 > 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode200 > 0);
    
    // Should not be any errors
    CPPUNIT_ASSERT( !(
                        serverStats[0].txResponseCode400 ||
                        serverStats[0].txResponseCode404 ||
                        serverStats[0].txResponseCode405 ||
                        clientStats[0].rxResponseCode400 ||
                        clientStats[0].rxResponseCode404 ||
                        clientStats[0].rxResponseCode405
                      )
                    );

    CPPUNIT_ASSERT(serverStats[0].unsuccessfulTransactions == 0);
    CPPUNIT_ASSERT(clientStats[0].unsuccessfulTransactions == 0);

    CPPUNIT_ASSERT(serverStats[0].successfulTransactions > 0);
    CPPUNIT_ASSERT(clientStats[0].successfulTransactions > 0);

    //TODO:  Add ABR STATS!

    // Shutdown the application
    {
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Deactivate, &app);
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    CPPUNIT_ASSERT(scheduler.Stop() == 0);
}

///////////////////////////////////////////////////////////////////////////////

void TestHttpAbrApplication::testLiveProgressive404(void)
{
    IL_DAEMON_LIB_NS::ActiveScheduler scheduler;
    HttpApplication::handleList_t serverHandles;
    HttpApplication::handleList_t clientHandles;
	HttpApplication app(1);

    // Start application
    CPPUNIT_ASSERT(scheduler.Start(1, NUM_IO_THREADS) == 0);

    {
        ACE_Reactor *appReactor = scheduler.AppReactor();
        std::vector<ACE_Reactor *> ioReactorVec(1, scheduler.IOReactor(0));
        std::vector<ACE_Lock *> ioBarrierVec(1, scheduler.IOBarrier(0));
    
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Activate, &app, appReactor, boost::cref(ioReactorVec), boost::cref(ioBarrierVec));
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    // Configure server block
    HttpApplication::serverConfig_t serverConfig;

    serverConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    serverConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    serverConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    serverConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    serverConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    serverConfig.Common.Endpoint.SrcIfHandle = 0;
    serverConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    serverConfig.ProtocolProfile.HttpServerTypes = http_1_port_server::HttpServerType_APACHE;
    serverConfig.ProtocolProfile.ServerPortNum = mServerPortNumber;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.ResponseTimingType = L4L7_BASE_NS::ResponseTimingOptions_FIXED;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.FixedResponseLatency = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyMean = 0;
    serverConfig.ResponseConfig.L4L7ServerResponseConfig.RandomResponseLatencyStdDeviation = 0;
    serverConfig.ResponseConfig.BodySizeType = L4L7_BASE_NS::BodySizeOptions_RANDOM;
    serverConfig.ResponseConfig.FixedBodySize = 0;
    serverConfig.ResponseConfig.RandomBodySizeMean = 1024;
    serverConfig.ResponseConfig.RandomBodySizeStdDeviation = 128;
    serverConfig.ResponseConfig.BodyContentType = L4L7_BASE_NS::BodyContentOptions_ASCII;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle = 1;
    serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.ServerName = "TestHttpAbrApplication";
    serverConfig.ProtocolConfig.MaxSimultaneousClients = 0;
    serverConfig.ProtocolConfig.MaxRequestsPerClient = 0;

    // Configure Server ABR settings
    serverConfig.ProtocolProfile.ServerTargetDuration = 10;
    serverConfig.ProtocolProfile.ServerMediaSeqNum = 1;
    serverConfig.ProtocolProfile.ServerSlidingWindowPlaylistSize = 3;
    serverConfig.ProtocolProfile.ServerVideoLength = 60;
    serverConfig.ProtocolProfile.VideoServerVersion = http_1_port_server::HttpVideoServerVersionOptions_VERSION_1_0;
    serverConfig.ProtocolProfile.VideoServerStreamType = http_1_port_server::HttpVideoServerStreamTypeOptions_PROGRESSIVE;
    serverConfig.ProtocolProfile.VideoServerType = http_1_port_server::HttpVideoServerTypeOptions_LIVE_STREAMING;

    // Add one bitrate
    serverConfig.ProtocolProfile.VideoServerBitrateList.push_back(http_1_port_server::HttpVideoServerBitrateOptions_BR_64);

    {
        const HttpApplication::serverConfigList_t servers(1, serverConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigServers, &app, 0, boost::cref(servers), boost::ref(serverHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(serverHandles.size() == 1);
    CPPUNIT_ASSERT(serverHandles[0] == serverConfig.ProtocolConfig.L4L7ProtocolConfigServerBase.L4L7ProtocolConfigBase.BllHandle);
    
    // Configure client block
    HttpApplication::clientConfig_t clientConfig;

    clientConfig.Common.Load.LoadProfile.LoadType = L4L7_BASE_NS::LoadTypes_CONNECTIONS;  // L4L7_BASE_NS::LoadTypes_CONNECTIONS_PER_TIME_UNIT
    clientConfig.Common.Load.LoadProfile.RandomizationSeed = 0;
    clientConfig.Common.Load.LoadProfile.MaxConnectionsAttempted = 0;
    clientConfig.Common.Load.LoadProfile.MaxOpenConnections = 0;
    clientConfig.Common.Load.LoadProfile.MaxTransactionsAttempted = 0;
    clientConfig.Common.Load.LoadPhases.resize(4);
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[0].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.Height = 1;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[0].FlatDescriptor.SteadyTime = 5;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[1].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
/*    
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.RampTime = 10;
    clientConfig.Common.Load.LoadPhases[1].FlatDescriptor.SteadyTime = 0;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[2].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.Height = 300;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.RampTime = 0;
    clientConfig.Common.Load.LoadPhases[2].FlatDescriptor.SteadyTime = 60;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPattern = L4L7_BASE_NS::LoadPatternTypes_FLAT;
    clientConfig.Common.Load.LoadPhases[3].LoadPhase.LoadPhaseDurationUnits = L4L7_BASE_NS::ClientLoadPhaseTimeUnitOptions_SECONDS;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.Height = 0;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.RampTime = 30;
    clientConfig.Common.Load.LoadPhases[3].FlatDescriptor.SteadyTime = 10;
*/

    clientConfig.Common.Profile.L4L7Profile.ProfileName = "TestHttpAbrApplication";
    clientConfig.Common.Profile.L4L7Profile.Ipv4Tos = 0;
    clientConfig.Common.Profile.L4L7Profile.Ipv6TrafficClass = 0;
    clientConfig.Common.Profile.L4L7Profile.RxWindowSizeLimit = 0;
    clientConfig.Common.Profile.L4L7Profile.EnableDelayedAck = true;
    clientConfig.Common.Endpoint.SrcIfHandle = 0;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors.resize(1);
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].ifType = IFSETUP_NS::STC_DM_IFC_IPv4;
    clientConfig.Common.Endpoint.DstIf.ifDescriptors[0].indexInList = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList.resize(1);
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsRange = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.EmulatedIf.IsDirectlyConnected = true;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfCountPerLowerIf = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.IfRecycleCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.TotalCount = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.BllHandle = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].NetworkInterface.AffiliatedInterface = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[0] = 127;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].Address.address[3] = 1;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[0] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[1] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[2] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrStep.address[3] = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].SkipReserved = false;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].AddrRepeatCount = 0;
    clientConfig.Common.Endpoint.DstIf.Ipv4InterfaceList[0].PrefixLength = 8;
    clientConfig.ProtocolProfile.HttpVersion = http_1_port_server::HttpVersionOptions_VERSION_1_1;
    clientConfig.ProtocolProfile.EnableKeepAlive = true;
    clientConfig.ProtocolProfile.UserAgentHeader = "Spirent Communications";
    clientConfig.ProtocolProfile.EnablePipelining = true;
    clientConfig.ProtocolProfile.MaxPipelineDepth = 8;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle = 2;
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.ClientName = "TestHttpAbrApplication";
    clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.EndpointConnectionPattern = L4L7_BASE_NS::EndpointConnectionPatternOptions_BACKBONE_SRC_FIRST;
    clientConfig.ProtocolConfig.MaxTransactionsPerServer = 16;
    clientConfig.ServerPortInfo = mServerPortNumber;

    //ABR client
    clientConfig.ProtocolProfile.EnableVideoClient = false;      // temporary
    clientConfig.ProtocolProfile.VideoClientVideoType = http_1_port_server::HttpVideoClientVideoTypeOptions_LIVE;
    clientConfig.ProtocolProfile.VideoClientViewTime = 20;
    clientConfig.ProtocolProfile.VideoClientStartBitrate = http_1_port_server::HttpVideoClientStartBitrateOptions_PREDEFINE;
    clientConfig.ProtocolProfile.VideoClientBitrateAlg = http_1_port_server::HttpVideoClientBitrateAlgOptions_SMART;
    clientConfig.ProtocolProfile.VideoClientPredefineMethod = http_1_port_server::HttpVideoClientPredefineMethodOptions_MINIMUM;
    clientConfig.ProtocolProfile.VideoClientType = http_1_port_server::HttpVideoClientTypeOptions_SPIRENT_HLS;
    {
        const HttpApplication::clientConfigList_t clients(1, clientConfig);
        
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::ConfigClients, &app, 0, boost::cref(clients), boost::ref(clientHandles));
        scheduler.Enqueue(req);
        req->Wait();
    }

    CPPUNIT_ASSERT(clientHandles.size() == 1);
    CPPUNIT_ASSERT(clientHandles[0] == clientConfig.ProtocolConfig.L4L7ProtocolConfigClientBase.L4L7ProtocolConfigBase.BllHandle);

    for (size_t run = 0; run < 1; run++)
    {
        // Start the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(serverHandles[0]);
            handles.push_back(clientHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StartProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }

        // Let the blocks run for a bit
        sleep(5);

        // Stop the server and client blocks
        {
            HttpApplication::handleList_t handles;

            handles.push_back(clientHandles[0]);
            handles.push_back(serverHandles[0]);
        
            IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::StopProtocol, &app, 0, boost::cref(handles));
            scheduler.Enqueue(req);
            req->Wait();
        }
    }
    
    std::vector<HttpServerRawStats> serverStats;
    std::vector<HttpClientRawStats> clientStats;
    
    app.GetHttpServerStats(0, serverHandles, serverStats);
    app.GetHttpClientStats(0, clientHandles, clientStats);

    CPPUNIT_ASSERT(serverStats.size() > 0);
    CPPUNIT_ASSERT(clientStats.size() > 0);

    CPPUNIT_ASSERT(serverStats[0].txResponseCode200 == 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode200 == 0);
    
    // Should be 404
    CPPUNIT_ASSERT(serverStats[0].txResponseCode404 > 0);
    CPPUNIT_ASSERT(clientStats[0].rxResponseCode404 > 0);

    CPPUNIT_ASSERT(serverStats[0].unsuccessfulTransactions > 0);
    CPPUNIT_ASSERT(clientStats[0].unsuccessfulTransactions > 0);

    CPPUNIT_ASSERT(serverStats[0].successfulTransactions == 0);
    CPPUNIT_ASSERT(clientStats[0].successfulTransactions == 0);

    //TODO:  Add ABR STATS!

    // Shutdown the application
    {
        IL_DAEMON_LIB_MAKE_SYNC_REQUEST(req, void, &HttpApplication::Deactivate, &app);
        scheduler.Enqueue(req);
        req->Wait();
    }
    
    CPPUNIT_ASSERT(scheduler.Stop() == 0);
}

///////////////////////////////////////////////////////////////////////////////

CPPUNIT_TEST_SUITE_REGISTRATION(TestHttpAbrApplication);
