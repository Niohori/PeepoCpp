#include "peepo_network.h"


std::vector<json> get_topologies(PeepoNetwork& peepo_network, unsigned max_removal)
{
	std::vector<std::vector<std::string>  > max_edges = fully_connected_network(peepo_network);
	if (max_removal >= max_edges.size()) { max_removal = max_edges.size() - 1; }
	std::vector<json> topologies;
	json a;
	for (unsigned comb = max_edges.size(); comb > max_removal; comb--) {
		Combinations<std::vector<std::string> > comb_obj(max_edges, comb);
		std::vector<std::vector<std::vector<std::string>>> combinations = comb_obj.get();
		for (auto edges : combinations) {
			a["egdes"] = edges;
			a["entropy"] = edges.size();
			topologies.push_back(a);
		}
	}
	return topologies;
}

std::vector<std::vector<std::string>  >   fully_connected_network(PeepoNetwork& peepo_network)
{
	std::vector<std::string> root_nodes = peepo_network.get_root_nodes();
	std::vector<std::string> leaf_nodes = peepo_network.get_leaf_nodes();
	peepo_network.edges.clear();
	for(unsigned root = 0; root < root_nodes.size(); root++){
		for (unsigned leaf = 0; leaf < leaf_nodes.size(); leaf++) {
			peepo_network.add_edge({ root_nodes[root],leaf_nodes[leaf] });
		}
	}
	std::vector<std::vector<std::string> > a = peepo_network.edges;
	return a;
}

void write_to_file(const std::string& name, const json&  peepo_network) {
	std::string my_path = std::experimental::filesystem::current_path().u8string()+ "\\resources";
	if (!std::experimental::filesystem::exists(my_path)) {std::experimental::filesystem::create_directory(my_path);};
	my_path += "\\" + name + ".json";
	std::ofstream o(my_path);
	o << std::setw(4) << std::boolalpha << peepo_network << std::endl;
	o.close();
}
/*
def read_from_file(name) :
	with open(ROOT_DIR + '/resources/' + str(name) + '.json') as json_data :
return PeepoNetwork().from_json(json.load(json_data)).assemble()*/

PeepoNetwork::PeepoNetwork()
{
	//ctor;
	 /* initialize random seed: */
	srand(time(NULL));
	
}


PeepoNetwork::PeepoNetwork(const std::string& source):identification(source + ".json")
{
	//identification = source + ".json";
	log =  std::ofstream("logfile.txt");
	from_json();
	/* initialize random seed: */
	srand(time(NULL));
}


PeepoNetwork:: ~PeepoNetwork()
{
 //destructor;
}

PeepoNetwork*  PeepoNetwork::get_instance(void)
{
	if (inst_ == NULL)
	{
		inst_ = new PeepoNetwork;
	}
	return(inst_);
}


PeepoNetwork& PeepoNetwork::operator=(const PeepoNetwork& a)
{
	return *this;
}


PeepoNetwork::PeepoNetwork (const PeepoNetwork& apeepo)
{
	identification = apeepo.identification;
	description = apeepo.description;
	train_from = apeepo.train_from;
	train_data = apeepo.train_data;
	frozen = apeepo.frozen;
	bel_nodes = apeepo.bel_nodes;
	mem_nodes = apeepo.mem_nodes;
	lan_nodes = apeepo.lan_nodes;
	ext_nodes = apeepo.ext_nodes;
	pro_nodes = apeepo.pro_nodes;
	int_nodes = apeepo.int_nodes;
	edges = apeepo.edges;
	cpds = apeepo.cpds;
	assemble();
	omega_map = apeepo.omega_map;
	bn = new directed_graph<bayes_node>::kernel_1a_c;
	//bn = directed_graph<bayes_node>::kernel_1a_c(apeepo.bn);
	//directed_graph<bayes_node>::kernel_1a_c bn(apeepo.bn);

}

void PeepoNetwork::from_json(void)
{
		// read the JSON file
		std::ifstream ntw_(identification);
		//stream the file to the network object
		json ntw;
		ntw_ >> ntw;
		from_json(ntw);
}

