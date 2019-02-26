#ifndef PEEPO_WORLD__H
#define PEEPO_WORLD__H
#include <dlib/bayes_utils.h>
#include <dlib/graph_utils.h>
#include <dlib/graph.h>
#include <dlib/rand.h>
#include <dlib/directed_graph.h>
#include <dlib/threads.h>


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
#include "combinations.h"
//#include "ppl.h"   
using namespace dlib;

class Parallel{

private:
// ----------------------------------------------------------------------------------------
   struct Peepo
   {
   	directed_graph<bayes_node>::kernel_1a_c bn;
	float fitness;
	unsigned identification;
   };
   struct Results
   {
	   matrix<double> probability;
	   unsigned identification;
   };

public:
	   Parallel();
	   ~Parallel();
	   static void process_parallel(const unsigned& trials, const unsigned&);
	   
} ;
#endif
