// Topology Solver Main Program

#include "incl/Topology.h"

using namespace std;

void getRouteAvgImpro(int iterations, int iterationsIn);
void getParetoComp(int iterations);

int main(int argc, char *argv[]) {

    //Topology topology2(3, 1, 5, 1, 1, 10, 10, 30); // Model Topology
    //Topology topology2(3, 1, 10, 1, 1, 10, 10, 30);
    //Topology topology2(9, 0, 50, 1, 1, 10, 10, 10);
    //Topology topology2("top/1train_short.txt");
    //Topology topology2("top/1train_noET.txt"); // train with 8K of 10 time units
    //topology2.fractionate(15);

    // Para el TotSim que la Rompe
    // ***************************
    Topology topology2(20, 0, 25, 1, 1, 10, 100, 20);
    topology2.setName("TotSim_01");
    topology2.solveByFairnessBlossom1();
    topology2.saveNSIM();
    topology2.saveStats(true);
    topology2.saveDOT();

    // Para paper ArgenCom - CaseStudy
    // ***************************
//    Topology topology2("top/1train_short.txt");
//    topology2.fractionate(500);
//    topology2.setName("train");
//    topology2.solveByFairnessBlossom1();
//    topology2.saveNSIM();
//    topology2.saveStats(true);
//    topology2.saveDOT();

//    // Para paper WiSEE - Case Study
//    // ***************************
//    Topology topology2("top/transv_1daysx5.txt");
//    topology2.fractionate(500);
//
//    topology2.setName("CaseStudy_Blossom");
//    topology2.solveByFairnessBlossom1();
//    topology2.floydWarshall(true,false);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//    topology2.saveNSIM();
//
//    topology2.setName("CaseStudy_LP_Fair");
//    topology2.solveByFairnessMinMaxLP("ibm",1,0.1);
//    topology2.floydWarshall(true,false);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//    topology2.saveNSIM();
//
//    topology2.setName("CaseStudy_LP_MaxC");
//    topology2.solveByMaxWeightLP("ibm");
//    topology2.floydWarshall(true,false);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//    topology2.saveNSIM();

    // Articulo DTN
    // ***************************
//    Topology topology2("top/exampleB.txt"); // train with 8K of 10 time units
//    topology2.setName("ExampleB");
//    topology2.solveByFairnessBlossom1();
//    topology2.floydWarshall(false,false);
//    topology2.saveStats(true);
//    topology2.saveDOT();


    // Estudio de Pareto
    // ***************************
//  Topology topology2(3, 1, 5, 1, 1, 10, 10, 30); // Model Topology
//    Topology topology2("top/1train_noET.txt"); // train with 8K of 10 time units
//    topology2.setName("Model");
//    topology2.saveDOT();
//    topology2.solveByBruteForce(false,false,"AvgDelay", true);
//    topology2.setName("Model_Blos");
//    topology2.solveByFairnessBlossom1();
//    topology2.floydWarshall(false,false);
//    topology2.saveStats(true);
//    topology2.saveDOT();
//    topology2.setName("Model_SA");
//    topology2.solveBySimulatedAnnealing(true, false, "AvgDelay", 1000, 500, false);
//    topology2.saveStats(true);
//    topology2.saveDOT();
//    topology2.setName("Model_SD");
//    topology2.solveBySteepestDescent(true, false, "AvgDelay", 20, 40);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//    topology2.setName("Model_FI");
//    topology2.solveByFirstImprovement(true, false, "AvgDelay", 900);
//    topology2.saveDOT();
//    topology2.saveStats(true);
      //getParetoComp(100);


    // Estudio de Comparison
    // ***************************
    //getRouteAvgImpro(10, 100);
    //getRouteAvgImpro(20, 500);

    // Estudio de SD, FI, & SA
    // ***************************
//    Topology topology2(3, 1, 5, 1, 1, 10, 10, 30); // Model Topology
//    Topology topology2("top/1train_noET.txt"); // train with 8K of 10 time units

//      topology2.saveDOT();
//      topology2.setName("Topo2_BF");
//      topology2.solveByBruteForce(false, false, "AvgDelay");
//      topology2.saveDOT();
//      topology2.saveMatrix("out/Topo2_BF.txt", false);

//    topology2.setName("Topo2_Blossom");
//    topology2.solveByFairnessBlossom1();
//    topology2.saveDOT();
//
//    topology2.setName("Topo_SD");
//    topology2.solveBySteepestDescent(true, false, "AvgDelay", 20, 40);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//
//    topology2.setName("Topo_SAAvgD");
//    topology2.solveBySimulatedAnnealing(true, false, "AvgDelay", 100, 80);
//    topology2.saveDOT();
//    topology2.saveStats(true);
//
//    topology2.setName("Topo_SAJain");
//    topology2.solveBySimulatedAnnealing(true, false, "AvgDelayJainI", 100, 80);
//    topology2.saveDOT();
//    topology2.saveStats(true);

//
//    topology2.setName("Topo_FI");
//    topology2.solveByFirstImprovement(true, false, "AvgDelay", 900);
//    topology2.saveDOT();
//    topology2.saveStats(true);

    //getAverageRouteDelay(100, 25, 8);

    // Floyd Warshall
    // ***************************
//    Topology topology2("top/topo2.txt");
//    topology2.setName("FWTest");
//    topology2.saveDOT();
//    topology2.floydWarshall(true,false);


    cout << "Done!" << endl;
    return 0;
}

