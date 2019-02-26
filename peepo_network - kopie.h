#ifndef PEEPONETWORK__H
#define PEEPONETWORK__H

#include <dlib/bayes_utils.h>//does not support C++17 yet
#include <dlib/graph_utils.h>
#include <dlib/graph.h>
#include <dlib/rand.h>
#include <dlib/directed_graph.h>
#include <dlib/threads.h>
#include <dlib/directed_graph.h>
#include <dlib/string.h>
#include <dlib/bayes_utils.h>
#include <dlib/set.h>
#include <dlib/graph_utils.h>
#include <dlib/stl_checked.h>

#include <boost/range/join.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/assign.hpp>
#include <boost/variant.hpp>
//#include <boost/filesystem.hpp>

#include <nlohmann/json.hpp>


#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <time.h>
#include <chrono>
#include <fstream>
#include <tuple>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <filesystem>
//#include <variant>//only in C++17-> use boost instead


#include "combinations.h"

using json = nlohmann::json;
using std::setw;
using std::setprecision;
using std::left;
using std::fixed;
using std::right;
using std::scientific;

//#include "ppl.h"   
using namespace dlib;
using namespace bayes_node_utils;


using namespace boost::assign;
using namespace boost::adaptors;
//using namespace boost::filesystem;
using namespace boost;




/*
-------------------
		Define 3 types   for the cpds
        This is necessary as the type of the cpds differ between a root node and e.g. a leaf node and as types have to be known at complile time...
		root nodes cpds have type std::vector<double> while leaf nodes have type std::vector<std::vectoor<double> >
-------------------
*/
typedef variant<std::vector<std::vector<double> >, std::vector<double> > t_cpd;//generic type for any cpds
typedef std::vector<std::vector<double> > t_cpd_leaf;//leaf node cpds type
typedef std::vector<double> t_cpd_root;//root node cpds type

typedef dlib::set<unsigned long>::compare_1b_c set_type;
typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;


/*typedef directed_graph<bayes_node>::kernel_1a_c directed_graph_type;
typedef directed_graph<bayes_node>::kernel_1a_c::node_type node_type;
typedef graph<dlib::set<unsigned long>::compare_1b_c, dlib::set<unsigned long>::compare_1b_c>::kernel_1a_c join_tree_type;*/

class PeepoNetwork;//forward declaration for non class functions

std::vector<json> get_topologies(PeepoNetwork& , unsigned = 0);
std::vector<std::vector<std::string>  >  fully_connected_network(PeepoNetwork&);

class PeepoNetwork{

private://members
	std::ofstream log;// ("logfile.txt");//TEMPORARY FOR DEBUGGING PURPOSES
// ----------------------------------------------------------------------------------------
	
private://functions



public://members
	PeepoNetwork* inst_;
	std::string identification;
	std::string description;
	std::string train_from;
	std::string train_data ;
	bool frozen;
	std::string date;
	std::vector<json> bel_nodes;
	std::vector<json> mem_nodes;
	std::vector<json> lan_nodes;
	std::vector<json> ext_nodes;
	std::vector<json> int_nodes;
	std::vector<json> pro_nodes;
	std::vector<std::vector<std::string> > edges;
	json cpds;
	json network;
	json cardinality_map;
	json omega_map;
	directed_graph<bayes_node>::kernel_1a_c* bn;
	//directed_graph<bayes_node>::kernel_1a_c bn = directed_graph<bayes_node>::kernel_1a_c( *pt_bn);
	std::map<std::string,unsigned>  node_dic;

public://functions
	PeepoNetwork();
	PeepoNetwork(const std::string&);
	~PeepoNetwork();
	PeepoNetwork(const PeepoNetwork&);

	PeepoNetwork* get_instance(void);
	PeepoNetwork& operator=(const PeepoNetwork&);


	void from_json();
	void from_json(const json&);
	void from_json(const std::string&);
	json to_json();
	json make_network(void);
	void assemble(void);
	void disassemble(void);
	void make_cardinality_map(void);
	json get_cardinality_map(void);
	std::vector<std::string> get_nodes(void);
	std::vector<std::string> get_root_nodes(void);
	std::vector<std::string> get_leaf_nodes(void);
	std::vector<std::string> get_lan_nodes(void);
	std::vector<std::string> get_bel_nodes(void);
	std::vector<std::string> get_mem_nodes(void);
	std::vector<std::string> get_ext_nodes(void);
	std::vector<std::string> get_int_nodes(void);
	std::vector<std::string> get_pro_nodes(void);
	void add_belief_node(const std::string&, const unsigned&);
	void add_memory_node(const std::string&, const unsigned&);
	void remove_node(const std::string&);
	void remove_belief_node(const std::string&);
	void make_omega_map(void);//TO DO : improve by use of existing cardinality_map
	json get_omega_map(void);
	void add_omega_map(const std::string&, const std::vector<float>&);
	void set_edges(const std::vector<std::vector<std::string> >&);
	std::vector<std::vector<std::string> >  get_edges(void);
	void add_edge(const std::vector<std::string>&);
	void remove_edge(const std::vector<std::string>&);
	std::vector<std::string>  get_incoming_edges(const std::string&);
	std::vector<std::string>  get_outgoing_edges(const std::string&);
	void set_cpds(const json&);
	json get_cpds();
	t_cpd get_cpds(const std::string&);
	void  add_cpds(const std::string&, const t_cpd_leaf&);
	void  add_cpds(const std::string, const t_cpd_root);
	void to_bayesian_network(void);

		void tests(void);//TO DELETE AFTER DEBUGGING PHASE	   
} ;
#endif
