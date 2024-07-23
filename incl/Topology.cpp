// Topology Class declarations

#include "Topology.h"

/**
 * Topology Default constructor. Do not use it, create
 * the topology by using the overload constructor which
 * either parses a file or creates a random topology.
 */
Topology::Topology() {
    cout << "Please use overload constructors to create a Topology!" << endl;
    exit(1);
}

/**
 * Topology constructor that reads a topology file to
 * populate the evolving graph. The parsed topology
 * is always the physical. It also inits related
 * variables such as numSat, numET, parsedK, and so on.
 * Weights are initialized all to 1.
 *
 * @param fileName 	File to parse the topology from
 */
Topology::Topology(string fileName) {
    solved = false;
    routed = false;
    debug = true;
    topologyName = "Unnamed";

    /// Init Log topology to 0
    this->clearLogTopology();

    /// Parse DTF file.
    readTopoFromFile(fileName);

    nodeNum = numSat + numET;

    /// Boundary check:
    if ((numSat + numET) >= MAXS) {
        cout << "Too many nodes: " << (numSat + numET) << endl;
        exit(1);
    }
    if (parsedK >= MAXK) {
        cout << "Too many states: " << parsedK << endl;
        exit(1);
    }

    /// Initialize all weight to 1

    setWeights(1, 1, 1, parsedK);

    /// Initialize all maxInterfaces to 1
    for (int i = 0; i < (numSat + numET); i++) {
        maxInterfaces[i] = 1;
    }

    /// Print total Topology Object size
    cout << "Topology Size: " << sizeof(Topology) << " Bytes" << endl;
    debug = false;
}

/**
 * Topology constructor that generates a random topology
 *
 * @param newNumSat Number of Satellite
 * @param newNumET Number of ET
 * @param newParsedK Number of iterations
 * @param lowWeight Min arc weight
 * @param highWeight Max arc weight
 * @param lowTime Min time for each k
 * @param highTime Max time for each k
 * @param linkDensity set the probability (%) of a link to exist
 */
Topology::Topology(int newNumSat, int newNumET, int newParsedK, int lowWeight,
        int highWeight, int lowTime, int highTime, int linkDensity) {
    solved = false;
    routed = false;
    debug = true;
    parsedK = newParsedK;
    numSat = newNumSat;
    numET = newNumET;
    nodeNum = numSat + numET;

    /// Topology Name;
    topologyName = "Unnamed";

    /// Boundary check:
    if ((numSat + numET) >= MAXS) {
        cout << "Too many nodes: " << (numSat + numET) << endl;
        exit(1);
    }
    if (parsedK >= MAXK) {
        cout << "Too many states: " << parsedK << endl;
        exit(1);
    }

    /// Init Log topology to 0
    this->clearLogTopology();

    /// Randomize Physical links
    randomizeTopo(lowTime, highTime, linkDensity);

    /// Set Random weights
    setWeights(lowWeight, highWeight, 1, parsedK);

    /// Initialize all maxInterfaces to 1
    for (int i = 0; i < (numSat + numET); i++) {
        maxInterfaces[i] = 1;
    }

    /// Print total Topology Object size
    cout << "Topology Size: " << sizeof(Topology) << " Bytes" << endl;
    debug = false;
}

/**
 * Default Class destructor
 */
Topology::~Topology() {

}

/**
 *  Set Topology Name
 */
void Topology::setName(string name) {
    topologyName = name;
}

/**
 * Fractionate Physical Topology so that we have
 * multiple maxTime States replacing a long time
 * State (higher than maxTime). Note that Weights
 * get also updated, and kept constant among
 * spanned states.
 *
 * @param	maxTime Max time to allow an a state for all K
 */
void Topology::fractionate(int maxTime) {
    char phyTopologyTemp[MAXK][MAXS][MAXS];
    short arcWeightTemp[MAXK][MAXS][MAXS];
    int tTemp[MAXK];

    /// Copy Actual Phy to PhyTemp, also time to tTemp. We
    /// will later base our decision on them
    for (int k = 1; k < parsedK + 1; k++) {
        tTemp[k] = t[k];
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                phyTopologyTemp[k][i][j] = phyTopology[k][i][j];
                arcWeightTemp[k][i][j] = arcWeight[k][i][j];
            }
        }
    }

    /// Now we go through PhyTemp and tTemp, if tTemp[k]>maxTime, fractionate,
    /// otherwise just copy the state as it is.
    int remainingTime = 0;
    int newK = 1; ///							/// We keep the new iteration counter here
    for (int k = 1; k < parsedK + 1; k++) {
        if (tTemp[k] > maxTime) ///				/// We need to fractionate
                {
            if (debug) {
                cout << endl << "K = " << newK << " fractionated into "
                        << tTemp[k] / maxTime + 1 << " states (" << newK << "-"
                        << newK + tTemp[k] / maxTime << ")" << endl;
            }
            remainingTime = tTemp[k];
            while (remainingTime > 0) /// While remainingTime>maxTime
            {
                if (remainingTime > maxTime) /// If we still have time to partition
                        {
                    t[newK] = maxTime; /// The new K is max allowed time
                } else {
                    t[newK] = remainingTime; /// else, new K is the remaining time
                }

                if (debug) {
                    cout << t[newK] << ",";
                }

                for (int i = 0; i < (numSat + numET); i++) {
                    for (int j = 0; j < (numSat + numET); j++) {
                        phyTopology[newK][i][j] = phyTopologyTemp[k][i][j];
                        arcWeight[newK][i][j] = arcWeightTemp[k][i][j];
                    }
                }
                remainingTime -= maxTime;
                if (newK < MAXK) {
                    newK++;
                } else {
                    cout << "Too many states (k=" << newK
                            << ") while fractionating!" << endl;
                    exit(1);
                }
            }
        } else /// We dont need to fractionate, save as it is.
        {
            t[newK] = tTemp[k]; /// I fixed this!
            if (debug) {
                cout << endl << t[newK] << ",";
            }

            for (int i = 0; i < (numSat + numET); i++) {
                for (int j = 0; j < (numSat + numET); j++) {
                    phyTopology[newK][i][j] = phyTopologyTemp[k][i][j];
                    arcWeight[newK][i][j] = arcWeightTemp[k][i][j];
                }
            }
            if (newK < MAXK) {
                newK++;
            } else {
                cout << "Too many states (k=" << newK
                        << ") while fractionating!" << endl;
                exit(1);
            }
        }
    }

    /// Update parsedK to newK. (note since we update k last we have one
    /// more K)
    parsedK = newK - 1;
}

/*
 * Set a particular arc weight
 */
void Topology::setWeight(int k, int i, int j, int weight) {
    arcWeight[k][i][j] = weight;
}

/*
 * Set Topology weights in random mode from lowWeight
 * to highWeight from state k1 to k2. If lowWeight is
 * equal to highWeight the weights are all set to such
 * a value.
 * @param lowWeight is the lowest weight
 * @param highWeigh is the highest weight
 *
 */
void Topology::setWeights(int lowWeight, int highWeight, int k1, int k2) {
    if (lowWeight > highWeight) {
        cout << "HighWeight=" << highWeight << " is lower than LowWeight="
                << lowWeight << ", cant set weights." << endl;
        exit(1);
    }

    if (highWeight >= MAXW) {
        cout << "HighWeight=" << highWeight
                << " is iqual or higher than MAX Weight="
                << MAXW << ", cant set weights." << endl;
        exit(1);
    }

    if (k1 > k2) {
        cout << "State K1=" << k1 << " is higher than K2=" << k2
                << ", cant set weights." << endl;
        exit(1);
    }

    if (debug) {
        cout << "Generating Random Weights for state " << k1 << " to " << k2
                << " ranging from " << lowWeight << " to " << highWeight << "."
                << endl;
    }

    /// Initialize all weight randomly
    /// Arc Weight are symetric
    for (int k = k1; k < k2 + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) {
                arcWeight[k][i][j] = arcWeight[k][j][i] = rand()
                        % (highWeight - lowWeight + 1) + lowWeight;
            }
        }
    }
}

/**
 * Writes the Matrix form of the Physical and
 * logical topologies in a text file.
 *
 * @param fileName txt file Name
 * @param Phy if true, only save Phy
 */
void Topology::saveMatrix(string fileName, bool Phy) {
    ofstream txtFile;
    txtFile.open(fileName.c_str());

    cout << endl << "Writing Topology in " << fileName << " file" << endl;
    string str;

    for (int k = 1; k < parsedK + 1; k++) {
        /// Go through a Matrix
        for (int i = 0; i < (numSat + numET); i++) {
            /// Go through a Line
            for (int j = 0; j < (numSat + numET); j++) {
                /// Go through a Line
                if (phyTopology != NULL) {
                    if (phyTopology[k][i][j] == -1) {
                        txtFile << "*,";
                    } else {
                        txtFile << (int) phyTopology[k][i][j] << ",";
                    }
                }
            } ///Done with this Row

            if (!Phy) { /// Also save Logical in the file
                txtFile << "\t";
                /// Go through a Line of Logic Topology
                for (int j = 0; j < (numSat + numET); j++) {
                    if (logTopology != NULL) {
                        if (logTopology[k][i][j] == -1) {
                            txtFile << "*,";
                        } else {
                            txtFile << (int) logTopology[k][i][j] << ",";
                        }
                    }
                } ///Done with this Row

            }
            txtFile << "\n";
        }
        /// Done with this state, go to the next
        txtFile << "k=" << k << endl;
        txtFile << "t=" << t[k] << endl << endl;
    }

//    if (!Phy) { /// If only Phy do not write extra info
//        // Print priorities:
//        txtFile << endl << "Arc Weights at final k (" << parsedK << ")" << endl;
//        for (int i = 0; i < (numSat + numET); i++) {
//            for (int j = 0; j < (numSat + numET); j++) {
//                txtFile << arcWeight[parsedK][i][j] << ",";
//            }
//            txtFile << endl;
//        }
//
//        // Print node maxInterfaces
//        txtFile << endl << "Node Max Interfaces:" << endl;
//        for (int i = 0; i < (numSat + numET); i++) {
//            txtFile << maxInterfaces[i] << ",";
//            //printf("%d,", maxInterfaces[i]);
//        }
//    }

    txtFile << endl;
    //printf("\n");

    txtFile.close();
}

/**
 * Write the topology in a pdf graph using DOT
 * library. The graph has the physical topology
 * arcs in dashed form, and the logical
 * topologies in solid color.
 *
 * @param fileName PDF file name to write
 */
void Topology::saveDOT() {

    ofstream dotFile;
    dotFile.open("tmp/topology.dot");

    /// Destiny pdf fileName:
    string fileName = "out/";
    fileName += topologyName;
    fileName += "_graph.pdf";

    if (debug) {
        cout << endl << "Writing Topology in " << fileName << " file " << endl;
    }

    if ((parsedK > 50) || ((numSat + numET) > 10)) {
        cout
                << "Warning, too many nodes/states, may take some minutes to complete!..."
                << endl;
    }

    dotFile << "//Archivo que contiene las directivas dot para grafo\n";
    dotFile << "//Comand: dot -Tpdf graph.dot -o graph.pdf\n";
    dotFile << "digraph G {\n";
    dotFile << "rank=same;\nranksep=equally;\nnodesep=equally;\n"; /// \nnode [shape=plaintext];
    dotFile << "label= \"\\nDotted = Physical Links \\n"
            << "Black = Logical Links Enabled by " << topologyName << "\"";

    for (int k = 1; k < parsedK + 1; k++) {
        /// Template header for this state:
        dotFile << "\n//Current k=" << k << "\n";
        /// We create all invisible links so we set the position
        for (int i = 0; i < (numSat + numET - 1); i++) {
            dotFile << i << "." << k << " -> " << (i + 1) << "." << k
                    << " [style=\"invis\"];\n";
        }
        /// K State and T time Edge:
        dotFile << (numSat + numET - 1) << "." << k << " -> "
                << (numSat + numET) << "." << k << " [style=\"invis\"];\n";

        /// Now we set all Nodes Label
        for (int i = 0; i < numET; i++) {
            dotFile << i << "." << k << " [label=ET" << i << "];\n";
        }
        for (int i = numET; i < (numSat + numET); i++) {
            dotFile << i << "." << k << " [label=L" << i << "];\n";
        }
        /// K State and T time Node Properties (Label):
        dotFile << (numSat + numET) << "." << k
                << " [shape=none,fontsize=14,label=\"k:" << k << "\\nt:" << t[k]
                << "\"];\n";
        /// Go through a Matrix
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) {
                if ((phyTopology[k][i][j] == 1)
                        && (logTopology[k][i][j] == 0)) { /// If Phy on and Log Off, then write dotted arc
                    if (solved) {

                        dotFile
                                << i
                                << "."
                                << k
                                << " -> "
                                << j
                                << "."
                                << k
                                << " [style=\"dotted\",dir=both,arrowhead=none,arrowtail=none,fontcolor=grey,label="
                                << arcWeight[k][i][j] << "];\n";
                    } else {
                        dotFile << i << "." << k << " -> " << j << "." << k
                                << " [dir=both,fontcolor=grey,label="
                                << arcWeight[k][i][j] << "];\n";
                    }
                }
                /// If Phy on and Log on, then write solid arc
                if ((phyTopology[k][i][j] == 1)
                        && (logTopology[k][i][j] == 1)) {
                    dotFile << i << "." << k << " -> " << j << "." << k
                            << " [dir=both,color=black,fontcolor=grey,label="
                            << arcWeight[k][i][j] << ",penwidth=2];\n";
                }
            } /// Done with this row
        } /// Done with this state, go to next
    }

    /// Close the file
    dotFile << "\n}\n";
    dotFile.close();

    /// Generate Pdf file
    string temp;
    temp += "dot -Tpdf tmp/topology.dot -o ";
    temp += +fileName.c_str();

    system(temp.c_str());

}

/**
 * Function to write the topology in a ION compatible
 * contact text file. The format is:
 *
 * a contact +1 +300 1 2 100000
 *
 * Where +1 to +300 is the relative time the contact starts,
 * 1 and 2 are source and node destiny
 * 100000 is the contact bandwith
 *
 * @param fileName indicates the file to write to
 * @param topoNumber the topology to write (0=phy, 1 and 2=logical)
 */
void Topology::saveION() {

    /// Destiny txt fileName:
    string fileName = "out/";
    fileName += topologyName;
    fileName += "_ion.txt";

    ofstream ionFile;
    ionFile.open(fileName.c_str());

    /// Generate Contacts from topoNumber topology
    generateContacts();

    /// Now write them
    cout << endl << "Writing Topology ION Contacts in " << fileName << " file"
            << endl;

    ionFile << "# ION Contact file for " << this->topologyName
            << " topology file" << endl;
    ionFile << "# with " << (numSat) << " LEO satellites and " << numET
            << " ET in " << parsedK << " topological states" << endl;
    ionFile << "# expressed in " << contact << " contacts for a total time of "
            << tAccum << " seconds." << endl;

    /// Write contactArray in ionFile:
    ionFile << endl << "# Add Contacts:" << endl;
    for (int c = 0; c < contact; c++) {
        ionFile << "a contact +" << contactArray[c][0] << " +"
                << contactArray[c][1] << " " << contactArray[c][2] << " "
                << contactArray[c][3] << " 100000" << endl; /// Write contact
        ionFile << "a contact +" << contactArray[c][0] << " +"
                << contactArray[c][1] << " " << contactArray[c][3] << " "
                << contactArray[c][2] << " 100000" << endl; /// Write contact
    }

    /// Evaluate i to j for range output and write ionFile:
    ionFile << endl << "# Add Ranges:" << endl;
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = i + 1; j < (numSat + numET); j++) {
            ionFile << "a range +0 +" << tAccum << " " << i << " " << j << " 1"
                    << endl;
        }
    }

    ionFile.close();
}

/**
 * Function to write the topology in a ION compatible
 * contact text file. The format is:
 *
 * a contact +1 +300 1 2 100000
 *
 * Where +1 to +300 is the relative time the contact starts,
 * 1 and 2 are source and node destiny
 * 100000 is the contact bandwith
 *
 * @param fileName indicates the file to write to
 * @param topoNumber the topology to write (0=phy, 1 and 2=logical)
 */
void Topology::saveNSIM() {

    /// Destiny txt fileName:
    string fileName = "out/";
    fileName += topologyName;
    fileName += "_netsim.txt";

    ofstream netFile;
    netFile.open(fileName.c_str());

    /// Generate Contacts from topoNumber topology
    generateContacts();

    /// Now write them
    cout << endl << "Writing Topology NetSim Contacts in " << fileName
            << " file" << endl;

    netFile
            << "# Archivo que contiene las ventanas para cada segmento del sistema."
            << endl;
    netFile << "# Formato de los Datos:" << endl;
    netFile << "# id	SegmA	SegmB	Tiempo_de_inicio(seg)	Duracion_ventana(seg)"
            << endl << endl;

    /// Write contactArray in netFile:
    /// 1	LEO1	LEO2	1400	5
    /// All ET are modified with the +1 factor to start from 1..numET
    /// All LEOs are modified with the -numET+1 factor to start from numET+1...
    /// as the first leo
    for (int c = 0; c < contact; c++) {
        /// We add 1 sec to start time if it is 0 for NetSim
        if (contactArray[c][0] == 0) {
            contactArray[c][0]++;
        }

        if (contactArray[c][2] < numET) { /// Source=ET
            netFile << c + 1 << '\t' << "ET" << contactArray[c][2] + 1 << '\t'
                    << "LEO" << contactArray[c][3] - numET + 1 << '\t'
                    << contactArray[c][0] << '\t'
                    << (contactArray[c][1] - contactArray[c][0]) << endl;
        } else if (contactArray[c][3] < numET) { /// Dest=ET
            netFile << c + 1 << '\t' << "LEO" << contactArray[c][2] - numET + 1
                    << '\t' << "ET" << contactArray[c][3] + 1 << '\t'
                    << contactArray[c][0] << '\t'
                    << (contactArray[c][1] - contactArray[c][0]) << endl;
        } else { /// All LEOs
            netFile << c + 1 << '\t' << "LEO" << contactArray[c][2] - numET + 1
                    << '\t' << "LEO" << contactArray[c][3] - numET + 1 << '\t'
                    << contactArray[c][0] << '\t'
                    << (contactArray[c][1] - contactArray[c][0]) << endl;
        }
    }

    netFile.close();
}

/**
 * Save Topology Stats.
 */