void getParetoComp(int iterations) {
    ofstream paretoResultCsv;
    paretoResultCsv.open("out/paretoCompResult.csv");
    paretoResultCsv << "Phy_AD" << "," << "Phy_UT" << "," << "Phy_JI" << ",";
    paretoResultCsv << "FCP_AD" << "," << "FCP_UT" << "," << "FCP_JI" << ",";
    paretoResultCsv << "SAD_AD" << "," << "SAD_UT" << "," << "SAD_JI" << ",";
    paretoResultCsv << "SAJ_AD" << "," << "SAJ_UT" << "," << "SAJ_JI" << ",";
    paretoResultCsv << "FI_AD" << "," << "FI_UT" << "," << "FI_JI" << ",";
    paretoResultCsv << "SD_AD" << "," << "SD_UT" << "," << "SD_JI" << ",";
    paretoResultCsv << endl;

    Topology * topo;
    for (int i = 0; i < iterations; i++) {
        topo = new Topology(5, 1, 10, 1, 1, 10, 20, 20);

        /// Physical
        cout << topo->topologyName << " Physical FloydWarshal:" << endl;
        topo->phyToLog();
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        /// FCP
        cout << topo->topologyName << " Blossom:" << endl;
        topo->solveByFairnessBlossom1();
        cout << topo->topologyName << " Blossom FloydWarshall:" << endl;
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        /// SA
        cout << topo->topologyName << " SimulatedAnnealing:" << endl;
        topo->solveBySimulatedAnnealing(false, false, "AvgDelay", 3000, 2000,
                false);
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        /// SAwJ
        cout << topo->topologyName << " SimulatedAnnealing:" << endl;
        topo->solveBySimulatedAnnealing(false, false, "AvgDelayJainI", 3000,
                2000, false);
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        /// FI
        cout << topo->topologyName << " FirstImprovement:" << endl;
        //topo->solveByFirstImprovement(writeFile, false, "AvgDelay", 500);
        topo->solveByFirstImprovement(false, false, "AvgDelayJainI", 500);
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        /// SD
        cout << topo->topologyName << " SteppestDescent:" << endl;
        //topo->solveBySteepestDescent(writeFile, false, "AvgDelay", 500, 200);
        topo->solveBySteepestDescent(false, false, "AvgDelayJainI", 500, 200);
        topo->floydWarshall(false, false);
        topo->saveStats(false);
        paretoResultCsv << topo->globalmaxAvgDelay << ","
                << topo->globalUnroutedKs << "," << topo->globalJ << ",";

        paretoResultCsv << endl;
    }

    paretoResultCsv.close();

    // Lets plot something
    ofstream paretoResultGplot;
    paretoResultGplot.open("out/paretoCompResult.gnuplot");
    paretoResultGplot << "set terminal svg size 700,400" << endl;
    paretoResultGplot << "set output \"paretoCompResult.svg\"" << endl;
    paretoResultGplot << "set datafile separator \",\"" << endl;
    paretoResultGplot << "set xlabel \"AD\"" << endl;
    paretoResultGplot << "set ylabel \"UT\"" << endl;
    paretoResultGplot << "set zlabel \"JI\"" << endl;
    //paretoResultGplot << "set view 29,53" << endl;
    //paretoResultGplot << "set hidden3d" << endl;
    //paretoResultGplot << "set dgrid3d 30,30" << endl;
    //paretoResultGplot << "set xyplane 0" << endl;
    //paretoResultGplot << "set pm3d at b" << endl;
    //paretoResultGplot << "set ticslevel 0.8" << endl;
    //paretoResultGplot << "set isosample 40,40" << endl;
    paretoResultGplot << "set view map" << endl;
    paretoResultGplot << "set key left bottom" << endl;
    paretoResultGplot << "set palette gray" << endl;
    paretoResultGplot << "set palette negative" << endl;
    //paretoResultGplot << "set yrange [10:60]" << endl;
    //paretoResultGplot << "set xrange [0:30]" << endl;
    //paretoResultGplot << "set zrange [(0.1):(0.7)]" << endl;

//        paretoResultCsv << "Phy_AD" << "," << "Phy_UT" << "," << "Phy_JI" << ","; 123
//        paretoResultCsv << "FCP_AD" << "," << "FCP_UT" << "," << "FCP_JI" << ","; 456
//        paretoResultCsv << "SAD_AD" << "," << "SAD_UT" << "," << "SAD_JI" << ","; 789
//        paretoResultCsv << "SAJ_AD" << "," << "SAJ_UT" << "," << "SAJ_JI" << ","; 10 11 12
//        paretoResultCsv << "FI_AD" << "," << "FI_UT" << "," << "FI_JI" << ","; 13 14 15
//        paretoResultCsv << "SD_AD" << "," << "SD_UT" << "," << "SD_JI" << ","; 16 17 18

    paretoResultGplot
            << "splot "
    //        << "'paretoCompResult.csv' using 1:2:(0.0):3  with points pointsize 1 pointtype 4 palette title \"Phy\", "
            << "'paretoCompResult.csv' using 4:5:(0.0):6  with points pointsize 1 pointtype 2 palette title \"FCP\","
            << "'paretoCompResult.csv' using 7:8:(0.0):9  with points pointsize 1 pointtype 6 palette title \"SAD\","
    //        << "'paretoCompResult.csv' using 10:11:(0.0):12  with points pointsize 1 pointtype 8 palette title \"SAJ\","
    //        << "'paretoCompResult.csv' using 13:14:(0.0):15  with points pointsize 1 pointtype 12 palette title \"FI\","
    //        << "'paretoCompResult.csv' using 16:17:(0.0):18  with points pointsize 1 pointtype 14 palette title \"SD\""
            << endl;
    paretoResultGplot.close();
    // Plot
    system(" cd out && gnuplot paretoCompResult.gnuplot");

}

