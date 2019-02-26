#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>
#include <time.h>

#include "peepo_world.h"
#include "peepo_network.h"
#include "genetic_algorithm.h"
using json = nlohmann::json;

using namespace std;

// ----------------------------------------------------------------------------------------

int main()
{
	const unsigned trials = 1;
	//Parallel Pp;
	//Pp.process_parallel(trials,number_of_peepos);
	//PeepoNetwork Pn("best_life_game_network");
	//PeepoNetwork Pn("xor");
	//PeepoNetwork Pn("survival");

	const int npop =  500;
	const unsigned n_evolutions = 10;
	{//non_parallel scope bracket

	GeneticAlgorithm ga("game_of_life", true, 10000, 1.0, npop, 1.0, 1.0, 0);
	std::vector<Agent> my_peepos = ga.get_population();
	//first we set the clock to check the performance
	auto start = std::chrono::high_resolution_clock::now();
	float m_fitness = 0.0;
	unsigned n_inferences = 0;
	for (unsigned evolv = 0; evolv < n_evolutions; evolv++) {
		unsigned count = 0;
		for (auto peepo : my_peepos) {
			if (count < npop ) {
				bool ok = peepo.network.to_bayesian_network();
				if (!ok) { std::cout << "Not resolvable graph structure" << std::endl; count++; continue; }//THIS IS IMPORTANT TO CHECK
				std::vector<std::string> leafs = peepo.network.get_leaf_nodes();
				std::vector<std::string> roots = peepo.network.get_root_nodes();
				std::map<std::string, unsigned>   evidences;

				for (unsigned r = 0; r < roots.size(); r++) { evidences[roots[r]] = 1; }//seting all root_nodes as evidence with staus = 1
				//evidences[roots[0]] = 1;// floor(std::rand() % 2);
				//evidences[leafs[0]] = 1;
				//evidences[leafs[3]] = 1;
				std::map<std::string, std::vector<float> > mynodes = peepo.network.get_inference(evidences);
				n_inferences++;
				
				
				/*std::cout << std::endl << std::endl << "********************** TEST inference for peepo nr : " << count << "  ******\n\n";
				std::cout << "The following nodes have ben set as an evidence :\n";
				for (std::map<std::string, unsigned >::iterator it = evidences.begin(); it != evidences.end(); ++it)
				{
					std::cout << "Node : " << it->first << " with state  " << it->second << std::endl;
				}
				std::cout << "The states probabilties of the different nodes given that evidence is : \n";
				for (std::map<std::string, std::vector<float> >::iterator it = mynodes.begin(); it != mynodes.end(); ++it)
				{
					std::cout << "Node " << it->first << " has probablity of states : (";
					for (auto p : it->second) { std::cout << p << "  "; };
					std::cout << " )" << std::endl;
				}
				std::cout << "Number of edges for peepo " << count << " :" << peepo.network.get_edges().size() << std::endl;
				*/
			}//endif for selected peepo
			count++;
			my_peepos[count].fitness = float(std::rand() % 10000) / 10000.0;
		}//peepo loop
		bool converge = false;// = ga.evolve(my_peepos, m_fitness);
		my_peepos = ga.evolve(my_peepos, m_fitness, converge);

		//std::cout << "Average fitness = " << m_fitness << " for " << my_peepos.size() << " peepos. Converging : " << boolalpha << converge << std::endl;
		//std::cout << "Hit a key." << std::endl;
	}//evolve loop

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << "\n\n\n" << "Time needed in NON  parallel mode for " << n_inferences << " full inferences : " << microseconds / 1000000.0 << " sec." << std::endl;
	}//non parallel scope


	{//parallel scope bracket

		GeneticAlgorithm ga("game_of_life", true, 10000, 1.0, npop, 1.0, 1.0, 0);
		std::vector<Agent> my_peepos = ga.get_population();
		//first we set the clock to check the performance
		auto start = std::chrono::high_resolution_clock::now();
		float m_fitness = 0.0;
		unsigned n_inferences = 0;
		const unsigned num_threads = npop;
		for (unsigned evolv = 0; evolv < n_evolutions; evolv++) {
			unsigned count = 0;
			//now we loop through all the peepos through a pseudo loop; dLib will try to split the loop in several chunks// loops
			parallel_for(num_threads, 0, my_peepos.size(), [&](long peepo) { //this is the pseudo loop cast as a lambda function
					bool ok = my_peepos[peepo].network.to_bayesian_network();
					//if (!ok) { std::cout << "Not resolvable graph structure" << std::endl; count++; continue; }//THIS IS IMPORTANT TO CHECK BUT IN // "continue" does not work -> to investigate
					std::vector<std::string> leafs = my_peepos[peepo].network.get_leaf_nodes();
					std::vector<std::string> roots = my_peepos[peepo].network.get_root_nodes();
					std::map<std::string, unsigned>   evidences;
					for (unsigned r = 0; r < roots.size(); r++) { evidences[roots[r]] = 1; }
					//evidences[roots[0]] = 1;// floor(std::rand() % 2);
					//evidences[leafs[0]] = 1;
					//evidences[leafs[3]] = 1;
					std::map<std::string, std::vector<float> > mynodes = my_peepos[peepo].network.get_inference(evidences);
					n_inferences++;


					/*std::cout << std::endl << std::endl << "********************** TEST inference for peepo nr : " << count << "  ******\n\n";
					std::cout << "The following nodes have ben set as an evidence :\n";
					for (std::map<std::string, unsigned >::iterator it = evidences.begin(); it != evidences.end(); ++it)
					{
						std::cout << "Node : " << it->first << " with state  " << it->second << std::endl;
					}
					std::cout << "The states probabilties of the different nodes given that evidence is : \n";
					for (std::map<std::string, std::vector<float> >::iterator it = mynodes.begin(); it != mynodes.end(); ++it)
					{
						std::cout << "Node " << it->first << " has probablity of states : (";
						for (auto p : it->second) { std::cout << p << "  "; };
						std::cout << " )" << std::endl;
					}
					std::cout << "Number of edges for peepo " << count << " :" << peepo.network.get_edges().size() << std::endl;
					*/
		
					my_peepos[peepo].fitness = float(std::rand() % 10000) / 10000.0;
			});//peepo loop and closining bracket for parallel lamdda funfion
			bool converge = false;// = ga.evolve(my_peepos, m_fitness);
			my_peepos = ga.evolve(my_peepos, m_fitness, converge);

			//std::cout << "Average fitness = " << m_fitness << " for " << my_peepos.size() << " peepos. Converging : " << boolalpha << converge << std::endl;
			//std::cout << "Hit a key." << std::endl;
		}//evolve loop

		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		std::cout << "\n\n\n" << "Time needed in PARALLLEL mode for " << n_inferences << " full inferences : " << microseconds / 1000000.0 << " sec." << std::endl;
	}// parallel scope
	
	std::cout << "Done " << std::endl;
    std::cin.get();
    return 0;
}





