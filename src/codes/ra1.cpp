#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
#include <queue>

using namespace std;

struct Order{
    int source;
    int destination;
    int weight;
};
struct Edge{
    int source;
    int destination;
    int distance;
};
struct AdjacencyList {
    int n;
    int ec;
    vector<vector<Edge>> list;

    AdjacencyList(int nodes): n(nodes), list(n+1) {}
    void addEdge(int source, int dest, int weight) {
        list[source].push_back({source, dest, weight});
        ec++;
    }
};

struct AdjacencyMatrix {
    int n, ec;
    vector<vector<int>> m;

    AdjacencyMatrix(int nodes): n(nodes), m(nodes+1, vector<int>(nodes+1, -1)) {}
    void addEdge(int source, int dest, int weight) {
        if (m[source][dest] == -1) {
            ec++;
        }
        m[source][dest] = weight;
    }
};


int nodes, m;
int trucks, cap;
int orders;
vector<vector<pair<int, int>>> adj;
vector<Order> pesanan;
vector<bool> done;
vector<vector<int>> bestpaths;
int bestdist;


AdjacencyMatrix calculateAllShorthestPath(const AdjacencyList &G) {
    AdjacencyMatrix H(G.n);
    if (G.n*G.n <= G.ec * log2(G.ec)) { // Flyod warshall
        for (const vector<Edge> &el : G.list) {
            for (const Edge &e : el) {
                H.addEdge(e.source, e.destination, e.distance);
            }
        }
        for (int k=1;k<=H.n;++k) {
            for (int i=1;i<=G.n;++i) {
                for (int j=1;j<=G.n;++j) {
                    if (i == j) continue;
                    if (H.m[i][k] == -1 || H.m[k][j] == -1) continue;
                    if (H.m[i][j] == -1 || H.m[i][j] > H.m[i][k] + H.m[k][j]) {
                        H.m[i][j] = H.m[i][k] + H.m[k][j];
                    }
                }
            }
        }
    } else { // Djikstra
        for(int i=1;i<=G.n;++i) {
            priority_queue<pair<int, int>> pq;
            pq.push({0, i});
            while(!pq.empty()) {
                pair<int, int> tmp = pq.top(); pq.pop();
                for (const Edge &e : G.list[tmp.second]) {
                    if (H.m[i][e.destination] == -1 || H.m[i][e.destination] > e.distance - tmp.first) {
                        H.m[i][e.destination] = e.distance - tmp.first;
                        pq.push({-H.m[i][e.destination], e.destination});
                    }
                }
            }
        }
    }
    return H;
}

vector<int> dropAll(const vector<int> &path, stack<int> st) {
    vector<int> res(path);
    while (!st.empty()) {
        int tmp = st.top(); st.pop();
        res.push_back(tmp);
    }
    return res;
}

int calculateDist(const AdjacencyMatrix &G, const vector<vector<int>> &paths) {
    int ans = 0;
    for(const vector<int> & path: paths) {
        for(int i=1;i<path.size();++i) {
            ans += G.m[path[i]][path[i-1]];
        }
    }
    return ans;
}

void solve(const AdjacencyMatrix &G, int rem, stack<int> &st, vector<int> &cpath, vector<vector<int>> &paths, int cm, int cc) {
    if (rem == 0) {
        paths.push_back(dropAll(cpath, st));
        int dist = calculateDist(G, paths);
        if (dist < bestdist) {
            bestdist = dist;
            bestpaths = paths;
        }
        paths.pop_back();
        return;
    }

    if (cm > 0 && cpath.size() > 0) {
        stack<int> tst;
        vector<int> tpath;
        paths.push_back(dropAll(cpath, st));
        solve(G, rem, tst, tpath, paths, cm-1, cap);
        paths.pop_back();
    }

    for (int i=0;i<orders;++i) {
        if (done[i] || pesanan[i].weight > cc) continue;
        done[i] = true;
        cpath.push_back(pesanan[i].source);
        st.push(i);
        solve(G, rem-1, st, cpath, paths, cm, cc-pesanan[i].weight);
        done[i] = false;
        cpath.pop_back();
        st.pop();
    }

    if(!st.empty()) {
        int tmp = st.top(); st.pop();
        cpath.push_back(pesanan[tmp].destination);
        solve(G, rem, st, cpath, paths, cm, cc + pesanan[tmp].weight);
        cpath.pop_back();
        st.push(tmp);
    }
}

int main(){
    cin >> nodes >> m >> trucks >> cap;
    AdjacencyList G(nodes);
    for(int i=0;i<m;++i){
        int a, b, w;
        cin >> a >> b >> w;
        G.addEdge(a, b, w);
        G.addEdge(b, a, w);
    }
    cin >> orders;
    for(int i=0;i<orders;++i){
        int p,d,w;
        cin >> p >> d >> w;
        pesanan.push_back({p, d, w});
    }
    cout<<"done output"<<endl;
    done = vector<bool>(orders, false);
    AdjacencyMatrix H = calculateAllShorthestPath(G);
    bestdist = INT_MAX;
    cout<<"done calculateAllShorthestPath"<<endl;

    stack<int> tst;
    vector<int> tpath;
    vector<vector<int>> tpaths;
    solve(H, orders, tst, tpath, tpaths, m, cap);
    cout<<"done solving"<<endl;
    cout<<"total distance: "<<bestdist<<endl;
    for(const vector<int> & path: bestpaths) {
        for(int i=0;i<path.size();++i) {
            if (i) {
                cout<<" -> ";
            }
            cout<<path[i];
        }
        cout<<endl;
    }
    return 0;
}