void getRouteAvgImpro(int iterations, int iterationsIn) {

    int BruteForceLimit = 5;

    /// Comparison csv fileName:
    string manyTopologyCompFileName = "out/";
    manyTopologyCompFileName += "manyTopo_RouteComp.csv";
    ofstream manyTopologyCompFile;
    manyTopologyCompFile.open(manyTopologyCompFileName.c_str());

    /// Comparison gnuplot fileName:
    string manyTopologyCompGFileName = "out/";
    manyTopologyCompGFileName += "manyTopo_RouteComp.gnuplot";
    ofstream manyTopologyCompGFile;
    manyTopologyCompGFile.open(manyTopologyCompGFileName.c_str());

    /// Comparison csv fileName:
    string manyTopologyCompRelativeFileName = "out/";
    manyTopologyCompRelativeFileName += "manyTopo_RouteComp_Relative.csv";
    ofstream manyTopologyCompRelativeFile;
    manyTopologyCompRelativeFile.open(manyTopologyCompRelativeFileName.c_str());

    /// Comparison gnuplot fileName:
    string manyTopologyCompRelativeGFileName = "out/";
    manyTopologyCompRelativeGFileName += "manyTopo_RouteComp_Relative.gnuplot";
    ofstream manyTopologyCompRelativeGFile;
    manyTopologyCompRelativeGFile.open(
            manyTopologyCompRelativeGFileName.c_str());

    /// File Header
    manyTopologyCompFile << 0 << endl;
    manyTopologyCompFile << 1 << endl;

    /// File Header
    manyTopologyCompRelativeFile << 0 << endl;
    manyTopologyCompRelativeFile << 1 << endl;

    Topology * topo;
    bool writeFile = false;
    int iterationsOut = iterations;

    for (int j = 2; j < iterationsOut; j++) {
        /// Phy
        float nonSolvedAvgDelay = 0;
        float nonSolvedUnroutedK = 0;
        float nonSolvedMaxDelay = 0;
        double nonSolvedJ = 0;
        /// FCP
        float SolvedByBlossAvgDelay = 0;
        float SolvedByBlossUnroutedK = 0;
        float SolvedByBlossMaxDelay = 0;
        double SolvedByBlossJ = 0;
        /// SA
        float SolvedBySAAvgDelay = 0;
        float SolvedBySAUnroutedK = 0;
        float SolvedBySAMaxDelay = 0;
        double SolvedBySAJ = 0;
        /// SA2
        float SolvedBySA2AvgDelay = 0;
        float SolvedBySA2UnroutedK = 0;
        float SolvedBySA2MaxDelay = 0;
        double SolvedBySA2J = 0;
        /// FI
        float SolvedByFIAvgDelay = 0;
        float SolvedByFIUnroutedK = 0;
        float SolvedByFIMaxDelay = 0;
        double SolvedByFIJ = 0;
        /// SD
        float SolvedBySDAvgDelay = 0;
        float SolvedBySDUnroutedK = 0;
        float SolvedBySDMaxDelay = 0;
        double SolvedBySDJ = 0;
        /// Brute
        float SolvedByBruteAvgDelay = 0;
        float SolvedByBruteUnroutedK = 0;
        float SolvedByBruteMaxDelay = 0;
        double SolvedByBruteJ = 0;
        //srand(time(NULL));

        for (int i = 0; i < iterationsIn; i++) {
            //topo = new Topology(3, 1, 5, 1, 1, 10, 10, 30);
            //topo = new Topology(5, 1, 5, 1, 1, 10, 20, 50);
            //topo = new Topology(5, 1, (j + 1), 1, 1, 10, 20, 30);
            //topo = new Topology((j), 1, 50, 1, 1, 10, 20, 30);
            //topo = new Topology(5, 1, 50, 1, 1, 10, 20, (j));
            topo = new Topology(5, 1, 10, 1, 1, 10, 20, (j)); // para probar Brute
            //topo = new Topology(5, 1, 50, 1, 1, 10, 20, (j)); // para probar Brute
            char intStr[100];
            sprintf(intStr, "topoComp_%04d_%04d", j, i);
            string intString = string(intStr);
            topo->topologyName = intString;

            /// Physical
            cout << topo->topologyName << " Physical FloydWarshal:" << endl;
            topo->phyToLog();
            topo->floydWarshall(false, false);
            topo->saveStats(false);
            nonSolvedAvgDelay += topo->globalmaxAvgDelay;
            nonSolvedUnroutedK += topo->globalUnroutedKs;
            nonSolvedMaxDelay += topo->globalmaxMaxDelay;
            nonSolvedJ += topo->globalJ;

            /// FCP
            cout << topo->topologyName << " Blossom:" << endl;
            topo->solveByFairnessBlossom1();
            cout << topo->topologyName << " Blossom FloydWarshall:" << endl;
            topo->floydWarshall(false, false);
            topo->saveStats(false);
            SolvedByBlossAvgDelay += topo->globalmaxAvgDelay;
            SolvedByBlossUnroutedK += topo->globalUnroutedKs;
            SolvedByBlossMaxDelay += topo->globalmaxMaxDelay;
            SolvedByBlossJ += topo->globalJ;

            /// SA
            cout << topo->topologyName << " SimulatedAnnealing:" << endl;
            topo->solveBySimulatedAnnealing(writeFile, false, "AvgDelay", 3000,
                    2000, false);

            topo->floydWarshall(false, false);
            topo->saveStats(false);
            SolvedBySAAvgDelay += topo->globalmaxAvgDelay;
            SolvedBySAUnroutedK += topo->globalUnroutedKs;
            SolvedBySAMaxDelay += topo->globalmaxMaxDelay;
            SolvedBySAJ += topo->globalJ;

            /// SAwJ
            cout << topo->topologyName << " SimulatedAnnealing:" << endl;
            topo->solveBySimulatedAnnealing(writeFile, false, "AvgDelayJainI",
                    3000, 2000, false);
            topo->floydWarshall(false, false);
            topo->saveStats(false);
            SolvedBySA2AvgDelay += topo->globalmaxAvgDelay;
            SolvedBySA2UnroutedK += topo->globalUnroutedKs;
            SolvedBySA2MaxDelay += topo->globalmaxMaxDelay;
            SolvedBySA2J += topo->globalJ;

            /// FI
            cout << topo->topologyName << " FirstImprovement:" << endl;
            //topo->solveByFirstImprovement(writeFile, false, "AvgDelay", 500);
            topo->solveByFirstImprovement(writeFile, false, "AvgDelayJainI",
                    500);
            topo->floydWarshall(false, false);
            topo->saveStats(false);
            SolvedByFIAvgDelay += topo->globalmaxAvgDelay;
            SolvedByFIUnroutedK += topo->globalUnroutedKs;
            SolvedByFIMaxDelay += topo->globalmaxMaxDelay;
            SolvedByFIJ += topo->globalJ;

            /// SD
            cout << topo->topologyName << " SteppestDescent:" << endl;
            //topo->solveBySteepestDescent(writeFile, false, "AvgDelay", 500, 200);
            topo->solveBySteepestDescent(writeFile, false, "AvgDelayJainI", 500,
                    200);
            topo->floydWarshall(false, false);
            topo->saveStats(false);
            SolvedBySDAvgDelay += topo->globalmaxAvgDelay;
            SolvedBySDUnroutedK += topo->globalUnroutedKs;
            SolvedBySDMaxDelay += topo->globalmaxMaxDelay;
            SolvedBySDJ += topo->globalJ;

            /// Bute
            if (j <= BruteForceLimit) {
                topo->solveByBruteForce(false, false, "AvgDelay", false);
                cout << topo->topologyName << " BruteForce:" << endl;
                topo->floydWarshall(false, false);
                topo->saveStats(false);
                SolvedByBruteAvgDelay += topo->globalmaxAvgDelay;
                SolvedByBruteUnroutedK += topo->globalUnroutedKs;
                SolvedByBruteMaxDelay += topo->globalmaxMaxDelay;
                SolvedByBruteJ += topo->globalJ;
            }

        }

        /// Phy
        nonSolvedAvgDelay = (float) nonSolvedAvgDelay / iterationsIn;
        nonSolvedUnroutedK = (float) nonSolvedUnroutedK / iterationsIn;
        nonSolvedMaxDelay = (float) nonSolvedMaxDelay / iterationsIn;
        nonSolvedJ = nonSolvedJ / iterationsIn;
        /// FCP
        SolvedByBlossAvgDelay = (float) SolvedByBlossAvgDelay / iterationsIn;
        SolvedByBlossUnroutedK = (float) SolvedByBlossUnroutedK / iterationsIn;
        SolvedByBlossMaxDelay = (float) SolvedByBlossMaxDelay / iterationsIn;
        SolvedByBlossJ = SolvedByBlossJ / iterationsIn;
        /// SA
        SolvedBySAAvgDelay = (float) SolvedBySAAvgDelay / iterationsIn;
        SolvedBySAUnroutedK = (float) SolvedBySAUnroutedK / iterationsIn;
        SolvedBySAMaxDelay = (float) SolvedBySAMaxDelay / iterationsIn;
        SolvedBySAJ = SolvedBySAJ / iterationsIn;
        /// SA2
        SolvedBySA2AvgDelay = (float) SolvedBySA2AvgDelay / iterationsIn;
        SolvedBySA2UnroutedK = (float) SolvedBySA2UnroutedK / iterationsIn;
        SolvedBySA2MaxDelay = (float) SolvedBySA2MaxDelay / iterationsIn;
        SolvedBySA2J = SolvedBySA2J / iterationsIn;
        /// FI
        SolvedByFIAvgDelay = (float) SolvedByFIAvgDelay / iterationsIn;
        SolvedByFIUnroutedK = (float) SolvedByFIUnroutedK / iterationsIn;
        SolvedByFIMaxDelay = (float) SolvedByFIMaxDelay / iterationsIn;
        SolvedByFIJ = SolvedByFIJ / iterationsIn;
        /// SD
        SolvedBySDAvgDelay = (float) SolvedBySDAvgDelay / iterationsIn;
        SolvedBySDUnroutedK = (float) SolvedBySDUnroutedK / iterationsIn;
        SolvedBySDMaxDelay = (float) SolvedBySDMaxDelay / iterationsIn;
        SolvedBySDJ = SolvedBySDJ / iterationsIn;
        /// Brute
        SolvedByBruteAvgDelay = (float) SolvedByBruteAvgDelay / iterationsIn;
        SolvedByBruteUnroutedK = (float) SolvedByBruteUnroutedK / iterationsIn;
        SolvedByBruteMaxDelay = (float) SolvedByBruteMaxDelay / iterationsIn;
        SolvedByBruteJ = SolvedByBruteJ / iterationsIn;

        // Column 1
        manyTopologyCompFile << j << ",";
        // Column 2 to 5 - Non Solved
        manyTopologyCompFile << nonSolvedAvgDelay << "," << nonSolvedMaxDelay
                << "," << nonSolvedUnroutedK << "," << nonSolvedJ << ",";
        // Column 6 to 9 - FCP
        manyTopologyCompFile << SolvedByBlossAvgDelay << ","
                << SolvedByBlossMaxDelay << "," << SolvedByBlossUnroutedK << ","
                << SolvedByBlossJ << ",";
        // Column 10 to 13 - SA
        manyTopologyCompFile << SolvedBySAAvgDelay << "," << SolvedBySAMaxDelay
                << "," << SolvedBySAUnroutedK << "," << SolvedBySAJ << ",";
        // Column 14 to 17 - FI
        manyTopologyCompFile << SolvedByFIAvgDelay << "," << SolvedByFIMaxDelay
                << "," << SolvedByFIUnroutedK << "," << SolvedByFIJ << ",";
        // Column 18 to 21 - SD
        manyTopologyCompFile << SolvedBySDAvgDelay << "," << SolvedBySDMaxDelay
                << "," << SolvedBySDUnroutedK << "," << SolvedBySDJ << ",";
        // Column 22 to 25 - Brute
        if (j <= BruteForceLimit) {
            manyTopologyCompFile << SolvedByBruteAvgDelay << ","
                    << SolvedByBruteMaxDelay << "," << SolvedByBruteUnroutedK
                    << "," << SolvedByBruteJ << ",";
        } else {
            manyTopologyCompFile << "nan,nan,nan,nan,";
        }
        // Column 26 to 29 - SA2
        manyTopologyCompFile << SolvedBySA2AvgDelay << ","
                << SolvedBySA2MaxDelay << "," << SolvedBySA2UnroutedK << ","
                << SolvedBySA2J << ",";

        // EndLine
        manyTopologyCompFile << endl;

        // Column 1
        manyTopologyCompRelativeFile << j << ",";
        // Column 2 to 5 - Non Solved -> Non Relative-> not plotted
        manyTopologyCompRelativeFile << nonSolvedAvgDelay << ","
                << nonSolvedMaxDelay << "," << nonSolvedUnroutedK << ","
                << nonSolvedJ << ",";
        // Column 6 to 9 - Blossom
        manyTopologyCompRelativeFile
                << SolvedByBlossAvgDelay / nonSolvedAvgDelay << ","
                << SolvedByBlossMaxDelay / nonSolvedMaxDelay << ","
                << SolvedByBlossUnroutedK / nonSolvedUnroutedK << ","
                << SolvedByBlossJ / nonSolvedJ << ",";
        // Column 10 to 13 - SA
        manyTopologyCompRelativeFile << SolvedBySAAvgDelay / nonSolvedAvgDelay
                << "," << SolvedBySAMaxDelay / nonSolvedMaxDelay << ","
                << SolvedBySAUnroutedK / nonSolvedUnroutedK << ","
                << SolvedBySAJ / nonSolvedJ << ",";
        // Column 14 to 17 - FI
        manyTopologyCompRelativeFile << SolvedByFIAvgDelay / nonSolvedAvgDelay
                << "," << SolvedByFIMaxDelay / nonSolvedMaxDelay << ","
                << SolvedByFIUnroutedK / nonSolvedUnroutedK << ","
                << SolvedByFIJ / nonSolvedJ << ",";
        // Column 18 to 21 - SD
        manyTopologyCompRelativeFile << SolvedBySDAvgDelay / nonSolvedAvgDelay
                << "," << SolvedBySDMaxDelay / nonSolvedMaxDelay << ","
                << SolvedBySDUnroutedK / nonSolvedUnroutedK << ","
                << SolvedBySDJ / nonSolvedJ << ",";
        // Column 22 to 25 - Brute
        if (j <= BruteForceLimit) {
            manyTopologyCompRelativeFile
                    << SolvedByBruteAvgDelay / nonSolvedAvgDelay << ","
                    << SolvedByBruteMaxDelay / nonSolvedMaxDelay << ","
                    << SolvedByBruteUnroutedK / nonSolvedUnroutedK << ","
                    << SolvedByBruteJ / nonSolvedJ << ",";
        } else {
            manyTopologyCompRelativeFile << "nan,nan,nan,nan,";
        }
        // Column 26 to 29 - SA2
        manyTopologyCompRelativeFile << SolvedBySA2AvgDelay / nonSolvedAvgDelay
                << "," << SolvedBySA2MaxDelay / nonSolvedMaxDelay << ","
                << SolvedBySA2UnroutedK / nonSolvedUnroutedK << ","
                << SolvedBySA2J / nonSolvedJ << ",";
        // EndLine
        manyTopologyCompRelativeFile << endl;

    }

    manyTopologyCompFile.close();
    manyTopologyCompRelativeFile.close();

    // Write GnuPlot
    manyTopologyCompGFile << "set terminal svg size 900,1920 dashed" << endl;
    manyTopologyCompGFile << "set output \"manyTopo_RouteComp.svg\"" << endl;
    manyTopologyCompGFile << "set multiplot layout 4,1" << endl;
    manyTopologyCompGFile << "set key" << endl;
    manyTopologyCompGFile << "set datafile separator \",\"" << endl;
    manyTopologyCompGFile << "set xlabel \"LinkDensity\"" << endl;
    manyTopologyCompGFile
            << "set style line 1 lc rgb '#0025ad' pt 7 lt 1 lw 1 # --- blue"
            << endl;
    manyTopologyCompGFile
            << "set style line 2 lc rgb '#0025ad' pt 7 lt 3 lw 2 # --- blue"
            << endl;
    manyTopologyCompGFile
            << "set style line 3 lc rgb '#09ad00' pt 7 lt 1 lw 1 # --- green"
            << endl;
    manyTopologyCompGFile
            << "set style line 4 lc rgb '#09ad00' pt 7 lt 3 lw 2 # --- green"
            << endl;
    manyTopologyCompGFile
            << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
            << endl;
    manyTopologyCompGFile
            << "set style line 6 lc rgb '#ba55cc' pt 11 lt 1 lw 1 # --- violet"
            << endl;
    manyTopologyCompGFile
            << "set style line 20 lc rgb '#000000' pt 3 lt 15 lw 1 # dot-dash line and *"
            << endl;
    manyTopologyCompGFile
            << "set style line 21 lc rgb '#000000' pt 4 lt 1 lw 1 # line and cuadradito vacio"
            << endl;
    manyTopologyCompGFile
            << "set style line 22 lc rgb '#000000' pt 6 lt 1 lw 1 # line and circulito vacio"
            << endl;
    manyTopologyCompGFile
            << "set style line 23 lc rgb '#000000' pt 8 lt 1 lw 1 # line and triangulito vacio"
            << endl;
    manyTopologyCompGFile
            << "set style line 24 lc rgb '#000000' pt 12 lt 1 lw 1 # line and rombito vacio"
            << endl;
    manyTopologyCompGFile
            << "set style line 25 lc rgb '#000000' pt 5 lt 1 lw 1 # line and cuadradito lleno"
            << endl;
    manyTopologyCompGFile
            << "set style line 26 lc rgb '#000000' pt 7 lt 1 lw 1 # line and hexa vacio"
            << endl;

    manyTopologyCompGFile << "set style line 11 lc rgb '#808080' lt 1 " << endl;
    //fairnessWarshallGFile << "set border 3 back ls 11" << endl;
    manyTopologyCompGFile << "set tics nomirror" << endl;
    manyTopologyCompGFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
            << endl;
    manyTopologyCompGFile << "set grid back ls 12" << endl;
    manyTopologyCompGFile << "set yrange [:]" << endl;
    manyTopologyCompGFile << "set offsets graph 0, 0, 0.05, 0.05" << endl;
    //manyTopologyCompGFile << "set xrange [0:" << iterations << "]" << endl;

    // PHY        FCP, SA,    FI     SD,      Brute   SA2
    // 2-5       6-9  10-13  14-17  18-21   22-25    26-29
    // AvgDelay, MaxDelay, UnK, J
    manyTopologyCompGFile << "set ylabel \"AvgDelay\"" << endl;
    manyTopologyCompGFile
            << "plot 'manyTopo_RouteComp.csv' using 2 with linespoints ls 20 title \"PHY\","
            << "'manyTopo_RouteComp.csv' using 6 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp.csv' using 10 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp.csv' using 26 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp.csv' using 14 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp.csv' using 18 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp.csv' using 22 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompGFile << "set ylabel \"MaxDelay\"" << endl;
    manyTopologyCompGFile
            << "plot 'manyTopo_RouteComp.csv' using 3 with linespoints ls 20 title \"PHY\","
            << "'manyTopo_RouteComp.csv' using 7 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp.csv' using 11 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp.csv' using 27 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp.csv' using 15 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp.csv' using 19 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp.csv' using 23 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompGFile << "set ylabel \"Jain Index\"" << endl;
    manyTopologyCompGFile
            << "plot 'manyTopo_RouteComp.csv' using 5 with linespoints ls 20 title \"PHY\","
            << "'manyTopo_RouteComp.csv' using 9 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp.csv' using 13 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp.csv' using 29 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp.csv' using 17 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp.csv' using 21 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp.csv' using 25 with linespoints ls 25 title \"BR\""
            << endl;

    manyTopologyCompGFile << "set ylabel \"UnroutedK\"" << endl;
    manyTopologyCompGFile
            << "plot 'manyTopo_RouteComp.csv' using 4 with linespoints ls 20 title \"PHY\","
            << "'manyTopo_RouteComp.csv' using 8 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp.csv' using 12 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp.csv' using 28 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp.csv' using 16 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp.csv' using 20 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp.csv' using 24 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompGFile.close();

    // Write GnuPlot RELATIVE
    manyTopologyCompRelativeGFile << "set terminal svg size 900,1920 dashed"
            << endl;
    manyTopologyCompRelativeGFile
            << "set output \"manyTopo_RouteComp_Relative.svg\"" << endl;
    manyTopologyCompRelativeGFile << "set multiplot layout 4,1" << endl;
    manyTopologyCompRelativeGFile << "set key" << endl;
    manyTopologyCompRelativeGFile << "set datafile separator \",\"" << endl;
    manyTopologyCompRelativeGFile << "set xlabel \"LinkDensity\"" << endl;
    manyTopologyCompRelativeGFile
            << "set style line 1 lc rgb '#0025ad' pt 7 lt 1 lw 1 # --- blue"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 2 lc rgb '#0025ad' pt 7 lt 3 lw 2 # --- blue"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 3 lc rgb '#09ad00' pt 7 lt 1 lw 1 # --- green"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 4 lc rgb '#09ad00' pt 7 lt 3 lw 2 # --- green"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 6 lc rgb '#ba55cc' pt 11 lt 1 lw 1 # --- violet"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 20 lc rgb '#000000' pt 3 lt 15 lw 1 # dot-dash line and *"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 21 lc rgb '#000000' pt 4 lt 1 lw 1 # line and cuadradito vacio"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 22 lc rgb '#000000' pt 6 lt 1 lw 1 # line and circulito vacio"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 23 lc rgb '#000000' pt 8 lt 1 lw 1 # line and triangulito vacio"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 24 lc rgb '#000000' pt 12 lt 1 lw 1 # line and rombito vacio"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 25 lc rgb '#000000' pt 5 lt 1 lw 1 # line and cuadradito lleno"
            << endl;
    manyTopologyCompRelativeGFile
            << "set style line 26 lc rgb '#000000' pt 7 lt 1 lw 1 # line and hexa vacio"
            << endl;

    manyTopologyCompRelativeGFile << "set style line 11 lc rgb '#808080' lt 1 "
            << endl;
    //fairnessWarshallGFile << "set border 3 back ls 11" << endl;
    manyTopologyCompRelativeGFile << "set tics nomirror" << endl;
    manyTopologyCompRelativeGFile
            << "set style line 12 lc rgb '#808080' lt 0 lw 1" << endl;
    manyTopologyCompRelativeGFile << "set grid back ls 12" << endl;
    manyTopologyCompRelativeGFile << "set yrange [:]" << endl;
    manyTopologyCompRelativeGFile << "set offsets graph 0, 0, 0.05, 0.05"
            << endl;
    //manyTopologyCompGFile << "set xrange [0:" << iterations << "]" << endl;

    // PHY        FCP, SA,    FI     SD,      Brute  SAJ
    // 2-5       6-9  10-13  14-17  18-21   22-25    26-29
    // AvgDelay, MaxDelay, UnK, J
    manyTopologyCompRelativeGFile << "set ylabel \"AvgDelay\"" << endl;
    manyTopologyCompRelativeGFile
            << "plot 'manyTopo_RouteComp_Relative.csv' using 6 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp_Relative.csv' using 10 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp_Relative.csv' using 26 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp_Relative.csv' using 14 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp_Relative.csv' using 18 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp_Relative.csv' using 22 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompRelativeGFile << "set ylabel \"MaxDelay\"" << endl;
    manyTopologyCompRelativeGFile
            << "plot 'manyTopo_RouteComp_Relative.csv' using 7 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp_Relative.csv' using 11 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp_Relative.csv' using 27 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp_Relative.csv' using 15 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp_Relative.csv' using 19 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp_Relative.csv' using 23 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompRelativeGFile << "set ylabel \"Jain Index\"" << endl;
    manyTopologyCompRelativeGFile
            << "plot 'manyTopo_RouteComp_Relative.csv' using 9 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp_Relative.csv' using 13 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp_Relative.csv' using 29 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp_Relative.csv' using 17 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp_Relative.csv' using 21 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp_Relative.csv' using 25 with linespoints ls 25 title \"BR\""
            << endl;

    manyTopologyCompRelativeGFile << "set ylabel \"UnroutedK\"" << endl;
    manyTopologyCompRelativeGFile
            << "plot 'manyTopo_RouteComp_Relative.csv' using 8 with linespoints ls 21 title \"FCP\","
            << "'manyTopo_RouteComp_Relative.csv' using 12 with linespoints ls 22 title \"SAD\","
            << "'manyTopo_RouteComp_Relative.csv' using 28 with linespoints ls 26 title \"SAJ\","
            << "'manyTopo_RouteComp_Relative.csv' using 16 with linespoints ls 23 title \"FI\","
            << "'manyTopo_RouteComp_Relative.csv' using 20 with linespoints ls 24 title \"SD\","
            << "'manyTopo_RouteComp_Relative.csv' using 24 with linespoints ls 25 title \"BR\""
            << endl;
    manyTopologyCompRelativeGFile.close();

    // Plot: manyTopo_RouteComp.gnuplot
    string temp;
    temp.clear();
    temp += " cd out && gnuplot "; // Concatenate two Linux Commands
    temp += "manyTopo_RouteComp.gnuplot";
    system(temp.c_str());

    // Plot: manyTopo_RouteComp.gnuplot
    temp.clear();
    temp += " cd out && gnuplot "; // Concatenate two Linux Commands
    temp += "manyTopo_RouteComp_Relative.gnuplot";
    system(temp.c_str());

}