void PeepoNetwork::from_json(const json& nw)
{
	//update class members
	if (identification == "") { identification = nw["header"]["identification"].get<std::string>(); }
	description = nw["header"]["description"].get<std::string>();
	train_from = nw["header"]["train_from"].get<std::string>();
	train_data = nw["header"]["train_from"].get<std::string>();
	frozen = nw["header"]["frozen"].get<bool>();
	date = nw["header"]["date"].get<std::string>();
	bel_nodes = nw["nodes"]["RON"]["BEL"].get< std::vector<json> >();
	mem_nodes = nw["nodes"]["RON"]["MEM"].get< std::vector<json> >();
	lan_nodes = nw["nodes"]["LAN"].get< std::vector<json> >();
	ext_nodes = nw["nodes"]["LEN"]["EXT"].get< std::vector<json> >();
	int_nodes = nw["nodes"]["LEN"]["INT"].get< std::vector<json> >();
	pro_nodes = nw["nodes"]["LEN"]["PRO"].get< std::vector<json> >();
	if (frozen) {
		edges = nw["edges"].get< std::vector<std::vector<std::string> > >();
		cpds = nw["cpds"].get<json>();
	}
	assemble();
}


void PeepoNetwork::from_json(const std::string& source)
{
	identification = source + ".json";
	from_json();
}

json PeepoNetwork::to_json(void)
{
	return network;
}


json PeepoNetwork::make_network(void)
{
	json nw;
	nw["header"]["identification"] = identification;
	nw["header"]["description"] = description;
	nw["header"]["train_from"] = train_from;
	nw["header"]["train_from"] = train_data;
	nw["header"]["frozen"] =  frozen ;
	nw["header"]["date"] = date; 
	nw["nodes"]["RON"]["BEL"] = bel_nodes;
	nw["nodes"]["RON"]["MEM"] = mem_nodes;
	nw["nodes"]["LAN"] = lan_nodes;
	nw["nodes"]["LEN"]["EXT"] = ext_nodes;
	nw["nodes"]["LEN"]["INT"] = int_nodes;
	nw["nodes"]["LEN"]["PRO"] = pro_nodes;
	nw["edges"] = edges;
	nw["cpds"] = cpds;
	return nw;
	}


void  PeepoNetwork::assemble(void)
{
	network = make_network();
	make_cardinality_map();
	make_omega_map();
}


void  PeepoNetwork::disassemble(void)
{
	edges.clear();
	cpds.clear();
	omega_map.clear();
}


void PeepoNetwork::make_cardinality_map(void)
{
	cardinality_map.clear();
	auto range1 = boost::join(boost::join(bel_nodes, mem_nodes), lan_nodes);
	auto range2 = boost::join(range1, ext_nodes);
	auto range3 = boost::join(range2, int_nodes);
	auto range  = boost::join(range3, pro_nodes);
	cardinality_map.clear();
	for (auto it = boost::begin(range); it != boost::end(range); ++it){
		auto node = *it;
		cardinality_map.emplace(node["name"], node["card"]);
	}
}

json PeepoNetwork::get_cardinality_map(void)
{
	return cardinality_map;
}

void PeepoNetwork::make_omega_map(void)//TO DO : improve by use of existing cardinality_map
{
	omega_map.clear();
	auto range1 = boost::join(boost::join(bel_nodes, mem_nodes), lan_nodes);
	auto range2 = boost::join(range1, ext_nodes);
	auto range3 = boost::join(range2, int_nodes);
	auto range  = boost::join(range3, pro_nodes);
	for (auto it = boost::begin(range); it != boost::end(range); ++it) {
		auto node = *it;
		std::vector<std::string> incoming_nodes = get_incoming_edges(node["name"]);
		float max_omega = 2.0f*3.141596f;
		for (auto itt = boost::begin(incoming_nodes); itt != boost::end(incoming_nodes); ++itt) {
			max_omega *= cardinality_map[*itt];
		}
		std::vector<float> omega(node["card"],0.0 );
		for (unsigned k = 0; k < omega.size(); k++) { omega[k] =  static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);; }
		omega_map.emplace(node["name"], omega);
	}

}

json PeepoNetwork::get_omega_map(void)
{
	return omega_map;
}

void PeepoNetwork::add_omega_map(const std::string& node, const std::vector<float>& omega)
{
	omega_map.update({ { node,omega } });
}


std::vector<std::vector<std::string> >  PeepoNetwork::get_edges(void)
{ 
	return edges;
}



std::vector<std::string>  PeepoNetwork::get_nodes(void)
{
	std::vector<std::string> nodes;
	for (auto& node : cardinality_map.items()){nodes.push_back(node.key());}
	std::sort(nodes.begin(), nodes.end());
	return nodes;
}


