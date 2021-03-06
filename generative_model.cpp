#include "generative_model.h"

GenerativeModel::GenerativeModel() {}; 


GenerativeModel::GenerativeModel(const PeepoNetwork& a_peepo, const SensoryInput& a_sensory):
	peepo_network(a_peepo),
	sensory_input(a_sensory)
{
	bool ok = peepo_network.to_bayesian_network();
};


float GenerativeModel::process(void)
{
	float total_prediction_error_size = 0.0f;
	std::map<std::string, std::vector<float> > predic = predict();
	for (auto node:predic) {
		std::string node_name = node.first;
		if (is_leaf(node_name)) {
			std::vector<float> prediction = node.second;
			std::vector<float> observation = sensory_input.value(node_name);
			std::vector<float> prediction_error = error(prediction, observation);
			float prediction_error_size = error_size(prediction, observation);
			float precision_ = precision(prediction);
			total_prediction_error_size += prediction_error_size;
			if (prediction_error_size > 0.1) {
				error_minimization(node_name, precision_, prediction_error, prediction);
			}
		}
	}
	return total_prediction_error_size;
};

std::map<std::string, std::vector<float> > GenerativeModel::predict(void)
{
	std::map<std::string, std::vector<float> > inference;
	std::map<std::string, unsigned>   evidences = get_root_values();
	return peepo_network.get_inference(evidences);
}

std::vector<float> GenerativeModel::error(const std::vector<float>& pred, const std::vector<float>& obs)
{
	std::vector<float> err;
	if (pred.size() != obs.size()) {
		std::cout << "Error in error estimation (prediction and observation have different size." << std::endl;
		std::cin.get();
	}
	for (int i = 0; i < pred.size(); i++) {
		err.push_back(obs[i] - pred[i]);
	}
	return err;
}


float GenerativeModel::error_size(const std::vector<float>& pred, const std::vector<float>& obs)
{
	if (pred.size() != obs.size()) {
		std::cout << "Error in error size estimation (prediction and observation have different size." << std::endl;
		std::cin.get();
	}
	float entropy = 0.f;
	for (int i = 0; i < pred.size(); i++) {
		entropy += obs[i] * log(pred[i] / obs[i]);
	}
	return entropy;
}

float GenerativeModel::precision(const std::vector<float>& pred)
{
	float entropy = 0.f;
	for (int i = 0; i < pred.size(); i++) {
		entropy -=pred[i] * log2f(pred[i] );
	}
	return entropy;
}

void GenerativeModel::error_minimization(const std::string& node_name, const float& precision, 
										const std::vector<float>& prediction_error, 
										const std::vector<float>& prediction)
{

	hypothesis_update(node_name,  prediction_error, prediction);
	//?? why both functions as error_minimization doesn't preform anything
}

void GenerativeModel::hypothesis_update(const std::string& node_name, 
										const std::vector<float>& prediction_error,
										const std::vector<float>& prediction)
{
	bool is_action = false;
	for(auto node: peepo_network.get_pro_nodes()){
		if (node == node_name) { is_action = true; break; }
	}
	if (is_action) {
		sensory_input.action(node_name, prediction);
	}
	else{
		std::map<std::string, unsigned>   evidence;
		std::vector<float> sum = prediction_error;
		std::transform(sum.begin(), sum.end(), prediction.begin(), sum.begin(), std::plus<float>());
		evidence[node_name] = std::max_element(sum.begin(), sum.end()) - sum.begin();
		std::map<std::string, std::vector<float> > inference = peepo_network.get_inference(evidence);
		for (auto root : get_roots()) {
			//old_hypo = self.bayesian_network.states[root_index].distribution.items()--> ??
			std::vector<float>  new_hypo = inference.at(root);
			peepo_network.add_cpds(root, new_hypo);
		}
	bool ok = peepo_network.to_bayesian_network();//we need to refresh the bayesian newtork with the new cpds of the root
	}
}


std::map<std::string, unsigned > GenerativeModel::get_root_values(void)
{
	std::map<std::string, unsigned > roots;
	for (auto node : get_roots())
	{
		std::vector<float> cpd = peepo_network.get_cpds_root(node);
		roots[node] = std::max_element(cpd.begin(), cpd.end()) - cpd.begin();
	}
	return roots;
}


std::vector<std::string > GenerativeModel::get_roots(void)
{
	return peepo_network.get_active_root_nodes();
}


std::vector<std::string > GenerativeModel::get_leafs(void)
{
	return peepo_network.get_leaf_nodes();
}



bool GenerativeModel::is_leaf(const std::string& node)
{
	for (auto leaf : get_leafs()) {
		if (leaf == node) { return true; }
	}
	return false;
}

/*
bool GenerativeModel::is_root(const std::string& node)
{
	for (auto root : get_roots()) {
		if (root == node) { return true; }
	}
	return false;
}*/


//unsigned GenerativeModel::get_node_index(const std::string&) { ; }