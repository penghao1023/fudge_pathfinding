#include <functional>
#include <iostream>
#include <sstream>
#include "multi_agent_map.h"
#include "astar_search.h"
#include "time_util.h"
#include "load_matrix.h"

using NodeType = std::shared_ptr<MultiAgentNode>;

// Strip the path. Only keep the end of turn nodes.
std::vector<const MultiAgentNode*> strip_path(
    const std::vector<NodeType> &path,
    MultiAgentNode *start) {
  std::vector<const MultiAgentNode*> result;
  for (auto i = path.rbegin(); i != path.rend(); ++i)
    if ((*i)->end_of_turn_)
      result.push_back((*i).get());
  return result;
}

// Helper to print and return result.
void print_result(const std::vector<const MultiAgentNode*> &path,
                  const MultiAgentMap &map){
  std::cout << "--------------------\n";
  ostringstream ss;
  for (auto i = path.begin(); i != path.end(); ++i)
    ss << (*i)->annotation_  << " --- " << (*i)->cost_ << std::endl;
  ss << map.stats_.to_string() << std::endl;;
  std::cout << ss.str();
}

// 10 agents in 2 groups find paths with different speed in a 10x10 map with
// obstacles. All agents except the first one of each group have predecessors.
int main (int argc, char *argv[]) {
  std::vector<int> matrix = load_matrix<int>("../data/matrix_10x10_agents.txt");
  //std::vector<int> matrix = load_matrix<int>("../data/matrix_10x10_plain.txt");

  // Increase heuristic weight to accelerate searching.
  // But the result may not be optimal.
  MultiAgentMap map(10, 10, matrix, 1.25);

  auto start = MultiAgentNode::create({}, {},
      {Agent(0, Pos(0, 0), Pos(9, 9), 5),
       Agent(1, Pos(0, 0), Pos(9, 9), 4, 0),
       Agent(2, Pos(0, 0), Pos(9, 9), 3, 1),
       Agent(3, Pos(0, 0), Pos(9, 9), 2, 2),
       Agent(4, Pos(0, 0), Pos(9, 9), 1, 3),
       Agent(5, Pos(9, 9), Pos(0, 0), 5),
       Agent(6, Pos(9, 9), Pos(0, 0), 4, 5),
       Agent(7, Pos(9, 9), Pos(0, 0), 3, 6),
       Agent(8, Pos(9, 9), Pos(0, 0), 2, 7),
       Agent(9, Pos(9, 9), Pos(0, 0), 1, 8),
      });
  auto end = MultiAgentNode::create();
  std::cout << start->to_string() << std::endl;

  PREPARE_TIMER
  START_TIMER
  std::vector<NodeType> &&path0 = AStarSearch::search(map,
          start, end,
          std::bind(&MultiAgentMap::heuristic_rra, &map,
                    placeholders::_1, placeholders::_2));
  END_TIMER
  PRINT_TIME_ELAPSED

  print_result(strip_path(path0, start.get()), map);

  return 0;
}