std::vector<std::string> PeepoNetwork::get_root_nodes(void)
{
	std::vector<std::string> root_nodes;
	auto range = boost::join(bel_nodes, mem_nodes);
	for_each(range.begin(), range.end(), [&root_nodes](json node) {root_nodes.push_back(node["name"]); });
	std::sort(root_nodes.begin(), root_nodes.end());
	return root_nodes;
}


std::vector<std::string> PeepoNetwork::get_leaf_nodes(void)
{
	std::vector<std::string> leaf_nodes;
	auto range2 = boost::join(pro_nodes, ext_nodes);
	auto range = boost::join(range2, int_nodes);
	for_each(range.begin(), range.end(), [&leaf_nodes](json node) {leaf_nodes.push_back(node["name"]); });
	std::sort(leaf_nodes.begin(), leaf_nodes.end());
	return leaf_nodes;
}


std::vector<std::string> PeepoNetwork::get_lan_nodes(void)
{
	std::vector<std::string> lan_;
	for_each(lan_nodes.begin(), lan_nodes.end(), [&lan_](json node) {lan_.push_back(node["name"]); });
	std::sort(lan_.begin(), lan_.end());
	return lan_;
}


std::vector<std::string> PeepoNetwork::get_bel_nodes(void)
{
	std::vector<std::string> bel_;
	for_each(bel_nodes.begin(), bel_nodes.end(), [&bel_](json node) {bel_.push_back(node["name"]); });
	std::sort(bel_.begin(),bel_.end());
	return bel_;
}


std::vector<std::string> PeepoNetwork::get_mem_nodes(void)
{
	std::vector<std::string> mem_;
	for_each(mem_nodes.begin(), mem_nodes.end(), [&mem_](json node) {mem_.push_back(node["name"]); });
	std::sort(mem_.begin(), mem_.end());
	return mem_;
}


std::vector<std::string> PeepoNetwork::get_ext_nodes(void)
{
	std::vector<std::string> ext_;
	for_each(ext_nodes.begin(), ext_nodes.end(), [&ext_](json node) {ext_.push_back(node["name"]); });
	std::sort(ext_.begin(), ext_.end());
	return ext_;
}


std::vector<std::string> PeepoNetwork::get_int_nodes(void)
{
	std::vector<std::string> int_;
	for_each(int_nodes.begin(), int_nodes.end(), [&int_](json node) {int_.push_back(node["name"]); });
	std::sort(int_.begin(), int_.end());
	return int_;
}


std::vector<std::string> PeepoNetwork::get_pro_nodes(void)
{
	std::vector<std::string> pro_;
	for_each(pro_nodes.begin(), pro_nodes.end(), [&pro_](json node) {pro_.push_back(node["name"]); });
	std::sort(pro_.begin(), pro_.end());
	return pro_;
}


void PeepoNetwork::add_belief_node(const std::string& node, const unsigned& cardinality)
{
	json a;
	a["name"] = node;
	a["card"] = cardinality;
	bel_nodes.push_back(a);
	cardinality_map[node] = cardinality;
}

void PeepoNetwork::add_memory_node(const std::string& node, const unsigned& cardinality)
{
	json a;
	a["name"] = node;
	a["card"] = cardinality;
	mem_nodes.push_back(a);
	cardinality_map[node] = cardinality;
}


void PeepoNetwork::remove_belief_node(const std::string& node)
{
	std::vector<json> belief_nodes;
	for (auto noode : bel_nodes) { if (noode["name"] != node) { belief_nodes.push_back(noode); } };
	bel_nodes = belief_nodes;
	remove_node(node);
}


void PeepoNetwork::remove_node(const std::string& node)
{
	cpds.erase(node);
	cardinality_map.erase(node);
	omega_map.erase(node);
	for (auto parent : get_incoming_edges(node)) { remove_edge({ parent,node }); };
	for (auto child : get_outgoing_edges(node)) { remove_edge({ node,child}); };
}


std::vector<std::string>  PeepoNetwork::get_incoming_edges(const std::string& node)
{
	std::vector<std::string> incoming_edges;
	for_each(edges.begin(), edges.end(), [node, &incoming_edges](std::vector<std::string> edge) {if(edge[1] == node) { incoming_edges.push_back(edge[0]); } });
	std::sort(incoming_edges.begin(), incoming_edges.end());
	return incoming_edges;
}


