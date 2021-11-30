#include <cstdlib>

#include "csrgraph.h"
#include "sliding_q.h"
#include "bitmap.h"
#include "timer.h"
#include "omp.h"
#include <fstream>
#include <string>
#include <random>

typedef int32_t Node;
typedef std::pair<Node, Node> Edge;
std::vector<Edge> edgelist;

auto thread_nums = {1, 2, 4, 8, 16, 32, 48, 64, 128};

std::string SplitFilename(const std::string &str) {
    std::size_t found = str.find_last_of("/\\");
    return str.substr(found + 1);
}

void data_reset(CSRGraph<Node> &g, int *parent, int *dist) {
#pragma omp parallel for
    for (Node i = 0; i < g.num_node(); ++i) {
        parent[i] = g.out_degree(i) != 0 ? -g.out_degree(i) : -1;
        dist[i] = -1;
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Usage: ./pbfs <graph_file> <repeat times> <outfile dir>" << std::endl;
        std::cout << "Example: ./pbfs /home/geekpie/data/web-Stanford.in  4 /home/murez/result/wow/" << std::endl;
        return -1;
    }
    freopen(argv[1], "rt", stdin);

    std::string infile(argv[1]);
    infile = SplitFilename(infile);
    int iteration = atoi(argv[2]);
    std::string outfile_dir("");
    bool write_to_file_flag = false;
    if (argc == 4) {
        outfile_dir = std::string(argv[3]);
        write_to_file_flag = true;
    }


    // graph generation
    int a, b, c;
    printf("reading file from %s\n", infile.c_str());
    scanf("%d %d %d", &a, &b, &c);


    while (scanf("%d %d %d", &a, &b, &c) != -1) edgelist.push_back({a, b});
    printf("creating csr\n");
    CSRGraph<Node> g(std::move(edgelist));
    printf("create csr successfully\n");

    Timer t;
    double avg_time = 0, max_time = 0, min_time = 99999;
    int bfs_steps;


    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(10, 100);//g.num_node() - 10);
    std::vector<int> start_nodes = {1, 2, 3, 4, 5};
//                                    (int) g.num_node() - 2, (int) g.num_node() - 3,
//                                    (int) g.num_node() - 4, (int) g.num_node() - 5,
//                                    (int) g.num_node() - 6};
//    for (int x = 0; x < 15; x++) {
//        int e = distr(eng);
//        bool flag = true;
//        for (auto &i: start_nodes) {
//            if (e == i) {
//                e = distr(eng);
//                x--;
//                flag = false;
//                break;
//            }
//        }
//        if (flag) start_nodes.push_back(e);
//    }
    for (auto &i: start_nodes)
        printf("%d ", i);
    printf("\n");

    for (auto thread_num: thread_nums) {
        printf("------------\nomp_thread: %d\n", thread_num);
        omp_set_num_threads(thread_num);
        long long allsteps = 0;
        double all_mteps = 0.0;
        int valid_times = 0;
        double alltime = 0.0;
        avg_time = 0, max_time = 0, min_time = 99999;
        for (auto start_node: start_nodes) {
            printf("---------------\nnode :%d\n", start_node);
            for (int i = 0; i < iteration; ++i) {
                Node *parent = new Node[g.num_node()];
                Node *dist = new Node[g.num_node()];
                data_reset(g, parent, dist);
                t.Start();
//                bfs_steps = pBFS(g, start_node, parent, dist);
                bfs_steps = g.pBFS(start_node, parent, dist);
                t.Stop();

                printf("gMTEPS: %f\n", (float) bfs_steps / t.Seconds() * 1e-6);
                printf("gTrial Time:\t%lf\n", t.Seconds());

//                t.Start();
//                bfs_steps = pBFS(g, start_node, parent, dist);
////                bfs_steps = g.pBFS(start_node, parent, dist);
//                printf("pMTEPS: %f\n", (float) bfs_steps / t.Seconds() * 1e-6);
//                printf("pTrial Time:\t%lf\n", t.Seconds());
//                t.Stop();
                avg_time += t.Seconds();

                if (t.Seconds() >= 0.001) {
                    allsteps += bfs_steps;
                    valid_times++;
                    alltime += t.Seconds();
                    all_mteps += bfs_steps / t.Seconds() * 1e-6;
                }

                if (i == 0 && write_to_file_flag) {
                    int y = 0;
                    std::ofstream out(
                            outfile_dir + infile + ".startnode" + std::to_string(start_node) + ".parallel." +
                            std::to_string(thread_num) +
                            ".out");
                    for (int i = 0; i <= g.num_node() + 1; i++) {
                        if (parent[i] >= 0) {
                            y += 1;
                            out << i << " " << dist[i] << " " << parent[i] << std::endl;
                        }
                    }
                    out.close();
                    printf("file lines is %d\n", y);
                }

                max_time = std::max(max_time, t.Seconds());
                min_time = std::min(min_time, t.Seconds());
                delete[] parent;
                delete[] dist;
            }
        }
        printf("Min time: %lf\n", min_time);
        printf("Max time: %lf\n", max_time);
        printf("Avg time: %lf\n", alltime / valid_times);
        printf("Avg MTEPS: %lf\n", all_mteps / valid_times);
        printf("Avg steps: %lld\n", allsteps / valid_times);
    }


}