#include "graph.h"
#include <iostream>
#include <iterator>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;


int main(int argc, char *argv[]) {
    omp_set_num_threads(64);
    string infile = "web-Stanford.in";

    Graph g("/home/geekpie/data/" + infile);
    auto starlist = {1, 2, 4, 8, 16, 32, 64};
    for (auto star: starlist) {
        auto start = chrono::system_clock::now();
        int edgep = g.pbfs(star);
        chrono::duration<double> durP = chrono::system_clock::now() - start;
        cout << "parallel: " << durP.count() << " MTEPS= " << (double) edgep / durP.count() / 1e6<<endl;

        ofstream out("/home/murez/" + infile + "parallel" + to_string(star) + ".txt");
        int x = 0;
        for (int i = 1; i <= g.n; i++) {
            if (g.distance[i] != -1) {
                out << i << " " << g.distance[i] << " " << g.parent[i] << endl;

            }
        }
        out.close();
        g.reset();
        // Run BFS from the 3rd vertex using the sequential strategy

        start = chrono::system_clock::now();
        int edges = g.sbfs(star);
        chrono::duration<double> durS = chrono::system_clock::now() - start;
        cout << "serially: " << durS.count() << " MTEPS= " << (double) edges / durS.count() / 1e6 << endl;
        ofstream sout("/home/murez/" + infile + "serial" + to_string(star) + ".txt");
        for (int i = 1; i <= g.n; i++) {
            if (g.distance[i] != -1) {
                sout << i << " " << g.distance[i] << " " << g.parent[i] << endl;
            }
        }
        g.reset();
        sout.close();
    }
    return 0;

}