void Topology::saveStats(bool writeFile) {

    ofstream statsFile;
    ofstream totalTimeFile;
    ofstream arcDistrFile;
    ofstream arcDistrGPlotFile;
    ofstream arcDistr3DFile;
    ofstream arcDistr3DGPlotFile;
    ofstream kFairRatioFile;
    ofstream kFairRatioGPlotFile;
    ofstream routeFWDelayFile;
    ofstream routeFWDelayGPlotFile;

    if (writeFile) {
        /// Destiny stats txt fileName:
        string statsFileName = "out/";
        statsFileName += topologyName;
        statsFileName += "_stats.txt";

        statsFile.open(statsFileName.c_str());

        /// Destiny total arc Time csv fileName:
        string totalTimeFileName = "out/";
        totalTimeFileName += topologyName;
        totalTimeFileName += "_totalTime.csv";

        totalTimeFile.open(totalTimeFileName.c_str());

        /// Destiny arc distribution csv fileName:
        string arcDistrFileName = "out/";
        arcDistrFileName += topologyName;
        arcDistrFileName += "_arcDistr.csv";

        arcDistrFile.open(arcDistrFileName.c_str());

        /// Destiny arc distribution gnuplot fileName:
        string arcDistrGPlotFileName = "out/";
        arcDistrGPlotFileName += topologyName;
        arcDistrGPlotFileName += "_arcDistr.gnuplot";

        arcDistrGPlotFile.open(arcDistrGPlotFileName.c_str());

        /// Destiny 3D arc distribution csv fileName:
        string arcDistr3DFileName = "out/";
        arcDistr3DFileName += topologyName;
        arcDistr3DFileName += "_arcDistr3D.csv";

        arcDistr3DFile.open(arcDistr3DFileName.c_str());

        /// Destiny 3D arc distribution gnuplot fileName:
        string arcDistr3DGPlotFileName = "out/";
        arcDistr3DGPlotFileName += topologyName;
        arcDistr3DGPlotFileName += "_arcDistr3D.gnuplot";

        arcDistr3DGPlotFile.open(arcDistr3DGPlotFileName.c_str());

        /// Destiny K Fairness Ratio csv fileName:
        string kFairRatioFileName = "out/";
        kFairRatioFileName += topologyName;
        kFairRatioFileName += "_kFairRatio.csv";

        kFairRatioFile.open(kFairRatioFileName.c_str());

        /// Destiny K Fairness Ratio gnuplot fileName:
        string kFairRatioGPlotFileName = "out/";
        kFairRatioGPlotFileName += topologyName;
        kFairRatioGPlotFileName += "_kFairRatio.gnuplot";

        kFairRatioGPlotFile.open(kFairRatioGPlotFileName.c_str());

        /// routeFWDelay csv fileName:
        string routeFWDelayFileName = "out/";
        routeFWDelayFileName += topologyName;
        routeFWDelayFileName += "_routeFWDelay.csv";

        routeFWDelayFile.open(routeFWDelayFileName.c_str());

        /// routeFWDelay gnuplot fileName:
        string routeFWDelayGPlotFileName = "out/";
        routeFWDelayGPlotFileName += topologyName;
        routeFWDelayGPlotFileName += "_routeFWDelay.gnuplot";

        routeFWDelayGPlotFile.open(routeFWDelayGPlotFileName.c_str());
    }

    /// temp string for system calls:
    string temp;

    /// Total enabled Arc Weight throughout all K
    int totalArcOnWeight = 0;
    /// Total Arc Number (physical) throughout all K
    int totalArc = 0;
    /// Total enabled Arc Number throughout all K
    int totalArcOn = 0;
    /// Total enabled Arc Time throughout all K
    int totalArcOnTime = 0;
    /// Total enabled Arc Time for a given K
    int totalArcOnTimeK[MAXK];

    /// Accumulated enabled Arc Weight throughout all K
    int arcOnWeight[MAXS][MAXS];
    /// Accumulated Arc On Contact Time throughout all K
    int arcOnTime[MAXS][MAXS];
    /// Accumulated Arc Off Contact Time throughout all K
    int arcOffTime[MAXS][MAXS];
    /// Accumulated Arc Total Physical Time throughout all K
    int arcPhyTime[MAXS][MAXS];

    /// Raj Jain Fairness Index
    double J[MAXK];

    /// Minimal Arc On Contact Time throughout all K
    int minArcOnTime = 2000000000; // int range: 2,147,483,647
    /// Minimal Arc On Contact throughout all K
    stringstream minArcOn;
    /// Maximal Arc On Contact Time throughout all K
    int maxArcOnTime = 0;
    /// Maximal Arc On Contact throughout all K
    stringstream maxArcOn;
    /// MinMax Time Ratio
    double minMaxTimeRatio[MAXK];

    /// Init all arrays
    for (int k = 1; k < parsedK + 1; k++) {
        J[k] = 0;
        minMaxTimeRatio[k] = 0;
    }
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            arcOnWeight[i][j] = 0;
            arcOnTime[i][j] = 0;
            arcOffTime[i][j] = 0;
            arcPhyTime[i][j] = 0;
        }
    }

    /// Go through topology and accumulate values
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] >= 1) {
                    arcPhyTime[i][j] += t[k];
                    totalArc++;
                    if (logTopology[k][i][j] >= 1) {
                        arcOnTime[i][j] += t[k];
                        totalArcOnTime += t[k];
                        totalArcOnTimeK[k] += t[k];
                        arcOnWeight[i][j] += arcWeight[k][i][j];
                        totalArcOnWeight += arcWeight[k][i][j];
                        totalArcOn++;
                    } else {
                        arcOffTime[i][j] += t[k];
                    }
                }
            }
        }
        /// Per state calculations:

        /// Raj Jain Fairness Index calculation (Ontime distribution)
        double J_Up = 0, J_Dw = 0;
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                J_Up += arcOnTime[i][j];
                J_Dw += arcOnTime[i][j] * arcOnTime[i][j];
            }
        }
        J_Up = J_Up * J_Up;
        J_Dw = J_Dw * (nodeNum * nodeNum);
        J[k] = J_Up / J_Dw;

        /// Max and Min Time Arc values calculation
        maxArcOnTime = 0;
        minArcOnTime = 2000000000;
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if ((arcPhyTime[i][j] > 0)
                        && (arcOnTime[i][j] > maxArcOnTime)) {
                    maxArcOnTime = arcOnTime[i][j];
                    maxArcOn.str("");
                    maxArcOn << i << "-" << j;
                }
                if ((arcPhyTime[i][j] > 0)
                        && (arcOnTime[i][j] < minArcOnTime)) {
                    minArcOnTime = arcOnTime[i][j];
                    minArcOn.str("");
                    minArcOn << i << "-" << j;
                }
            }
        }
        /// Calculate and Store MinMax Time Ratio
        minMaxTimeRatio[k] = (double) minArcOnTime / (double) maxArcOnTime;
    }

    //////////////////////////////////
    /// Write Total Arc Time File
    //////////////////////////////////

    if (writeFile) {
        for (int k = 1; k < parsedK + 1; k++) {
            totalTimeFile << k << "," << totalArcOnTimeK[k] << endl;
        }
        /// Close File
        totalTimeFile.close();
    }

    //////////////////////////////////
    /// Write Arc Distribution File
    //////////////////////////////////
    int linealArcOnTime[1 + (nodeNum * nodeNum - nodeNum) / 2];
    int linealArcOffTime[1 + (nodeNum * nodeNum - nodeNum) / 2];
    int linealArcPhyTime[1 + (nodeNum * nodeNum - nodeNum) / 2];
    stringstream arcs[1 + (nodeNum * nodeNum - nodeNum) / 2];
    int l = 0;
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = i + 1; j < (numSat + numET); j++) {
            linealArcOnTime[l] = arcOnTime[i][j];
            linealArcOffTime[l] = arcOffTime[i][j];
            linealArcPhyTime[l] = arcPhyTime[i][j];
            arcs[l] << i << "-" << j;
            l++;
        }
    }
    /// Sort the values by arcPhyTime
    bool swapped = true;
    int tmp;
    stringstream stmp;
    /// First Bubble Sort for end Time
    while (swapped == true) {
        swapped = false;
        for (int l = 1; l < (nodeNum * nodeNum - nodeNum) / 2; l++) {
            if (linealArcPhyTime[l - 1] < linealArcPhyTime[l]) {
                ///Swap
                tmp = linealArcPhyTime[l];
                linealArcPhyTime[l] = linealArcPhyTime[l - 1];
                linealArcPhyTime[l - 1] = tmp;
                tmp = linealArcOffTime[l];
                linealArcOffTime[l] = linealArcOffTime[l - 1];
                linealArcOffTime[l - 1] = tmp;
                tmp = linealArcOnTime[l];
                linealArcOnTime[l] = linealArcOnTime[l - 1];
                linealArcOnTime[l - 1] = tmp;
                stmp.str("");
                stmp << arcs[l].rdbuf();
                arcs[l].str("");
                arcs[l] << arcs[l - 1].rdbuf();
                arcs[l - 1].str("");
                arcs[l - 1] << stmp.rdbuf();
                swapped = true;
            }
        }
    }
    /// Write the values Ordered in Arc Distribution File
    if (writeFile) {
        for (int l = 0; l < (nodeNum * nodeNum - nodeNum) / 2; l++) {
            arcDistrFile << arcs[l].str() << "," << linealArcOnTime[l] << ","
                    << linealArcOffTime[l] << "," << linealArcPhyTime[l]
                    << endl;
        }
    }

    if (writeFile) {
        /// Close Arc Distribution File
        arcDistrFile.close();
        /// Write Gnuplot File
        arcDistrGPlotFile << "set terminal svg size 1280,480" << endl;
        arcDistrGPlotFile << "set output \"" << topologyName
                << "_arcDistr.svg\"" << endl;
        arcDistrGPlotFile << "set key" << endl;
        arcDistrGPlotFile << "set datafile separator \",\"" << endl;
        arcDistrGPlotFile << "set xlabel \"Arc (i-j)\"" << endl;
        arcDistrGPlotFile << "set ylabel \"Time\"" << endl;
        arcDistrGPlotFile
                << "set style line 1 lc rgb '#0025ad' pt 5 lt 1 lw 2 # --- blue"
                << endl;
        arcDistrGPlotFile
                << "set style line 2 lc rgb '#00ad88' pt 7 lt 1 lw 2 # --- cyan"
                << endl;
        arcDistrGPlotFile
                << "set style line 3 lc rgb '#09ad00' pt 9 lt 1 lw 2 # --- green"
                << endl;
        arcDistrGPlotFile
                << "set style line 4 lc rgb '#ba55cc' pt 13 lt 1 lw 2 # --- violet"
                << endl;
        arcDistrGPlotFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        arcDistrGPlotFile << "set style line 11 lc rgb '#808080' lt 1 " << endl;
        arcDistrGPlotFile << "set border 3 back ls 11" << endl;
        arcDistrGPlotFile << "set tics nomirror" << endl;
        arcDistrGPlotFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        arcDistrGPlotFile << "set grid back ls 12" << endl;
        arcDistrGPlotFile << "set style data histogram" << endl;
        arcDistrGPlotFile << "set style fill solid" << endl;
        arcDistrGPlotFile << "set boxwidth 0.9" << endl;
        arcDistrGPlotFile << "set yrange [0:]" << endl;
        arcDistrGPlotFile << "set xtics rotate by -90 left" << endl;
        arcDistrGPlotFile << "" << endl;
        arcDistrGPlotFile
                << "plot '"
                << topologyName
                << "_arcDistr.csv' using 2:xticlabels(1) ls 1 title \""
                << topologyName
                << "\", '"
                << topologyName
                << "_arcDistr.csv' using 4:xticlabels(1) with points ls 2 title \""
                << topologyName << "_PhyTime\"" << endl;
        /// Close GnuPlot File
        arcDistrGPlotFile.close();
        /// Plot Graph
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_arcDistr.gnuplot";
        system(temp.c_str());
    }

    //////////////////////////////////
    /// Write 3D Arc Distribution File
    //////////////////////////////////
    int linealOnContactTime[1 + (nodeNum * nodeNum - nodeNum) / 2][parsedK + 1];
    int linealMaxContactTime[1 + (nodeNum * nodeNum - nodeNum) / 2][parsedK + 1];
    stringstream arcs3d[1 + (nodeNum * nodeNum - nodeNum) / 2];
    /// Init arrays
    for (int k = 1; k < parsedK + 1; k++) {
        int l = 0;
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i + 1; j < (numSat + numET); j++) {
                linealOnContactTime[l][k] = 0;
                linealMaxContactTime[l][k] = 0;
                if (k == 1) {
                    arcs3d[l] << i << "-" << j;
                }
                l++;
            }
        }
    }
    /// Go thorugh the Topology to Evaluate
    for (int k = 1; k < parsedK + 1; k++) {
        int l = 0;
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i + 1; j < (numSat + numET); j++) {
                /// Lineal part update:
                if (k == 1) {
                    linealMaxContactTime[l][k] = t[k];
                    linealOnContactTime[l][k] = t[k];
                } else {
                    linealMaxContactTime[l][k] = linealMaxContactTime[l][k - 1];
                    linealOnContactTime[l][k] = linealOnContactTime[l][k - 1];
                }
                if (phyTopology[k][i][j] >= 1 && logTopology[k][i][j] == 1) {
                    linealOnContactTime[l][k] += t[k];
                }
                if (phyTopology[k][i][j] >= 1) {
                    linealMaxContactTime[l][k] += t[k];
                }
                l++;
            }
        }
    }
    /// Sort the values by maxContactTime
    swapped = true;
    int tmp3d[parsedK + 1];
    /// First Bubble Sort for end Time
    while (swapped == true) {
        swapped = false;
        for (int l = 1; l < (nodeNum * nodeNum - nodeNum) / 2; l++) {
            /// Sort in ascending order of latest K max time
            if (linealMaxContactTime[l - 1][parsedK]
                    < linealMaxContactTime[l][parsedK]) {
                ///Swap lineal Max
                for (int k = 1; k < parsedK + 1; k++) {
                    tmp3d[k] = linealMaxContactTime[l][k];
                }
                for (int k = 1; k < parsedK + 1; k++) {
                    linealMaxContactTime[l][k] = linealMaxContactTime[l - 1][k];
                }
                for (int k = 1; k < parsedK + 1; k++) {
                    linealMaxContactTime[l - 1][k] = tmp3d[k];
                }
                ///Swap lineal On Contact
                for (int k = 1; k < parsedK + 1; k++) {
                    tmp3d[k] = linealOnContactTime[l][k];
                }
                for (int k = 1; k < parsedK + 1; k++) {
                    linealOnContactTime[l][k] = linealOnContactTime[l - 1][k];
                }
                for (int k = 1; k < parsedK + 1; k++) {
                    linealOnContactTime[l - 1][k] = tmp3d[k];
                }
                stmp.str("");
                stmp << arcs3d[l].rdbuf();
                arcs3d[l].str("");
                arcs3d[l] << arcs3d[l - 1].rdbuf();
                arcs3d[l - 1].str("");
                arcs3d[l - 1] << stmp.rdbuf();
                swapped = true;
            }
        }
    }
    /// Write 3D Arc Distribution File
    /// Write the values Ordered
    if (writeFile) {
        for (int l = 0; l < (nodeNum * nodeNum - nodeNum) / 2; l++) {
            for (int k = 1; k < parsedK + 1; k++) {
                arcDistr3DFile << l << "," << arcs3d[l].str() << "," << k << ","
                        << linealOnContactTime[l][k] << ","
                        << linealMaxContactTime[l][k] << endl;
            }
            arcDistr3DFile << endl;
        }
        /// Close File
        arcDistr3DFile.close();
        /// Write Gnuplot File
        arcDistr3DGPlotFile << "set terminal svg size 1500,1000" << endl;
        arcDistr3DGPlotFile << "set output \"" << topologyName
                << "_arcDistr3D.svg\"" << endl;
        arcDistr3DGPlotFile << "set key" << endl;
        arcDistr3DGPlotFile << "set datafile separator \",\"" << endl;
        arcDistr3DGPlotFile << "set xlabel \"Arc (i-j)\"" << endl;
        arcDistr3DGPlotFile << "set ylabel \"K State\"" << endl;
        arcDistr3DGPlotFile << "set zlabel \"Time\"" << endl;
        arcDistr3DGPlotFile << "set view 29,53" << endl;
        arcDistr3DGPlotFile << "set hidden3d" << endl;
        arcDistr3DGPlotFile << "set xyplane 0" << endl;
        arcDistr3DGPlotFile
                << "splot '"
                << topologyName
                << "_arcDistr3D.csv' using 1:3:4:xticlabels(2) with pm3d title \""
                << topologyName << "\"" << endl;
        /// Close GnuPlot File
        arcDistr3DGPlotFile.close();
        /// Plot Graph
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_arcDistr3D.gnuplot";
        system(temp.c_str());
    }

    //////////////////////////////////
    /// Write Fairness Ratio Evolution File
    //////////////////////////////////
    if (writeFile) {
        for (int k = 1; k < parsedK + 1; k++) {
            kFairRatioFile << k << "," << J[k] << "," << minMaxTimeRatio[k]
                    << endl;
        }
        /// Close csv file
        kFairRatioFile.close();
        /// Write gnuplot file
        kFairRatioGPlotFile << "set terminal svg size 1280,480" << endl;
        kFairRatioGPlotFile << "set output \"" << topologyName
                << "_kFairRatio.svg\"" << endl;
        kFairRatioGPlotFile << "set key left top" << endl;
        kFairRatioGPlotFile << "set datafile separator \",\"" << endl;
        kFairRatioGPlotFile << "set xlabel \"State [k]\"" << endl;
        kFairRatioGPlotFile << "set ylabel \"Ratio\"" << endl;
        kFairRatioGPlotFile
                << "set style line 1 lc rgb '#0025ad' pt 5 lt 1 lw 2 # --- blue"
                << endl;
        kFairRatioGPlotFile
                << "set style line 2 lc rgb '#00ad88' pt 7 lt 1 lw 2 # --- cyan"
                << endl;
        kFairRatioGPlotFile
                << "set style line 3 lc rgb '#09ad00' pt 9 lt 1 lw 2 # --- green"
                << endl;
        kFairRatioGPlotFile
                << "set style line 4 lc rgb '#ba55cc' pt 13 lt 1 lw 2 # --- violet"
                << endl;
        kFairRatioGPlotFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        kFairRatioGPlotFile << "set style line 11 lc rgb '#808080' lt 1 "
                << endl;
        kFairRatioGPlotFile << "set border 3 back ls 11" << endl;
        kFairRatioGPlotFile << "set tics nomirror" << endl;
        kFairRatioGPlotFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        kFairRatioGPlotFile << "set grid back ls 12" << endl;
        kFairRatioGPlotFile << "set xtics rotate by -90 left" << endl;
        kFairRatioGPlotFile << "set yrange [0:]" << endl;
        kFairRatioGPlotFile
                << "plot '"
                << topologyName
                << "_kFairRatio.csv' using 2:xticlabels(1) with linespoints ls 1 title \""
                << topologyName
                << "_Raj_Jain\", '"
                << topologyName
                << "_kFairRatio.csv' using 3:xticlabels(1) with linespoints ls 2 title \""
                << topologyName << "_MinMax\"" << endl;
        /// Close gnuplot file
        kFairRatioGPlotFile.close();
        /// Plot gnuplot
        if (solved) {
            temp.clear();
            temp += " cd out && gnuplot "; // Concatenate two Linux Commands
            temp += topologyName.c_str();
            temp += "_kFairRatio.gnuplot";
            system(temp.c_str());
        }
    }

    //////////////////////////////////
    /// Save in Route File
    //////////////////////////////////

    int maxDelay[nodeNum][nodeNum];
    int maxMaxDelay = 0;
    int minDelay[nodeNum][nodeNum];
    int avgDelay[nodeNum][nodeNum];
    int maxAvgDelay = 0;
    int unRoutedKs[nodeNum][nodeNum];
    /// Init arrays;
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            maxDelay[i][j] = 0;
            minDelay[i][j] = INF;
            avgDelay[i][j] = 0;
            unRoutedKs[i][j] = 0;
        }
    }
    /// Evaluate Routes:
    this->globalUnroutedKs = 0;
    if (routed) { // If I routed
        for (int k = 1; k < parsedK + 1; k++) {
            for (int i = 0; i < nodeNum; i++) {
                for (int j = 0; j < nodeNum; j++) {
                    if (distance[k][i][j] >= INF) { // no-route here
                        unRoutedKs[i][j]++;
                        this->globalUnroutedKs++;
                    }

                    if (distance[k][i][j] > maxDelay[i][j]) {
                        if (distance[k][i][j] >= INF) { // no-route here

                        } else {
                            maxDelay[i][j] = distance[k][i][j];
                            if (maxDelay[i][j] > maxMaxDelay) {
                                maxMaxDelay = maxDelay[i][j];
                            }
                        }
                    }
                    if (distance[k][i][j] < minDelay[i][j]) {
                        minDelay[i][j] = distance[k][i][j];
                    }
                    if (distance[k][i][j] < INF) {
                        avgDelay[i][j] += distance[k][i][j];
                    }
                }
            }
        }
    }

    // Calculate average:
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i != j) {
                if (unRoutedKs[i][j] >= parsedK) {
                    avgDelay[i][j] = INF;
                } else {
                    avgDelay[i][j] = avgDelay[i][j]
                            / (parsedK - unRoutedKs[i][j]);
                }
                if (writeFile && avgDelay[i][j] < INF) {
                    routeFWDelayFile << i << "-" << j << "," << unRoutedKs[i][j]
                            << "," << avgDelay[i][j] << "," << minDelay[i][j]
                            << "," << maxDelay[i][j] << endl;
                }
                if (avgDelay[i][j] > maxAvgDelay && avgDelay[i][j] < INF) {
                    maxAvgDelay = avgDelay[i][j];
                }
            }
        }
    }

    // Close csv file
    if (writeFile) {
        routeFWDelayFile.close();

        routeFWDelayGPlotFile << "set terminal svg size 1280,480" << endl;
        routeFWDelayGPlotFile << "set output \"" << topologyName
                << "_routeFWDelay.svg\"" << endl;
        routeFWDelayGPlotFile << "set key left top" << endl;
        routeFWDelayGPlotFile << "set datafile separator \",\"" << endl;
        routeFWDelayGPlotFile << "set xlabel \"Arc [i-j]\"" << endl;
        routeFWDelayGPlotFile << "set ylabel \"Delay\"" << endl;
        routeFWDelayGPlotFile
                << "set style line 1 lc rgb '#0025ad' pt 5 lt 1 lw 2 # --- blue"
                << endl;
        routeFWDelayGPlotFile
                << "set style line 2 lc rgb '#00ad88' pt 7 lt 1 lw 2 # --- cyan"
                << endl;
        routeFWDelayGPlotFile
                << "set style line 3 lc rgb '#09ad00' pt 9 lt 1 lw 2 # --- green"
                << endl;
        routeFWDelayGPlotFile
                << "set style line 4 lc rgb '#ba55cc' pt 13 lt 1 lw 2 # --- violet"
                << endl;
        routeFWDelayGPlotFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        routeFWDelayGPlotFile << "set style line 11 lc rgb '#808080' lt 1 "
                << endl;
        routeFWDelayGPlotFile << "set border 3 back ls 11" << endl;
        routeFWDelayGPlotFile << "set tics nomirror" << endl;
        routeFWDelayGPlotFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        routeFWDelayGPlotFile << "set grid back ls 12" << endl;
        routeFWDelayGPlotFile << "set xtics rotate by -90 left" << endl;
        //routeFWDelayGPlotFile << "set offsets graph 0.0 0.0, 1, 0" << endl;
        //routeFWDelayGPlotFile << "set yrange [0:6000]" << endl;
        routeFWDelayGPlotFile
                << "plot '"
                << topologyName
                << "_routeFWDelay.csv' using 0:3:4:5:xticlabels(1) with yerrorbars ls 1 title \""
                << topologyName << "_FW_RouteDelay\", '";
        routeFWDelayGPlotFile
                << topologyName
                << "_routeFWDelay.csv' using 0:5:($2) with labels offset 0,1 notitle "
                << endl;
        // Close GnuPlot File
        routeFWDelayGPlotFile.close();
        // Plot the graph:
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_routeFWDelay.gnuplot";
        system(temp.c_str());
    }

    //////////////////////////////////
    /// Save in Statistic File
    //////////////////////////////////
    if (writeFile) {
        statsFile << "Statistic File for " << topologyName << " Topology"
                << endl;
        statsFile << endl;
        statsFile << "Physical:" << endl;
        statsFile << "--------------------------------------------------"
                << endl;
        statsFile << "Topology Object Size: " << sizeof(Topology) << " Bytes"
                << endl;
        statsFile << "Topology Nodes: " << numET << " ET + " << numSat
                << " Sat = " << nodeNum << " Nodes" << endl;
        statsFile << "Topology States: " << parsedK << endl;
        statsFile
                << "Topology Average Arc Density: "
                << 100
                        * (((float) totalArc)
                                / (float) (nodeNum * (nodeNum - 1) * parsedK))
                << "%" << endl;
        statsFile << "Topology Solved: " << solved << endl;

        statsFile << endl;
        statsFile << "Logical:" << endl;
        statsFile << "--------------------------------------------------"
                << endl;
        statsFile << "Total Enabled Arc Quantity: " << totalArcOn << endl;
        statsFile << "Total Enabled Arc Time: " << totalArcOnTime << endl;
        statsFile << "Total Enabled Arc Weight: " << totalArcOnWeight << endl;
        statsFile << endl;
        statsFile << "Minimum Enabled Arc: " << minArcOn.str() << endl;
        statsFile << "Minimum Enabled Arc Time: " << minArcOnTime << endl;
        statsFile << "Maximum Enabled Arc: " << maxArcOn.str() << endl;
        statsFile << "Maximum Enabled Arc Time: " << maxArcOnTime << endl;
        statsFile << "MinMax Time Ratio: " << minMaxTimeRatio[parsedK] << endl;
        statsFile << "Raj Jain Fairness Index: " << J[parsedK] << endl;
        statsFile << endl;
        statsFile << "Fairness LP Model tmin: " << tmin << endl;
        statsFile << "Fairness LP Model tmax: " << tmax << endl;
        statsFile << endl;
        statsFile << "Routes:" << endl;
        statsFile << "--------------------------------------------------"
                << endl;
        statsFile << "MaxMaxDelay: " << maxMaxDelay << endl;
        statsFile << "MaxAvgDelay: " << maxAvgDelay << endl;
        statsFile << "UnroutedKs: " << this->globalUnroutedKs << endl;

        /// Close Stats File
        statsFile.close();
    }

    /// Save some stats to global so we can pull them off
    globalTotalArcOnTime = totalArcOnTime;
    globalTotalArcOnWeight = totalArcOnWeight;
    globalMinMaxTimeRatio = minMaxTimeRatio[parsedK];
    globalJ = J[parsedK];
    globalMinArcOnTime = minArcOnTime;
    globalMaxArcOnTime = maxArcOnTime;
    globalmaxMaxDelay = maxMaxDelay;
    globalmaxAvgDelay = maxAvgDelay;

}

/**
 * Solves the Topology using MaxWeight LP Model.
 *
 * @param solver specifies whether to solve by "ibm" or "glpk"
 */
void Topology::solveByMaxWeightLP(string solver) {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "MaxWeightLP";
    }
    if (solver == "ibm" || solver == "glpk") {
        cout << endl << "Solving Topology " << topologyName
                << " by MaxWeightLP method with " << solver.c_str()
                << " solver." << endl;
    } else {
        cout << "Wrong solver when solveByMaxWeightLP, use ibm or glpk."
                << endl;
        exit(1);
    }

    /// Initialize logTopology to all -1
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = 0;
                if (i == j) {
                    logTopology[k][i][j] = -1;
                }
            }
        }
    }

    // Write CPLEX file
    writeCPLEXMaxWeight("tmp/topology.cplex");

    if (solver == "glpk") {
        system("glpsol --lp tmp/topology.cplex -o tmp/topologySol.txt"); /// Solve via glpk
        readSolGLPK("tmp/topologySol.txt"); /// Parse Solution
        cout << endl;
    } else if (solver == "ibm") {
        system("rm tmp/topologySol.txt"); /// Solve via ibm
        system(
                "cplex -c read tmp/topology.cplex lp optimize write tmp/topologySol.txt sol");
        readSolIBM("tmp/topologySol.txt");
        cout << endl;
    }

}

/**
 * Solves the topology by using a max weight strict Algorithm.
 * The algorithm basically go trough the physical topology
 * matrix and enables the first encountered possible link.
 */
void Topology::solveByMaxWeightStrict() {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "MaxWeightStrict";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by MaxWeightStrict method." << endl;

    for (int k = 1; k < parsedK + 1; k++) {
        solveKArcStrictMaxWeight(k);
    }
}

/*
 * Solve the physical topology by applying the reduced Blossom
 * Maximum Matching Algorithm for 1-Matching Problem in
 * Undirected Networks to each topology iteration.
 */
void Topology::solveByMaxWeightBlossom() {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "MaxWeightBlossom";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by MaxWeightBlossom method." << endl;

    for (int k = 1; k < parsedK + 1; k++) {
        solveKBlossom(k, true);
    }
}

/**
 * Fairness MaxMin model: Two iteration LP model that first determine the
 * max tmin, and then minimize tmax. Uses tmin and tmax global variables
 * to store arguments.
 *
 * @param solver specifies whether to solve by "ibm" or "glpk"
 * @param epsilon capacity multiplier for tmin maximization (Max: tmin + Eps[arcTime])
 * @param beta capacity multiplier for tmax minimization (Sub: sum[arc[i][j]]>= Beta*Tobj)
 */
void Topology::solveByFairnessMinMaxLP(string solver, float beta,
        float epsilon) {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FairnessMinMaxLP";
    }
    if (solver == "ibm" || solver == "glpk") {
        cout << endl << "Solving Topology " << topologyName
                << " by FairnessMinMaxLP method with " << solver.c_str()
                << " solver." << endl;
    } else {
        cout << "Wrong solver when solveByFairnessMinMaxLP, use ibm or glpk."
                << endl;
        exit(1);
    }

    //Initialize logTopology to all -1
    //Pongo todos a 0 por que despues solo escribo
    //los 1s o 0s de activado desactivados
    //de la salida del CPLEX
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = 0;
                if (i == j) {
                    logTopology[k][i][j] = -1;
                }
            }
        }
    }

    // Write first CPLEX model (Max t_min)
    writeCPLEXFairnessMaxTmin("tmp/topology.cplex", 1, parsedK, epsilon);

    if (solver == "glpk") {
        system("glpsol --lp tmp/topology.cplex -o tmp/topologySol.txt"); /// Solve via glpk
        readSolGLPK("tmp/topologySol.txt"); /// Parse Solution
        cout << endl;
    } else if (solver == "ibm") {
        system("rm tmp/topologySol.txt"); /// Solve via ibm
        system(
                "cplex -c read tmp/topology.cplex lp optimize write tmp/topologySol.txt sol");
        readSolIBM("tmp/topologySol.txt");
        cout << endl;
    }

    ///Evaluate the Objective Capacity and reset logical topology
    int tobj = 0;
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (logTopology[k][i][j] == 1) {
                    tobj += t[k];
                }
                logTopology[k][i][j] = 0;
                if (i == j) {
                    logTopology[k][i][j] = -1;
                }
            }
        }
    }

    // Write second CPLEX model (Min t_max)
    writeCPLEXFairnessMinTmax("tmp/topology.cplex", 1, parsedK, tobj, beta);

    if (solver == "glpk") {
        system("glpsol --lp tmp/topology.cplex -o tmp/topologySol.txt"); /// Solve via glpk
        readSolGLPK("tmp/topologySol.txt"); /// Parse Solution
        cout << endl;
    } else if (solver == "ibm") {
        system("rm tmp/topologySol.txt"); /// Solve via ibm
        system(
                "cplex -c read tmp/topology.cplex lp optimize write tmp/topologySol.txt sol");
        readSolIBM("tmp/topologySol.txt");
        cout << endl;
    }

    if (debug) {
        cout << "FairnessMinMaxLP values:" << endl;
        cout << "tmin: " << tmin << endl;
        cout << "tmax: " << tmax << endl;
        cout << "tobj: " << tobj << endl;
        cout << "beta: " << beta << endl;
    }
}

