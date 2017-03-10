#include <iostream>
#include <cassert>
#include <vector>
#include <queue>

#define MAX_FLOW 2000000000
#define MAX_DISTANCE 2000000000

using namespace std;

// Adjacency List Graph
class ALGraph{
private:
    struct Edge{
        int source;
        int dest;
        int cost;
        int flow;
        Edge* rev;

        Edge(int source, int dest, int cost) {
            this->source = source;
            this->dest = dest;
            this->cost = cost;
            flow = 0;
            rev = 0;
        }
    };

    // Used to find lowest cost edge in MST
    class EdgeCompare {
    public:
        bool operator()(const Edge* a, const Edge* b)
        {
            return (a->cost > b->cost);
        }
    };

    int numVertices;
    vector<int> vertexValues;
    vector<vector<Edge*> > edges;
public:
    ALGraph(int numVertices) {
        this->numVertices = numVertices;
        vertexValues.resize(numVertices);
        edges.resize(numVertices);
    }

    void setVertexValue(int vertex, int value) {
        vertexValues[vertex] = value;
    }

    void addDirectedEdge(int source, int dest, int cost) {
        edges[source].push_back(new Edge(source, dest, cost));
    }

    void addEdge(int source, int dest, int cost) {
        Edge* e1 = new Edge(source, dest, cost);
        Edge* e2 = new Edge(dest, source, cost);
        e1->rev = e2;
        e2->rev = e1;
        edges[source].push_back(e1);
        edges[dest].push_back(e2);
    }

    int MST();
    int MST(vector<pair<int, int> > &treeEdges);

    void BFS(int root, vector<int> &distances);

    void Dijkstra(int root, vector<int> &distances);

    int MaxFlow(int source, int sink);
};

// Breadth-First Search: returns the distance to each node form
//    the root, assuming edge costs are all 1. Returns -1 distance
//    for nodes that are not reachable from the root.
void ALGraph::BFS(int root, vector<int> &distances) {
    bool visited[numVertices];
    for (int i = 0; i < numVertices; i++) visited[i] = false;
    distances.resize(numVertices, -1);
    queue<int> Q;
    Q.push(root);
    distances[root] = 0;
    visited[root] = true;
    while (!Q.empty()) {
        int v = Q.front(); Q.pop();
        for (vector<Edge*>::iterator it = edges[v].begin(); it != edges[v].end(); it++){
            Edge* e = *it;
            if (visited[e->dest]) continue;
            visited[e->dest] = true;
            distances[e->dest] = distances[v] + 1;
            Q.push(e->dest);
        }
    }
}

// Dijkstra's algorithm: returns the shortest distance from
//    the root to every node. Returns -1 distance for nodes
//    that are not reachable from the root.
void ALGraph::Dijkstra(int root, vector<int> &distances) {
    distances.resize(numVertices, MAX_DISTANCE);
    bool visited[numVertices];
    for (int i = 0; i < numVertices; i++) {
        visited[i] = false;
    }
    // Queue of pairs of <distance, vertexNumber>
    priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> >> Q;
    Q.push(make_pair(0, root));
    while (!Q.empty()) {
        pair<int, int> p = Q.top(); Q.pop();
        distances[p.second] = min(distances[p.second], p.first);
        if (visited[p.second]) continue;
        visited[p.second] = true;
        for (vector<Edge*>::iterator it = edges[p.second].begin(); it != edges[p.second].end(); it++) {
            Edge* e = *it;
            if (visited[e->dest]) continue;
            if (distances[p.second] + e->cost < distances[e->dest])
                Q.push(make_pair(distances[p.second] + e->cost, e->dest));
        }
    }

    for (int i = 0; i < numVertices; i++)
        if (distances[i] == MAX_DISTANCE) distances[i] = -1;
}

