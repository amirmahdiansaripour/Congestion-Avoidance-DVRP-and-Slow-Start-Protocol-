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
            if(weight == 10000) cout << 0 << "\t";
            else cout << weight << "\t";
        }
        cout << "\n";
    }
    cout << "\n---------------------------------------------\n\n";
}

void Graph::DVRP(){
    map<pair<string, string>, int> table;
    set<string> :: iterator it, po, xy;
    clock_t start, end;
    start = clock();    
    for(it = nodes.begin(); it != nodes.end(); it++){
        for(po = nodes.begin(); po != nodes.end(); po++){
            if(*po == *it){
                table[make_pair(*it, *po)] = 0; 
            }
            else if(edges[make_pair(*it, *po)] != 0){
                table[make_pair(*it, *po)] = edges[make_pair(*it, *po)];
            }
            else if(edges[make_pair(*it, *po)] == 0){
                table[make_pair(*it, *po)] = 10000;
            }
        }
    }

    int iter = 1;

    for(it = nodes.begin(); it != nodes.end(); it++){
        for(po = nodes.begin(); po != nodes.end(); po++){
            for(xy = nodes.begin(); xy != nodes.end(); xy++){
                if (table[make_pair(*po, *it)] != 10000 &&
                    table[make_pair(*it, *xy)] != 10000 &&
                    table[make_pair(*po, *it)] + table[make_pair(*it, *xy)] < table[make_pair(*po, *xy)])

                    table[make_pair(*po, *xy)] = table[make_pair(*po, *it)] + table[make_pair(*it, *xy)];
            }
        }
        printIteration(table, iter);
        iter++;
    }
    end = clock();
    float duration = (float)((end - start) * 1000)/ (CLOCKS_PER_SEC);
    cout << "DVRP convergence time: " << duration << " ms\n";
    for(it = nodes.begin(); it != nodes.end(); it++){
        bellmanFord(*it);
    }
}

void Graph::addHost(string hosts){
    vector<string> splittedMessage = splitBySpace(hosts);
    for(int i = 2; i < splittedMessage.size(); i++){
        nodes.insert(splittedMessage[i]);
        if(splittedMessage[1] == "routers") 
            isRouter[splittedMessage[i]] = true;
    }
    logger.record(hosts);
}

void Graph::addEdge(string data){
    vector<string> splittedMessage = splitBySpace(data);
    edges[make_pair(splittedMessage[2], splittedMessage[3])] = stoi(splittedMessage[4]);
    edges[make_pair(splittedMessage[3], splittedMessage[2])] = stoi(splittedMessage[4]); 
    logger.record(data); 
}

typedef pair<int, string> iPair;

void Graph::writeInFiles(string source, map<string, int>dist, map<string, string>parent){
    // cout << "Source " << source << "\n";
    set<string> :: iterator t;
    for(t = nodes.begin(); t != nodes.end(); t++){
        if(*t != source){
            string nod = *t;
            int dis = dist[*t];
            string temp = *t;
            // cout << "Node " << *t << " and its parent " << parent[*t] << "\n";
            while(parent[temp] != source){
                temp = parent[temp];
                // cout << "HI\n";
            }
            routingTables[source].push_back({("Node: " + nod), ("Dist: " + to_string(dis)), ("Interface: " + temp)});
        }
    }

    string path = "../routingtables/" + source + ".txt"; 
    // std::experimental::filesystem::remove("../routingtables");
    ofstream file;
    file.open(path);
    for(vector<string> s : routingTables[source]){
        file << s[0] << "\t" << s[1] << "\t" << s[2] << "\n";
    }
    file.close();

}

void Graph::bellmanFord(string source){
    priority_queue<iPair, vector<iPair>, greater<iPair>>pq;
    map<string, int> dist;

    pq.push(make_pair(0, source));
    dist[source] = 0;

    map<string, string> parent;
    set<string> :: iterator p;
    for(p = nodes.begin(); p != nodes.end(); p++){
        dist[*p] = 10000;
        parent[*p] = *p;
    }
    dist[source] = 0;

    while(!pq.empty()){
        string u = pq.top().second;
        pq.pop();
        map<pair<string, string>, int> :: iterator it;
        for(it = edges.begin(); it != edges.end(); it++){
            string src = it->first.first;
            if(src == u){
                string dest = it->first.second;
                int w = it->second;
                if((dist[src] + w < dist[dest]) && w != 0){
                    dist[dest] = dist[src] + w;
                    parent[dest] = src;
                    pq.push(make_pair(dist[dest], dest));
                }
            }
        }
    }
    writeInFiles(source, dist, parent);
}

void Graph::showTable(string src){
    vector<vector<string>> data = routingTables[src];
    for(vector<string> s : data){
        cout << s[0] << "\n" << s[1] << "\n" << s[2] << "\n----------------------------\n";
    }
}

void Graph::removeLink(string s){
    vector<string> splittedMessage = splitBySpace(s);
    edges[make_pair(splittedMessage[2], splittedMessage[3])] = 10000;
    edges[make_pair(splittedMessage[3], splittedMessage[2])] = 10000;  
    logger.record(s);
    DVRP();
}

Graph::Graph(){}