/**
 * Fairness K MaxMin model: K*Two iteration LP model.
 * Solves state by state aiming ar maximizing tmin and min tmax as
 * it advances.
 * Uses tmin and tmax global variables to store arguments.
 *
 * @param solver specifies whether to solve by "ibm" or "glpk"
 * @param epsilon capacity multiplier for tmin maximization (Max: tmin + Eps[arcTime])
 * @param beta capacity multiplier for tmax minimization (Sub: sum[arc[i][j]]>= Beta*Tobj)
 */
void Topology::solveByFairnessKMinMaxLP(string solver, float beta,
        float epsilon) {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FairnessKMinMaxLP";
    }
    if (solver == "ibm" || solver == "glpk") {
        cout << endl << "Solving Topology " << topologyName
                << " by FairnessKMinMaxLP method with " << solver.c_str()
                << " solver." << endl;
    } else {
        cout << "Wrong solver when solveByKFairnessMinMaxLP, use ibm or glpk."
                << endl;
        exit(1);
    }

    //Initialize logTopology to all -1
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = 0;
                if (i == j) {
                    logTopology[k][i][j] = -1;
                }
            }
        }
    }

    /// Solve State by State
    for (int k = 1; k < parsedK + 1; k++) {
        //for (int k = 1; k < 2 + 1; k++) {
        // Write first CPLEX model (Max t_min) for this k
        writeCPLEXFairnessMaxTmin("tmp/topology.cplex", k, k, epsilon);

        if (solver == "glpk") {
            system("glpsol --lp tmp/topology.cplex -o tmp/topologySol.txt"); /// Solve via glpk
            readSolGLPK("tmp/topologySol.txt"); /// Parse Solution
            cout << endl;
        } else if (solver == "ibm") {
            system("rm tmp/topologySol.txt"); /// Solve via ibm
            system(
                    "cplex -c read tmp/topology.cplex lp optimize write tmp/topologySol.txt sol");
            readSolIBM("tmp/topologySol.txt");
            cout << endl;
        }

        ///Evaluate this k Objective Capacity and reset logical topology
        int tobj = 0;
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (logTopology[k][i][j] == 1) {
                    tobj += t[k];
                }
                logTopology[k][i][j] = 0;
                if (i == j) {
                    logTopology[k][i][j] = -1;
                }
            }
        }

        // Write second CPLEX model (Min t_max)
        writeCPLEXFairnessMinTmax("tmp/topology.cplex", k, k, tobj, beta);

        if (solver == "glpk") {
            system("glpsol --lp tmp/topology.cplex -o tmp/topologySol.txt"); /// Solve via glpk
            readSolGLPK("tmp/topologySol.txt"); /// Parse Solution
            cout << endl;
        } else if (solver == "ibm") {
            system("rm tmp/topologySol.txt"); /// Solve via ibm
            system(
                    "cplex -c read tmp/topology.cplex lp optimize write tmp/topologySol.txt sol");
            readSolIBM("tmp/topologySol.txt");
            cout << endl;
        }

        if (debug) {
            cout << "k: " << k << " FairnessMinMaxLP values: (beta: " << beta
                    << ", epsilon: " << epsilon << ")" << endl;
            cout << "k: " << k << " tmin: " << tmin << endl;
            cout << "k: " << k << " tmax: " << tmax << endl;
            cout << "k: " << k << " tobj: " << tobj << endl;
        }

    }

}

/**
 * Solves the topology by using a fairness strict Algorithm.
 * The algorithm basically go trough the physical topology
 * matrix and enables the first encountered possible link.
 */
void Topology::solveByFairnessStrict() {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FairnessStrict";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by FairnessStrict method." << endl;

    /// Time without contact for link i to k
    unsigned int offContactTime[MAXS][MAXS];

    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            offContactTime[i][j] = 1;
        }
    }

    /// Go Through all States Solving:
    for (int k = 1; k < parsedK + 1; k++) {

        /// SET WEIGHTS TO UN-CONNECTED TIME
        /// We set the weight values for this k
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (offContactTime[i][j] > 4294967294) /// Upper Bound=  4294967294
                        {
                    arcWeight[k][i][j] = 4294967294;
                    offContactTime[i][j] = 4294967294;
                } else {
                    arcWeight[k][i][j] = offContactTime[i][j];
                }
            }
        }

//        /// SET WEIGHTS TO ORDERED UN-CONNECTED TIME
//        int highestI, highestJ, highestTime;
//        bool done[nodeNum][nodeNum];
//
//        // Init done[][] array to false
//        for (int i = 0; i < nodeNum; i++) {
//            for (int j = 0; j < nodeNum; j++) {
//                done[i][j] = false;
//            }
//        }
//
//        // Go through all arcs and save it weight in order as per
//        // the un-connected time. Weights then go from 1 to N(N-1)/2
//        // which is the quantity of arcs in the topology
//        for (int order = nodeNum * (nodeNum - 1) / 2; order > 0; order--) {
//            highestTime = 0;
//            for (int i = 0; i < nodeNum; i++) {
//                for (int j = 0; j < nodeNum; j++) {
//                    if ((offContactTime[i][j] > highestTime) && (!done[i][j])
//                            && (i != j)) {
//                        highestTime = offContactTime[i][j];
//                        highestI = i;
//                        highestJ = j;
//                    }
//                }
//            }
//            arcWeight[k][highestI][highestJ] = order;
//            arcWeight[k][highestJ][highestI] = order;
//            done[highestI][highestJ] = true;
//            done[highestJ][highestI] = true;
//            if (debug) {
//                cout << endl << "K:" << k << ", Order:" << order << "\t"
//                        << highestI << "-" << highestJ << " and " << highestJ
//                        << "-" << highestI << " with "
//                        << offContactTime[highestI][highestJ]
//                        << " secs. of un-connected time.";
//            }
//        }

        /// Solve as per the new weights. We want to enforce
        /// a strict priority selection rather than weighted:
        if (k == 1)
            solveKBlossom(k, true);
        else
            solveKArcStrictMaxWeight(k);

        /// We update contact time for each assigned node
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                /*
                 * Removing Phy evaluation implies incrementing weight even
                 * if this was not a feasible link. This modification
                 * allows to highly prioritize sacrce links like intra-
                 * group ones
                 */
                if (logTopology[k][i][j] == 0) {
                    offContactTime[i][j] += t[k];
                }
            }
        }
    }
}

/**
 * Solve By Fairness Algorithm v1
 */
void Topology::solveByFairnessBlossom1() {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FairnessBlossom";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by FairnessBlossom method." << endl;

    /// Time without contact for link i to k
    unsigned int offContactTime[MAXS][MAXS];

    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            offContactTime[i][j] = 1;
        }
    }

//    /// Explore Topology for less capacited Arcs
//    /// in search for the minOffPhyTime.
//    /// Then init those arcs with extra weight penalty
//    unsigned int minOffPhyTime = 4000000000;
//    unsigned int offPhyTime = 0;
//    for (int i = 0; i < nodeNum; i++) {
//        for (int j = 0; j < nodeNum; j++) {
//            offPhyTime = 0;
//            for (int k = 1; k < parsedK + 1; k++) {
//                if (phyTopology[k][i][j] == 0) {
//                    offPhyTime += t[k];
//                }
//            }
//            if(offPhyTime<=minOffPhyTime){
//                minOffPhyTime=offPhyTime;
//            }
//        }
//    }
//    for (int i = 0; i < nodeNum; i++) {
//        for (int j = 0; j < nodeNum; j++) {
//            offPhyTime = 0;
//            for (int k = 1; k < parsedK + 1; k++) {
//                if (phyTopology[k][i][j] == 0) {
//                    offPhyTime += t[k];
//                }
//            }
//            if(offPhyTime<=(minOffPhyTime*2)){
//                offContactTime[i][j]+=parsedK*5;
//            }
//            if(offPhyTime<=(minOffPhyTime*1)){
//                offContactTime[i][j]+=parsedK*10;
//            }
//        }
//    }

    /// Go Through all States Solving:
    for (int k = 1; k < parsedK + 1; k++) {

        /// SET WEIGHTS TO UN-CONNECTED TIME
        /// We set the weight values for this k
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (offContactTime[i][j] > 4294967294) /// Upper Bound 4294967294
                        {
                    arcWeight[k][i][j] = 4294967294;
                    offContactTime[i][j] = 4294967294;
                    cout << "WARNING: Max Weight Achieved!!!" << endl;
                } else {
                    arcWeight[k][i][j] = offContactTime[i][j];
                }
            }
        }

        /// Solve as per the new weights.
        solveKBlossom(k, true);

        /// We update contact time for each assigned node
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                /*
                 * Removing Phy evaluation implies incrementing weight even
                 * if this was not a feasible link. This modification
                 * allows to highly prioritize sacrce links like intra-
                 * group ones
                 */
                if (logTopology[k][i][j] == 0) {
                    offContactTime[i][j] += t[k];
                }
            }
        }
    }
}

/**
 * Solve by Fairness Algorithm v2
 */

void Topology::solveByFairnessBlossom2() {
    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FairnessBlossom";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by FairnessBlossom method." << endl;

    /// Time without contact for link i to k
    unsigned int offContactTime[MAXS][MAXS];

    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            offContactTime[i][j] = 1;
        }
    }

    /// Go Through all States Solving:
    for (int k = 1; k < parsedK + 1; k++) {

        /// SET WEIGHTS
        /// We set the weight values for this k
        /// Search for offContactTimeMax and offContactTimeMin
        unsigned int offContactTimeMin = 4294967290;
        unsigned int offContactTimeMax = 0;
        unsigned int offContactTimeRange = 0;
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (offContactTime[i][j] < offContactTimeMin) {
                    offContactTimeMin = offContactTime[i][j];
                }
                if (offContactTime[i][j] > offContactTimeMax) {
                    offContactTimeMax = offContactTime[i][j];
                }
            }
        }
        offContactTimeRange = offContactTimeMax - offContactTimeMin;

        cout << "offContactTimeRange=" << offContactTimeRange << endl;
        cout << "offContactTimeRange*0.3=" << offContactTimeRange * 0.3 << endl;

        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (offContactTime[i][j] > 4294967294) /// Upper Bound 4294967294
                        {
                    arcWeight[k][i][j] = 4294967294;
                    offContactTime[i][j] = 4294967294;
                    cout << "WARNING: Max Weight Achieved!!!" << endl;
                } else {
                    /// Classify among two classes:
                    /// if between the 30% more poor, weight = 100
                    /// else, weight =1
                    if (offContactTime[i][j]
                            < (offContactTimeMin + (offContactTimeRange * 0.3))) {
                        arcWeight[k][i][j] = 100;
                    } else {
                        arcWeight[k][i][j] = 1;
                    }
                    cout << k << "." << i << "." << j << "="
                            << arcWeight[k][i][j] << endl;
                }
            }
        }

        /// Solve as per the new weights.
        solveKBlossom(k, true);

        /// We update contact time for each assigned node
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                /*
                 * Removing Phy evaluation implies incrementing weight even
                 * if this was not a feasible link. This modification
                 * allows to highly prioritize sacrce links like intra-
                 * group ones
                 */
                if (logTopology[k][i][j] == 0) {
                    offContactTime[i][j] += t[k];
                }
            }
        }
    }
}

/*
 * Solve Topology By Brute Force:
 * @criteria AvgDelay or MaxDelay
 *
 */
void Topology::solveByBruteForce(bool writeFile, bool writeFileAdv,
        string criteria, bool pareto) {

    int initialglobalMaxMaxDelay;
    int initialglobalmaxAvgDelay;
    int initialglobalUnroutedKs;
    int actualglobalMaxMaxDelay;
    int actualglobalmaxAvgDelay;
    int actualglobalUnroutedKs;
    char bestLogTopology[parsedK + 1][nodeNum][nodeNum];

    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "BruteForce";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by BruteForce method." << endl;

    /// Get Initial Blossom Stats
    this->solveByFairnessBlossom1(); // Puedo resolverla de cualquier forma
    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->evaluateRoutes(); // We get globalmaxAvgDelay calculated
    }
    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
    actualglobalUnroutedKs = this->globalUnroutedKs;
    initialglobalMaxMaxDelay = this->globalmaxMaxDelay;
    initialglobalmaxAvgDelay = this->globalmaxAvgDelay;
    initialglobalUnroutedKs = this->globalUnroutedKs;

    ofstream paretoADFile;
    ofstream paretoUTFile;
    ofstream paretoJIFile;
    // If pareto enabled
    if (pareto == true) {
        paretoADFile.open("tmp/paretoADFile");
        paretoUTFile.open("tmp/paretoUTFile");
        paretoJIFile.open("tmp/paretoJIFile");
    }

    // This is our best topology so far
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                bestLogTopology[k][i][j] = logTopology[k][i][j];
            }
        }
    }

    /// Init Topology Solver
    this->clearLogTopology();

    // Get Arc Number
    unsigned int arcsNumber = 0;
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) { // for all nodes
            for (int j = i; j < nodeNum; j++) {
                if (phyTopology[k][i][j] >= 1)
                    arcsNumber++;
            }
        }
    }
    cout << "Arcs:" << arcsNumber << " (" << pow(2.0, (double) arcsNumber)
            << " combinations)" << endl;
    if (arcsNumber >= 256) {
        cout << "Too many arcs to Brute force! " << arcsNumber << endl;
        exit(1);
    }
    std::bitset<256> * arcs;
    unsigned long arcsulong = 0;

    for (int iteration = 0; iteration < pow(2.0, (double) arcsNumber) - 1;
            iteration++) {

        if (debug)
            cout
                    << "BruteForce: "
                    << iteration
                    << "/"
                    << pow(2.0, (double) arcsNumber) - 1
                    << " ("
                    << 100 * ((float) iteration)
                            / ((float) pow(2.0, (double) arcsNumber)) << "%)"
                    << endl;

        int index = 0;
        arcsulong++;
        arcs = new std::bitset<256>(arcsulong);
        this->clearLogTopology();

        // Set log topology to arc combination:
        for (int k = 1; k < parsedK + 1; k++) {
            for (int i = 0; i < nodeNum; i++) { // for all nodes
                for (int j = i; j < nodeNum; j++) {
                    if (phyTopology[k][i][j] >= 1) { // an arc
                        if (arcs->test(index) == 1) { // turn on!
                            logTopology[k][i][j] = phyTopology[k][i][j];
                            logTopology[k][j][i] = phyTopology[k][j][i];
                        }
                        index++;
                    }
                }
            }
        }

        solved = true;

        // Evaluate if satisfies interface 1 restriction
        if (this->checkSingleInterface()) {

            // Calculate routes and values for this log
            this->saveStats(false);
            this->floydWarshall(false, false);
            this->evaluateRoutes(); // We get globalmaxAvgDelay calculated

            // Save Pareto values of true
            if (pareto == true) {
                paretoADFile << this->globalmaxAvgDelay << endl;
                paretoUTFile << this->globalUnroutedKs << endl;
                paretoJIFile << this->globalJ << endl;
//                if (this->globalmaxAvgDelay==24&&this->globalUnroutedKs==16&&this->globalJ>=0.375) {
//                    char intStr[30];
//                    sprintf(intStr, "paretoAD%d_UT%d_JI%1.3f", this->globalmaxAvgDelay, this->globalUnroutedKs, this->globalJ);
//                    this->topologyName = string(intStr);
//                    this->saveDOT();
//                }
//                if (this->globalmaxAvgDelay==23&&this->globalUnroutedKs==24&&this->globalJ>=0.368) {
//                    char intStr[30];
//                    sprintf(intStr, "paretoAD%d_UT%d_JI%1.3f", this->globalmaxAvgDelay, this->globalUnroutedKs, this->globalJ);
//                    this->topologyName = string(intStr);
//                    this->saveDOT();
//                }
//                if (this->globalmaxAvgDelay==23&&this->globalUnroutedKs==30&&this->globalJ>=0.375) {
//                    char intStr[30];
//                    sprintf(intStr, "paretoAD%d_UT%d_JI%1.3f", this->globalmaxAvgDelay, this->globalUnroutedKs, this->globalJ);
//                    this->topologyName = string(intStr);
//                    this->saveDOT();
//                }
//                if (this->globalmaxAvgDelay==22&&this->globalUnroutedKs==44&&this->globalJ>=0.375) {
//                    char intStr[30];
//                    sprintf(intStr, "paretoAD%d_UT%d_JI%1.3f", this->globalmaxAvgDelay, this->globalUnroutedKs, this->globalJ);
//                    this->topologyName = string(intStr);
//                    this->saveDOT();
//                }
                                if (this->globalmaxAvgDelay == 15
                        && this->globalUnroutedKs == 60
                        && this->globalJ >= 0.375) {
                    char intStr[30];
                    sprintf(intStr, "paretoAD%d_UT%d_JI%1.3f",
                            this->globalmaxAvgDelay, this->globalUnroutedKs,
                            this->globalJ);
                    this->topologyName = string(intStr);
                    this->saveDOT();
                }
            }

            if (criteria == "AvgDelay") { // If AvgDelay improves AND UnroutedKs is equal or better, adopt.
                if ((this->globalmaxAvgDelay < actualglobalmaxAvgDelay)
                        && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                    // Adopt
                    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                    actualglobalUnroutedKs = this->globalUnroutedKs;
                    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                    // Copy to best Log Topo
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                bestLogTopology[k][i][j] = logTopology[k][i][j];
                            }
                        }
                    }
                }
            }

            if (criteria == "MaxDelay") {
                // If MaxDelay improves AND UnroutedKs is equal or better, adopt.
                if ((this->globalmaxMaxDelay < actualglobalMaxMaxDelay)
                        && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                    // Adopt
                    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                    actualglobalUnroutedKs = this->globalUnroutedKs;
                    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                    // Copy to best Log Topo
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                bestLogTopology[k][i][j] = logTopology[k][i][j];
                            }
                        }
                    }
                }
            }
        }
    }

    // Save resulting topology as the best one:
    // Restore Best Logical:
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = bestLogTopology[k][i][j];
            }
        }
    }

    // Solve Routes Again
    this->floydWarshall(false, false);
    this->evaluateRoutes(); // We get globalmaxAvgDelay calculated

    // Print Result:
    if (debug) {
        cout << endl << "maxAvgDelay: " << initialglobalmaxAvgDelay << " -> "
                << actualglobalmaxAvgDelay << " (" << globalmaxAvgDelay << ")"
                << endl;
        cout << "maxMaxDelay: " << initialglobalMaxMaxDelay << " -> "
                << actualglobalMaxMaxDelay << " (" << globalmaxMaxDelay << ")"
                << endl;
        cout << "UnroutedKs:  " << initialglobalUnroutedKs << " -> "
                << actualglobalUnroutedKs << endl;
    }

    // If pareto enabled
    if (pareto == true) {
        bool rest = false;
        arcsulong++;
        paretoUTFile.close();
        paretoJIFile.close();
        paretoADFile.close();
        // Generateoutput
        // ./pareto -l example.file1 -l example.file2 > example.result
        // The flag -l indicates larger number is better (maximize);
        // alternatively, the -s flag means smaller is better (minimize).
        system(
                "./incl/pareto -s tmp/paretoADFile -l tmp/paretoJIFile -s tmp/paretoUTFile > tmp/paretoResult");
        // Now open the file save a csv with points
        ifstream paretoADFilein;
        ifstream paretoUTFilein;
        ifstream paretoJIFilein;
        paretoADFilein.open("tmp/paretoADFile");
        paretoUTFilein.open("tmp/paretoUTFile");
        paretoJIFilein.open("tmp/paretoJIFile");
        ifstream paretoResultin;
        paretoResultin.open("tmp/paretoResult");
        ofstream paretoResultCsv;
        paretoResultCsv.open("out/paretoResult_Front.csv");
        ofstream paretoResultCsvRest;
        paretoResultCsvRest.open("out/paretoResult_Rest.csv");
        string StringRes, StringAD, StringUT, StringJI;
        size_t found1;
        paretoResultCsv << "AD" << "," << "UT" << "," << "JI" << endl;
        paretoResultCsvRest << "AD" << "," << "UT" << "," << "JI" << endl;
        while (paretoResultin.eof() == false) {
            getline(paretoResultin, StringRes);
            getline(paretoADFilein, StringAD);
            getline(paretoUTFilein, StringUT);
            getline(paretoJIFilein, StringJI);
            found1 = StringRes.find("1");
            if (found1 != string::npos) { /// We found 1!
                float AD = atof(StringAD.c_str());
                float UT = atof(StringUT.c_str());
                float JI = atof(StringJI.c_str());
                paretoResultCsv << AD << "," << UT << "," << JI << endl;
            } else { /// Dominated option
                if (rest) {
                    float AD = atof(StringAD.c_str());
                    float UT = atof(StringUT.c_str());
                    float JI = atof(StringJI.c_str());
                    paretoResultCsvRest << AD << "," << UT << "," << JI << endl;
                }
            }
        }
        paretoADFilein.close();
        paretoUTFilein.close();
        paretoJIFilein.close();
        paretoResultin.close();
        paretoResultCsv.close();

        // Lets plot something
        ofstream paretoResultGplot;
        paretoResultGplot.open("out/paretoResult.gnuplot");
        paretoResultGplot << "set terminal svg size 700,400" << endl;
        paretoResultGplot << "set output \"paretoResult.svg\"" << endl;
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
        paretoResultGplot << "set palette gray" << endl;
        paretoResultGplot << "set palette negative" << endl;
//        paretoResultGplot
//                << "splot 'paretoResult_Front.csv' using 1:2:3 with pm3d title \"Pareto Frontier\""
//                << endl;
        paretoResultGplot
                << "splot 'paretoResult_Front.csv' using 1:2:(0.0):3  with points pointsize 1 pointtype 7 palette title \"Pareto Frontier\""
                << endl;
        paretoResultGplot.close();
        // Plot
        system(" cd out && gnuplot paretoResult.gnuplot");

    } // End Pareto

    solved = true;
}

/**
 * @criteria    AvgDelayJainI or AvgDelay
 */