std::vector<std::string>  PeepoNetwork::get_outgoing_edges(const std::string& node)
{
	std::vector<std::string>outcoming_edges;
	for_each(edges.begin(), edges.end(), [node, &outcoming_edges](std::vector<std::string> edge) {if (edge[0] == node) { outcoming_edges.push_back(edge[1]); } });
	std::sort(outcoming_edges.begin(), outcoming_edges.end());
	return outcoming_edges;
}


void PeepoNetwork::set_edges(const std::vector<std::vector<std::string> >& edges_)
{
	edges = edges_;
}


void PeepoNetwork::add_edge(const std::vector<std::string>& edge)
{
	edges.push_back(edge);
}


void PeepoNetwork::remove_edge(const std::vector<std::string>& edge)
{
	edges.erase(std::remove_if(edges.begin(), edges.end(), [&edge](std::vector<std::string> edg) {return  edg[0] == edge[0] && edg[1] == edge[1]; }), edges.end());
}


void PeepoNetwork::set_cpds(const json& cpds_)
{
	cpds = cpds_;
}


json PeepoNetwork::get_cpds()
{
	return cpds;
}


t_cpd PeepoNetwork::get_cpds(const std::string& node)
{
	std::vector<std::string> root_nodes = get_root_nodes();
	bool is_root = false;
	for_each(root_nodes.begin(), root_nodes.end(), [&is_root, node](std::string node_) { if (node == node_) { is_root = true; }});
	if (is_root) {return cpds[node].get <t_cpd_root>(); }
	return cpds[node].get <t_cpd_leaf>();
}



void  PeepoNetwork::add_cpds(const std::string& node, const t_cpd_leaf& cpd)
{
	cpds.update({ { node,cpd } });
}


void  PeepoNetwork::add_cpds(const std::string node, const t_cpd_root cpd)
{
	cpds.update({ { node,cpd } });
}



void PeepoNetwork::to_bayesian_network(void)
{
	std::vector<std::string> all_nodes = get_nodes();
	unsigned number_of_nodes = all_nodes.size();
	unsigned number_of_edges = edges.size();
	// Create a dictionary of node names coupled to a number, as dLib respresnet node by integers/unsigned
	for (unsigned node = 0; node < number_of_nodes; node++) { node_dic[all_nodes[node]] = node; };
	//inform how many the nodes  the network will have
	bn->set_number_of_nodes(number_of_nodes);
	//add the edges
	for (unsigned edge = 0; edge < number_of_edges; edge++) { bn->add_edge(node_dic[edges[edge][0]], node_dic[edges[edge][1]]); }
	// Now we inform all the nodes in the network about their cardinality
	for (unsigned node = 0; node < number_of_nodes; node++) { set_node_num_values(bn, node_dic[all_nodes[node]], cardinality_map[all_nodes[node]]); }
	//Add all the conditional probability information for each node
	// Each node's conditional probability is dependent on the state of its parents.  
	// To specify this state we need to use the assignment object.  This assignment 
	// object allows us to specify the state of each nodes parents. 
	// For parents nodes this object is empty use parents_state.clear() if previously assigned

	assignment parent_state;
	//first the root nodes
	std::vector<std::string> root_nodes = get_root_nodes();
	for (unsigned node = 0; node < root_nodes.size(); node++)
	{
		unsigned cardinality = cardinality_map[root_nodes[node]];
		t_cpd_root probability = cpds[root_nodes[node]];
		parent_state.clear();
		for (unsigned state = 0; state < cardinality; state++) { set_node_probability(bn, node_dic[root_nodes[node]], state, parent_state, probability[state]); }
	}

	//now the leaf nodes
	std::vector<std::string> leaf_nodes = get_leaf_nodes();
	for (unsigned leaf = 0; leaf < leaf_nodes.size(); leaf++) {
		unsigned cardinality = cardinality_map[leaf_nodes[leaf]];
		t_cpd_leaf probability = cpds[leaf_nodes[leaf]];
		//get incoming nodes, their cardinality and make a matrix of posssible parents state combinations
		std::vector<std::string> in_nodes = get_incoming_edges(leaf_nodes[leaf]);
		std::vector<int> parents_states_cardinality;
		for (unsigned parent = 0; parent < in_nodes.size(); parent++) { parents_states_cardinality.push_back(cardinality_map[in_nodes[parent]]); }
		std::vector<std::vector<int> > parents_states_matrix = States::get_index_matrix(parents_states_cardinality);
		for (unsigned states = 0; states < parents_states_matrix[0].size(); states++) {
			parent_state.clear();
			for (unsigned parent = 0; parent < parents_states_matrix.size(); parent++) { parent_state.add(node_dic[root_nodes[parent]], parents_states_matrix[parent][states]); }
			for (unsigned leaf_state = 0; leaf_state < cardinality; leaf_state++) { set_node_probability(bn, node_dic[leaf_nodes[leaf]], leaf_state, parent_state, probability[leaf_state][states]); }
			std::cout << "---------> in bn\n";
			std::cin.get();
		}
	}
}