void getAverageRouteDelay(int iterations, int maxDensity, int nodes) {

    Topology *topology2;

    /// if seed != 0, seed is fixed (set iterations to 1!)
    unsigned int seed = 1;

    int maxAvgDelayB[maxDensity + 1]; // 30 density steps
    int maxMaxDelayB[maxDensity + 1];
    int maxAvgDelayL[maxDensity + 1]; // 30 density steps
    int maxMaxDelayL[maxDensity + 1];

    for (int i = 0; i < 30; i++) {
        maxAvgDelayB[i] = 0;
        maxMaxDelayB[i] = 0;
        maxAvgDelayL[i] = 0;
        maxMaxDelayL[i] = 0;
    }

    for (int i = 1; i <= iterations; i++) {
        if (seed != 0) {
            srand(seed);
        } else {
            srand(time(NULL));
        }
        for (int density = 1; density <= maxDensity; density++) {
            topology2 = new Topology(nodes, 0, 30, 1, 1, 10, 10, density);
            topology2->solveByFairnessBlossom1();
            topology2->floydWarshall(true, false);
            topology2->saveStats(false);
            maxAvgDelayB[density] += topology2->globalmaxAvgDelay;
            if (topology2->globalmaxMaxDelay > maxMaxDelayB[density]) {
                maxMaxDelayB[density] = topology2->globalmaxMaxDelay;
            }
            //maxMaxDelayB[density] += topology2->globalmaxMaxDelay;
            delete topology2;
            topology2 = new Topology(nodes, 0, 30, 1, 1, 10, 10, density);
            topology2->solveByFairnessMinMaxLP("ibm", 1, 0.1);
            topology2->floydWarshall(true, false);
            topology2->saveStats(false);
            maxAvgDelayL[density] += topology2->globalmaxAvgDelay;
            if (topology2->globalmaxMaxDelay > maxMaxDelayL[density]) {
                maxMaxDelayL[density] = topology2->globalmaxMaxDelay;
            }
            //maxMaxDelayL[density] += topology2->globalmaxMaxDelay;
            delete topology2;
        }
    }

    string averageStatsFileName = "out/AverageRouteDelay.csv";
    ofstream averageStatsFile;
    averageStatsFile.open(averageStatsFileName.c_str());

    averageStatsFile
            << "Density,maxAvgDelayBlossom,maxMaxDelayBlossom,maxAvgDelayLP,maxMaxDelayLP"
            << endl;
    for (int density = 1; density <= maxDensity; density++) {
        maxAvgDelayB[density] = maxAvgDelayB[density] / iterations;
        //maxMaxDelayB[density] = maxMaxDelayB[density] / iterations;
        maxAvgDelayL[density] = maxAvgDelayL[density] / iterations;
        //maxMaxDelayL[density] = maxMaxDelayL[density] / iterations;

        averageStatsFile << density << "," << maxAvgDelayB[density] << ","
                << maxMaxDelayB[density] << "," << maxAvgDelayL[density] << ","
                << maxMaxDelayL[density] << endl;
    }

    averageStatsFile.close();
}
