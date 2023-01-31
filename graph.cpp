#include "graph.hpp"
#include <bits/stdc++.h>
using namespace std;

vector<string> Graph::splitBySpace(string unsplitted){
    string split = "";
    vector<string> lastRes;
    for(int i = 0; i < unsplitted.size(); i++){
        if(unsplitted[i] == ' '){
            lastRes.push_back(split);
            split = "";
        }
        else
            split.push_back(unsplitted[i]);
    }
    lastRes.push_back(split);
    return lastRes;
}

void Graph::printData(){
    set<string> :: iterator p;
    cout << "nodes : ";
    for(p = nodes.begin(); p != nodes.end(); p++){
        cout << *p << " ";
    }
    cout << "edges \n";
    map<pair<string, string>, int> :: iterator it;
    for(it = edges.begin(); it != edges.end(); it++){
        string src = it->first.first;
        string dest = it->first.second;
        int w = it->second;
        cout << "src " << src << "\tdest " << dest << "\tcost " << w << "\n";
    }
}

void Graph::addHost(string hosts){
    vector<string> splittedMessage = splitBySpace(hosts);
    for(int i = 2; i < splittedMessage.size(); i++){
        nodes.insert(splittedMessage[i]);
        if(splittedMessage[1] == "routers") 
            isRouter[splittedMessage[i]] = true;
    }
}

void Graph::addEdge(string data){
    vector<string> splittedMessage = splitBySpace(data);
    edges[make_pair(splittedMessage[2], splittedMessage[3])] = stoi(splittedMessage[4]);
    edges[make_pair(splittedMessage[3], splittedMessage[2])] = stoi(splittedMessage[4]);  
}

void Graph::bellmanFord(){
    // printData();
    map<string, int> dist;
    map<string, string> parent;
    set<string> :: iterator p;
    for(p = nodes.begin(); p != nodes.end(); p++){
        dist[*p] = INT_MAX;
        parent[*p] = *p;
    }
    // cout << "HI\n";
    dist[*(nodes.begin())] = 0;
    while(1){
        bool change = false;
        map<pair<string, string>, int> :: iterator it;
        for(it = edges.begin(); it != edges.end(); it++){
            string src = it->first.first;
            string dest = it->first.second;
            int w = it->second;
            if(dist[src] + w < dist[dest]){
                dist[dest] = dist[src] + w;
                change = true;
                parent[dest] = src;
            }
        }
        if(change == false) break;
    }
    // cout << "HI\n";
    set<string> :: iterator t;
    for(t = nodes.begin(); t != nodes.end(); t++){
        cout << "Node: " << *t << "\n";
        cout << "Dist: " << dist[*t] << "\n";
        cout << "Parent : " << parent[*t] << "\n\n\n";
    }
}

Graph::Graph(){}