#include "graph.h"

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <queue>
#include <fstream>
#include "chainadj.h"
#include <omp.h>

Graph::Graph(std::string filename) {
    std::ifstream in(filename);
    in >> n >> n >> m;
    std::cout << "n: " << n << " m: " << m << std::endl;
    cnt = -1;
    to_pre = new int[m + 10];
    next_pre = new int[m + 10];
    head_pre = new int[n + 10];
    distance = new int[n + 10];
    parent = new int[n + 10];
    to = new int[m + 10];
    edge = new int[n + 10];
    edge_all = 0;
    memset(head_pre, -1, sizeof(int) * (n + 10));
    memset(distance, -1, sizeof(int) * (n + 10));
    memset(parent, -1, sizeof(int) * (n + 10));
    memset(edge, 0, sizeof(int) * (n + 10));
    for (int i = 0; i < m; i++) {
        int u, v, val;
        in >> u >> v >> val;
        add_edge(u, v);
    }
    int index = 0;

    for (int i = 0; i <= n + 1; i++) {
        for (int j = head_pre[i]; ~j; j = next_pre[j]) {
            int to_node = to_pre[j];
            to[index] = to_node;
            index++;
        }
        edge[i] = index;
    }

    std::cout << "read in successfully" << std::endl;
}

Graph::~Graph() {
    delete to_pre;
    delete next_pre;
    delete head_pre;
    delete distance;
    delete parent;
}

void Graph::add_edge(int u, int v) {
    next_pre[++cnt] = head_pre[u];
    head_pre[u] = cnt;
    to_pre[cnt] = v;
}

void Graph::reset() {
    memset(distance, -1, sizeof(int) * (n + 10));
    memset(parent, -1, sizeof(int) * (n + 10));
    edge_all = 0;
}

// Instantiate non-specialized templates

int Graph::sbfs(int vertex) {
    std::queue<int> frontier;
    frontier.push(vertex);
    distance[vertex] = 0;

    while (!frontier.empty()) {
        int v = frontier.front();
        frontier.pop();
        edge_all += edge[v] - edge[v - 1];
        for (int i = edge[v - 1]; i < edge[v]; i++) {
            int u = to[i];
            if (distance[u] == -1) {
                distance[u] = distance[v] + 1;
                parent[u] = v;
                frontier.push(u);
            }
        }
    }
    return edge_all;
}

int Graph::pbfs(int vertex) {
    int d = 0;

    Bag curBag;
    curBag.insert(vertex);

    while (!curBag.isEmpty()) {
        Bag nextBag = processLevel(curBag, d++);
        curBag = std::move(nextBag);
    }
    return edge_all;
}


// TODO: find a better place for this function
template<typename Iterator>
Bag &reduce(Iterator first, Iterator last) {
    if (last - first <= 1)
        return *first;
    else {
        auto k = (last - first) / 2;

#pragma omp task
        reduce(first, first + k);

#pragma omp task
        reduce(first + k, last);

#pragma omp taskwait
        return first->merge(std::move(*(first + k)));
    }
}

Bag Graph::processLevel(Bag &inBag, int level) {
    std::vector<Bag> outBags(omp_get_max_threads());

#pragma omp parallel
    {
        // the paper suggests a parallel for
#pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < inBag.getSize(); ++i) {
            if (Pennant *pennant = inBag.getPennant(i))        // TODO: consider using a shared pointer
                processPennant(*pennant, outBags[omp_get_thread_num()], level);
        }

        // combine thread-private bags
#pragma omp single
        reduce(outBags.begin(), outBags.end());
    }

    // NRVO doesn't seem to_pre help here
    return std::move(outBags[0]);
}

void Graph::processPennant(Pennant &pennant, Bag &outBag, int level) {
    if (pennant.getSize() > 1) {

        std::unique_ptr<Pennant> other = pennant.split();
        Bag outBagOther;
//#pragma omp parallel sections
//        {
//#pragma omp section
//            { processPennant(*other, outBagOther, level); }
//
//#pragma omp section
//            { processPennant(pennant, outBag, level); }
//        }
//        outBag.merge(std::move(outBagOther));

#pragma omp task shared(other, outBagOther, level)
        processPennant(*other, outBagOther, level);

#pragma omp task shared(pennant, outBag, level)
        processPennant(pennant, outBag, level);

#pragma omp taskwait
        outBag.merge(std::move(outBagOther));
    } else {
        int u = pennant.getVertex();
#pragma omp atomic
        edge_all += edge[u] - edge[u - 1];
//#pragma omp parallel for shared(distance, parent) schedule(dynamic, 1)
        for (int i = edge[u - 1]; i < edge[u]; i++) {
            int v = to[i];
            int d;
//#pragma omp atomic read
            d = distance[v];
            if (d == -1) {
//#pragma omp atomic write
                distance[v] = distance[u] + 1;
//#pragma omp atomic write
                parent[v] = u;
                outBag.insert(v);
            }
        }
    }    // pennant size <= 1
}