void Topology::solveBySteepestDescent(bool writeFile, bool writeFileAdv,
        string criteria, int maxIterations, int maxNeighbours) {

    int initialglobalMaxMaxDelay;
    int initialglobalmaxAvgDelay;
    int initialglobalUnroutedKs;
    double initialglobalJ;
    int actualglobalMaxMaxDelay;
    int actualglobalmaxAvgDelay;
    int actualglobalUnroutedKs;
    double actualglobalJ;
    char bestLogTopology[parsedK + 1][nodeNum][nodeNum];
    string tempName = this->topologyName;
    int iteration = 0;
    string intString;
    char intStr[10];

    /// Init Topology Solver
    this->clearLogTopology();

    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "SteepestDescent";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by SteepestDescent method." << endl;

    // Initial Topology
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    this->solveByFairnessBlossom1(); // Puedo resolverla de cualquier forma

    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->saveStats(false);
    }

    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
    actualglobalUnroutedKs = this->globalUnroutedKs;
    actualglobalJ = this->globalJ;
    initialglobalMaxMaxDelay = this->globalmaxMaxDelay;
    initialglobalmaxAvgDelay = this->globalmaxAvgDelay;
    initialglobalUnroutedKs = this->globalUnroutedKs;
    initialglobalJ = this->globalJ;

    // This is our best topology so far
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                bestLogTopology[k][i][j] = logTopology[k][i][j];
            }
        }
    }
    if (debug) {
        cout << endl << "globalMaxAvgDelay: " << actualglobalmaxAvgDelay
                << endl;
        cout << "globalUnroutedKs: " << actualglobalUnroutedKs << endl;
        cout << "globalMaxMaxDelay: " << actualglobalMaxMaxDelay << endl;

    }

    ofstream fairnessWarshallFile;
    ofstream fairnessWarshallGFile;

    if (writeFile) {
        /// FairnessWarshall csv fileName:
        string fairnessWarshallFileName = "out/";
        fairnessWarshallFileName += tempName;
        fairnessWarshallFileName += "_SteeptestDescent.csv";

        fairnessWarshallFile.open(fairnessWarshallFileName.c_str());

        /// FairnessWarshall gnuplot fileName:
        string fairnessWarshallGFileName = "out/";
        fairnessWarshallGFileName += tempName;
        fairnessWarshallGFileName += "_SteeptestDescent.gnuplot";

        fairnessWarshallGFile.open(fairnessWarshallGFileName.c_str());
    }

    if (writeFile) {
        fairnessWarshallFile
                << "iteration,maxAvgDelay,actualmaxAvgDelay,UnroutedKs,actualUnroutedKs,Temp,BadNeighbour,MaxMaxDelay,actualMaxMaxDelay,Jain"
                << endl;
        fairnessWarshallFile << iteration << "," << this->globalmaxAvgDelay
                << "," << actualglobalmaxAvgDelay << ","
                << this->globalUnroutedKs << "," << actualglobalUnroutedKs
                << "," << "NaN" << "," << "NaN" << ","
                << this->globalmaxMaxDelay << "," << actualglobalMaxMaxDelay
                << "," << this->globalJ << endl;
    }

    /// Outer iteration:
    for (iteration = 1; iteration <= maxIterations; iteration++) {
        //if (debug)
        cout << "Steepest Descent Iteration " << iteration << " with N: "
                << maxNeighbours << " neighbours per iteration" << endl;

        /// Generate maxNeighbours Neighbours
        int randK[maxNeighbours];
        int randI[maxNeighbours];
        int randJ[maxNeighbours];
        for (int neighbours = 0; neighbours < maxNeighbours; neighbours++) {
            /// Get a Neighbour
            bool changeDetected = false;
            int changeCounter = 0;
            while ((changeDetected == false) && (changeCounter <= 20000)) {
                changeCounter++;
                randK[neighbours] = rand() % (parsedK) + 1; // between 1 and parsedK
                randI[neighbours] = rand() % nodeNum; // between 0 and nodeNum-1
                int phyArcs = 0;
                int logArcs = 0;
                for (int j = 0; j < nodeNum; j++) {
                    if (phyTopology[randK[neighbours]][randI[neighbours]][j]
                            >= 1) {
                        phyArcs++;
                        if (logTopology[randK[neighbours]][randI[neighbours]][j]
                                >= 1) {
                            randJ[neighbours] = j;
                            logArcs++;
                        }
                    }
                }
                if (phyArcs > 1) { // maxInterfaces[randI]=1
                    changeDetected = true;
                    if (debug)
                        cout << neighbours << " - Change Detected for k: "
                                << randK[neighbours] << " node i: "
                                << randI[neighbours] << " whith " << phyArcs
                                << " physical Arcs (logical j: "
                                << randJ[neighbours] << ")" << endl;
                } else {
                    if (debug)
                        cout << neighbours << " - No Change Detected for k: "
                                << randK[neighbours] << " node i: "
                                << randI[neighbours] << " whith " << phyArcs
                                << " physical Arcs" << endl;
                }
                if (logArcs == 0) {
                    //no hay logs para cambiar! que buen issue que encontre :)
                    changeDetected = false;
                }
            } /// End a single Neighbour detecion

            if (changeCounter >= 20000) {
                if (debug)
                    cout << "Cant detect Change for " << this->topologyName
                            << endl;
                randK[neighbours] = -1;
                //iteration = maxIterations + 1; // Dont search for a new change
            }

        } /// End All Neighbour generation

        /// Now test which is the best of them
        int neighbourGlobalmaxAvgDelay = actualglobalmaxAvgDelay;
        int neighbourGlobalUnroutedKs = actualglobalUnroutedKs;
        double neighboutGlobalJ = actualglobalJ;
        int bestNeighbour = -1;
        for (int neighbours = 0; neighbours < maxNeighbours; neighbours++) {
            /// bestLogTopology has the topology up to this point

            if (randK[neighbours] != -1) {
                char tempPhy =
                        phyTopology[randK[neighbours]][randI[neighbours]][randJ[neighbours]];
                phyTopology[randK[neighbours]][randI[neighbours]][randJ[neighbours]] =
                        0;
                phyTopology[randK[neighbours]][randJ[neighbours]][randI[neighbours]] =
                        0;
                solveKBlossom(randK[neighbours], true);
                phyTopology[randK[neighbours]][randI[neighbours]][randJ[neighbours]] =
                        tempPhy; // Restore previous Phy
                phyTopology[randK[neighbours]][randJ[neighbours]][randI[neighbours]] =
                        tempPhy;

                // Update Name
                sprintf(intStr, "%03d_%03d", iteration, neighbours);
                intString = string(intStr);
                this->topologyName = tempName;
                this->topologyName += intString;

                // Solve Routes Again
                if (writeFileAdv) {
                    this->floydWarshall(true, false);
                    this->saveStats(true); // We get globalmaxAvgDelay calculated
                    this->saveDOT();
                } else {
                    this->floydWarshall(false, false);
                    this->evaluateRoutes(); // We get globalmaxAvgDelay calculated
                }

                //Other Criteria:
                if (criteria == "AvgDelay") {
                    // Criteria Description
                    // If AvgDelay improves AND UnroutedKs is equal or better, adopt.
                    // Else, reject or continue.
                    if ((this->globalmaxAvgDelay < neighbourGlobalmaxAvgDelay)
                            && (this->globalUnroutedKs
                                    <= neighbourGlobalUnroutedKs)) {
                        // Adopt
                        bestNeighbour = neighbours;
                        neighbourGlobalmaxAvgDelay = this->globalmaxAvgDelay;
                        neighbourGlobalUnroutedKs = this->globalUnroutedKs;
                    } else {
                        // Reject
                    }
                }

                //Other Criteria:
                if (criteria == "AvgDelayJainI") {
                    // Criteria Description
                    // If AvgDelay improves AND UnroutedKs is equal or better, adopt.
                    // Else, reject or continue.
                    if ((this->globalmaxAvgDelay < neighbourGlobalmaxAvgDelay)
                            && (this->globalUnroutedKs
                                    <= neighbourGlobalUnroutedKs)
                            && (this->globalJ >= neighboutGlobalJ)) {
                        // Adopt
                        bestNeighbour = neighbours;
                        neighbourGlobalmaxAvgDelay = this->globalmaxAvgDelay;
                        neighbourGlobalUnroutedKs = this->globalUnroutedKs;
                        neighboutGlobalJ = this->globalJ;
                    } else {
                        // Reject
                    }
                }

                /// Restore bestLogTopology to logTopology
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            logTopology[k][i][j] = bestLogTopology[k][i][j];
                        }
                    }
                }
            } // Enf if (randK[neighbours] != -1) {
        } // End all Neighbours evaluation

        // if (debug)
        cout << "Steepest Descent Iteration " << iteration
                << " best neighbour is " << bestNeighbour << endl;

        if (bestNeighbour == -1) {
            cout << "No best Neighbour found in iteration " << iteration
                    << endl;
            iteration = maxIterations + 1; // I do this in the
        } else {
            /// Adopt new neighbour
            char tempPhy =
                    phyTopology[randK[bestNeighbour]][randI[bestNeighbour]][randJ[bestNeighbour]];
            phyTopology[randK[bestNeighbour]][randI[bestNeighbour]][randJ[bestNeighbour]] =
                    0;
            phyTopology[randK[bestNeighbour]][randJ[bestNeighbour]][randI[bestNeighbour]] =
                    0;
            solveKBlossom(randK[bestNeighbour], true);
            phyTopology[randK[bestNeighbour]][randI[bestNeighbour]][randJ[bestNeighbour]] =
                    tempPhy; // Restore previous Phy
            phyTopology[randK[bestNeighbour]][randJ[bestNeighbour]][randI[bestNeighbour]] =
                    tempPhy;

            for (int k = 1; k < parsedK + 1; k++) {
                for (int i = 0; i < (numSat + numET); i++) {
                    for (int j = 0; j < (numSat + numET); j++) {
                        bestLogTopology[k][i][j] = logTopology[k][i][j];
                    }
                }
            }

            // Solve Routes Again
            if (writeFileAdv) {
                this->floydWarshall(true, false);
                this->saveStats(true); // We get globalmaxAvgDelay calculated
                this->saveDOT();
            } else {
                this->floydWarshall(false, false);
                this->saveStats(false);
            }

            actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
            actualglobalUnroutedKs = this->globalUnroutedKs;
            actualglobalMaxMaxDelay = this->globalmaxMaxDelay;

            if (writeFile) {
                fairnessWarshallFile << iteration << ","
                        << this->globalmaxAvgDelay << ","
                        << actualglobalmaxAvgDelay << ","
                        << this->globalUnroutedKs << ","
                        << actualglobalUnroutedKs << "," << 0 << ",";
                if (true) { // Accepted and Good always!
                    fairnessWarshallFile << 3;
                }
                fairnessWarshallFile << "," << this->globalmaxMaxDelay << ","
                        << actualglobalMaxMaxDelay << "," << this->globalJ
                        << endl;
            }
        }

    } // End of main iteraton

    // Final Topology:
    // Restore Best Logical:
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = bestLogTopology[k][i][j];
            }
        }
    }

    // Update Name
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    // Solve Routes Again
    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->evaluateRoutes(); // We get globalmaxAvgDelay calculated
    }

    if (writeFile) {
        fairnessWarshallFile.close();
    }

    // Print Result:
    if (debug) {
        cout << endl << "maxAvgDelay: " << initialglobalmaxAvgDelay << " -> "
                << actualglobalmaxAvgDelay << " (" << globalmaxAvgDelay << ")"
                << endl;
        cout << "maxMaxDelay: " << initialglobalMaxMaxDelay << " -> "
                << actualglobalMaxMaxDelay << " (" << globalmaxMaxDelay << ")"
                << endl;
        cout << "UnroutedKs:  " << initialglobalUnroutedKs << " -> "
                << actualglobalUnroutedKs << endl;
    }

    // Restore TopologyName:
    this->topologyName = tempName;

    // Write GnuPlot and plot
    if (writeFile) {
        fairnessWarshallGFile << "set terminal svg size 600,1440 dashed"
                << endl;
        fairnessWarshallGFile << "set output \"" << topologyName
                << "_SteeptestDescent.svg\"" << endl;
        fairnessWarshallGFile << "set multiplot layout 6,1" << endl;

        fairnessWarshallGFile << "set key" << endl;
        fairnessWarshallGFile << "set datafile separator \",\"" << endl;
        fairnessWarshallGFile << "set xlabel \"Iteration\"" << endl;
        fairnessWarshallGFile
                << "set style line 1 lc rgb '#0025ad' pt 7 lt 1 lw 1 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 2 lc rgb '#0025ad' pt 7 lt 3 lw 2 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 3 lc rgb '#09ad00' pt 7 lt 1 lw 1 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 4 lc rgb '#09ad00' pt 7 lt 3 lw 2 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        fairnessWarshallGFile
                << "set style line 6 lc rgb '#ba55cc' pt 11 lt 1 lw 1 # --- violet"
                << endl;
        fairnessWarshallGFile
                << "set style line 20 lc rgb '#000000' pt 2 lt 1 lw 1 # line and X"
                << endl;
        fairnessWarshallGFile
                << "set style line 21 lc rgb '#000000' pt 3 lt 1 lw 1 # line and *"
                << endl;
        fairnessWarshallGFile
                << "set style line 22 lc rgb '#000000' pt 3 lt 1 lw 1 # line and cuadradito vacio"
                << endl;
        fairnessWarshallGFile
                << "set style line 30 lc rgb '#000000' pt 3 lt 15 lw 1 # dot-dash and cuadradito vacio"
                << endl;
        fairnessWarshallGFile << "set style line 11 lc rgb '#808080' lt 1 "
                << endl;
        //fairnessWarshallGFile << "set border 3 back ls 11" << endl;
        fairnessWarshallGFile << "set tics nomirror" << endl;
        fairnessWarshallGFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        fairnessWarshallGFile << "set grid back ls 12" << endl;
        //fairnessWarshallGFile << "set style data histogram" << endl;
        //fairnessWarshallGFile << "set style fill solid" << endl;
        //fairnessWarshallGFile << "set boxwidth 0.9" << endl;
        fairnessWarshallGFile << "set yrange [:]" << endl;
        fairnessWarshallGFile << "set xrange [0:" << maxIterations << "]"
                << endl;
        fairnessWarshallGFile << "set offsets graph 0, 0, 0.05, 0.05" << endl;
        //fairnessWarshallGFile << "set xtics rotate by -90 left" << endl;
        fairnessWarshallGFile << "" << endl;
//    fairnessWarshallGFile << "plot '" << topologyName
//            << "_FairnessWarshall.csv' using 2 with linespoints ls 1 title \""
//            << topologyName << "_maxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 3 with linespoints ls 2 title \""
//            << topologyName << "_actualMaxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 4 with linespoints ls 3 title \""
//            << topologyName << "_UnroutedKs\", '" << topologyName
//            << "_FairnessWarshall.csv' using 5 with linespoints ls 4 title \""
//            << topologyName << "_actualUnroutedKs\"" << endl;

        fairnessWarshallGFile << "set ylabel \"MaxDelay\"" << endl;

        fairnessWarshallGFile << "plot '" << topologyName
                << "_SteeptestDescent.csv' using 8 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"AvgDelay\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SteeptestDescent.csv' using 2 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Jain Index\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SteeptestDescent.csv' using 10 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"UnroutedKs\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SteeptestDescent.csv' using 4 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

//        fairnessWarshallGFile << "set ylabel \"Decission\"" << endl;
//        fairnessWarshallGFile
//                << "plot '"
//                << topologyName
//                << "_SteeptestDescent.csv' using 7:ytic($7==1?\"Rjt\":$7==2?\"Cnt\":\"Acc\") with points ls 6 title \""
//                << topologyName << "_Decission\"" << endl;
//
//        fairnessWarshallGFile << "set ylabel \"Temperature\"" << endl;
//        fairnessWarshallGFile << "plot '" << topologyName
//                << "_SteeptestDescent.csv' using 6 with lines ls 5 title \""
//                << topologyName << "_Temperature\"" << endl;

        fairnessWarshallGFile.close();
        // Plot:
        string temp;
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_SteeptestDescent.gnuplot";
        system(temp.c_str());
    }
}

/**
 * @criteria    AvgDelayJainI or AvgDelay
 */
void Topology::solveByFirstImprovement(bool writeFile, bool writeFileAdv,
        string criteria, int maxIterations) {

    int actualglobalMaxMaxDelay;
    int actualglobalmaxAvgDelay;
    int actualglobalUnroutedKs;
    double actualglobalJ;
    char bestLogTopology[parsedK + 1][nodeNum][nodeNum];
    int iteration = 0;
    string intString;
    string tempName = this->topologyName;
    char intStr[10];

    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "FirstImprovement";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by Simulated Annealing method." << endl;

    this->solveByFairnessBlossom1(); // Puedo resolverla de cualquier forma

    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->saveStats(false);
    }

    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
    actualglobalUnroutedKs = this->globalUnroutedKs;
    actualglobalJ = this->globalJ;

    // This is our best topology so far
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                bestLogTopology[k][i][j] = logTopology[k][i][j];
            }
        }
    }

    ofstream fairnessWarshallFile;
    ofstream fairnessWarshallGFile;

    if (writeFile) {
        /// FairnessWarshall csv fileName:
        string fairnessWarshallFileName = "out/";
        fairnessWarshallFileName += tempName;
        fairnessWarshallFileName += "_FirstImprovement.csv";

        fairnessWarshallFile.open(fairnessWarshallFileName.c_str());

        /// FairnessWarshall gnuplot fileName:
        string fairnessWarshallGFileName = "out/";
        fairnessWarshallGFileName += tempName;
        fairnessWarshallGFileName += "_FirstImprovement.gnuplot";

        fairnessWarshallGFile.open(fairnessWarshallGFileName.c_str());
    }

    if (writeFile) {
        fairnessWarshallFile
                << "iteration,maxAvgDelay,actualmaxAvgDelay,UnroutedKs,actualUnroutedKs,Temp,BadNeighbour,MaxMaxDelay,actualMaxMaxDelay,Jain,actualJain"
                << endl;
        fairnessWarshallFile << iteration << "," << this->globalmaxAvgDelay
                << "," << this->globalmaxAvgDelay << ","
                << this->globalUnroutedKs << "," << this->globalUnroutedKs
                << "," << "NaN" << "," << "NaN" << ","
                << this->globalmaxMaxDelay << "," << this->globalmaxMaxDelay
                << "," << this->globalJ << "," << this->globalJ << endl;
    }

    for (iteration = 1; iteration <= maxIterations; iteration++) {
        // Branch-and-Exchange
        int randK;
        int randI;
        int randJ;
        bool changeDetected = false;
        int changeCounter = 0;

        while ((changeDetected == false) && (changeCounter <= 20000)) {
            changeCounter++;
            randK = rand() % (parsedK) + 1; // between 1 and parsedK
            randI = rand() % nodeNum; // between 0 and nodeNum-1
            int phyArcs = 0;
            int logArcs = 0;
            for (int j = 0; j < nodeNum; j++) {
                if (phyTopology[randK][randI][j] >= 1) {
                    phyArcs++;
                    if (logTopology[randK][randI][j] >= 1) {
                        randJ = j;
                        logArcs++;
                    }
                }
            }
            if (phyArcs > 1) { // maxInterfaces[randI]=1
                changeDetected = true;
                if (debug)
                    cout << " - Change Detected for k: " << randK << " node i: "
                            << randI << " whith " << phyArcs
                            << " physical Arcs (logical j: " << randJ << ")"
                            << endl;
            } else {
                if (debug)
                    cout << " - No Change Detected for k: " << randK
                            << " node i: " << randI << " whith " << phyArcs
                            << " physical Arcs" << endl;
            }
            if (logArcs == 0) {
                //no hay logs para cambiar! que buen issue que encontre :)
                changeDetected = false;
            }
        }

        if (changeCounter >= 20000) {
            if (debug)
                cout << "Cant detect Change for " << this->topologyName << endl;
            iteration = maxIterations + 1; // Dont search for a new change
        }

        char tempPhy = phyTopology[randK][randI][randJ];
        phyTopology[randK][randI][randJ] = 0;
        phyTopology[randK][randJ][randI] = 0;
        solveKBlossom(randK, true);
        phyTopology[randK][randI][randJ] = tempPhy; // Restore previous Phy
        phyTopology[randK][randJ][randI] = tempPhy;

        // Update Name
        sprintf(intStr, "%03d", iteration);
        intString = string(intStr);
        this->topologyName = tempName;
        this->topologyName += intString;

        // Solve Routes Again
        if (writeFileAdv) {
            this->floydWarshall(true, false);
            this->saveStats(true); // We get globalmaxAvgDelay calculated
            this->saveDOT();
        } else {
            this->floydWarshall(false, false);
            this->saveStats(false);
        }

        bool acceptedAndGood = false;
        bool rejected = false;

        if (criteria == "AvgDelay") {
            if ((this->globalmaxAvgDelay < actualglobalmaxAvgDelay)
                    && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                // Adopt
                if (debug)
                    cout << "Wow! we improved globalAvgDelay from "
                            << actualglobalmaxAvgDelay << " to "
                            << this->globalmaxAvgDelay << endl;
                acceptedAndGood = true;
                actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                actualglobalUnroutedKs = this->globalUnroutedKs;
                actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                actualglobalJ = this->globalJ;
                // Copy to best Log Topo
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            bestLogTopology[k][i][j] = logTopology[k][i][j];
                        }
                    }
                }
            } else {
                // Reject
                rejected = true;
                // Restore Best Logical:
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            logTopology[k][i][j] = bestLogTopology[k][i][j];
                        }
                    }
                }
            }

            if (writeFile) {
                fairnessWarshallFile << iteration << ","
                        << this->globalmaxAvgDelay << ","
                        << actualglobalmaxAvgDelay << ","
                        << this->globalUnroutedKs << ","
                        << actualglobalUnroutedKs << "," << 0 << ",";
                if (acceptedAndGood == true) {
                    fairnessWarshallFile << 3;
                } else if (rejected) {
                    fairnessWarshallFile << 1;
                }
                fairnessWarshallFile << "," << this->globalmaxMaxDelay << ","
                        << actualglobalMaxMaxDelay << "," << this->globalJ
                        << "," << actualglobalJ << endl;
            }
        }

        // Other criteria
        if (criteria == "AvgDelayJainI") {
            if ((this->globalmaxAvgDelay < actualglobalmaxAvgDelay)
                    && (this->globalUnroutedKs <= actualglobalUnroutedKs)
                    && (this->globalJ > actualglobalJ)) {
                // Adopt
                if (debug)
                    cout << "Wow! we improved globalAvgDelay from "
                            << actualglobalmaxAvgDelay << " to "
                            << this->globalmaxAvgDelay << endl;
                acceptedAndGood = true;
                actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                actualglobalUnroutedKs = this->globalUnroutedKs;
                actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                actualglobalJ = this->globalJ;
                // Copy to best Log Topo
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            bestLogTopology[k][i][j] = logTopology[k][i][j];
                        }
                    }
                }
            } else {
                // Reject
                rejected = true;
                // Restore Best Logical:
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            logTopology[k][i][j] = bestLogTopology[k][i][j];
                        }
                    }
                }
            }

            if (writeFile) {
                fairnessWarshallFile << iteration << ","
                        << this->globalmaxAvgDelay << ","
                        << actualglobalmaxAvgDelay << ","
                        << this->globalUnroutedKs << ","
                        << actualglobalUnroutedKs << "," << 0 << ",";
                if (acceptedAndGood == true) {
                    fairnessWarshallFile << 3;
                } else if (rejected) {
                    fairnessWarshallFile << 1;
                }
                fairnessWarshallFile << "," << this->globalmaxMaxDelay << ","
                        << actualglobalMaxMaxDelay << "," << this->globalJ
                        << "," << actualglobalJ << endl;
            }
        }

    } // end main iteration

    // Restore Best Logical:
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = bestLogTopology[k][i][j];
            }
        }
    }

    // Update Name
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    // Solve Routes Again
    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->saveStats(false);
    }

    if (writeFile) {
        fairnessWarshallFile.close();
    }

    // Restore TopologyName:
    this->topologyName = tempName;

    // Write GnuPlot and plot
    if (writeFile) {
        fairnessWarshallGFile << "set terminal svg size 600,1440 dashed"
                << endl;
        fairnessWarshallGFile << "set output \"" << topologyName
                << "_FirstImprovement.svg\"" << endl;
        fairnessWarshallGFile << "set multiplot layout 6,1" << endl;

        fairnessWarshallGFile << "set key" << endl;
        fairnessWarshallGFile << "set datafile separator \",\"" << endl;
        fairnessWarshallGFile << "set xlabel \"Iteration\"" << endl;
        fairnessWarshallGFile
                << "set style line 1 lc rgb '#0025ad' pt 7 lt 1 lw 1 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 2 lc rgb '#0025ad' pt 7 lt 3 lw 2 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 3 lc rgb '#09ad00' pt 7 lt 1 lw 1 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 4 lc rgb '#09ad00' pt 7 lt 3 lw 2 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        fairnessWarshallGFile
                << "set style line 6 lc rgb '#ba55cc' pt 11 lt 1 lw 1 # --- violet"
                << endl;
        fairnessWarshallGFile
                << "set style line 20 lc rgb '#000000' pt 2 lt 1 lw 1 # line and X"
                << endl;
        fairnessWarshallGFile
                << "set style line 21 lc rgb '#000000' pt 3 lt 1 lw 1 # line and *"
                << endl;
        fairnessWarshallGFile
                << "set style line 22 lc rgb '#000000' pt 3 lt 1 lw 1 # line and cuadradito vacio"
                << endl;
        fairnessWarshallGFile
                << "set style line 30 lc rgb '#000000' pt 3 lt 15 lw 1 # dot-dash and cuadradito vacio"
                << endl;
        fairnessWarshallGFile << "set style line 11 lc rgb '#808080' lt 1 "
                << endl;
        //fairnessWarshallGFile << "set border 3 back ls 11" << endl;
        fairnessWarshallGFile << "set tics nomirror" << endl;
        fairnessWarshallGFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        fairnessWarshallGFile << "set grid back ls 12" << endl;
        //fairnessWarshallGFile << "set style data histogram" << endl;
        //fairnessWarshallGFile << "set style fill solid" << endl;
        //fairnessWarshallGFile << "set boxwidth 0.9" << endl;
        fairnessWarshallGFile << "set yrange [:]" << endl;
        fairnessWarshallGFile << "set xrange [0:" << maxIterations << "]"
                << endl;
        fairnessWarshallGFile << "set offsets graph 0, 0, 0.05, 0.05" << endl;
        //fairnessWarshallGFile << "set xtics rotate by -90 left" << endl;
        fairnessWarshallGFile << "" << endl;
//    fairnessWarshallGFile << "plot '" << topologyName
//            << "_FairnessWarshall.csv' using 2 with linespoints ls 1 title \""
//            << topologyName << "_maxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 3 with linespoints ls 2 title \""
//            << topologyName << "_actualMaxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 4 with linespoints ls 3 title \""
//            << topologyName << "_UnroutedKs\", '" << topologyName
//            << "_FairnessWarshall.csv' using 5 with linespoints ls 4 title \""
//            << topologyName << "_actualUnroutedKs\"" << endl;

        fairnessWarshallGFile << "set ylabel \"MaxDelay\"" << endl;

        fairnessWarshallGFile << "plot '" << topologyName
                << "_FirstImprovement.csv' using 8 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_FirstImprovement.csv' using 9 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"AvgDelay\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_FirstImprovement.csv' using 2 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_FirstImprovement.csv' using 3 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Jain Index\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_FirstImprovement.csv' using 10 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_FirstImprovement.csv' using 11 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"UnroutedKs\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_FirstImprovement.csv' using 4 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_FirstImprovement.csv' using 5 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Decission\"" << endl;
        fairnessWarshallGFile
                << "plot '"
                << topologyName
                << "_FirstImprovement.csv' using 7:ytic($7==1?\"Rjt\":$7==2?\"Cnt\":\"Acc\") with points ls 21 title \""
                << topologyName << "_Decission\"" << endl;

        fairnessWarshallGFile.close();
        // Plot:
        string temp;
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_FirstImprovement.gnuplot";
        system(temp.c_str());
    }

}

/*
 *  Solve by Simulated Annealing
 *  The fairness warshall will try to maximize the criteria:
 *  @criteria: "UnroutedK" -> then maximize AvgDelay
 *  @criteria: "AvgDelay"-> while not decreasing UnroutedK
 *  @criteria: "MaxDelay" -> while not decreasing UnroutedK
 *  @criteria: "AvgDelayJainI"-> while not decreasing UnroutedK
 */