// Minimum-spanning tree: returns the total cost of the tree
//    as well as placing all of the edges in treeEdges.
int ALGraph::MST(vector<pair<int, int> > &treeEdges) {
    bool inTree[numVertices];
    for (int i = 0; i < numVertices; i++) inTree[i] = false;
    inTree[0] = true;
    priority_queue<Edge*, vector<Edge*>, EdgeCompare> Q;
    for (vector<Edge*>::iterator it = edges[0].begin(); it != edges[0].end(); it++) {
        Q.push(*it);
    }
    int out = 0;
    while (!Q.empty()) {
        Edge* e = Q.top(); Q.pop();
        if (inTree[e->dest]) continue;
        inTree[e->dest] = true;
        out += e->cost;
        treeEdges.push_back(make_pair(e->source, e->dest));
        for (vector<Edge*>::iterator it = edges[e->dest].begin(); it != edges[e->dest].end(); it++) {
            Q.push(*it);
        }
    }
    return out;
}

int ALGraph::MST() {
    vector<pair<int, int> > v;
    return MST(v);
}

// Maximum Flow: Edmonds-Karp Algorithm
int ALGraph::MaxFlow(int source, int sink) {
    for (int i = 0; i < numVertices; i++) {
        for (vector<Edge*>::iterator it = edges[i].begin(); it != edges[i].end(); it++) {
            (*it)->flow = 0;
        }
    }

    int flow = 0;
    queue<int> Q;
    Edge* pred[numVertices];
    while (true) { // spooky scary infinite loop
        // Run a BFS to find the shortest path from source to sink
        for (int i = 0; i < numVertices; i++) pred[i] = 0;
        Q.push(source);
        while (!Q.empty()) {
            int vertex = Q.front(); Q.pop();
            for (vector<Edge*>::iterator it = edges[vertex].begin(); it != edges[vertex].end(); it++) {
                Edge* e = *it;
                if (pred[e->dest] == 0 && e->dest != source && e->cost > e->flow) {
                    pred[e->dest] = e;
                    Q.push(e->dest);
                }
            }
        }

        // Stop if no more augmenting paths found
        if (pred[sink] == 0) break;

        // Calculate new flow
        int df = MAX_FLOW;
        for (Edge* e = pred[sink]; e != 0; e = pred[e->source]) {
            df = min(df, e->cost - e->flow);
        }

        // Apply new flow
        for (Edge* e = pred[sink]; e != 0; e = pred[e->source]) {
            e->flow += df;
            if (e->rev) e->rev->flow -= df;
        }

        flow += df;
    }

    return flow;
}

// Tests
int main() {

    ALGraph* G = new ALGraph(5);
    G->addEdge(0, 1, 2);
    G->addEdge(0, 2, 4);
    G->addEdge(1, 2, 1);
    G->addEdge(1, 3, 5);
    G->addEdge(2, 3, 3);

    vector<int> bfsDist;
    G->BFS(0, bfsDist);
    assert(bfsDist[0] ==  0);
    assert(bfsDist[1] ==  1);
    assert(bfsDist[2] ==  1);
    assert(bfsDist[3] ==  2);
    assert(bfsDist[4] == -1);

    vector<int> dist;
    G->Dijkstra(0, dist);
    assert(dist[0] ==  0);
    assert(dist[1] ==  2);
    assert(dist[2] ==  3);
    assert(dist[3] ==  6);
    assert(dist[4] == -1);

    assert(G->MST() == 6);

    G = new ALGraph(18);
    for (int i = 2; i <= 16; i += 2) G->addDirectedEdge(0, i, 2);
    for (int i = 3; i <= 17; i += 2) G->addDirectedEdge(i, 1, 1);
    G->addDirectedEdge( 2,  9, 1);
    G->addDirectedEdge( 4,  9, 1);
    G->addDirectedEdge( 6,  9, 1);
    G->addDirectedEdge( 8,  3, 1);
    G->addDirectedEdge( 8,  5, 1);
    G->addDirectedEdge( 8,  7, 1);
    G->addDirectedEdge( 8, 11, 1);
    G->addDirectedEdge(10,  9, 1);
    G->addDirectedEdge(10, 13, 1);
    G->addDirectedEdge(10, 15, 1);
    G->addDirectedEdge(10, 17, 1);
    G->addDirectedEdge(12, 11, 1);
    G->addDirectedEdge(14, 11, 1);
    G->addDirectedEdge(16, 11, 1);
    assert(G->MaxFlow(0, 1) == 6);

    return 0;
}
