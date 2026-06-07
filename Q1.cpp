#include <stdint.h>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using TensorId = uint32_t;
using OperatorId = uint32_t;

enum class OperatorType {
  Conv,
  ReLU,
  Add,
  Mul,
};

struct Operator {
  OperatorId id;
  OperatorType type;
  std::vector<TensorId> inputs;
  std::vector<TensorId> outputs;
};
enum class DataType {
  INT,
  BOOL,
  FLOAT,
};
struct Tensor {
  TensorId id;
  std::vector<uint32_t> shape;
  OperatorId producer =-1;
  std::vector<OperatorId> consumers;
  DataType dtype;
};

class Graph {
 public:
  std::vector<Operator> nodes;
  std::vector<Tensor> edges;

  std::vector<OperatorId> dependecy_order;
  void build();
  void print_dependency();
  void print_graph();
};

void Graph::build() {
  for (auto& node : nodes) {
    for (auto tensorId : node.outputs) {
      edges[tensorId].producer = node.id;
    }
    for (auto tensorId : node.inputs) {
      edges[tensorId].consumers.push_back(node.id);
    }
  }

  std::vector<int> indeg(nodes.size(), 0);

  for (auto& edge : edges) {
    for (auto node : edge.consumers) {
      indeg[node]++;
    }
  }

  std::queue<OperatorId> q;
  for (int i = 0; i < nodes.size(); i++) {
    if (indeg[i] == 0) {
      q.push(i);
    }
  }
  while (!q.empty()) {
    auto node = q.front();
    q.pop();
    dependecy_order.push_back(node);
    for (auto id : nodes[node].outputs) {
      auto& edge = edges[id];
      for (auto x : edge.consumers) {
        indeg[x]--;
        if (indeg[x] == 0) {
          q.push(x);
        }
      }
    }
  }
}

void Graph::print_dependency() {
  for (auto id : dependecy_order) {
    std::cout << "n" << id + 1 << " ";
  }
  std::cout << "\n";
}
void Graph::print_graph() {
  std::cout << "Graph : \n";
  for(const auto& edge:edges){
    if(edge.producer == -1){
      continue;
  }
  for(auto consumer : edge.consumers){
    std::cout << "n" << edge.producer + 1 << " -> n" << consumer + 1 << "\n";
  }
  }
}

std::vector<int> earliestExecutionTime(Graph& g,std::vector<int>& time) {
    std::vector<int> start(g.nodes.size(),0);

    for(auto nodeId : g.dependecy_order) {
        auto& node = g.nodes[nodeId];

        for(auto tensorId : node.outputs) {
            auto& edge = g.edges[tensorId];
            int time_taken = start[nodeId] + time[nodeId];
            for(auto consumer : edge.consumers) {
                start[consumer] = std::max(start[consumer],time_taken);

            }
        }
    }
    return start;
}

int main() {
  Tensor a{.id = 0};
  Tensor b{.id = 1};
  Tensor c{.id = 2};
  Tensor s{.id = 3};
  Tensor d{.id = 4};
  Tensor e{.id = 5};
  Tensor f{.id = 6};
  Tensor y{.id = 7};
  std::vector<Tensor> edges{a, b, c, s, d, e, f, y};
  // inputs donot include external inputs like x,w1 etc..
  // assume node id is its pos in nodes vector
  Operator n1{
      .id = 0, .type = OperatorType::Conv, .inputs = {}, .outputs = {a.id}};
  Operator n2{
      .id = 1, .type = OperatorType::ReLU, .inputs = {a.id}, .outputs = {b.id}};
  Operator n3{
      .id = 2, .type = OperatorType::Conv, .inputs = {b.id}, .outputs = {c.id}};
  Operator n4{
      .id = 3, .type = OperatorType::Conv, .inputs = {}, .outputs = {s.id}};
  Operator n5{
      .id = 4, .type = OperatorType::Add, .inputs = {c.id,s.id}, .outputs = {d.id}};
  Operator n6{
      .id = 5, .type = OperatorType::ReLU, .inputs = {d.id}, .outputs = {e.id}};
  Operator n7{
      .id = 6, .type = OperatorType::Mul, .inputs = {e.id}, .outputs = {f.id}};
  Operator n8{
      .id = 7, .type = OperatorType::Add, .inputs = {f.id}, .outputs = {y.id}};
  std::vector<Operator> nodes{n1,n2,n3,n4,n5,n6,n7,n8};

  Graph g;
  g.nodes = std::move(nodes);
  g.edges = std::move(edges);

  g.build();
  g.print_dependency();  // gives : n1 n4 n2 n3 n5 n6 n7 
  g.print_graph();
  std::vector<int> time = {8,1,10,5,1,1,1,1};
  // execution assuming sequential execution
  auto execution_start_time = earliestExecutionTime(g,time);
  int i=1;
  std::cout << "Execution start time for each node assuming sequential execution : \n";
  for(int x : execution_start_time){
    std::cout << 'n' <<i++<<" -> " << x << "ms" <<'\n';
  }
}