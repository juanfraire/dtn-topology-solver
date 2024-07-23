// Topology Header File

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <vector>
#include <istream>
#include <stdlib.h>
#include <cmath>
#include <bitset>

#include "Blossom/PerfectMatching.h"
#include "Dijkstra/Dijkstra.h"
#include "Bipartite/munkres.h"

using namespace std;

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#define MAXK 100 /// Max States in topology object
#define MAXS 21 /// Max Node Number in topology object
#define MAXC 600 /// Max Contacts in topology object
#define MAXW 100 /// Max Weight in an arc
#define INF 2000000000 /// Integer infinite
#define START 0 //contactArray 0 = cStart, 1 = cEnd, 2 = Source, 3 = Dest
#define END 1
#define SOURCE 2
#define DEST 3

class Topology {

public:
    /// Constructor Methods:
    Topology();
    Topology(string fileName);
    Topology(int numSat, int numET, int parsedK, int lowWeight, int highWeight,
            int lowTime, int highTime, int linkDensity);
    ~Topology();

    /// Configure Methods:
    void setName(string name);
    void fractionate(int maxTime);
    void setWeights(int lowWeight, int highWeight, int k1, int k2);
    void setWeight(int i, int j, int k, int weight);
    void checkSimetry(void);
    bool checkSingleInterface(void);
    void printPhyK(int k);
    void phyToLog(void);
    void clearLogTopology(void);

    /// Output Methods
    void saveMatrix(string fileName, bool Phy);
    void saveDOT();
    void saveION();
    void saveNSIM();
    void saveStats(bool writeFile);

    /// Solve Methods:
    void solveByMaxWeightLP(string solver);
    void solveByMaxWeightStrict();
    void solveByMaxWeightBlossom();
    void solveByFairnessMinMaxLP(string solver, float beta, float epsilon);
    void solveByFairnessKMinMaxLP(string solver, float beta, float epsilon);
    void solveByFairnessStrict();
    void solveByFairnessBlossom1();
    void solveByFairnessBlossom2();
    void solveByBruteForce(bool writeFile, bool writeFileAdv, string criteria, bool pareto);
    void solveBySteepestDescent(bool writeFile, bool writeFileAdv,
            string criteria, int maxIterations, int maxNeighbours);
    void solveByFirstImprovement(bool writeFile, bool writeFileAdv,
                string criteria, int maxIterations);
    void solveBySimulatedAnnealing(bool writeFile, bool writeFileAdv,
            string criteria, int maxIterations, int maxTemp, bool pareto); // Starting from Blossom
    void solveByFairnessWarshall02(bool writeFile, bool writeFileAdv,
            int maxIterations, int maxTemp, string criteria, int src, int dst);

    /// Route Methods
    void dijkstra(int s, int k1, int k2, bool phyTopo, int predecessor[MAXV],
            int distance[MAXV]);
    void floydWarshall(bool writeFile, bool singleInt);
    void contactGraph(bool writeFile);
    void contactGraphRFC00(int S, int D, int X, int T);
    void detectUnusedArcs();

    /// Enable for screen printing
    bool debug;

    /// Topology Name
    string topologyName;
    /// Some stats to pull off from outside
    int globalTotalArcOnTime;
    int globalTotalArcOnWeight;
    double globalMinMaxTimeRatio;
    double globalJ;
    int globalMinArcOnTime;
    int globalMaxArcOnTime;
    int globalmaxMaxDelay;
    int globalmaxAvgDelay;
    int globalUnroutedKs;

private:

    void readTopoFromFile(string);
    void randomizeTopo(int lowTime, int highTime, int linkDensity);
    void generateContacts();


    void writeCPLEXMaxWeight(string fileName);
    void writeCPLEXFairnessMaxTmin(string fileName, int Kmin, int Kmax,
            float epsilon);
    void writeCPLEXFairnessMinTmax(string fileName, int Kmin, int Kmax,
            int tobj, float beta);
    void readSolGLPK(string fileName);
    void readSolIBM(string fileName);

    void solveKNodeStrictMaxWeight(int k);
    void solveKArcStrictMaxWeight(int k);
    void solveKBlossom(int k, bool maxW);
    void evaluateRoutes(void);
    bool isInProxNodes(int D);
    bool isInexcludedNodes(int D);

    /// Minimum and Maximum time a given arc is enabled (used by fairness)
    double tmin, tmax;
    /// Number of Satellites and ET. The sum is the total node number
    int numSat, numET, nodeNum;
    /// The number of valid K iterations topology
    int parsedK;
    /// On if the Topology is solved or routed by any mean
    bool solved, routed;
    /// The time associated to iteration K (index)
    int t[MAXK];
    /// Physical and Logical Topology. [outgoing][incident][State]
    char phyTopology[MAXK][MAXS][MAXS], logTopology[MAXK][MAXS][MAXS];
    /// Arcs weights. unsigned int range: 0 to 4,294,967,295
    unsigned int arcWeight[MAXK][MAXS][MAXS];
    /// Max interfaces for node i. Set to 1 in constructor.
    short maxInterfaces[MAXS];

    /// Contact array to store contacts 0 = cStart, 1 = cEnd, 2 = Source, 3 = Dest
    int contactArray[MAXC][4];
    int contact, tAccum;

    /// Route files
    int distance[MAXK + 2][MAXS][MAXS];
    int nHop[MAXK + 1][MAXS][MAXS];
    /// CGR Stuff
    int excludedNodes[MAXS];
    int excludedNodesSize;
    int proxNodes[MAXS];
    int proxNodesSize;
    int forfeit[MAXS];
    int forfeitGlobal;
    int bestDelivery[MAXS];
    int bestDeliveryGlobal;
    int recursion;
    int level;

};

#endif

