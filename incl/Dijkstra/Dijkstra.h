/*
 * Dijktra.h
 *
 *  Created on: Feb 26, 2013
 *      Author: totin
 *
 * http://programming-technique.blogspot.com.ar/2012/01/implementation-of-dijkstras-shortest.html
 */

#include<iostream>

#define INFI 999
#define MAXV 50

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

using namespace std;

class Dijkstra{
    private:


        bool mark[MAXV]; //keep track of visited node


    public:
        int predecessor[MAXV],distance[MAXV];
        int numOfVertices;
        int source;
        int adjMatrix[MAXV][MAXV];
    /*
    * Function read() reads No of vertices, Adjacency Matrix and source
    * Matrix from the user. The number of vertices must be greather than
    * zero, all members of Adjacency Matrix must be postive as distances
    * are always positive. The source vertex must also be positive from 0
    * to noOfVertices - 1

    */
        void read();

    /*
    * Function initialize initializes all the data members at the begining of
    * the execution. The distance between source to source is zero and all other
    * distances between source and vertices are infinity. The mark is initialized
    * to false and predecessor is initialized to -1
    */

        void initialize();

    /*
    * Function getClosestUnmarkedNode returns the node which is nearest from the
    * Predecessor marked node. If the node is already marked as visited, then it search
    * for another node.
    */

        int getClosestUnmarkedNode();
    /*
    * Function calculateDistance calculates the minimum distances from the source node to
    * Other node.
    */

        void calculateDistance();
    /*
    * Function output prints the results
    */

        void output(int nodeNum);
        void printPath(int, int nodeNum);
};

#endif


