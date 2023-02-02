#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include "logger.hpp"
#include <experimental/filesystem>

const int INF = 10000;

using namespace std;

class Graph{
private:
    map<string, vector<vector<string>>> routingTables;
    map<pair<string, string>, int> edges;
    set<string> nodes;
    map<string, bool> isRouter;
    Logger logger;
public:
    Graph();
    vector<string> splitBySpace(string);
    void addHost(string);
    void addEdge(string);
    void removeLink(string);
    void bellmanFord(string);
    void showTable(string);
    // void printData();
    void DVRP();
    void printIteration(map<pair<string, string>, int>, int);
    void writeInFiles(string, map<string, int>, map<string, string>);
};

#endif