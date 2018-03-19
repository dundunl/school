// Shortest path algorithm using stl

#include <bits/stdc++.h>
#define ASCII_CAP 65 //Capital letter A
#define COL_WIDTH 15

using namespace std;

enum Algorithm{SHP, SDP, STP, FTP, MGP};

// Convert capital letter to int
int ctoi(char a){
  return (int)a - ASCII_CAP;
}

class Graph{
  // number of vertices
  int v;
  
  // adjacency list
  map< char, vector<int> > *adj;

public:
  // Constructor
  Graph(int v){
    adj = new map< char, vector<int> >[v];
    
  }

  ~Graph(){
    delete[] adj;
  }

  // Print for debugging
  void print(){
    map<char, vector<int>>::iterator i;

    for(i = adj.begin(); i != adj.end(); i++){
      cout << i->first << endl;
      for (int j = 0; j < 4; j++)
	cout << i->second[j] << " ";
    }
    cout << endl;
  }

  // Add edge to adjacency list
  void add_edge(char a, char b, vector<int> weights){
    adj[ctoi(a)].insert(make_pair(a, weights));
    adj[ctoi(b)].insert(make_pair(b, weights));
  }

  // Returns shortest path
  vector<char> shortest_path(char bilbo, char dwarf, int algo){
    // Initialize and set all distances to infinity
    set< pair<char, int> > v_set;
    vector<int> dist(v, INT_MAX);
    vector<char> path(v);

    // Add bilbo's position as 0
    v_set.insert(make_pair(0, bilbo));
    dist[ctoi(bilbo)] = 0;

    //TODO: finish shortest path algorithm
  }
};

int main(){
  // Initialize input variables
  list< pair<string, char> > addr;
  pair <string, char> bilbo;
  string algo_string, addr_file, map_file;
  int algo = -1;
  int vertices = 0;

  // Temporary variables
  string s1;
  char c1, c2;
  vector<int> iv(4);
  
  // Get routing algorithm from user
  while(algo < 0){
    cout << "Enter routing algorithm (SHP, SDP, STP, FTP, MGP): ";
    cin >> algo_string;
    if(!algo_string.compare("SHP"))
      algo = SHP;
    else if(!algo_string.compare("SDP"))
      algo = SDP;
    else if(!algo_string.compare("STP"))
      algo = STP;
    else if(!algo_string.compare("FTP"))
      algo = FTP;
    else if(!algo_string.compare("MGP"))
      algo = MGP;
    else
      cout << "Algorithm not valid. Try again.\n";
  }

  // Get addresses
  cout << "Enter address file: ";
  cin >> addr_file;
  ifstream ifs;
  ifs.open(addr_file, ifstream::in);
  if(ifs.fail()){
    cout << "Could not open file " << addr_file << ".\n";
    return -1;
  }

  // Put addresses in list
  while(ifs >> s1 >> c1){
    addr.push_back(make_pair(s1,c1));
  }

  ifs.close();

  // Get map
  cout << "Enter map file: ";
  cin >> map_file;
  ifs.open(map_file);
  if(ifs.fail()){
    cout << "Could not open file " << map_file << ".\n";
    return -1;
  }

  // Determine number of vertices
  vertices = count(istreambuf_iterator<char>(ifs),
		   istreambuf_iterator<char>(), '\n') + 1;

  // Create Graph
  Graph dwarf_graph(vertices);

  // Fill adjacency list
  while(ifs >> c1 >> c2 >> iv[0] >> iv[1] >> iv[2] >> iv[3]){
    dwarf_graph.add_edge(c1, c2, iv);
  }

  dwarf_graph.print();

  ifs.close();
  
  return 0;
}
