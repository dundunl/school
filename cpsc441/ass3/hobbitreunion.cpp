#include <bits/stdc++.h>
#define ASCII_CAP 65 //Capital letter A
#define COL_WIDTH 15

using namespace std;

// class Dwarf_Map{
//   string dwarf;
//   char v1, v2;
//   int km, time, coins, trolls;

// public:
//   Dwarf_Map(string dwarf, char v1, char v2, int km, int time, int coins, int trolls){
//     dwarf = dwarf;
//     v1 = v1;
//     v2 = v2;
//     km = km;
//     time = time;
//     coins = coins;
//     trolls = trolls;
//   }

//   void print(){
//     cout << setw(COL_WIDTH) << left << dwarf
// 	 << setw(COL_WIDTH) << left << v1
// 	 << setw(COL_WIDTH) << left << km
// 	 << setw(COL_WIDTH) << left << time
// 	 << setw(COL_WIDTH) << left << coins
// 	 << setw(COL_WIDTH) << left << trolls;
//   }
// };


class Graph{
  // number of vertices
  int v;
  
  // adjacency list
  list< pair<char, int> > *adj;

public:
  // Constructor
  Graph(int v){
    adj = new list< pair<char, int> >[v];
  }

  ~Graph(){
    delete adj;
  }

  // Add edge to adjacency list
  void add_edge(char a, char b, int weight){
    adj[(int)a - ASCII_CAP].push_back(make_pair(a, weight));
    adj[(int)b - ASCII_CAP].push_back(make_pair(b, weight));
  }

  // Returns shortest path
  vector<char> shortest_path(char bilbo, char dwarf){
    // Initialize and set all distances to infinity
    set< pair<char, int> > v_set;
    vector<int> dist(v, INT_MAX);
    vector<char> path(v);

    // Add bilbo's position as 0
    v_set.insert(make_pair(0, bilbo));
    dist[(int)bilbo - ASCII_CAP] = 0;
  }
};

int main(){
  // Initialize input variables
  list< pair< string, char > > addr;
  pair <string, char> bilbo;
  string algo_string, addr_file, map_file;
  char n1, n2;
  int weight;
  int algo;

  // Get routing algorithm from user
  bool valid_algo = false;
  while(!valid_algo){
    cout << "Please enter routing algorithm (SHP, SDP, STP, FTP):";
    cin >> algo;
    if(!algo.compare("SHP") || !algo.compare("SDP") ||
       !algo.compare("STP") || !algo.compare("FTP"))
      valid_algo = true;
    else
      cout << "\nAlgorithm not valid.\n";
  }

  

  return 0;
}