//UNDER THIS - > TO REMOVE AFTER DEBUG


void PeepoNetwork::tests(void)
{
	assemble();
	std::cout << std::endl << std::endl << "********************** TEST for getting cardinality_map, cpds and edges ***************************************\n\n";
	std::cout << cardinality_map << std::endl;
	std::cout << cpds << std::endl;
	for (unsigned ed = 0; ed < edges.size(); ed++) { std::cout << "[" << edges[ed][0] << " ," << edges[ed][1] << "]\n"; };
	

	std::cout << std::endl << std::endl << "********************** TEST OF add_cpds() for a root node and add_bel_node() ***************************************\n\n";
	std::cout << "Push key\n"; std::cin.get();
	std::cout << "Change existing root node \n";
	std::string node = bel_nodes[0]["name"].get < std::string>();
	std::cout << "Node : " << node  << std::endl;
	t_cpd cpd = get_cpds(node);
	std::vector<double> a = get<t_cpd_root>(cpd);
	std::cout << "Present cpd :\n";
	std::cout << "[" << a[0]<< ", " << a[1] << "]" <<  std::endl;
	std::cout << "Changing cpd to ";
	t_cpd_root ab = { 0.9, 0.1 };
	std::cout << "[" << ab[0] << ", " << ab[1] << "]" << std::endl;
	add_cpds(node, ab);
	cpd = get_cpds(node);
	std::vector<double> ac = get<t_cpd_root>(cpd);
	std::cout << "Adapted cpd \n";
	std::cout << "[" << ac[0] << ", " << ac[1] << "]" << std::endl;
	//removing this node
	std::cout << "Removing this belief now\n";
	remove_belief_node(node);
	std::cout << "Adapted  Belief_nodes\n\n";
	for (auto nod: bel_nodes) { std::cout << nod << std::endl; };
	std::cout << "with edges\n\n";
	for (auto ed : edges) { std::cout << "[" << ed[0] << ", " << ed[1] << "]\n\n"; };
	std::cout << "Ading a new belief \n";
	node = "AnewRootNode";
	std::cout << "Adding non existing root node " << node << std::endl;
	add_belief_node(node, 2);
	t_cpd_root az = { 0.5, 0.5 };
	std::cout << "Node : " << node << " with cpd " << "[" << az[0] << ", " << az[1] << "]" << std::endl;
	add_cpds(node, az);
	cpd = get_cpds(node);
	std::vector<double> acc = get<t_cpd_root>(cpd);
	std::cout << "cpd after performing function " << "[" << acc[0] << ", " << acc[1] << "]" << std::endl;
	std::cout << "Final  Belief_nodes\n\n";
	for (auto nod : bel_nodes) { std::cout << nod << std::endl; };
	//connecting new node
	std::vector<std::string> zeleafs = get_leaf_nodes();
	for (auto lf : zeleafs) { edges.push_back({ node, lf }); };
	std::cout << "with edges\n\n";
	for (auto ed : edges) { std::cout << "[" << ed[0] << ", " << ed[1] << "]\n\n"; };
	
	std::cout << std::endl << std::endl << "********************** TEST OF add_cpds() for a leaf node ***************************************\n\n"; 
	assemble();
	std::cout << "Change existing leaf node \n";
	node = pro_nodes[0]["name"].get <std::string>();
	std::cout << "Node : " << node << std::endl;
	t_cpd cpd2 = get_cpds(node);
	std::vector<std::vector<double> > b = get<t_cpd_leaf>(cpd2);
	std::cout << "Present cpd :\n";
	for (unsigned row = 0; row < b.size(); row++)
	{
		std::cout << "[";
		for (unsigned column = 0; column < b[0].size(); column++) {std::cout << b[row][column] << " ";}
		std::cout << "]\n";
	}
	std::cout << "Changing cpd to ";
	t_cpd_leaf lab = { { 0.7, 0.5, 0.5, 0.8 },{0.3, 0.5, 0.5, 0.7} };
	for (unsigned row = 0; row < lab.size(); row++)
	{
		std::cout << "[";
		for (unsigned column = 0; column < lab[0].size(); column++) { std::cout << lab[row][column] << " "; }
		std::cout << "]\n";
	}
	add_cpds(node, lab);
	std::cout <<  "changed leaf node" << std::endl;
	cpd2 = get_cpds(node);
	std::vector<std::vector<double> > lac = get<t_cpd_leaf>(cpd2);
	std::cout << "Adapted cpd \n";
	for (unsigned row = 0; row < lac.size(); row++)
	{
		std::cout << "[";
		for (unsigned column = 0; column < lac[0].size(); column++) { std::cout << lac[row][column] << " "; }
		std::cout << "]\n";
	}
	assemble();
	



	std::cout << std::endl << std::endl << "********************** TEST of topologies ***************************************\n\n";
	std::cout << "Push key\n"; std::cin.get(); 
	std::vector<json> topo = get_topologies(*this, 1);
	std::cout << " size topologies : " << topo.size() << std::endl;
	for (unsigned top = 0; top < topo.size(); top++) { std::cout << topo[top] << std::endl; }
	edges = fully_connected_network(*this);


	std::cout << std::endl << std::endl << "********************** TEST various get methods  ***************************************\n\n";
	std::cout << "Push key\n"; std::cin.get();
	std::cout << "List of all nodes : \n";
	std::vector<std::string> all_nodes = get_nodes();
	for_each(all_nodes.begin(), all_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	std::cout << "List of all root nodes : \n";
	std::vector<std::string> root_nodes = get_root_nodes();
	for_each(root_nodes.begin(), root_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	std::cout << "List of all leaf nodes : \n";
	std::vector<std::string> leaf_nodes = get_leaf_nodes();
	for_each(leaf_nodes.begin(), leaf_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	std::cout << "List of all len nodes : \n";
	std::vector<std::string> len_nodes = get_bel_nodes();
	for_each(len_nodes.begin(), len_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	std::cout << "List of all ext nodes : \n";
	std::vector<std::string> ext_nodes = get_ext_nodes();
	for_each(ext_nodes.begin(), ext_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	std::cout << "List of all pro nodes : \n";
	std::vector<std::string> pro_nodes = get_pro_nodes();
	for_each(pro_nodes.begin(), pro_nodes.end(), [](std::string node) {std::cout << node << std::endl; });
	
	
	std::cout << std::endl << std::endl << "********************** TEST of bayesian network ***************************************\n\n";
	std::cout << "Push key\n"; std::cin.get();
	to_bayesian_network();
	//typedef dlib::set<unsigned long>::compare_1b_c set_type;
	//typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
	join_tree_type join_tree;
	create_moral_graph(bn, join_tree);
	create_join_tree(join_tree, join_tree);
	std::vector<std::string> leafs = get_leaf_nodes();
	set_node_value(bn, node_dic[leafs[1]], 1);
	set_node_as_evidence(bn, node_dic[leafs[1]]);
	bayesian_network_join_tree solution_with_evidence(bn, join_tree);
	std::cout << "Leaf node " << leafs[1] << " was set to state 1 and is set as evidence.\n\n";
	std::cout << "The probabilty of the different nodes given that evidence is\n\n\n";
	std::vector<std::string> mynodes = get_nodes();
	for (unsigned node = 0; node < mynodes.size(); node++) {
		unsigned anode = node_dic[mynodes[node]];
		matrix<double> prob = solution_with_evidence.probability(anode);//just an example we check the probablity of node 0 to be in state 1
		std::cout << "Node " << mynodes[node] << ": " << prob << std::endl;
	}

	std::cout << std::endl << std::endl << "********************** TEST for to_json -> see output_test.json file ***************************************\n\n";
	std::cout << "Push key\n"; std::cin.get();
	assemble();
	json anetwork = to_json();
	write_to_file("output_test", anetwork);



	std::cout << "\n\nDone" << std::endl;
	log.close();
}




