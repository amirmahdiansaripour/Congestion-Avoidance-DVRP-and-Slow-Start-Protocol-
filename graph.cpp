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


void Graph::printIteration(map<pair<string, string>, int> table, int iter){
    cout << "ITERATION NO. " << iter << "\n"; 
    set<string> :: iterator it;
    set<string> :: iterator po;
    for(it = nodes.begin(); it != nodes.end(); it++){
        cout << "\t" << *it;
    }
    cout << "\n";
    for(it = nodes.begin(); it != nodes.end(); it++){
        cout << *it << "\t";
        for(po = nodes.begin(); po != nodes.end(); po++){
            int weight = table[make_pair(*it, *po)];
            if(weight == INT_MAX) cout << 0 << "\t";
            else cout << weight << "\t";
        }
        cout << "\n";
    }
    cout << "\n---------------------------------------------\n\n";
}

void Graph::DVRP(){
    map<pair<string, string>, int> table;
    set<string> :: iterator it;
    set<string> :: iterator po;
    set<string> :: iterator xy;
    
    for(it = nodes.begin(); it != nodes.end(); it++){
        for(po = nodes.begin(); po != nodes.end(); po++){
            if(*po == *it){
                table[make_pair(*it, *po)] = 0; 
            }
            else if(edges[make_pair(*it, *po)] != 0){
                table[make_pair(*it, *po)] = edges[make_pair(*it, *po)];
            }
            else if(edges[make_pair(*it, *po)] == 0){
                table[make_pair(*it, *po)] = INT_MAX;
            }
        }
    }

    int iter = 1;

    for(it = nodes.begin(); it != nodes.end(); it++){
        printIteration(table, iter);
        iter++;
        for(po = nodes.begin(); po != nodes.end(); po++){
            for(xy = nodes.begin(); xy != nodes.end(); xy++){
                if (table[make_pair(*po, *it)] != INT_MAX &&
                    table[make_pair(*it, *xy)] != INT_MAX &&
                    table[make_pair(*po, *it)] + table[make_pair(*it, *xy)] < table[make_pair(*po, *xy)])

                    table[make_pair(*po, *xy)] = table[make_pair(*po, *it)] + table[make_pair(*it, *xy)];
            }
        }
    }

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

void Graph::bellmanFord(string source){
    // printData();
    map<string, int> dist;
    map<string, string> parent;
    set<string> :: iterator p;
    for(p = nodes.begin(); p != nodes.end(); p++){
        dist[*p] = INT_MAX;
        parent[*p] = *p;
    }
    // cout << "HI\n";
    dist[source] = 0;
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
    set<string> :: iterator t;
    for(t = nodes.begin(); t != nodes.end(); t++){
        if(*t != source){
            while(parent[parent[*t]] != source){
                parent[*t] = parent[parent[*t]];
            }
            cout << "Node: " << *t << "\n";
            cout << "Dist: " << dist[*t] << "\n";
            cout << "Interface : " << parent[*t] << "\n";    
            cout << "-------------------------------\n";        
        }
    }
}

void Graph::removeLink(string s){
    vector<string> splittedMessage = splitBySpace(s);
    edges[make_pair(splittedMessage[2], splittedMessage[3])] = INT_MAX;
    edges[make_pair(splittedMessage[3], splittedMessage[2])] = INT_MAX;  
}

Graph::Graph(){}