void Topology::solveBySimulatedAnnealing(bool writeFile, bool writeFileAdv,
        string criteria, int maxIterations, int maxTemp, bool pareto) {

    int initialglobalMaxMaxDelay;
    int initialglobalmaxAvgDelay;
    int initialglobalUnroutedKs;
    double initialglobalJ;
    int actualglobalMaxMaxDelay;
    int actualglobalmaxAvgDelay;
    int actualglobalUnroutedKs;
    double actualglobalJ;
    char bestLogTopology[parsedK + 1][nodeNum][nodeNum];
    int iteration = 0;
    string intString;
    string tempName = this->topologyName;
    char intStr[10];
    double Temperature = maxTemp;

    /// Init Topology Solver
    this->clearLogTopology();
    solved = true;
    if (topologyName == "Unnamed") {
        topologyName = "SimulatedAnnealing";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by Simulated Annealing method." << endl;

    // Initial Topology
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    this->solveByFairnessBlossom1(); // Puedo resolverla de cualquier forma

    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->saveStats(false);
    }

    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
    actualglobalUnroutedKs = this->globalUnroutedKs;
    actualglobalJ = this->globalJ;
    initialglobalMaxMaxDelay = this->globalmaxMaxDelay;
    initialglobalmaxAvgDelay = this->globalmaxAvgDelay;
    initialglobalUnroutedKs = this->globalUnroutedKs;
    initialglobalJ = this->globalJ;

    // If pareto enabled
    ofstream paretoADFile;
    ofstream paretoUTFile;
    ofstream paretoJIFile;
    if (pareto == true) {
        paretoADFile.open("tmp/paretoADFile");
        paretoUTFile.open("tmp/paretoUTFile");
        paretoJIFile.open("tmp/paretoJIFile");
    }

    // This is our best topology so far
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                bestLogTopology[k][i][j] = logTopology[k][i][j];
            }
        }
    }
    if (debug) {
        cout << endl << "globalMaxAvgDelay: " << actualglobalmaxAvgDelay
                << endl;
        cout << "globalUnroutedKs: " << actualglobalUnroutedKs << endl;
        cout << "globalMaxMaxDelay: " << actualglobalMaxMaxDelay << endl;

    }


    ofstream fairnessWarshallFile;
    ofstream fairnessWarshallGFile;

    if (writeFile) {
        /// FairnessWarshall csv fileName:
        string fairnessWarshallFileName = "out/";
        fairnessWarshallFileName += tempName;
        fairnessWarshallFileName += "_SimulatedAnnealing.csv";

        fairnessWarshallFile.open(fairnessWarshallFileName.c_str());

        /// FairnessWarshall gnuplot fileName:
        string fairnessWarshallGFileName = "out/";
        fairnessWarshallGFileName += tempName;
        fairnessWarshallGFileName += "_SimulatedAnnealing.gnuplot";

        fairnessWarshallGFile.open(fairnessWarshallGFileName.c_str());
    }

    if (writeFile) {
        fairnessWarshallFile
                << "iteration,maxAvgDelay,actualmaxAvgDelay,UnroutedKs,actualUnroutedKs,Temp,BadNeighbour,MaxMaxDelay,actualMaxMaxDelay,Jain,actualJain"
                << endl;
        fairnessWarshallFile << iteration << "," << this->globalmaxAvgDelay
                << "," << actualglobalmaxAvgDelay << ","
                << this->globalUnroutedKs << "," << actualglobalUnroutedKs
                << "," << "NaN" << "," << "NaN" << ","
                << this->globalmaxMaxDelay << "," << actualglobalMaxMaxDelay
                << "," << initialglobalJ << "," << actualglobalJ << endl;
    }

    for (iteration = 1; iteration <= maxIterations; iteration++) {
        // Branch-and-Exchange
        // rand() % (high - low + 1) + low

        if (debug)
            cout << "Simulated Annealing Iteration " << iteration << " with T: "
                    << Temperature << endl;
        int randK=0;
        int randI=0;
        int randJ=0;
        bool changeDetected = false;
        int changeCounter = 0;

        while ((changeDetected == false) && (changeCounter <= 20000)) {
            changeCounter++;
            randK = rand() % (parsedK) + 1; // between 1 and parsedK
            randI = rand() % nodeNum; // between 0 and nodeNum-1
            int phyArcs = 0;
            int logArcs = 0;
            for (int j = 0; j < nodeNum; j++) {
                if (phyTopology[randK][randI][j] >= 1) {
                    phyArcs++;
                    if (logTopology[randK][randI][j] >= 1) {
                        randJ = j;
                        logArcs++;
                    }
                }
            }
            if (phyArcs > 1) { // maxInterfaces[randI]=1
                changeDetected = true;
                if (debug)
                    cout << " - Change Detected for k: " << randK << " node i: "
                            << randI << " whith " << phyArcs
                            << " physical Arcs (logical j: " << randJ << ")"
                            << endl;
            } else {
                if (debug)
                    cout << " - No Change Detected for k: " << randK
                            << " node i: " << randI << " whith " << phyArcs
                            << " physical Arcs" << endl;
            }
            if (logArcs == 0) {
                //no hay logs para cambiar! que buen issue que encontre :)
                changeDetected = false;
            }
        }

        if (changeCounter >= 20000) {
            if (debug)
                cout << "Cant detect Change for " << this->topologyName << endl;
            iteration = maxIterations + 1; // Dont search for a new change
            // Como los randJIK=0 no hay drama por que no se cambia la topo.
        }


//        // We now know that at randK, I, J we need to disable for new Blossom calculation
//        // Store log in temp:
//        for (int i = 0; i < (numSat + numET); i++) {
//            for (int j = 0; j < (numSat + numET); j++) {
//                tempLogTopology[i][j] = logTopology[randK][i][j];
//            }
//        }
        char tempPhy = phyTopology[randK][randI][randJ];
        phyTopology[randK][randI][randJ] = 0;
        phyTopology[randK][randJ][randI] = 0;
        solveKBlossom(randK, true);
        phyTopology[randK][randI][randJ] = tempPhy; // Restore previous Phy
        phyTopology[randK][randJ][randI] = tempPhy;


        // Update Name
        sprintf(intStr, "%03d", iteration);
        intString = string(intStr);
        this->topologyName = tempName;
        this->topologyName += intString;


        // Solve Routes Again
        if (writeFileAdv) {
            this->floydWarshall(true, false);
            this->saveStats(true); // We get globalmaxAvgDelay calculated
            this->saveDOT();
        } else {
            this->floydWarshall(false, false);
            this->saveStats(false);
        }

        if (pareto == true) {
            paretoADFile << this->globalmaxAvgDelay << endl;
            paretoUTFile << this->globalUnroutedKs << endl;
            paretoJIFile << this->globalJ << endl;
        }

        // Betterness criteria:
        bool acceptedAndBad = false;
        bool acceptedAndGood = false;
        bool rejected = false;
        if (criteria == "UnroutedK") {
            // If better UnroutedKs, adopt no matter AvgDelay
            // Then, if worst UnroutedKs, reject no matter better Avg Delay
            // If UnroutedK the same, evaluate Avg Delay
            if (this->globalUnroutedKs < actualglobalUnroutedKs) {
                // Adopt
                if (debug)
                    cout << "Wow! we improved globalUnroutedKs from "
                            << actualglobalUnroutedKs << " to "
                            << this->globalUnroutedKs << endl;
                acceptedAndGood = true;
                actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                actualglobalUnroutedKs = this->globalUnroutedKs;
                actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                actualglobalJ = this->globalJ;
                // Copy to best Log Topo
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            bestLogTopology[k][i][j] = logTopology[k][i][j];
                        }
                    }
                }
            } else if (this->globalUnroutedKs > actualglobalUnroutedKs) {
                // Reject
                if (debug)
                    cout << " - Oh! no improvement of globalUnroutedKs from "
                            << actualglobalUnroutedKs << " to "
                            << this->globalUnroutedKs << "and maxAvgDelay from "
                            << actualglobalmaxAvgDelay << " to "
                            << this->globalmaxAvgDelay << endl;
                if (exp(
                        -(this->globalUnroutedKs - actualglobalUnroutedKs)
                                / Temperature)
                        > ((double) rand() / (RAND_MAX))) {
                    if (debug)
                        cout << " - But we accept the neighbour with T:"
                                << Temperature << endl;
                    acceptedAndBad = true;
                } else {
                    if (debug)
                        cout << " - We also reject the neighbour with T:"
                                << Temperature << endl;
                    rejected = true;
                    // Restore Best Logical:
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                logTopology[k][i][j] = bestLogTopology[k][i][j];
                            }
                        }
                    }
                }

            } else {
                // Evaluate AvgDelay
                if (this->globalmaxAvgDelay < actualglobalmaxAvgDelay) {
                    if (debug)
                        cout << " - Wow! we improved maxAvgDelay from "
                                << actualglobalmaxAvgDelay << " to "
                                << this->globalmaxAvgDelay << endl;
                    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                    actualglobalUnroutedKs = this->globalUnroutedKs;
                    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                    actualglobalJ = this->globalJ;
                    acceptedAndGood = true;
                    // Copy to best Log Topo
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                bestLogTopology[k][i][j] = logTopology[k][i][j];
                            }
                        }
                    }
                } else {
                    if (debug)
                        cout
                                << " - Oh! no improvement of globalUnroutedKs from "
                                << actualglobalUnroutedKs << " to "
                                << this->globalUnroutedKs
                                << "and maxAvgDelay from "
                                << actualglobalmaxAvgDelay << " to "
                                << this->globalmaxAvgDelay << endl;
                    if (exp(
                            -(this->globalUnroutedKs - actualglobalUnroutedKs)
                                    / Temperature)
                            > ((double) rand() / (RAND_MAX))) {
                        if (debug)
                            cout << " - But we accept the neighbour with T:"
                                    << Temperature << endl;
                        acceptedAndBad = true;
                    } else {
                        if (debug)
                            cout << " - We also reject the neighbour with T:"
                                    << Temperature << endl;
                        rejected = true;
                        // Restore Best Logical:
                        for (int k = 1; k < parsedK + 1; k++) {
                            for (int i = 0; i < (numSat + numET); i++) {
                                for (int j = 0; j < (numSat + numET); j++) {
                                    logTopology[k][i][j] =
                                            bestLogTopology[k][i][j];
                                }
                            }
                        }
                    }
                }
            }
        }
        //Other Criteria:
        if (criteria == "AvgDelayJainI") {

            // If delay Improves
            if ((this->globalmaxAvgDelay < actualglobalmaxAvgDelay)
                    && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                //If Jain Index Improves (higher)
                if (this->globalJ > actualglobalJ) {
                    //Adopt
                    acceptedAndGood = true;
                    actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                    actualglobalUnroutedKs = this->globalUnroutedKs;
                    actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                    actualglobalJ = this->globalJ;
                    // Copy to best Log Topo
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                bestLogTopology[k][i][j] = logTopology[k][i][j];
                            }
                        }
                    }
                    //If Jain Index does not Improves (lower)
                } else {
                    //Temp Function
                    if (exp((this->globalJ - actualglobalJ) * 100 / Temperature)
                            > ((double) rand() / (RAND_MAX))) {
                        if (debug)
                            cout << " - But we accept the neighbour with T:"
                                    << Temperature << endl;
                        acceptedAndBad = true;
                    } else {
                        if (debug)
                            cout << " - We also reject the neighbour with T:"
                                    << Temperature << endl;
                        rejected = true;
                        // Restore Best Logical:
                        for (int k = 1; k < parsedK + 1; k++) {
                            for (int i = 0; i < (numSat + numET); i++) {
                                for (int j = 0; j < (numSat + numET); j++) {
                                    logTopology[k][i][j] =
                                            bestLogTopology[k][i][j];
                                }
                            }
                        }
                    }

                }
            } else {
                // No delay impro, pero me la juego
                if (exp(
                        -(this->globalmaxAvgDelay - actualglobalmaxAvgDelay)
                                / Temperature)
                        > ((double) rand() / (RAND_MAX))) {
                    if (debug)
                        cout << " - But we accept the neighbour with T:"
                                << Temperature << endl;
                    acceptedAndBad = true;
                } else {
                    if (debug)
                        cout << " - We also reject the neighbour with T:"
                                << Temperature << endl;
                    rejected = true;
                    // Restore Best Logical:
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                logTopology[k][i][j] = bestLogTopology[k][i][j];
                            }
                        }
                    }
                }
            }
        }

        if (criteria == "AvgDelay") {
            // Criteria Description
            // If AvgDelay improves AND UnroutedKs is equal or better, adopt.
            // Else, reject or continue.
            if ((this->globalmaxAvgDelay < actualglobalmaxAvgDelay)
                    && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                // Adopt
                if (debug)
                    cout << "Wow! we improved globalAvgDelay from "
                            << actualglobalmaxAvgDelay << " to "
                            << this->globalmaxAvgDelay << endl;
                acceptedAndGood = true;
                actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                actualglobalUnroutedKs = this->globalUnroutedKs;
                actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                actualglobalJ = this->globalJ;
                // Copy to best Log Topo
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            bestLogTopology[k][i][j] = logTopology[k][i][j];
                        }
                    }
                }
            } else {
                // Reject
                if (debug)
                    cout << " - Oh! no improvement of globalmaxAvgDelay from "
                            << actualglobalmaxAvgDelay << " to "
                            << this->globalmaxAvgDelay << "and UnroutedKs from "
                            << actualglobalUnroutedKs << " to "
                            << this->globalUnroutedKs << endl;
                // Shall we accept it anyway/
                if (exp(
                        -(this->globalmaxAvgDelay - actualglobalmaxAvgDelay)
                                / Temperature)
                        > ((double) rand() / (RAND_MAX))) {
                    if (debug)
                        cout << " - But we accept the neighbour with T:"
                                << Temperature << endl;
                    acceptedAndBad = true;
                } else {
                    if (debug)
                        cout << " - We also reject the neighbour with T:"
                                << Temperature << endl;
                    rejected = true;
                    // Restore Best Logical:
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                logTopology[k][i][j] = bestLogTopology[k][i][j];
                            }
                        }
                    }
                }
            }
        }
        //Other Criteria:
        if (criteria == "MaxDelay") {
            // Criteria Description
            // If MaxDelay improves AND UnroutedKs is equal or better, adopt.
            // Else, reject or continue.
            if ((this->globalmaxMaxDelay < actualglobalMaxMaxDelay)
                    && (this->globalUnroutedKs <= actualglobalUnroutedKs)) {
                // Adopt
                if (debug)
                    cout << "Wow! we improved globalmaxMaxDelay from "
                            << actualglobalMaxMaxDelay << " to "
                            << this->globalmaxMaxDelay << endl;
                acceptedAndGood = true;
                actualglobalmaxAvgDelay = this->globalmaxAvgDelay;
                actualglobalUnroutedKs = this->globalUnroutedKs;
                actualglobalMaxMaxDelay = this->globalmaxMaxDelay;
                actualglobalJ = this->globalJ;
                // Copy to best Log Topo
                for (int k = 1; k < parsedK + 1; k++) {
                    for (int i = 0; i < (numSat + numET); i++) {
                        for (int j = 0; j < (numSat + numET); j++) {
                            bestLogTopology[k][i][j] = logTopology[k][i][j];
                        }
                    }
                }
            } else {
                // Reject
                if (debug)
                    cout << " - Oh! no improvement of globalmaxMaxDelay from "
                            << actualglobalMaxMaxDelay << " to "
                            << this->globalmaxMaxDelay << "and UnroutedKs from "
                            << actualglobalUnroutedKs << " to "
                            << this->globalUnroutedKs << endl;
                // Shall we accept it anyway/
                if (exp(
                        -(this->globalmaxMaxDelay - actualglobalMaxMaxDelay)
                                / Temperature)
                        > ((double) rand() / (RAND_MAX))) {
                    if (debug)
                        cout << " - But we accept the neighbour with T:"
                                << Temperature << endl;
                    acceptedAndBad = true;
                } else {
                    if (debug)
                        cout << " - We also reject the neighbour with T:"
                                << Temperature << endl;
                    rejected = true;
                    // Restore Best Logical:
                    for (int k = 1; k < parsedK + 1; k++) {
                        for (int i = 0; i < (numSat + numET); i++) {
                            for (int j = 0; j < (numSat + numET); j++) {
                                logTopology[k][i][j] = bestLogTopology[k][i][j];
                            }
                        }
                    }
                }
            }
        }

        if (debug) {
            cout << " - globalMaxAvgDelay: " << actualglobalmaxAvgDelay << endl;
            cout << " - globalUnroutedKs: " << actualglobalUnroutedKs << endl;
            cout << " - globalMaxMaxDelay: " << actualglobalMaxMaxDelay << endl;

        }
        if (writeFile) {
            fairnessWarshallFile << iteration << "," << this->globalmaxAvgDelay
                    << "," << actualglobalmaxAvgDelay << ","
                    << this->globalUnroutedKs << "," << actualglobalUnroutedKs
                    << "," << Temperature << ",";
            if (acceptedAndGood == true) {
                fairnessWarshallFile << 3;
            } else if (acceptedAndBad == true) {
                fairnessWarshallFile << 2;
            } else if (rejected) {
                fairnessWarshallFile << 1;
            }
            fairnessWarshallFile << "," << this->globalmaxMaxDelay << ","
                    << actualglobalMaxMaxDelay << "," << this->globalJ << ","
                    << actualglobalJ << endl;
        }

        // PROBAR ESTO!
        /// if (Temperature > 0) {
        if (Temperature > 0 && (!rejected)) {
            Temperature = Temperature - 1;
        }

    } /// End main for!

    // Final Topology:
    // Restore Best Logical:
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = bestLogTopology[k][i][j];
            }
        }
    }

    // Update Name
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    // Solve Routes Again
    if (writeFileAdv) {
        this->floydWarshall(true, false);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        this->saveDOT();
    } else {
        this->floydWarshall(false, false);
        this->saveStats(false);
    }

    if (writeFile) {
        fairnessWarshallFile.close();
    }

    // Print Result:
    if (debug) {
        cout << endl << "maxAvgDelay: " << initialglobalmaxAvgDelay << " -> "
                << actualglobalmaxAvgDelay << " (" << globalmaxAvgDelay << ")"
                << endl;
        cout << "maxMaxDelay: " << initialglobalMaxMaxDelay << " -> "
                << actualglobalMaxMaxDelay << " (" << globalmaxMaxDelay << ")"
                << endl;
        cout << "UnroutedKs:  " << initialglobalUnroutedKs << " -> "
                << actualglobalUnroutedKs << endl;
    }

    // Restore TopologyName:
    this->topologyName = tempName;

    // Write GnuPlot and plot
    if (writeFile) {
        fairnessWarshallGFile << "set terminal svg size 600,1440 dashed"
                << endl;
        fairnessWarshallGFile << "set output \"" << topologyName
                << "_SimulatedAnnealing.svg\"" << endl;
        fairnessWarshallGFile << "set multiplot layout 6,1" << endl;

        fairnessWarshallGFile << "set key" << endl;
        fairnessWarshallGFile << "set datafile separator \",\"" << endl;
        fairnessWarshallGFile << "set xlabel \"Iteration\"" << endl;
        fairnessWarshallGFile
                << "set style line 1 lc rgb '#0025ad' pt 7 lt 1 lw 1 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 2 lc rgb '#0025ad' pt 7 lt 3 lw 2 # --- blue"
                << endl;
        fairnessWarshallGFile
                << "set style line 3 lc rgb '#09ad00' pt 7 lt 1 lw 1 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 4 lc rgb '#09ad00' pt 7 lt 3 lw 2 # --- green"
                << endl;
        fairnessWarshallGFile
                << "set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange"
                << endl;
        fairnessWarshallGFile
                << "set style line 6 lc rgb '#ba55cc' pt 11 lt 1 lw 1 # --- violet"
                << endl;
        fairnessWarshallGFile
                << "set style line 20 lc rgb '#000000' pt 2 lt 1 lw 1 # line and X"
                << endl;
        fairnessWarshallGFile
                << "set style line 21 lc rgb '#000000' pt 3 lt 1 lw 1 # line and *"
                << endl;
        fairnessWarshallGFile
                << "set style line 22 lc rgb '#000000' pt 3 lt 1 lw 1 # line and cuadradito vacio"
                << endl;
        fairnessWarshallGFile
                << "set style line 30 lc rgb '#000000' pt 3 lt 15 lw 1 # dot-dash and cuadradito vacio"
                << endl;
        fairnessWarshallGFile << "set style line 11 lc rgb '#808080' lt 1 "
                << endl;
        //fairnessWarshallGFile << "set border 3 back ls 11" << endl;
        fairnessWarshallGFile << "set tics nomirror" << endl;
        fairnessWarshallGFile << "set style line 12 lc rgb '#808080' lt 0 lw 1"
                << endl;
        fairnessWarshallGFile << "set grid back ls 12" << endl;
        //fairnessWarshallGFile << "set style data histogram" << endl;
        //fairnessWarshallGFile << "set style fill solid" << endl;
        //fairnessWarshallGFile << "set boxwidth 0.9" << endl;
        fairnessWarshallGFile << "set yrange [:]" << endl;
        fairnessWarshallGFile << "set xrange [0:" << maxIterations << "]"
                << endl;
        fairnessWarshallGFile << "set offsets graph 0, 0, 0.05, 0.05" << endl;
        //fairnessWarshallGFile << "set xtics rotate by -90 left" << endl;
        fairnessWarshallGFile << "" << endl;
//    fairnessWarshallGFile << "plot '" << topologyName
//            << "_FairnessWarshall.csv' using 2 with linespoints ls 1 title \""
//            << topologyName << "_maxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 3 with linespoints ls 2 title \""
//            << topologyName << "_actualMaxAvgDelay\", '" << topologyName
//            << "_FairnessWarshall.csv' using 4 with linespoints ls 3 title \""
//            << topologyName << "_UnroutedKs\", '" << topologyName
//            << "_FairnessWarshall.csv' using 5 with linespoints ls 4 title \""
//            << topologyName << "_actualUnroutedKs\"" << endl;

        fairnessWarshallGFile << "set ylabel \"MaxDelay\"" << endl;

        fairnessWarshallGFile << "plot '" << topologyName
                << "_SimulatedAnnealing.csv' using 8 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_SimulatedAnnealing.csv' using 9 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"AvgDelay\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SimulatedAnnealing.csv' using 2 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_SimulatedAnnealing.csv' using 3 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Jain Index\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SimulatedAnnealing.csv' using 10 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_SimulatedAnnealing.csv' using 11 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"UnroutedKs\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SimulatedAnnealing.csv' using 4 with lines ls 30 title \""
                << topologyName << "_Actual\", '" << topologyName
                << "_SimulatedAnnealing.csv' using 5 with lines ls 21 title \""
                << topologyName << "_Best\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Decission\"" << endl;
        fairnessWarshallGFile
                << "plot '"
                << topologyName
                << "_SimulatedAnnealing.csv' using 7:ytic($7==1?\"Rjt\":$7==2?\"Cnt\":\"Acc\") with points ls 21 title \""
                << topologyName << "_Decission\"" << endl;

        fairnessWarshallGFile << "set ylabel \"Temperature\"" << endl;
        fairnessWarshallGFile << "plot '" << topologyName
                << "_SimulatedAnnealing.csv' using 6 with lines ls 21 title \""
                << topologyName << "_Temperature\"" << endl;

        fairnessWarshallGFile.close();
        // Plot:
        string temp;
        temp.clear();
        temp += " cd out && gnuplot "; // Concatenate two Linux Commands
        temp += topologyName.c_str();
        temp += "_SimulatedAnnealing.gnuplot";
        system(temp.c_str());
    } // End write FIle

    // If pareto process!
    if (pareto == true) {
        bool rest = true;
        paretoUTFile.close();
        paretoJIFile.close();
        paretoADFile.close();
        // Generateoutput
        // ./pareto -l example.file1 -l example.file2 > example.result
        // The flag -l indicates larger number is better (maximize);
        // alternatively, the -s flag means smaller is better (minimize).
        system(
                "./incl/pareto -s tmp/paretoADFile -l tmp/paretoJIFile -s tmp/paretoUTFile > tmp/paretoResult");
        // Now open the file save a csv with points
        ifstream paretoADFilein;
        ifstream paretoUTFilein;
        ifstream paretoJIFilein;
        paretoADFilein.open("tmp/paretoADFile");
        paretoUTFilein.open("tmp/paretoUTFile");
        paretoJIFilein.open("tmp/paretoJIFile");
        ifstream paretoResultin;
        paretoResultin.open("tmp/paretoResult");
        ofstream paretoResultCsv;
        paretoResultCsv.open("out/paretoResult_Front.csv");
        ofstream paretoResultCsvRest;
        paretoResultCsvRest.open("out/paretoResult_Rest.csv");
        string StringRes, StringAD, StringUT, StringJI;
        size_t found1;
        paretoResultCsv << "AD" << "," << "UT" << "," << "JI" << endl;
        paretoResultCsvRest << "AD" << "," << "UT" << "," << "JI" << endl;
        while (paretoResultin.eof() == false) {
            getline(paretoResultin, StringRes);
            getline(paretoADFilein, StringAD);
            getline(paretoUTFilein, StringUT);
            getline(paretoJIFilein, StringJI);
            found1 = StringRes.find("1");
            if (found1 != string::npos) { /// We found 1!
                float AD = atof(StringAD.c_str());
                float UT = atof(StringUT.c_str());
                float JI = atof(StringJI.c_str());
                paretoResultCsv << AD << "," << UT << "," << JI << endl;
            } else { /// Dominated option
                if (rest) {
                    float AD = atof(StringAD.c_str());
                    float UT = atof(StringUT.c_str());
                    float JI = atof(StringJI.c_str());
                    paretoResultCsvRest << AD << "," << UT << "," << JI << endl;
                }
            }
        }
        paretoADFilein.close();
        paretoUTFilein.close();
        paretoJIFilein.close();
        paretoResultin.close();
        paretoResultCsv.close();

        // Lets plot something
        ofstream paretoResultGplot;
        paretoResultGplot.open("out/paretoResult.gnuplot");
        paretoResultGplot << "set terminal svg size 700,400" << endl;
        paretoResultGplot << "set output \"paretoResult.svg\"" << endl;
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
        paretoResultGplot << "set yrange [10:60]" << endl;
        //paretoResultGplot << "set xrange [0:30]" << endl;
        //paretoResultGplot << "set zrange [(0.1):(0.7)]" << endl;

//        paretoResultGplot
//                << "splot 'paretoResult_Front.csv' using 1:2:3 with pm3d title \"Pareto Frontier\""
//                << endl;
        paretoResultGplot
                << "splot 'paretoResult_Front.csv' using 1:2:(0.0):3  with points pointsize 1 pointtype 7 palette title \"Pareto Frontier\", 'paretoResult_Rest.csv' using 1:2:(0.0):3  with points pointsize 1 pointtype 2 palette title \"Pareto Dominated\""
                << endl;
        paretoResultGplot.close();
        // Plot
        system(" cd out && gnuplot paretoResult.gnuplot");

    } // End Pareto

}

