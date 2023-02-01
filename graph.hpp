#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Graph{
private:
    map<pair<string, string>, int> edges;
    set<string> nodes;
    map<string, bool> isRouter;
public:
    Graph();
    vector<string> splitBySpace(string);
    void addHost(string);
    void addEdge(string);
    void removeLink(string);
    void bellmanFord(string);
    void printData();
    void DVRP();
    void printIteration(map<pair<string, string>, int>, int);
};

#endif