#include "peepo_world.h"


using namespace dlib;
using namespace std;


Parallel::Parallel()
{
 ;
}


Parallel:: ~Parallel()
{
 ;
}
void Parallel::process_parallel(const unsigned& TRIALS, const unsigned& num_peepos)
{
    try
    {
		//std::string log_file = "PARALLEL_VISUALCPP.txt";
		std::string log_file = "PARALLEL_MINGW.txt";
		std::ofstream dest(log_file.c_str(), std::ios::binary);
        using namespace bayes_node_utils;
		srand (time(NULL));
		std::vector<Peepo*> my_peepos;
	    //unsigned long num_threads = static_cast<unsigned> (std::thread::hardware_concurrency()/2);
		const unsigned num_threads = num_peepos;
		for(unsigned organism = 0; organism < num_peepos; organism++){
        	Peepo* a = new Peepo;
			my_peepos.push_back(a);
		           }

        // This statement declares a bayesian network called bn.  Note that a bayesian network
        // in the dlib world is just a directed_graph object that contains a special kind 
        // of node called a bayes_node.

        // Use an enum to make some more readable names for our nodes.
        enum nodes
        {
            A = 0,//root
            B = 1,
            C = 2,
            D = 3,
			E = 4,

			F = 5, //leaf
		  	G = 6,
		  	H = 7,
			I = 8,
			J = 9

        };
        for(unsigned i = 0; i < num_peepos; i++)
		{
        // The next few blocks of code setup our bayesian network.
		my_peepos[i]->fitness = 0.0;
		my_peepos[i]->identification  = i;

        my_peepos[i]->bn.set_number_of_nodes(10);
        my_peepos[i]->bn.add_edge(A, F);
        my_peepos[i]->bn.add_edge(A, G);
        my_peepos[i]->bn.add_edge(A, H);
		my_peepos[i]->bn.add_edge(A, I);
        my_peepos[i]->bn.add_edge(A, J);
        my_peepos[i]->bn.add_edge(B, F);
        my_peepos[i]->bn.add_edge(B, G);
        my_peepos[i]->bn.add_edge(B, H);
		my_peepos[i]->bn.add_edge(B, I);
        my_peepos[i]->bn.add_edge(B, J);
        my_peepos[i]->bn.add_edge(C, F);
        my_peepos[i]->bn.add_edge(C, G);
        my_peepos[i]->bn.add_edge(C, H);
		my_peepos[i]->bn.add_edge(C, I);
        my_peepos[i]->bn.add_edge(C, J);
        my_peepos[i]->bn.add_edge(D, F);
        my_peepos[i]->bn.add_edge(D, G);
        my_peepos[i]->bn.add_edge(D, H);
		my_peepos[i]->bn.add_edge(D, I);
        my_peepos[i]->bn.add_edge(D, J);
        my_peepos[i]->bn.add_edge(E, F);
        my_peepos[i]->bn.add_edge(E, G);
        my_peepos[i]->bn.add_edge(E, H);
		my_peepos[i]->bn.add_edge(E, I);
        my_peepos[i]->bn.add_edge(E, J);



        // Now we inform all the nodes in the network that they are binary
        // nodes.  That is, they only have two possible values.  
        set_node_num_values(my_peepos[i]->bn, A, 2);
        set_node_num_values(my_peepos[i]->bn, B, 2);
        set_node_num_values(my_peepos[i]->bn, C, 2);
        set_node_num_values(my_peepos[i]->bn, D, 2);
        set_node_num_values(my_peepos[i]->bn, E, 2);
        set_node_num_values(my_peepos[i]->bn, F, 2);
        set_node_num_values(my_peepos[i]->bn, G, 2);
        set_node_num_values(my_peepos[i]->bn, H, 2);
        set_node_num_values(my_peepos[i]->bn, I, 2);
        set_node_num_values(my_peepos[i]->bn, J, 2);


        assignment parent_state;
        // Now we will enter all the conditional probability information for each node.
        // Each node's conditional probability is dependent on the state of its parents.  
        // To specify this state we need to use the assignment object.  This assignment 
        // object allows us to specify the state of each nodes parents. 


        // Here we specify that p(B=1) = 0.01
        // parent_state is empty in this case since B is a root node.
        set_node_probability(my_peepos[i]->bn, A, 1, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, A, 0, parent_state, 0.5); 
        set_node_probability(my_peepos[i]->bn, B, 1, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, B, 0, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, C, 1, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, C, 0, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, D, 1, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, D, 0, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, E, 1, parent_state, 0.5);
        set_node_probability(my_peepos[i]->bn, E, 0, parent_state, 0.5);
		std::vector<int> parents_states_cardinality = { 2,2,2,2,2};
 		std::vector<std::vector<int> > parents_states_matrix = States::get_index_matrix(parents_states_cardinality)	;	
		for(unsigned child = 5; child < 10; child++){
			std::srand (1); 
			for(unsigned states = 0; states < parents_states_matrix[0].size(); states++){
					parent_state.clear();
					for(unsigned parent = 0; parent < parents_states_matrix.size(); parent++){
                        parent_state.add(parent,parents_states_matrix[parent][states]);
						}
					int x = std::rand() % 100; // v1 in the range 0 to 99
				    float p = (x+1)/101.0;         
		  			set_node_probability(my_peepos[i]->bn, child, 0, parent_state, p);
           		   	set_node_probability(my_peepos[i]->bn, child, 1, parent_state, 1.-p);				
					}
			}
	   }
        // We have now finished setting up our bayesian network.  So let's compute some 
        // probability values.  The first thing we will do is compute the prior probability
        // of each node in the network.  To do this we will use the join tree algorithm which
        // is an algorithm for performing exact inference in a bayesian network.   

        // First we need to create an undirected graph which contains set objects at each node and
        // edge.  This long declaration does the trick.
        typedef dlib::set<unsigned long>::compare_1b_c set_type;
        typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;


        // Now we need to populate the join_tree with data from our bayesian network.  The next  
        // function calls do this.  Explaining exactly what they do is outside the scope of this
        // example.  Just think of them as filling join_tree with information that is useful 
        // later on for dealing with our bayesian network.  


        // Now that we have a proper join_tree we can use it to obtain a solution to our
        // bayesian network.  Doing this is as simple as declaring an instance of
        // the bayesian_network_join_tree object as follows:
        


        // Now to make things more interesting let's say that we have discovered that the leaf node F 
        // node really has a value of 1.  That is to say, we now have evidence that 
        // F is 1.  We can represent this in the network using the following  functions
        // calls  
		//set_node_value(my_peepos[peepo]->bn, child, 1); with child = F or 5 (see enum)
        //set_node_as_evidence(my_peepos[peepo]->bn, child); 
        //bayesian_network_join_tree solution_with_evidence(my_peepos[peepo]->bn, join_tree);
		//now we are able to get the result of this inference with :
		//bayesian_network_join_tree solution(my_peepos[peepo]->bn, join_tree); 
		//matrix<double> prob = solution.probability(0) ;//just an example we check the probablity of node 0 to be in state 1  
		//     				


		//for illustration :we create a container to hold the probabilties of the inferences for 1 node eg node 0 and this for all peepos
		Results rst;
		std::vector<Results> probabilities_1;//for a inference with a child state 1
		std::vector<Results> probabilities_0;//for a inference with a child state 0
		//we now set the number of threads which is best for our configuration
		// Here we make an instance of the thread pool object.  We use the
		// global dlib::default_thread_pool(), which automatically selects the number of
		// threads based on your hardware.  
		//thread_pool tp(3); thread_pool& default_thread_pool tp;
		//thread_pool default_thread_pool();


		
				//the real stuf begins, first we set the clock to check the performance
		auto start = std::chrono::high_resolution_clock::now();

		//now we loop through all the peepos through a pseudo loop; dLib will try to split the loop in several chunks// loops
		parallel_for(num_threads, 0, my_peepos.size(), [&](long peepo) { //this is the pseudo loop cast as a lambda function
		//Concurrency::parallel_for(0, int(num_peepos), [&](int peepo) {
		//in the pseudo loop we create the unique objects needed for that peepo
		//trying  to incorporate in the pseudo_loop dependencies between chunks will cause problems!
        join_tree_type join_tree;
        create_moral_graph(my_peepos[peepo]->bn, join_tree);
        create_join_tree(join_tree, join_tree);

		for(unsigned trials = 0; trials < TRIALS; trials++)
		{
           for(unsigned child = 5; child < 10; child++){
           				{set_node_value(my_peepos[peepo]->bn, child, 1);
        				set_node_as_evidence(my_peepos[peepo]->bn, child); 
           		 	  	bayesian_network_join_tree solution_with_evidence(my_peepos[peepo]->bn, join_tree);
						//bayesian_network_join_tree solution(my_peepos[peepo]->bn, join_tree); 
						matrix<double> prob = solution_with_evidence.probability(0) ;//just an example we check the probablity of node 0 to be in state 1
						rst.probability = prob;
						rst.identification = peepo;
						probabilities_1.push_back(rst);
						} //scope bracket
           				{set_node_value(my_peepos[peepo]->bn, child, 0);
        				set_node_as_evidence(my_peepos[peepo]->bn, child);
           		 	  	bayesian_network_join_tree solution_with_evidence(my_peepos[peepo]->bn, join_tree);
						//bayesian_network_join_tree solution(my_peepos[peepo]->bn, join_tree);
						matrix<double> prob = solution_with_evidence.probability(0);//just an example we check the probablity of node 0 to be in state 1
						rst.probability = prob;
						rst.identification = peepo;
						probabilities_0.push_back(rst);
						} //scope bracket
						
				 } 

			}
}) ;
		//the work is done-> stop the clock and calculate cpu time
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

		//print the results
		dest << "States probablity parent node[0] for all peepos with leaf nodes at state 0  " << std::endl;
		for (unsigned k = 0; k < probabilities_0.size(); k++) { dest << probabilities_0[k].identification << ") : " << probabilities_0[k].probability << std::endl; }
		dest << "\n\n\n" << "States probablity parent node[0] for all peepos with leaf nodes at state 1  " << std::endl;
		for (unsigned k = 0; k < probabilities_1.size(); k++) { dest << probabilities_1[k].identification << ") : " << probabilities_1[k].probability << std::endl; }
		std::cout << "\n\n\n" << "Time needed in  parallel mode for " << probabilities_0.size() + probabilities_1.size() << " full inferences : " << microseconds / 1000000.0 << " sec." << std::endl;
		dest << "\n\n\n" << "Time needed in  parallel mode for " << probabilities_0.size() + probabilities_1.size() << " full inferences : " << microseconds / 1000000.0 << " sec." << std::endl;
		dest.close();
    }
    catch (std::exception& e)
    {
        cout << "exception thrown: " << endl;
        cout << e.what() << endl;
        cout << "hit enter to terminate" << endl;
		cin.get();
    }
}