/*
 *  Solve by Fairness Warshall
 *  @criteria: "UnroutedK"
 *  @criteria: "AvgDelay"
 *  @criteria: "MaxMaxDelay"
 *  @src: source to optimize
 *  @dst: dest to optimize
 */
void Topology::solveByFairnessWarshall02(bool writeFile, bool writeFileAdv,
        int maxIterations, int maxTemp, string criteria, int src, int dst) {

    bool writeFile2 = writeFileAdv; //Advanced Write File
    solved = true;

    /// Init Topology Solver
    this->clearLogTopology();

    if (topologyName == "Unnamed") {
        topologyName = "FairnessWarshall";
    }
    cout << endl << "Solving Topology " << topologyName
            << " by FairnessWarshall 01 method." << endl;

    ofstream fairnessWarshallFile;
    ofstream fairnessWarshallGFile;

    if (writeFile) {
        /// FairnessWarshall csv fileName:
        string fairnessWarshallFileName = "out/";
        fairnessWarshallFileName += topologyName;
        fairnessWarshallFileName += "_FairnessWarshall.csv";

        fairnessWarshallFile.open(fairnessWarshallFileName.c_str());

        /// FairnessWarshall gnuplot fileName:
        string fairnessWarshallGFileName = "out/";
        fairnessWarshallGFileName += topologyName;
        fairnessWarshallGFileName += "_FairnessWarshall.gnuplot";

        fairnessWarshallGFile.open(fairnessWarshallGFileName.c_str());
    }

//    int initialglobalMaxMaxDelay;
//    int initialglobalmaxAvgDelay;
//    int initialglobalUnroutedKs;
//    int actualglobalMaxMaxDelay;
//    int actualglobalmaxAvgDelay;
//    int actualglobalUnroutedKs;
//    //char tempLogTopology[nodeNum][nodeNum];
//    char bestLogTopology[parsedK + 1][nodeNum][nodeNum];
//    double Temperature = maxTemp;
    int iteration = 0;
    string intString;
    string tempName = this->topologyName;
    char intStr[10];

    // Initial Topology
    sprintf(intStr, "%03d", iteration);
    intString = string(intStr);
    this->topologyName = tempName;
    this->topologyName += intString;

    // Solve Initial Topology
    solved = false; // cheat FW in order to gey phy sol
    if (writeFile2) {
        this->floydWarshall(true, true);
        this->saveStats(true); // We get globalmaxAvgDelay calculated
        //this->saveDOT();
    } else {
        this->floydWarshall(false, true);
        this->evaluateRoutes(); // We get globalmaxAvgDelay calculated
    }
    solved = true;

    // Detect Used Arcs and create LogTopo
    this->clearLogTopology();

    // Enable all links used by a Particular Route given
    // by src and dst in function parameter
    for (int k = 1; k < parsedK + 1; k++) {
        if (debug)
            cout << "K:" << k << ", src:" << src << ", nHop:"
                    << nHop[k][src][dst] << ", distance:"
                    << distance[k][src][nHop[k][src][dst]] << endl;
        // If we have an outgoing route at this k, follow route until destiny
        if (distance[k][src][nHop[k][src][dst]] == 0) {
            int auxK = k;
            int auxSrc = src;
            while (auxSrc != dst) {
                // advance k if nex hop is not now
                if (distance[auxK][auxSrc][nHop[auxK][auxSrc][dst]] != 0) {
                    auxK++;
                } else { // turn arc on and move to next hop
                    if (debug)
                        cout << "Log_" << auxK << "." << auxSrc << "."
                                << nHop[auxK][auxSrc][dst] << " is on!, ";
                    logTopology[auxK][auxSrc][nHop[auxK][auxSrc][dst]] =
                            phyTopology[auxK][auxSrc][nHop[auxK][auxSrc][dst]];
                    logTopology[auxK][nHop[auxK][auxSrc][dst]][auxSrc] =
                            phyTopology[auxK][nHop[auxK][auxSrc][dst]][auxSrc];
                    // move to next hop
                    if (debug)
                        cout << "moving from node:" << auxSrc << " to "
                                << nHop[auxK][auxSrc][dst];
                    auxSrc = nHop[auxK][auxSrc][dst];
                    if (debug)
                        cout
                                << ", nHop is "
                                << nHop[auxK][auxSrc][dst]
                                << ", distance: "
                                << distance[auxK][auxSrc][nHop[auxK][auxSrc][dst]]
                                << ", to dst:" << distance[auxK][auxSrc][dst]
                                << endl;
                }
            }
        }
    }

    this->saveDOT();

}

void Topology::checkSimetry(void) {
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if ((int) phyTopology[k][i][j] != (int) phyTopology[k][j][i]) {
                    cout << this->topologyName
                            << " Is not symetric in [k][i][j]:" << k << "-" << i
                            << "-" << j << endl;
                    exit(1);
                }
            }
        }
    }
}

bool Topology::checkSingleInterface(void) {
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            int intUsed = 0;
            for (int j = 0; j < nodeNum; j++) {
                if (logTopology[k][i][j] >= 1) {
                    intUsed++;
                }
            }
            if (intUsed > 1) {
                return false;
            }
        }
    }
    return true;
}

void Topology::printPhyK(int k) {
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (phyTopology[k][i][j] == -1)
                cout << "*,";
            if (phyTopology[k][i][j] != -1)
                cout << (int) phyTopology[k][i][j] << ",";
        }
        cout << endl;
    }
    cout << endl;
}

/*
 * Copy Physical to Logical topology

 */
void Topology::phyToLog(void) {
    /// Initialize the Topology to 0 (*) in all k state
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = phyTopology[k][i][j];
            }
        }
    }
}

/**
 *  Set Logical Topology to all 0s (inactive) links.
 *  And also erases all weights since they might have
 *  been modified when using algorithms.
 */
void Topology::clearLogTopology() {
    /// Initialize the Topology to 0 (*) in all k state
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                logTopology[k][i][j] = 0;
                arcWeight[k][i][k] = 1;
            }
        }
    }
    /// Restore LP local variables
    this->tmax = 0;
    this->tmin = 0;

    this->solved = false;

    ///
}

/**
 * Function to randomize the physical topology
 * and weight array. Phy will be set between 0
 * and 1 with equal probability. Weight will
 * be set between the two parameters:
 *
 */
void Topology::randomizeTopo(int lowTime, int highTime, int linkDensity) {

    if (debug) {
        cout << "Generating Random Topology with " << numSat << " Sat, "
                << numET << " ET, and " << parsedK << " states with "
                << linkDensity << "% of link density and duration from "
                << lowTime << " to " << highTime << "." << endl;
    }

    for (int k = 1; k < parsedK + 1; k++) {
        t[k] = rand() % (highTime - lowTime + 1) + lowTime; /// Radom Time
        for (int i = 0; i < (numSat + numET); i++) /// Go through a Matrix
                {
            for (int j = i; j < (numSat + numET); j++) //recorro un renglon
                    {
                if (i == j) {
                    phyTopology[k][i][j] = phyTopology[k][j][i] = -1;
                } else {
                    if ((rand() % 100) < linkDensity) { // Between 0 and 99
                        phyTopology[k][i][j] = phyTopology[k][j][i] = 1;
                    } else {
                        phyTopology[k][i][j] = phyTopology[k][j][i] = 0;
                    }
                }
            }
        }
    }
}

/**
 * Private Function that parses the topology from a
 * text file. Only called from overload constructor. The
 * topology parsed is always the physical. Here parsedK,
 * numSat and numET topology variables gets populated.
 *
 * @param fileName Text file to parse the topology from
 */
void Topology::readTopoFromFile(string fileName) {
    string str;
    ifstream file;
    file.open(fileName.c_str());

    cout << "Parsing Topology from file: " << fileName << endl;

    /// Parsing Header:
    for (int i = 0; i < 7; i++) {
        getline(file, str, '\n');
        if (str.compare(0, 6, "numSat") == 0) {
            numSat = atoi(str.substr(7, 1).c_str());
        }
        if (str.compare(0, 5, "numET") == 0) {
            numET = atoi(str.substr(6, 1).c_str());
        }
    }

    /// Check if the Node number is not too big
    if ((numSat + numET) >= MAXS) {
        cout << "Too many nodes to parse: " << (numSat + numET) << endl;
        exit(1);
    }

    /// Parsing Topology Body
    for (int k = 1; (k < MAXK) && (file.eof() == false); k++) {

        for (int i = 0; i < (numSat + numET); i++) /// Im on a matrix
                {

            for (int j = 0; j < (numSat + numET); j++) /// Im on a line
                    {
                getline(file, str, ',');

                if (str.compare("*") != 0) /// If different from '*'
                        {
                    phyTopology[k][i][j] = atoi(str.c_str()); /// Store value
                } else /// If it is '*'
                {
                    phyTopology[k][i][j] = -1; /// Store '*'
                }
            }
            file.get(); /// Jump next endl
        } /// Done with this line
          /// Done with this matrix

        getline(file, str, '\n'); /// Now Im on "k" line
        if ((str.compare(0, 2, "k=") == 0)) {
            parsedK = atoi(str.substr(2).c_str());
        }
        getline(file, str, '\n'); /// Now Im on "t" line
        if ((str.compare(0, 2, "t=") == 0)) {
            t[k] = atoi(str.substr(2).c_str());
        }
        getline(file, str, '\n');
    } /// Done with the file

    cout << "Parsed Topology from file: " << fileName << " with " << numSat
            << " Sat, " << numET << " ET, and " << parsedK << " states."
            << endl;

    /// Check if there is more K state than MAXK
    if (file.eof() == false) {
        cout << "Warning! file might have more states than parsed (Max K="
                << MAXK << ")";
    }

    file.close();
}

/**
 * The generateContacts function parses the topology in topoNumber
 * and saves it in a contact fashion instead of a matrix form.
 * TopoNumber can be 0, 1, or 2 to parse the physical, or logical
 * topology.
 * This function populates the contactArray of the topology object
 * and then bubble sort it so it can be written later in ION, NetSim
 * or any other contact format.
 */
void Topology::generateContacts() {
    bool ImOnContact;

    /// contactArray pointer
    contact = 0;

    if (this->solved) {
        /// Evaluate i to j for all k and store contacts in contactArray
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) /// We do it symmetrical to keep order
                    {
                ImOnContact = false; /// Start all contact inactive
                tAccum = 0; /// Save accumulated time here
                for (int k = 1; k < parsedK + 1; k++) {
                    if ((logTopology[k][i][j] >= 1) && (!ImOnContact)
                            && (i != j)) { /// If link is active and I'm not on a contact, start contact
                        contactArray[contact][0] = tAccum; /// Start Time
                        ImOnContact = true; /// Turn contact on
                    }
                    if ((!logTopology[k][i][j] >= 1) && (ImOnContact)
                            && (i != j)) { /// If link is inactive and I'm on a contact, end contact
                        contactArray[contact][1] = tAccum; /// End Time
                        contactArray[contact][2] = i; /// Source Node
                        contactArray[contact][3] = j; /// Dest Node
                        ImOnContact = false; /// Turn contact off
                        contact++; /// Prepare to store next contact
                        if (contact >= MAXC) {
                            cout
                                    << "Too many contacts when Generating Contacts (Max is "
                                    << MAXC << ")" << endl;
                            exit(1);
                        }
                    }
                    tAccum += t[k]; /// Acumulate Time
                    /// Evaluate if Im on the last state
                    if ((k == parsedK) && (logTopology[k][i][j] >= 1)
                            && (ImOnContact) && (i != j)) /// If this is the last state
                            { /// End on going contact because topology ended
                        contactArray[contact][1] = tAccum;
                        contactArray[contact][2] = i;
                        contactArray[contact][3] = j;
                        ImOnContact = false; /// Turn contact off
                        contact++; /// Prepare to store next contact
                    }
                }
            }
        }
    } else { // Not Solved: read phy!
        /// Evaluate i to j for all k and store contacts in contactArray
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) /// We do it symmetrical to keep order
                    {
                ImOnContact = false; /// Start all contact inactive
                tAccum = 0; /// Save accumulated time here
                for (int k = 1; k < parsedK + 1; k++) {
                    if ((phyTopology[k][i][j] >= 1) && (!ImOnContact)
                            && (i != j)) { /// If link is active and I'm not on a contact, start contact
                        contactArray[contact][0] = tAccum; /// Start Time
                        ImOnContact = true; /// Turn contact on
                    }
                    if ((!phyTopology[k][i][j] >= 1) && (ImOnContact)
                            && (i != j)) { /// If link is inactive and I'm on a contact, end contact
                        contactArray[contact][1] = tAccum; /// End Time
                        contactArray[contact][2] = i; /// Source Node
                        contactArray[contact][3] = j; /// Dest Node
                        ImOnContact = false; /// Turn contact off
                        contact++; /// Prepare to store next contact
                        if (contact >= MAXC) {
                            cout
                                    << "Too many contacts when Generating Contacts (Max is "
                                    << MAXC << ")" << endl;
                            exit(1);
                        }
                    }
                    tAccum += t[k]; /// Acumulate Time
                    /// Evaluate if Im on the last state
                    if ((k == parsedK) && (phyTopology[k][i][j] >= 1)
                            && (ImOnContact) && (i != j)) /// If this is the last state
                            { /// End on going contact because topology ended
                        contactArray[contact][1] = tAccum;
                        contactArray[contact][2] = i;
                        contactArray[contact][3] = j;
                        ImOnContact = false; /// Turn contact off
                        contact++; /// Prepare to store next contact
                    }
                }
            }
        }
    }

    /// Bubble sort the array by start time (contactArray[][0])
    bool swapped = true;
    int tmp;
    /// First Bubble Sort for end Time
    while (swapped == true) {
        swapped = false;
        for (int b = 1; b < contact; b++) {
            if (contactArray[b - 1][1] > contactArray[b][1]) { /// If Start time is bigger, Swap the values
                tmp = contactArray[b][0];
                contactArray[b][0] = contactArray[b - 1][0];
                contactArray[b - 1][0] = tmp;
                tmp = contactArray[b][1];
                contactArray[b][1] = contactArray[b - 1][1];
                contactArray[b - 1][1] = tmp;
                tmp = contactArray[b][2];
                contactArray[b][2] = contactArray[b - 1][2];
                contactArray[b - 1][2] = tmp;
                tmp = contactArray[b][3];
                contactArray[b][3] = contactArray[b - 1][3];
                contactArray[b - 1][3] = tmp;
                swapped = true;
            }
        }
    }
    swapped = true;
    /// Then Bubble Sort By Start Time
    while (swapped == true) {
        swapped = false;
        for (int b = 1; b < contact; b++) {
            if (contactArray[b - 1][0] > contactArray[b][0]) { /// If Start time is bigger, Swap the values
                tmp = contactArray[b][0];
                contactArray[b][0] = contactArray[b - 1][0];
                contactArray[b - 1][0] = tmp;
                tmp = contactArray[b][1];
                contactArray[b][1] = contactArray[b - 1][1];
                contactArray[b - 1][1] = tmp;
                tmp = contactArray[b][2];
                contactArray[b][2] = contactArray[b - 1][2];
                contactArray[b - 1][2] = tmp;
                tmp = contactArray[b][3];
                contactArray[b][3] = contactArray[b - 1][3];
                contactArray[b - 1][3] = tmp;
                swapped = true;
            }
        }
    }

    if (debug) {
        cout << "Bidirectional contacts parsed: " << contact
                << ", in a total accumulated time of: " << tAccum << "."
                << endl;
    }
}

/**
 * Writes the CPLEX text file with a Max Weight objective
 * function.
 *
 * @param fileName Cplex text file name
 */
void Topology::writeCPLEXMaxWeight(string fileName) {

    cout << endl << "Writing MaxWeight CPLEX in " << fileName << " file"
            << endl;

    ofstream cplexFile;
    cplexFile.open(fileName.c_str());

    cplexFile
            << "\\Archivo CPLEX para el procesamiento lineal de la topologia\n";
    cplexFile
            << "\\Command: glpsol --lp topologyCplex.cplex -o topologyCplexSol.txt\n\n";

    ///Maximize Function:
    cplexFile << "maximize obj: \n";
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    //Con prioridad del Nodo i
                    cplexFile << "+ " << arcWeight[k][i][j] << " v_" << i << "."
                            << k << "_" << j << "." << k << "\n";
                }
            }
        }
    }

    ///Restricciones:
    cplexFile << "\nsubject to\n";
    //Bidireccionalidad
    cplexFile << "\\Bidireccionalidad\n";

    int restNum = 0;
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "bi" << restNum << ": v_" << i << "." << k
                            << "_" << j << "." << k << " - " << "v_" << j << "."
                            << k << "_" << i << "." << k << " = 0\n";
                    restNum++;
                }
            }
        }
    }

    //Transponders
    cplexFile << "\n\\Transponders\n";
    restNum = 0;
    bool writeFlag = 0;
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        cplexFile << "tr" << restNum << ": ";
                        writeFlag = true;
                    }
                    cplexFile << " + v_" << i << "." << k << "_" << j << "."
                            << k;
                    restNum++;
                }
            }
            if (writeFlag == 1) {
                cplexFile << " <=" << maxInterfaces[i] << "\n"; //Max transponders = 1
                writeFlag = false;
            }
        }
    }

    ///Binary restriction:
    cplexFile << "\n\\Binary Variables\n";
    cplexFile << "binary\n";
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "v_" << i << "." << k << "_" << j << "." << k
                            << "\n";
                }
            }
        }
    }

    cplexFile << "end";
    cplexFile.close();
}

/**
 * Writes the CPLEX text file with a objective function
 * of maximizing t_min
 *
 * @param beta fraction of capacity willing to lose to win fairness
 * @param epsilon capacity multiplier for tmin maximization (Max: tmin + Eps[arcTime])
 * @param beta capacity multiplier for tmax minimization (Sub: sum[arc[i][j]]>= Beta*Tobj)
 */
void Topology::writeCPLEXFairnessMaxTmin(string fileName, int Kmin, int Kmax,
        float epsilon) {

    cout << endl << "Writing Topology in " << fileName << " file" << endl;

    ofstream cplexFile;
    cplexFile.open(fileName.c_str());

    cplexFile
            << "\\Archivo CPLEX para el procesamiento lineal de la topologia\n";
    cplexFile
            << "\\Command: glpsol --lp topologyCplex.cplex -o topologyCplexSol.txt\n\n";

    ///Maximize Function:
    cplexFile << "maximize obj: \n";

    /// Tmin
    cplexFile << "+ 1000 t_min\n";

    /// Since k=1 and not Kmin because we set the variables from
    /// k=1 to k=Kmin as fixed. They are taken from previous
    /// solutions saved in logical topology.
    for (int k = 1; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    //Con prioridad del Nodo i
                    cplexFile << "+ " << (epsilon * t[k]) << " v_" << i << "."
                            << k << "_" << j << "." << k << "\n";
                }
            }
        }
    }

    cplexFile << "\n";

    ///Restricciones:
    cplexFile << "\nsubject to\n";

    /// For k=1 to k=Kmin we restore saved variables
    cplexFile << "\\Previously solved arcs:\n";
    for (int k = 1; k < Kmin; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (logTopology[k][i][j] >= 1) {
                    cplexFile << " v_" << i << "." << k << "_" << j << "." << k
                            << "= 1" << endl;
                }
                if (logTopology[k][i][j] == 0) {
                    cplexFile << " v_" << i << "." << k << "_" << j << "." << k
                            << "= 0" << endl;
                }

            }
        }
    }

// sum[sobre k](v_i.k_j.k) >= t_min forall i,j
    cplexFile << "\\Limite de Justicia\n";

    int restNum = 0;
    bool writeFlag = 0;

    /// We start from k=1 as t_min also involves evaluating previous states
    /// Anyway we have previously solved variables fixed.
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = 0; j < (numSat + numET); j++) {
            for (int k = 1; k < Kmax + 1; k++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        // Si es el primero en este esado escribo cabeza:
                        cplexFile << "just" << restNum << ": - t_min ";
                        writeFlag = 1;
                    }
                    cplexFile << "+ " << t[k] << " v_" << i << "." << k << "_"
                            << j << "." << k << " ";
                }
            }
            // Fin, si escribi hago:
            if (writeFlag == 1) {
                restNum++;
                cplexFile << " >= 0\n";
                writeFlag = 0;
            }
        }
    }

    //Bidireccionalidad
    cplexFile << "\\Bidireccionalidad\n";

    restNum = 0;

    for (int k = 1; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "bi" << restNum << ": v_" << i << "." << k
                            << "_" << j << "." << k << " - " << "v_" << j << "."
                            << k << "_" << i << "." << k << " = 0\n";
                    restNum++;
                }
            }
        }
    }

    //Transponders
    cplexFile << "\n\\Transponders\n";

    restNum = 0;
    writeFlag = 0;

    for (int k = Kmin; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        cplexFile << "tr" << restNum << ": ";
                        writeFlag = true;
                    }
                    cplexFile << " + v_" << i << "." << k << "_" << j << "."
                            << k;
                    restNum++;
                }
            }
            if (writeFlag == 1) {
                cplexFile << " <=" << maxInterfaces[i] << "\n"; //Max transponders = 1
                writeFlag = false;
            }
        }
    }

    ///Binary restriction:
    cplexFile << "\n\\Binary Variables\n";
    cplexFile << "binary\n";

    for (int k = Kmin; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "v_" << i << "." << k << "_" << j << "." << k
                            << "\n";
                }
            }
        }
    }

    cplexFile << "end";
    cplexFile.close();
}

/**
 * Writes the CPLEX text file with a objective function
 * of minimizing t_max
 *
 * @param fileName Cplex text file name
 * @param tmin previously calculated t_min
 * @param tobj objective capacity previously achieved
 * @param epsilon capacity multiplier for tmin maximization (Max: tmin + Eps[arcTime])
 * @param beta capacity multiplier for tmax minimization (Sub: sum[arc[i][j]]>= Beta*Tobj)
 */
