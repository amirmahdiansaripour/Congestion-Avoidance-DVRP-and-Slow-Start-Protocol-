#include "graph.hpp"

vector<string> splitBySpace(string unsplitted){
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

int main(){
    Graph g;
    string s;
    while(getline(cin, s)){
        vector<string> splitted = splitBySpace(s);
        if(splitted[0] == "remove"){
            g.removeLink(s);
        }
        else if(splitted[0] == "run"){
            g.DVRP();
        }
        else if(splitted[0] == "show" && splitted[1] == "table"){
            g.showTable(splitted[2]);
        }
        else if(splitted[0] == "add" && (splitted[1] == "hosts" || splitted[1] == "routers")){
            g.addHost(s);
        }
        else if(splitted[1] == "link" && (splitted[0] == "add" || splitted[0] == "update")){
            g.addEdge(s);
        }
        
    }
}