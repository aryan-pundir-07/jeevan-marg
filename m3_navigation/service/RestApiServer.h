#ifndef M3_SERVICE_REST_API_SERVER_H
#define M3_SERVICE_REST_API_SERVER_H

#include "ShortestPathFinder.h"
#include "NearbyServicesFinder.h"
#include "TransportEstimator.h"
#include "../repository/ILocationRepository.h"
#include <memory>
#include <string>
#include <thread>
#include <atomic>

// Forward declaration of httplib::Server
namespace httplib {
    class Server;
}

class RestApiServer {
private:
    int port;
    std::string host;
    std::shared_ptr<ShortestPathFinder> pathFinder;
    std::shared_ptr<NearbyServicesFinder> nearbyFinder;
    std::shared_ptr<TransportEstimator> transportEstimator;
    std::shared_ptr<ILocationRepository> locationRepo;
    std::shared_ptr<RoadGraph> graph;

    std::unique_ptr<httplib::Server> server;
    std::thread serverThread;
    std::atomic<bool> running;

    void registerRoutes();

public:
    RestApiServer(int port,
                  std::shared_ptr<ShortestPathFinder> pf,
                  std::shared_ptr<NearbyServicesFinder> nf,
                  std::shared_ptr<TransportEstimator> te,
                  std::shared_ptr<ILocationRepository> lr,
                  std::shared_ptr<RoadGraph> rg,
                  const std::string& host = "0.0.0.0");
    ~RestApiServer();

    void start(bool blocking = false);
    void stop();
    bool isRunning() const { return running.load(); }
    int getPort() const { return port; }
};

#endif // M3_SERVICE_REST_API_SERVER_H