void Topology::writeCPLEXFairnessMinTmax(string fileName, int Kmin, int Kmax,
        int tobj, float beta) {

    cout << endl << "Writing Topology in " << fileName << " file" << endl;

    ofstream cplexFile;
    cplexFile.open(fileName.c_str());

    cplexFile
            << "\\Archivo CPLEX para el procesamiento lineal de la topologia\n";
    cplexFile
            << "\\Command: glpsol --lp topologyCplex.cplex -o topologyCplexSol.txt\n\n";

    ///Maximize Function:
    cplexFile << "minimize obj: \n";

    /// Tmax is our variable now
    //cplexFile << "+ t_max - " << tmin << "\n";
    cplexFile << "+ t_max \n";

    //cplexFile << "\n";

    ///Restricciones:
    cplexFile << "\nsubject to\n";

    /// For k=1 to k=Kmin we restore saved variables
    cplexFile << "\\Previously solved arcs:\n";
    for (int k = 1; k < Kmin; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (logTopology[k][i][j] >= 1) {
                    cplexFile << " v_" << i << "." << k << "_" << j << "." << k
                            << "= 1" << endl;
                }
                if (logTopology[k][i][j] == 0) {
                    cplexFile << " v_" << i << "." << k << "_" << j << "." << k
                            << "= 0" << endl;
                }
            }
        }
    }

    cplexFile << "\\Capacidad Objetivo\n";
    cplexFile << "Cobj: ";
    for (int k = Kmin; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    //Con prioridad del Nodo i
                    cplexFile << "+ " << t[k] << " v_" << i << "." << k << "_"
                            << j << "." << k << " ";
                }
            }
        }
    }
    cplexFile << " >= " << tobj * beta << "\n";

    int restNum = 0;
    bool writeFlag = 0;

    // sum[sobre k](v_i.k_j.k) >= t_min forall i,j
    cplexFile << "\\Limite de Justicia mayor a tmin\n";

    /// Since k=1 and not Kmin because we set the variables from
    /// k=1 to k=Kmin as fixed. They are taken from previous
    /// solutions saved in logical topology.
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = 0; j < (numSat + numET); j++) {
            for (int k = 1; k < Kmax + 1; k++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        // Si es el primero en este esado escribo cabeza:
                        //cplexFile << "just" << restNum << ": - t_min ";
                        // Now tmin is known and fixed:
                        cplexFile << "just" << restNum << ": ";
                        writeFlag = 1;
                    }
                    cplexFile << "+ " << t[k] << " v_" << i << "." << k << "_"
                            << j << "." << k << " ";
                }
            }
            // Fin, si escribi hago:
            if (writeFlag == 1) {
                restNum++;
                cplexFile << " >= " << tmin << "\n";
                writeFlag = 0;
            }
        }
    }

    // sum[sobre k](v_i.k_j.k) >= t_min forall i,j
    cplexFile << "\\Limite de Justicia menor a tmax\n";

    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = 0; j < (numSat + numET); j++) {
            for (int k = 1; k < Kmax + 1; k++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        // Si es el primero en este esado escribo cabeza:
                        //cplexFile << "just" << restNum << ": - t_min ";
                        // Now tmin is known and fixed:
                        cplexFile << "just" << restNum << ": - t_max ";
                        writeFlag = 1;
                    }
                    cplexFile << "+ " << t[k] << " v_" << i << "." << k << "_"
                            << j << "." << k << " ";
                }
            }
            // Fin, si escribi hago:
            if (writeFlag == 1) {
                restNum++;
                cplexFile << " <= 0\n";
                writeFlag = 0;
            }
        }
    }

    //Bidireccionalidad
    cplexFile << "\\Bidireccionalidad\n";

    restNum = 0;

    for (int k = Kmin; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "bi" << restNum << ": v_" << i << "." << k
                            << "_" << j << "." << k << " - " << "v_" << j << "."
                            << k << "_" << i << "." << k << " = 0\n";
                    restNum++;
                }
            }
        }
    }

    //Transponders
    cplexFile << "\n\\Transponders\n";

    restNum = 0;
    writeFlag = 0;

    for (int k = Kmin; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    if (writeFlag == 0) {
                        cplexFile << "tr" << restNum << ": ";
                        writeFlag = true;
                    }
                    cplexFile << " + v_" << i << "." << k << "_" << j << "."
                            << k;
                    restNum++;
                }
            }
            if (writeFlag == 1) {
                cplexFile << " <=" << maxInterfaces[i] << "\n"; //Max transponders = 1
                writeFlag = false;
            }
        }
    }

    ///Binary restriction:
    cplexFile << "\n\\Binary Variables\n";
    cplexFile << "binary\n";

    for (int k = 1; k < Kmax + 1; k++) {
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = 0; j < (numSat + numET); j++) {
                if (phyTopology[k][i][j] == 1) {
                    cplexFile << "v_" << i << "." << k << "_" << j << "." << k
                            << "\n";
                }
            }
        }
    }

    cplexFile << "end";
    cplexFile.close();
}

/**
 * Function that parses GLPK solution file and then
 * writes the indicated Logical topology array with it.
 * If t_max and t_min are present in the solution they
 * get updated (fairness methods).
 *
 * @param fileName GLPK Solution file name to parse
 */
void Topology::readSolGLPK(string fileName) {

    ifstream cplexSolFile;
    cplexSolFile.open(fileName.c_str());
    int i, j, k;
    int active;
    string aux1, aux2, aux3, aux4, aux5;
    size_t foundY, found_, foundspace, founddot, foundTmin, foundTmax;

    while (cplexSolFile.eof() == false) {
        getline(cplexSolFile, aux1); /// Read a Line

        foundTmin = aux1.find("t_min");
        if (foundTmin != string::npos) { /// We found t_min
            //cout << aux1 << endl; /// foundTmin points to the beginning of t_min
            aux1 = aux1.substr(foundTmin + 20, 9); // tmin hasta 9 digitos
            //cout << aux1 << endl;
            tmin = atof(aux1.c_str());
            //cout << "t_min=" << tmin << endl;
        }

        foundTmax = aux1.find("t_max");
        if (foundTmax != string::npos) { /// We found t_min
            //cout << aux1 << endl; /// foundTmin points to the beginning of t_min
            aux1 = aux1.substr(foundTmax + 20, 9); // tmin hasta 9 digitos
            //cout << aux1 << endl;
            tmax = atof(aux1.c_str());
            //cout << "t_max=" << tmax << endl;
        }

        foundY = aux1.find("v_"); /// Search for "v_"
        if (foundY != string::npos) /// If found, it is an arc!
                {
            found_ = aux1.find('_', foundY + 3); /// Find second "_"
            //copio en aux2 desde el final de v_ hasta _ (1er nodo)
            aux2 = aux1.substr(foundY + 2, (found_ - foundY + 2) - 4);
            //Find the final space
            //Returns -1 if failed
            foundspace = aux1.find(' ', found_);
            //copio en aux3 desde el _ hasta el espacio
            aux3 = aux1.substr(found_ + 1, (foundspace - found_ + 1) - 2);
            //en aux4 ya tengo la string del arco
            aux4 = aux2 + " -> " + aux3;

            //Determinamos el k de la iteración:
            founddot = aux2.find('.', 0);
            //Guardamos en aux3
            aux5 = aux2.substr(founddot + 1);
            //k ahora guarda el k iteration de esta linea
            k = atoi(aux5.c_str());

            //Determinamos el i de la iteración
            founddot = aux2.find('.', 0);
            //Guardamos en aux5
            aux5 = aux2.substr(0, founddot);
            //i ahora guarda el i iteration de esta linea
            i = atoi(aux5.c_str());

            //Determinamos el j de la iteración
            founddot = aux3.find('.', 0);
            //Guardamos en aux5
            aux5 = aux3.substr(0, founddot);
            //j ahora guarda el j iteration de esta linea
            j = atoi(aux5.c_str());

            //Si encuentro el espacio...
            if (foundspace != string::npos) {
                //Esta en el mismo renglon
                //16 lugares mas adelante estoy antes del 1 o 0
                //Guardo en aux 5 un caracter 0 o 1 si es activo o no
                aux5 = aux1.substr(foundY + 28, 1);
                //Guardamos si esta activo
                active = atoi(aux5.c_str());

            } else //Si no lo encuentro es que...
            {
                getline(cplexSolFile, aux1); //Sigue en el renglon de abajo
                aux5 = aux1.substr(35, 1); //Guardo en aux 5 un caracter 0 o 1 si es activo o no
                active = atoi(aux5.c_str()); //Guardamos si esta activo
            }

            //printf("%s %s = [%d][%d][%d]  (%d)\n", aux2.c_str(), aux3.c_str(),
            //		k, i, j, active);

            //guardo en logTopology lo leido
            logTopology[k][i][j] = active;
        }
    }
    cplexSolFile.close();
}

/**
 * Function that parses IBM solution file and then
 * writes the indicated Logical topology array with it.
 *
 * @param fileName IBM Solution file name to parse
 */
void Topology::readSolIBM(string fileName) {

    ifstream cplexSolFile;
    cplexSolFile.open(fileName.c_str());
    int i, j, k;
    double active;
    string aux1, aux2, aux3, aux4, aux5;
    size_t foundY, found_, foundspace, founddot, foundTmin, foundTmax;

    while (cplexSolFile.eof() == false) {
        getline(cplexSolFile, aux1); /// Read a Line
        foundTmin = aux1.find("t_min");
        if (foundTmin != string::npos) { /// We found t_min, returns begining
            //cout << aux1 << endl; /// foundTmin points to the beginning of t_min
            aux1 = aux1.substr(foundTmin + 24, 16); // tmin hasta 9 digitos
            //cout << aux1 << endl;
            tmin = atof(aux1.c_str());
            //cout << "t_min=" << tmin << endl;
        }

        foundTmax = aux1.find("t_max");
        if (foundTmax != string::npos) { /// We found t_min
            //cout << aux1 << endl; /// foundTmin points to the beginning of t_min
            aux1 = aux1.substr(foundTmax + 24, 16); // tmin hasta 9 digitos
            //cout << aux1 << endl;
            tmax = atof(aux1.c_str());
            //cout << "t_max=" << tmax << endl;
        }

        foundY = aux1.find("v_"); /// Search for "v_"
        if (foundY != string::npos) /// If found, it is an arc!
                {
            found_ = aux1.find('_', foundY + 3); /// Find second "_"
            //copio en aux2 desde el final de v_ hasta _ (1er nodo)
            aux2 = aux1.substr(foundY + 2, (found_ - foundY + 2) - 4);
            //Find the final space "
            //Returns -1 if failed
            foundspace = aux1.find('"', found_);
            //copio en aux3 desde el _ hasta el espacio
            aux3 = aux1.substr(found_ + 1, (foundspace - found_ + 1) - 2);
            //en aux4 ya tengo la string del arco
            aux4 = aux2 + " -> " + aux3;

            //Determinamos el k de la iteración:
            founddot = aux2.find('.', 0);
            //Guardamos en aux3
            aux5 = aux2.substr(founddot + 1);
            //k ahora guarda el k iteration de esta linea
            k = atoi(aux5.c_str());

            //Determinamos el i de la iteración
            founddot = aux2.find('.', 0);
            //Guardamos en aux5
            aux5 = aux2.substr(0, founddot);
            //i ahora guarda el i iteration de esta linea
            i = atoi(aux5.c_str());

            //Determinamos el j de la iteración
            founddot = aux3.find('.', 0);
            //Guardamos en aux5
            aux5 = aux3.substr(0, founddot);
            //j ahora guarda el j iteration de esta linea
            j = atoi(aux5.c_str());

            //we search the value of the arc:
            foundspace = aux1.find("value="); // at the begining of value
            //store the value string
            aux5 = aux1.substr(foundspace + 7, 21);

            active = atof(aux5.c_str());

            //guardo en logTopology lo leido
            //IBM devuelve valores muy raros en los booleanos del tipo:
            // 1.04385099597908e-13 -> 0
            // 0.999999999999896 -> 1
            // -0 -> 0
            if (active > 0.5)
                logTopology[k][i][j] = 1;
            else
                logTopology[k][i][j] = 0;

            if (debug) {
                cout << "arc: " << k << "." << i << "." << j << " is " << active
                        << endl;
            }

        }
    }
    cplexSolFile.close();
}

/**
 * Solve State K by Node Strict Priority. Strict Priority
 * enables arcs in scrict ascending order no matter if it
 * involves disabling others whose priority sum is higher.
 */
void Topology::solveKNodeStrictMaxWeight(int k) {

    int numCon = 0;

/// Initialize the Topology to -1 (*) in k state
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = 0; j < (numSat + numET); j++) {
            logTopology[k][i][j] = -1;
        }
    }

/// Then we need to order the matrix rows acording to the
/// weight of the nodes. Since the weight is associated to
/// an arc, we sum the weight of all the outgoing arcs from
/// a given node, and then we generate an o[] array that
/// basically translates the access to the matrix rows (and
/// columns) in acordance to the sum of the weghts.

    int o[numSat + numET];
/// Initially, the matrix is accessed in order. (i.e like if
/// the node 0 has the highest weight and so forth).
    for (int i = 0; i < (numSat + numET); i++) {
        o[i] = i;
    }

    int temp;
    bool swapped = true;
    int nodeWeight[MAXS];
    bool reservedNode[MAXS];

/// Now we store in the nodeWeight array the MAX of all the
/// arc weights of the outgoing arc of the node i at this moment
/// k.
    for (int i = 0; i < (numSat + numET); i++) {
        reservedNode[i] = false;
        nodeWeight[i] = 0;
        for (int j = 0; j < (numSat + numET); j++) {
            if ((phyTopology[k][i][j] >= 1)
            /*&& (nodeWeight[i] < arcWeight[k][i][j])*/) {
                nodeWeight[i] += arcWeight[k][i][j];
            }
        }

        if (debug) {
            cout << endl << "k=" << k << " - NodeWeight[" << i << "]="
                    << nodeWeight[i];
        }
    }

/// Now we bubble sort the o[] array but copairing
/// the nodeWeight array it currently maps to.
    while (swapped == true) {
        swapped = false;
        for (int i = 1; i < (numSat + numET); i++) {
            //printf("%d < %d\n", priority[o[i - 1]][o[i - 1]] , priority[o[i]][o[i]]);
            if (nodeWeight[o[i - 1]] < nodeWeight[o[i]]) {
                temp = o[i - 1];
                o[i - 1] = o[i];
                o[i] = temp;
                swapped = true;
            }
        }
    }

    if (debug) {
        printf("\n");
        for (int i = 0; i < (numSat + numET); i++) {
            printf("o[%d]=%d, ", i, o[i]);
        }
        printf("\n\n");
    }

/// Now we go through the matrix as per the o[] array
/// Go through a Matrix
    for (int i = 0; i < (numSat + numET); i++) {
        /// Go through a Line corresponding to node "i"
        /// initial connections of the node are= 0
        numCon = 0;
        /// For all the elements in this line...
        for (int j = 0; j < (numSat + numET); j++) {
            /// If there is a potential link,
            /// and still have at least 1 interface,
            /// and the symmetric logical channel is not yet decided, then:
            if (phyTopology[k][o[i]][o[j]] >= 1 && numCon < maxInterfaces[o[i]]
                    && logTopology[k][o[j]][o[i]] != 0
                    && reservedNode[j] == false) {
                /// Activate the link
                logTopology[k][o[i]][o[j]] = phyTopology[k][o[i]][o[j]];
                /// It opossite
                logTopology[k][o[j]][o[i]] = phyTopology[k][o[j]][o[i]];
                /// and take one interface as well
                numCon++;
                reservedNode[j] = true;
                if (debug) {
                    cout << "Enabled from " << o[i] << " to " << o[j] << endl;
                }
            }
            /// If not, this link is set inactive
            else {
                logTopology[k][o[i]][o[j]] = 0;
                logTopology[k][o[j]][o[i]] = 0;
                if (debug) {
                    cout << "Disabed from " << o[i] << " to " << o[j] << endl;
                }
            }
            /// We set a -1 for the i=j in order to see
            /// a "*" when printing
            if (i == j) {
                logTopology[k][o[i]][o[j]] = -1;
            }
        }
        /// Done with the line
    }
/// Done with the matrix
}

/**
 * Solve State K by Arc Strict Priority. Strict Priority
 * enables arcs in scrict ascending order no matter if it
 * involves disabling others whose priority sum is higher.
 */
void Topology::solveKArcStrictMaxWeight(int k) {

    if (debug) {
        cout << endl << "Solving k=" << k << " by arcStrictPriority" << endl;
    }

/// Variables
    unsigned int highArcWeight = 0;
    int highI = 0, highJ = 0;
    bool usedArc[(numSat + numET)][(numSat + numET)];
    bool intLeft[(numSat + numET)];
    bool stop = false;

/// Init used:
    for (int i = 0; i < (numSat + numET); i++) {
        intLeft[i] = true;
        for (int j = i + 1; j < (numSat + numET); j++) {
            usedArc[i][j] = usedArc[j][i] = false;
        }
    }

    if (debug) {
        cout << "Iterating...";
    }
/// Iterate until no un-used arc is found
    while (!stop) {
        /// highArcWeight will remain in -1 if no free arc is found on iteration
        stop = true;
        highArcWeight = 0;
        highI = 0;
        highJ = 0;

        /// Find the highest weight arc in this k Phy
        for (int i = 0; i < (numSat + numET); i++) {
            for (int j = i + 1; j < (numSat + numET); j++) {
                // If This is an arc, and it is the highest weight, and not yet used
                if ((phyTopology[k][i][j] >= 1)
                        && (arcWeight[k][i][j] > highArcWeight)
                        && (!usedArc[i][j])) {
                    highArcWeight = arcWeight[k][i][j];
                    highI = i;
                    highJ = j;
                    stop = false;
                    if (debug) {
                        cout << endl << "Allocating arc[" << highI << "]["
                                << highJ << "] with " << highArcWeight
                                << " weight.";
                    }
                }
            }
        } // Now I have the highest unused arc highI to highJ

        if (stop == false) {
            /// Try to allocate it, if possible
            usedArc[highI][highJ] = true;
            // If node i and j still have interfaces activate arc in logic
            if (intLeft[highI] && intLeft[highJ]) {
                // write logical topology
                logTopology[k][highI][highJ] = phyTopology[k][highI][highJ];
                logTopology[k][highJ][highI] = phyTopology[k][highJ][highI];
                // occupy interfaces
                intLeft[highI] = false;
                intLeft[highJ] = false;
            }
        }
    }
}

/*
 * Solve a K state of the physical topology by applying the Blossom
 * Maximum Matching Algorithm.
 *
 * @param k state to solve
 * @param maxW true to use max weight, false to min weight
 */
void Topology::solveKBlossom(int k, bool maxW) {

    // Clear Log Topology:
    for (int i = 0; i < (numSat + numET); i++) {
        for (int j = 0; j < (numSat + numET); j++) {
            logTopology[k][i][j] = 0;
        }
    }

    int edgeID;
/// If some day we wish to try if Guido reduction
/// to min weight
/// bool maxW = true;

/// We apply the Guido Shaufers Reduction to solve the Maximum
/// Weighted Matching Problem via the Maximum Weighted Perfect
/// Matching. Guido Thesis:
/// http://homepages.cwi.nl/~schaefer/ftp/pdf/masters-thesis.pdf

/// First we create a new graph with G + G'' where G''= G
/// G'' is what Guido calls G~ in the document
/// We need a new arc and cost matrix, both twice the size
    char phyTopologyT[nodeNum * 2][nodeNum * 2];
    //short arcWeightT[nodeNum * 2][nodeNum * 2];
    unsigned int arcWeightT[nodeNum * 2][nodeNum * 2];

/// We create a pointer to PerfectMatching object
    PerfectMatching *perfectMatching;

/// Now lets solve the Perfect Matching!
/// We update the Transformed Topology as per Guido Shaufers
/// Thesis

/// Initialize it so that given G(V,E) and G''(V'',E'') we can
/// create a Reduction graph G'(V',E') with G + G''.
/// We use the same loop for the arc and cost matrix
    if (maxW == true) {
        for (int i = 0; i < (nodeNum * 2); i++) {
            for (int j = 0; j < (nodeNum * 2); j++) {
                if (i < (nodeNum)) /// 		/// This is the source Node is in G
                        {
                    if (j < (nodeNum)) /// 	/// and the destination Node is in G
                            {
                        phyTopologyT[i][j] = phyTopology[k][i][j];
                        arcWeightT[i][j] = -arcWeight[k][i][j];
                    } else ///				/// and the destination Node is in G''
                    {
                        if (i == (j - nodeNum)) /// v to v'' arc
                                {
                            phyTopologyT[i][j] = 1;
                            arcWeightT[i][j] = 0;
                        } else ///				/// No arc
                        {
                            phyTopologyT[i][j] = 0;
                            arcWeightT[i][j] = 0;
                        }
                    }
                } else ///					/// This is the source Node is in G''
                {
                    if (j < (nodeNum)) /// 	/// and the destination Node is in G
                            {
                        if ((i - nodeNum) == j) /// v to v'' arc
                                {
                            phyTopologyT[i][j] = 1;
                            arcWeightT[i][j] = 0;
                        } else ///				/// No arc
                        {
                            phyTopologyT[i][j] = 0;
                            arcWeightT[i][j] = 0;
                        }
                    } else ///				/// and the destination Node is in G''
                    {
                        phyTopologyT[i][j] = phyTopology[k][i - nodeNum][j
                                - nodeNum];
                        arcWeightT[i][j] = -arcWeight[k][i - nodeNum][j
                                - nodeNum];
                    }
                }
            }
        }
    } else /// maxW==false (i.e min. weight)
    {
        for (int i = 0; i < (nodeNum * 2); i++) {
            for (int j = 0; j < (nodeNum * 2); j++) {
                if (i < (nodeNum)) /// 		/// This is the source Node is in G
                        {
                    if (j < (nodeNum)) /// 	/// and the destination Node is in G
                            {
                        phyTopologyT[i][j] = phyTopology[k][i][j];
                        arcWeightT[i][j] = arcWeight[k][i][j];
                    } else ///				/// and the destination Node is in G''
                    {
                        if (i == (j - nodeNum)) /// v to v'' arc
                                {
                            phyTopologyT[i][j] = 1;
                            arcWeightT[i][j] = MAXW;
                        } else ///				/// No arc
                        {
                            phyTopologyT[i][j] = 0;
                            arcWeightT[i][j] = MAXW;
                        }
                    }
                } else ///					/// This is the source Node is in G''
                {
                    if (j < (nodeNum)) /// 	/// and the destination Node is in G
                            {
                        if ((i - nodeNum) == j) /// v to v'' arc
                                {
                            phyTopologyT[i][j] = 1;
                            arcWeightT[i][j] = MAXW;
                        } else ///				/// No arc
                        {
                            phyTopologyT[i][j] = 0;
                            arcWeightT[i][j] = MAXW;
                        }
                    } else ///				/// and the destination Node is in G''
                    {
                        phyTopologyT[i][j] = phyTopology[k][i - nodeNum][j
                                - nodeNum];
                        arcWeightT[i][j] =
                                arcWeight[k][i - nodeNum][j - nodeNum];
                    }
                }
            }
        }
    }

    if (debug) {
        cout << endl << "Arc Matrix: K= " << k << endl;
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (phyTopology[k][i][j] == -1) {
                    cout << "*,";
                } else {
                    cout << (int) phyTopology[k][i][j] << ",";
                }
            }
            cout << endl;
        }
        cout << endl;
        /// We Print the resulting Extended Graph Matrix
        cout << endl << "Reduced Arc Matrix: K= " << k << endl;
        for (int i = 0; i < nodeNum * 2; i++) {
            for (int j = 0; j < nodeNum * 2; j++) {
                if (phyTopologyT[i][j] == -1) {
                    cout << "*,";
                } else {
                    cout << (int) phyTopologyT[i][j] << ",";
                }
            }
            cout << endl;
        }

        /// We Print the resulting Extended Cost Matrix
        cout << endl << "Reduced Cost Matrix: K= " << k << endl;
        for (int i = 0; i < nodeNum * 2; i++) {
            for (int j = 0; j < nodeNum * 2; j++) {
                cout << arcWeightT[i][j] << ",";
            }
            cout << endl;
        }
        cout << endl;
    }

/// Create Perfect Matching Object
/// The pointer allows to free memory later
    perfectMatching = new PerfectMatching(nodeNum * 2,
            nodeNum * 2 * nodeNum * 2 * 2);

/// Set perfect matching verbose mode as per debug flag
    if (debug) {
        (*perfectMatching).options.verbose = true;
    } else {
        (*perfectMatching).options.verbose = false;
    }

/// We add edges with half of the original directed matrix,
/// since Blossom assumes undirected arcs.
    if (debug)
        cout << "Input Arcs for Blossom:" << endl;
    for (int i = 0; i < nodeNum * 2; i++) {
        for (int j = i; j < nodeNum * 2; j++) /// j=i because it is undirected arc (symetrical)
                {
            if (phyTopologyT[i][j] >= 1) /// If Link Active, add Edge to PM
                    {
                edgeID = (*perfectMatching).AddEdge(i, j, arcWeightT[i][j]); // Add arc from NodeID i to NodeID j
                if (debug) {
                    cout << "EdgeID_" << edgeID << ": from " << i << " to " << j
                            << ", with " << (-arcWeightT[i][j]) << " cost"
                            << endl;
                }
            }
        }
    }
/// Solve the matching
    (*perfectMatching).Solve(true);

/// To recover edge result we go through the phyTopology
/// array with edgeId starting from 0
    edgeID = 0;
    if (debug)
        cout << endl << "Chosen Arcs from Blossom:" << endl;
    for (int i = 0; i < nodeNum * 2; i++) {
        for (int j = i; j < nodeNum * 2; j++) {
            if ((i < nodeNum) && (j < nodeNum)) {
                if (i == j) /// Stay within non reduced matrix limits
                        {
                    logTopology[k][i][j] = -1;
                }
                if (phyTopology[k][i][j] >= 1) {
                    if ((*perfectMatching).GetSolution(edgeID) == 1) {
                        if (debug) {
                            cout << "EdgeID_" << edgeID << ": from " << i
                                    << " to " << j << ", with "
                                    << (-arcWeightT[i][j]) << " cost" << endl;
                        }
                        logTopology[k][i][j] = phyTopology[k][i][j];
                        logTopology[k][j][i] = phyTopology[k][j][i];
                    }
                }
            }
            if (phyTopologyT[i][j] >= 1) {
                edgeID++;
            }
        }
    }
/// We completed this iteration (k). We free the object
/// memory and get ready for a new matching in the next k.
    delete perfectMatching;

}

/**
 * Process Routes without writing statistic Files
 */
void Topology::evaluateRoutes() {
    int maxDelay[nodeNum][nodeNum];
    int maxMaxDelay = 0;
    int minDelay[nodeNum][nodeNum];
    int avgDelay[nodeNum][nodeNum];
    int maxAvgDelay = 0;
    int unRoutedKs[nodeNum][nodeNum];
    /// Init arrays;
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            maxDelay[i][j] = 0;
            minDelay[i][j] = INF;
            avgDelay[i][j] = 0;
            unRoutedKs[i][j] = 0;
        }
    }
    /// Evaluate Routes:
    this->globalUnroutedKs = 0;
    if (routed) { // If I routed
        for (int k = 1; k < parsedK + 1; k++) {
            for (int i = 0; i < nodeNum; i++) {
                for (int j = 0; j < nodeNum; j++) {

                    if (distance[k][i][j] >= INF) { // no-route here
                        unRoutedKs[i][j]++;
                        this->globalUnroutedKs++;
                        //cout << "Distance " << k << "." << i << "." << j << " >= INF" << endl;
                    }

                    if (distance[k][i][j] > maxDelay[i][j]) {
                        if (distance[k][i][j] >= INF) { // no-route here

                        } else {
                            maxDelay[i][j] = distance[k][i][j];
                            if (maxDelay[i][j] > maxMaxDelay) {
                                maxMaxDelay = maxDelay[i][j];
                            }
                        }
                    }
                    if (distance[k][i][j] < minDelay[i][j]) {
                        minDelay[i][j] = distance[k][i][j];
                    }
                    if (distance[k][i][j] < INF) {
                        avgDelay[i][j] += distance[k][i][j];
                    }
                }
            }
        }
    }

    // Calculate average:
    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            if (i != j) {
                if (unRoutedKs[i][j] >= parsedK) {
                    avgDelay[i][j] = INF;
                } else {
                    avgDelay[i][j] = avgDelay[i][j]
                            / (parsedK - unRoutedKs[i][j]);
                }
                if (avgDelay[i][j] > maxAvgDelay && avgDelay[i][j] < INF) {
                    maxAvgDelay = avgDelay[i][j];
                }
            }
        }
    }

    globalmaxMaxDelay = maxMaxDelay;
    globalmaxAvgDelay = maxAvgDelay;
}

/**
 * Solve Dijkstra Algorithm in topology from node
 * s to all nodes evaluating the topology in topoNumber
 * from state k1 to k2 inclusive.
 *
 * @param s	source node [0;nodeNum)
 * @param k1 start state
 * @param k2 end state
 * @param topoNumner topology to evaluate the route on
 * @param predecessor[] the predecessor list from node s
 * @param distance[] the distance from s to all nodes
 */
void Topology::dijkstra(int s, int k1, int k2, bool phyTopo,
        int predecessor[MAXV], int distance[MAXV]) {

    /// MAXV defined in Dijkstra.h
    if (k1 > parsedK || k2 > parsedK) {
        cout << "Wrong K number (" << k2 << " and " << k2
                << ") when calling dijkstra() function!" << endl;
        exit(1);
    }

    if (s >= nodeNum) {
        cout << "Wrong source number (" << s
                << ") when calling Dijkstra() function!" << endl;
        exit(1);
    }

    if (nodeNum >= MAXV) {
        cout << "Wrong nodeNum number (" << nodeNum << " when max is " << MAXV
        << ") when calling Dijkstra() function!" << endl;
        exit(1);
    }

    /// Create Dijkstra object;
    Dijkstra dijkstra;

    /// Set num of vertices with dijkstra.numOfVertices; MAXV=50
    /// If k1=k2, we evaluate that state only
    dijkstra.numOfVertices = nodeNum * (k2 - k1 + 1);

    /// Populate int dijkstra.adjMatrix[15][15]; with positive costs
    /// to same vertex=0, no arc=INFI=999
    /// First initialize all to INFI:
    for (int i = 0; i < nodeNum * (k2 - k1 + 1); i++) {
        for (int j = 0; j < nodeNum * (k2 - k1 + 1); j++) {
            dijkstra.adjMatrix[i][j] = INFI;
        }
    }

    /// Then populate the arc values only:
    for (int k = k1; k <= k2; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if ((phyTopology[k][i][j] >= 1) && (phyTopo)) /// We have an intra k arc
                        {
                    //dijkstra.adjMatrix[i][j] = arcWeight[k][i][j];
                    /// NODE I TO NODE J ARCS COST GO HERE!
                    dijkstra.adjMatrix[(k - k1) * nodeNum + i][(k - k1)
                            * nodeNum + j] = 1; /// Shortest path
                }
                if ((logTopology[k][i][j] >= 1) && (!phyTopo)) /// We have an intra k arc
                        {
                    //dijkstra.adjMatrix[i][j] = arcWeight[k][i][j];
                    /// NODE I TO NODE J ARCS COST GO HERE!
                    dijkstra.adjMatrix[(k - k1) * nodeNum + i][(k - k1)
                            * nodeNum + j] = 1; /// Shortest path
                }

                if (i == j) /// same node
                        {
                    /// 0 to same node
                    dijkstra.adjMatrix[(k - k1) * nodeNum + i][(k - k1)
                            * nodeNum + j] = 0;
                    /// Set value to inter k arcs
                    /// NODE I TO NODE I ARCS COST GO HERE!
                    if (k != k2) { /// If Im not on the last k, write arcs to the future k
                        dijkstra.adjMatrix[(k - k1) * nodeNum + i][(k - k1 + 1)
                                * nodeNum + j] = 1;
                    }

                }
            }
        }
    }

    if (debug) /// Print Adjacency Matrix
    {
        int kCounter = 0;
        cout << endl << "Dijkstra Adjacency Matrix (Size="
                << nodeNum * (k2 - k1 + 1) << "):" << endl;
        for (int i = 0; i < nodeNum * (k2 - k1 + 1); i++) {
            if (i % nodeNum == 0) { // Nueva Linea para cada estado
                kCounter++;
                cout << "k:" << kCounter << endl;
            }
            cout << "node " << i % nodeNum << " ";

            for (int j = 0; j < nodeNum * (k2 - k1 + 1); j++) {
                if (j % nodeNum == 0) { // Nueva Linea para cada estado
                    cout << "  ";
                }
                if (dijkstra.adjMatrix[i][j] >= INFI) {
                    cout << "*,";
                } else {
                    cout << dijkstra.adjMatrix[i][j] << ",";
                }

            }
            cout << endl;
        }
    }

    /// Set Source Node dijkstra.source
    dijkstra.source = s;

    /// Run dijkstra.calculateDistance();
    dijkstra.calculateDistance();

    /// Retrive
    /// distance[i] -> Cost from source to node i
    /// predecessor[i] -> predecesor (-1 if not path)
    for (int i = 0; i < nodeNum * (k2 - k1 + 1); i++) {
        distance[i] = dijkstra.distance[i];
        predecessor[i] = dijkstra.predecessor[i];
    }

    /// Print Path from s:
    if (debug) {
        cout << endl << "Paths from source node " << s << ".0 taking 0 as k="
                << k1 << endl;
        dijkstra.output(nodeNum);
        cout << endl << "Source Node " << s << ":" << endl << "Dest." << '\t'
                << "Dist." << '\t' << "Pred." << endl;
        for (int i = 0; i < nodeNum * (k2 - k1 + 1); i++) {
            cout << i % nodeNum << "." << i / nodeNum << '\t'
                    << dijkstra.distance[i] << '\t'
                    << dijkstra.predecessor[i] % nodeNum << "."
                    << dijkstra.predecessor[i] / nodeNum << endl;
        }
        cout << endl;
    }
}

/**
 * Floyd-Warshall based routing calculation
 */
void Topology::floydWarshall(bool writeFile, bool singleInt) {

    routed = true;

    for (int i = 0; i < nodeNum; i++) {
        for (int j = 0; j < nodeNum; j++) {
            distance[parsedK + 1][i][j] = INF;
        }
    }

    // Initialize Delay
    for (int k = parsedK; k >= 1; k--) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (solved) {
                    if (logTopology[k][i][j] >= 1) { // If link available
                        distance[k][i][j] = 0;
                        nHop[k][i][j] = j;
                    } else if (distance[k + 1][i][j] < INF) {
                        // If link available in the next state
                        distance[k][i][j] = distance[k + 1][i][j] + t[k];
                        //distance[k][i][j] = distance[k + 1][i][j] + 1;
                        nHop[k][i][j] = j;

                    } else { // If no link available
                        distance[k][i][j] = INF;
                        nHop[k][i][j] = -1;
                    }
                    if (i == j) {
                        distance[k][i][j] = 0;
                        nHop[k][i][j] = i;
                    }
                } else {
                    if (phyTopology[k][i][j] >= 1) { // If link available
                        distance[k][i][j] = 0;
                        nHop[k][i][j] = j;
                    } else if (distance[k + 1][i][j] < INF) {
                        // If link available in the next state
                        distance[k][i][j] = distance[k + 1][i][j] + t[k];
                        //distance[k][i][j] = distance[k + 1][i][j] + 1;
                        nHop[k][i][j] = j;

                    } else { // If no link available
                        distance[k][i][j] = INF;
                        nHop[k][i][j] = -1;
                    }
                    if (i == j) {
                        distance[k][i][j] = 0;
                        nHop[k][i][j] = i;
                    }
                }
            }
        }
    }

    if (debug) { // Print Initialize Delay
        for (int k = 1; k < parsedK + 1; k++) {
            cout << endl << "k:" << k << " (Initialize_Delay)" << endl;
            for (int i = 0; i < nodeNum; i++) {
                for (int j = 0; j < nodeNum; j++) {
                    if (i == j) {
                        cout << "*" << "(*)\t";
                    } else {
                        if (distance[k][i][j] >= INF || nHop[k][i][j] == -1) {
                            cout << "-(-)\t";
                        } else {
                            cout << nHop[k][i][j] << "(" << distance[k][i][j]
                                    << ")\t";
                        }
                    }
                }
                cout << endl;
            }
            cout << endl;
        }
    }

//    // Debug a particuar route:
//    int iDeb = 1, jDeb = 3, kDeb = 1;
//    if (distance[kDeb][iDeb][jDeb] >= INF || nHop[kDeb][iDeb][jDeb] == -1) {
//        cout << "Route at k=" << kDeb << " from i=" << iDeb << " to j=" << jDeb
//                << " is -(-)\t" << endl;
//    } else {
//        cout << "Route at k=" << kDeb << " from i=" << iDeb << " to j=" << jDeb
//                << " is " << nHop[kDeb][iDeb][jDeb] << "("
//                << distance[kDeb][iDeb][jDeb] << ")\t" << endl;
//    }

    // Shortest Delay Path
    for (int n = 0; n < nodeNum; n++) { // k iterations
        for (int i = 0; i < nodeNum; i++) { // for all nodes
            for (int j = 0; j < nodeNum; j++) {
                for (int k = 1; k < parsedK + 1; k++) { // at all states
                    int dCurr = distance[k][i][j];
                    int d1 = distance[k][i][n];
                    if ((d1 < INF)) {
                        // Lets determine in which state is t[k]+d1
                        int kd1 = k, tk = 0, tkd1;
                        for (int k2 = 1; k2 < k; k2++) {
                            tk += t[k];
                        } // tk now has the accum time in this k
                        tkd1 = tk;
                        while (tk + d1 > tkd1) {
                            tkd1 += t[kd1]; // Alto issue encontro Pablito aca!
                            kd1++;
                        }
                        //cout << "In k: " << k << ", " << t[k] + d1
                        //        << " time ahead is in state " << kd1 << endl;
                        // t[k]+d1 state is now stored in kd1=k+d1
                        int d2 = distance[kd1][n][j];
                        int dAlt = d1 + d2;
                        if (dAlt < dCurr) {
                            distance[k][i][j] = dAlt;
                            nHop[k][i][j] = n;
                            // If particular route debug is on:
                            //int kDeb = 3, iDeb = 1, jDeb = 3;
//                            if (kDeb == k && iDeb == i && jDeb == j) {
//                                if (distance[k][iDeb][jDeb] >= INF
//                                        || nHop[k][iDeb][jDeb] == -1) {
//                                    cout << "Route at k=" << k << " from i="
//                                            << iDeb << " to j=" << jDeb
//                                            << " is -(-)\t (n=" << n << ")"
//                                            << endl;
//                                } else {
//                                    cout << "Route at k=" << k << " from i="
//                                            << iDeb << " to j=" << jDeb
//                                            << " is " << nHop[k][iDeb][jDeb]
//                                            << "(" << distance[k][iDeb][jDeb]
//                                            << ")\t (n=" << n << ", kd=" << kd1
//                                            << ", d1=distance[" << k << "]["
//                                            << i << "][" << n << "]=" << d1
//                                            << ", d2=distance[" << kd1 << "]["
//                                            << n << "][" << j << "]=" << d2
//                                            << ") -> distance[" << k << "]["
//                                            << i << "][" << j << "]=" << dAlt
//                                            << endl;
//                                }
//                            }
//                            // Done particular route
                        }
                    }
                }
            }
        }
    }

    /// Get Next Hop
    int nextHop;
    int loopCount = 0;
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                nextHop = nHop[k][i][j];
                if ((nHop[k][i][nextHop] != -1) && (i != j) && (nextHop!=-1)) { //If nHop exists
                    while (nHop[k][i][nextHop] != nextHop
                            && nHop[k][i][nextHop] != -1 && loopCount <= 1000) {
                        nextHop = nHop[k][i][nextHop];
                        loopCount++;
//                        cout << this->topologyName << "solved: " << this->solved
//                                << ": " << k << "-" << i << "-" << j
//                                << " NextHop: " << nextHop << endl;
                    }
                    nHop[k][i][j] = nextHop;
                    if (loopCount == 1000) {
                        cout << "WARNING: Loop danger for Floyd Warshall in "
                                << this->topologyName << endl;
                    }
                }
            }
        }
    }

    if (writeFile) {
        // Store Route:
        /// Destiny stats txt fileName:
        string routeFileName = "out/";
        routeFileName += topologyName;
        routeFileName += "_FW_route.txt";
        ofstream routeFile;
        routeFile.open(routeFileName.c_str());

        for (int k = 1; k < parsedK + 1; k++) {
            routeFile << endl << "k:" << k << " (Shortest_Delay_Path)" << endl;
            for (int i = 0; i < nodeNum; i++) {
                for (int j = 0; j < nodeNum; j++) {
                    if (i == j) {
                        routeFile << "*" << "(*)\t";
                    } else {
                        if (distance[k][i][j] >= INF || nHop[k][i][j] == -1) {
                            routeFile << "-(-)\t";
                        } else {
                            routeFile << nHop[k][i][j] << "("
                                    << distance[k][i][j] << ")\t";
                        }
                    }
                }
                routeFile << endl;
            }
            routeFile << endl;
        }

        routeFile.close();
    }
}

void Topology::contactGraph(bool writeFile) {

    routed = true;

    // Init
    forfeitGlobal = INF;
    bestDeliveryGlobal = 0;
    excludedNodesSize = 0;
    proxNodesSize = 0;
    recursion = 0;
    level = 0;
    for (int i = 0; i < nodeNum; i++) {
        excludedNodes[i] = -1;
        proxNodes[i] = -1;
        forfeit[i] = INF;
        bestDelivery[i] = 0;
    }

    // Generate Contacts (they end ordered by start time)
    this->generateContacts();

    /// Parte recursiva!
    int S = 3;
    int D = 1;
    int X = 40;
    contactGraphRFC00(S, D, X, 0);

    if (writeFile) {
        // Store Route:
        /// Destiny stats txt fileName:
        string routeFileName = "out/";
        routeFileName += topologyName;
        routeFileName += "_CGR_route.txt";
        ofstream routeFile;
        routeFile.open(routeFileName.c_str());

        routeFile << "Contacts: " << endl;
        for (int c = 0; c < contact; c++) {
            routeFile << "Contact " << c << ": " << contactArray[c][0] << "-"
                    << contactArray[c][1] << " " << contactArray[c][2] << " to "
                    << contactArray[c][3] << endl;
        }

        routeFile << endl << "ExcludedNodes(" << excludedNodesSize << "): ";
        for (int e = 0; e < excludedNodesSize; e++) {
            routeFile << excludedNodes[e] << ",";
        }

        routeFile << endl << "ProxNodes(" << proxNodesSize << "): ";
        for (int e = 0; e < proxNodesSize; e++) {
            routeFile << proxNodes[e] << "(BD:" << bestDelivery[e] + 1
                    << ", FT:" << forfeit[e] - 1 << "),";
        }

        routeFile << endl << "Routes: " << endl;
        for (int k = 1; k < parsedK + 1; k++) {
            routeFile << endl << "k:" << k << " " << endl;
            for (int i = 0; i < nodeNum; i++) {
                for (int j = 0; j < nodeNum; j++) {
                    if (i == j) {
                        routeFile << "*" << "(*)\t";
                    } else {
                        if (distance[k][i][j] >= INF || nHop[k][i][j] == -1) {
                            routeFile << "-(-)\t";
                        } else {
                            routeFile << nHop[k][i][j] << "("
                                    << distance[k][i][j] << ")\t";
                        }
                    }
                }
                routeFile << endl;
            }
            routeFile << endl;
        }
        routeFile.close();
    }

}

/**
 * @S   Source
 * @D   Destiny
 * @X   Deadline
 * @T   Time of Issue
 */
void Topology::contactGraphRFC00(int S, int D, int X, int T) {

    // Exclude Destiny
    excludedNodes[excludedNodesSize] = D;
    excludedNodesSize++;

    int forfeitLocal = forfeitGlobal;
    int bestDeliveryLocal = bestDeliveryGlobal;

    if (recursion >= 100) {
        exit(1);
    }
    if (debug) {
        cout << "Rec: " << recursion << " (" << level << ") Going for CGR S:"
                << S << " D:" << D << " X:" << X << " T:" << T;
        cout << ", ExcludedNodes(" << excludedNodesSize << "): ";
        for (int e = 0; e < excludedNodesSize; e++) {
            cout << excludedNodes[e] << ",";
        }
        cout << ", ";
        cout << "ProxNodes(" << proxNodesSize << "): ";
        for (int e = 0; e < proxNodesSize; e++) {
            cout << proxNodes[e] << "(" << bestDelivery[e] << "-" << forfeit[e]
                    << "),";
        }
        cout << ", FTLocal:" << forfeitLocal << ", BDLocal:"
                << bestDeliveryLocal << endl;
    }

    // For each contact with D as destiny
    for (int c = 0; c < contact; c++) {
        if (debug)
            cout << "Rec: " << recursion << " (" << level << ") Contact: " << c
                    << " from: " << contactArray[c][SOURCE] << " to "
                    << contactArray[c][DEST] << " (" << setfill('0') << setw(3)
                    << contactArray[c][START] << "-" << setfill('0') << setw(3)
                    << contactArray[c][END] << ")";
        if (contactArray[c][START] >= T) {
            if (debug)
                cout << ", start>=T";
            if (contactArray[c][DEST] == D) { // evaluate xmit
                if (debug)
                    cout << ", dst is D";
                if (contactArray[c][START] <= X) { // If not discard contact
                    if (debug)
                        cout << ", start is <=X";
                    if (contactArray[c][SOURCE] == S) { // Im the neighbour!
                        if (debug)
                            cout << ", neighbour";
                        // Compute ECC, Asume we always have capacity
                        if (!isInProxNodes(D)) { // If D is not in proxNodes
                            if (debug)
                                cout << ", D not in ProxNodes";
                            if (contactArray[c][END] < forfeitLocal) {
                                forfeitGlobal = contactArray[c][END];
                            }
                            if (contactArray[c][START] > bestDeliveryLocal) {
                                bestDeliveryGlobal = contactArray[c][START];
                            }
                            if (debug)
                                cout << ", BD: " << bestDeliveryGlobal
                                        << " FT: " << forfeitGlobal;
                            forfeit[proxNodesSize] = forfeitGlobal;
                            bestDelivery[proxNodesSize] = bestDeliveryGlobal;
                            proxNodes[proxNodesSize] = D;
                            proxNodesSize++;
                        } else {
                            if (debug)
                                cout << ", already in ProxNodes";
                        }
                    } else { // Im not the neighbour!
                        if (debug)
                            cout << ", recursion";
                        if (!isInexcludedNodes(contactArray[c][SOURCE])) { // If not excluded
                            if (debug)
                                cout << ", Source(" << contactArray[c][SOURCE]
                                        << ") not in excludedNodes";
                            if (contactArray[c][END] < forfeitLocal) {
                                forfeitGlobal = contactArray[c][END];
                            }
                            if (contactArray[c][START] > bestDeliveryLocal) {
                                bestDeliveryGlobal = contactArray[c][START];
                            }
                            recursion++;
                            level++;
                            int newX;
                            if (contactArray[c][END] > X)
                                newX = X;
                            if (contactArray[c][END] <= X)
                                newX = contactArray[c][END];
                            if (debug)
                                cout << " BD:" << bestDeliveryGlobal << " FT:"
                                        << forfeitGlobal << " newX:" << newX
                                        << endl;
                            this->contactGraphRFC00(S, contactArray[c][SOURCE],
                                    newX - 1, T);
                            level--;
                            forfeitGlobal = forfeitLocal; //Restore previous metrics
                            bestDeliveryGlobal = bestDeliveryLocal;
                        } else {
                            if (debug)
                                cout << ", but excluded";
                        }
                    }
                }
            }
            if (contactArray[c][SOURCE] == D) {
                if (debug)
                    cout << ", src is D";
                if (contactArray[c][START] <= X) { // If not discard contact
                    if (debug)
                        cout << ", start is <=X";
                    if (contactArray[c][DEST] == S) { // Im the neighbour!
                        if (debug)
                            cout << ", neighbour";
                        // Compute ECC, Asume we always have capacity
                        if (!isInProxNodes(D)) { // If D is not in proxNodes
                            if (debug)
                                cout << ", D not in ProxNodes";
                            if (contactArray[c][END] < forfeitLocal) {
                                forfeitGlobal = contactArray[c][END];
                            }
                            if (contactArray[c][START] > bestDeliveryLocal) {
                                bestDeliveryGlobal = contactArray[c][START];
                            }
                            if (debug)
                                cout << ", BD: " << bestDeliveryGlobal
                                        << " FT: " << forfeitGlobal;
                            forfeit[proxNodesSize] = forfeitGlobal;
                            bestDelivery[proxNodesSize] = bestDeliveryGlobal;
                            proxNodes[proxNodesSize] = D;
                            proxNodesSize++;
                        } else {
                            if (debug)
                                cout << ", already in ProxNodes";
                        }
                    } else { // Im not the neighbour!
                        if (debug)
                            cout << ", recursion";
                        if (!isInexcludedNodes(contactArray[c][DEST])) { // If not excluded
                            if (debug)
                                cout << ", Source(" << contactArray[c][DEST]
                                        << ") not in excludedNodes";
                            if (contactArray[c][END] < forfeitLocal) {
                                forfeitGlobal = contactArray[c][END];
                            }
                            if (contactArray[c][START] > bestDeliveryLocal) {
                                bestDeliveryGlobal = contactArray[c][START];
                            }
                            recursion++;
                            level++;
                            int newX;
                            if (contactArray[c][END] > X)
                                newX = X;
                            if (contactArray[c][END] <= X)
                                newX = contactArray[c][END];
                            if (debug)
                                cout << " BD:" << bestDeliveryGlobal << " FT:"
                                        << forfeitGlobal << " newX:" << newX
                                        << endl;
                            this->contactGraphRFC00(S, contactArray[c][SOURCE],
                                    newX - 1, T);
                            level--;
                            forfeitGlobal = forfeitLocal; //Restore previous metrics
                            bestDeliveryGlobal = bestDeliveryLocal;
                        } else {
                            if (debug)
                                cout << ", but excluded";
                        }
                    }
                }
            }
        }
        if (debug)
            cout << endl;
        // Contact ended, restore local times
//        forfeitGlobal = forfeitLocal; //Restore previous metrics
//        bestDeliveryGlobal = bestDeliveryLocal;
    }
}

bool Topology::isInexcludedNodes(int D) {
//    int proxNodes[MAXS];
//    int proxNodesSize;
    for (int i = 0; i < excludedNodesSize; i++) {
        if (excludedNodes[i] == D) {
            return true;
        }
    }
    return false;
}

bool Topology::isInProxNodes(int D) {
//    int proxNodes[MAXS];
//    int proxNodesSize;
    for (int i = 0; i < proxNodesSize; i++) {
        if (proxNodes[i] == D) {
            return true;
        }
    }
    return false;
}

void Topology::detectUnusedArcs(void) {

    if (routed == false) {
        cout << "Warning: Topology not routed!" << endl;
    }

    short arcUsage[parsedK + 1][nodeNum][nodeNum];

    // Init
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                arcUsage[k][i][j] = 0;
            }
        }
    }

    // Determine Usage
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = 0; j < nodeNum; j++) {
                if (solved) {
                    if (logTopology[k][i][j] >= 1) {
                        for (int j2 = 0; j2 < nodeNum; j2++) {
                            if (nHop[k][i][j2] == j && distance[k][i][j] == 0) {
                                arcUsage[k][i][j]++;
                            }
                        }
                    }
                } else {
                    if (phyTopology[k][i][j] >= 1) {
                        for (int j2 = 0; j2 < nodeNum; j2++) {
                            if (nHop[k][i][j2] == j && distance[k][i][j] == 0) {
                                arcUsage[k][i][j]++;
                            }
                        }
                    }
                }
            }
        }
    }

    // Print Usage
    for (int k = 1; k < parsedK + 1; k++) {
        for (int i = 0; i < nodeNum; i++) {
            for (int j = i + 1; j < nodeNum; j++) {
                if (solved) {
                    if (logTopology[k][i][j] >= 1) {
                        cout << "At k:" << k << ", Arc [" << i << "][" << j
                                << "] is used by " << arcUsage[k][i][j]
                                << " and [" << j << "][" << i << "] is used by "
                                << arcUsage[k][j][i] << " nextHops ("
                                << arcUsage[k][i][j] + arcUsage[k][j][i] << ")"
                                << endl;
                    }
                } else {
                    if (phyTopology[k][i][j] >= 1) {
                        cout << "At k:" << k << ", Arc [" << i << "][" << j
                                << "] is used by " << arcUsage[k][i][j]
                                << " and [" << j << "][" << i << "] is used by "
                                << arcUsage[k][j][i] << " nextHops ("
                                << arcUsage[k][i][j] + arcUsage[k][j][i] << ")"
                                << endl;
                    }
                }

            }
        }
    }
}
