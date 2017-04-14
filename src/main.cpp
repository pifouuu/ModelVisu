/*
 * displayResults.cpp
 *
 *  Created on: 27 févr. 2017
 *      Author: pierre
 */

#include <vector>
#include <cmath>
// include input and output archivers
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// include this header to serialize vectors
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include "../include/gnuplot-iostream.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <algorithm>
#include <functional>

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}

std::vector<float> expo_smooth(std::vector<float>& raw, float alpha){
	std::vector<float> smooth;
	smooth.push_back(raw[0]);
	for (int i=1;i<raw.size();i++){
		smooth.push_back(alpha*raw[i]+(1-alpha)*smooth[i-1]);
	}
	return smooth;
}

boost::tuple<std::vector<float>,std::vector<float>> postproc(std::vector<float>& graph, std::vector<int>& axis,
		std::vector<float>& trials, float alpha)
{
    std::vector<float> result_graph;
    std::vector<float> result_axis;
    for (int i =0; i<graph.size();i++){
    	if (trials[i]!=0){
    		result_graph.push_back(graph[i]/trials[i]);
    		result_axis.push_back(axis[i]);
    	}
    }
    return boost::make_tuple(result_axis, expo_smooth(result_graph,alpha));
}



std::vector<float> avg_smooth(std::vector<float>& raw, int ws){
	std::vector<float> smooth;
	for (int j=0;j<ws;j++){
		smooth.push_back(raw[j]);
	}
	for (int i=ws;i<raw.size()-ws;i++){
		float val = 0.;
		for (int k=-ws;k<ws;k++){
			val += raw[i+k];
		}
		smooth.push_back(val/(2*ws));
	}
	for (int i=raw.size()-ws;i<raw.size();i++){
		smooth.push_back(raw[i]);
	}
	return smooth;
}

int main(){
	Gnuplot gp;
	std::vector<std::string> actions;
	actions.push_back(std::string("GO_TO_EYE"));
	actions.push_back(std::string("LOOK_RED_BOX"));
	actions.push_back(std::string("LOOK_BLUE_BOX"));
	actions.push_back(std::string("PICK"));
	//actions.push_back(std::string("PUT_DOWN"));
	actions.push_back(std::string("PUT_IN"));
//	actions.push_back(std::string("LOOK_RED_BLOCK_0"));
//	actions.push_back(std::string("LOOK_RED_BLOCK_1"));
	actions.push_back(std::string("LOOK_BLUE_BLOCK_0"));
	//actions.push_back(std::string("LOOK_BLUE_BLOCK_1"));
	//actions.push_back(std::string("LOOK_BLUE_BLOCK_2"));
	int NUMACTIONS = actions.size();

	float alpha = 0.8;
	int ws = 10;
	int numComp = 14;

	/*bool ACT_ACC = false;
	bool ACT_TRY = false;
	bool ACT_SUC = false;
	bool BLOCKS_IN = false;
	bool BLOCKS_RIGHT = false;
	bool MODEL_ACC = false;
	bool MODEL_ACC_R = false;
	bool ACCU_R = true;
	bool ACCU_TUTOR_R = false;*/

/*	fs::path basedir("../myTexplore/resultats/");
	  fs::directory_iterator end_iter;

	  typedef std::multimap<std::time_t, fs::path> result_set_t;
	  result_set_t dirnames;

	  if ( fs::exists(basedir) && fs::is_directory(basedir))
	  {
	    for( fs::directory_iterator dir_iter(basedir) ; dir_iter != end_iter ; ++dir_iter)
	    {
	        result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
	    }
	  }
	  if(fs::is_directory(basedir)) {
	    std::cout << basedir << " is a directory containing:\n";

	    for(auto& entry : boost::make_iterator_range(fs::directory_iterator(basedir), {})){
	//      std::cout << entry << "\n";
	      dirnames.insert(result_set_t::value_type(fs::last_write_time(entry), entry));
	    }
	  }
	  auto folder = dirnames.rbegin();
	  auto myEnd = std::next(result_set.rbegin(),);
	  while (folder != myEnd && folder != dirnames.rend()){
	    std::cout << folder->second << std::endl;
	    ++folder;
	  }*/



	std::string basedir = "../myTexplore/resultats_3/";
//	std::string basedir = "/home/pierre/Dropbox/resultats/";
	std::vector<std::string> dirnames;
	dirnames.push_back("14-04-2017_11-31-35_2__v_0_n_0_tb_0_pretrain_0_fR_100_nbR_0_nbB_1/");
	dirnames.push_back("14-04-2017_11-32-31_2__v_0_n_0_tb_0_pretrain_0_fR_0_nbR_0_nbB_1/");
//	dirnames.push_back("12-04-2017_16-45-09_v_0_n_0_tb_0_pretrain_10000_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("13-04-2017_12-46-01_v_0_n_0_tb_0_pretrain_10000_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("08-04-2017_05-53-21_v_10_n_30_tb_10_pretrain_1000_fR_100_nbR_1_nbB_0/");
//	dirnames.push_back("08-04-2017_07-35-04_v_10_n_30_tb_30_pretrain_1000_fR_100_nbR_1_nbB_0/");
//	dirnames.push_back("08-04-2017_09-16-31_v_10_n_30_tb_50_pretrain_1000_fR_100_nbR_1_nbB_0/");

//	dirnames.push_back("01-04-2017_01-37-02_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_01-42-09_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_01-48-37_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_01-52-17_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_01-56-40_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-01-31_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-06-00_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-10-47_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-14-36_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-18-46_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-23-39_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-29-13_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-34-42_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-40-05_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-44-59_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");
//	dirnames.push_back("01-04-2017_02-48-48_v_20_n_20_tb_20_pretrain_100_fR_100_nbR_0_nbB_1/");

	std::string name;
	std::ifstream ifs;
	int numdir = dirnames.size();



	for (int i=0; i<NUMACTIONS; i++){
		gp << "set xrange [0:1000]\nset yrange [0:1]\n";
		gp << "set title '" << actions[i] << "'\n";
		gp << "set terminal x11 "<< i <<" \n";
		gp << "plot";
		for (auto dirname: dirnames){
			name = basedir+dirname+"model_acc_";
			std::vector<float> graph;
			std::vector<int> x_axis;

			ifs.open(name+actions[i]+".ser");
			boost::archive::text_iarchive graph_archive(ifs);
			graph_archive & graph;
			ifs.close();
			ifs.clear();

			ifs.open(basedir+dirname+"x_axis.ser");
			boost::archive::text_iarchive axis_archive(ifs);
			axis_archive & x_axis;
			ifs.close();
			ifs.clear();

			std::vector<float> num_trials;
			ifs.open(basedir+dirname+"num_trials.ser");
			boost::archive::text_iarchive num_trials_archive(ifs);
			num_trials_archive & num_trials;
			ifs.close();
			ifs.clear();

			auto res = postproc(graph,x_axis,num_trials,alpha);

			gp << gp.file1d(res) << "with lines title '"<< dirname <<"',";
		}
		gp << std::endl;
	}

	gp << "set xrange [0:1000]\nset yrange [0:1]\n";
	gp << "set title 'global model'\n";
	gp << "set terminal x11 "<< NUMACTIONS <<" \n";
	gp << "plot";
	for (auto dirname: dirnames){

		std::vector<int> x_axis;
		ifs.open(basedir+dirname+"x_axis.ser");
		boost::archive::text_iarchive axis_archive(ifs);
		axis_archive & x_axis;
		ifs.close();
		ifs.clear();

		std::vector<float> graph_tot(x_axis.size(),0.);

		name = basedir+dirname+"model_acc_";

		for (int i=0;i<NUMACTIONS;i++){
			std::vector<float> graph;
			ifs.open(name+actions[i]+".ser");
			boost::archive::text_iarchive graph_archive(ifs);
			graph_archive & graph;
			ifs.close();
			ifs.clear();
			graph_tot = graph_tot+graph;
		}

		for (int i=0;i<graph_tot.size();i++){
			graph_tot[i]/=NUMACTIONS;
		}
		std::vector<float> num_trials;
		ifs.open(basedir+dirname+"num_trials.ser");
		boost::archive::text_iarchive num_trials_archive(ifs);
		num_trials_archive & num_trials;
		ifs.close();
		ifs.clear();

		auto res = postproc(graph_tot,x_axis,num_trials,alpha);

		gp << gp.file1d(res) << "with lines title '"<< dirname <<"',";
	}
	gp << std::endl;


	gp << "set xrange [0:10000]\nset yrange [0:1]\n";
	gp << "set title 'Reward model error'\n";
	gp << "set terminal x11 "<< NUMACTIONS+1 <<" \n";
	gp << "plot";
	for (auto dirname: dirnames){
		name = basedir+dirname+"reward_model_acc.ser";
		std::vector<float> graph;
		std::vector<int> x_axis;

		ifs.open(name);
		boost::archive::text_iarchive ia(ifs);
		ia & graph;
		ifs.close();
		ifs.clear();

		ifs.open(basedir+dirname+"x_axis.ser");
		boost::archive::text_iarchive axis_archive(ifs);
		axis_archive & x_axis;
		ifs.close();
		ifs.clear();

		std::vector<float> num_trials;
		ifs.open(basedir+dirname+"num_trials.ser");
		boost::archive::text_iarchive num_trials_archive(ifs);
		num_trials_archive & num_trials;
		ifs.close();
		ifs.clear();
		auto res = postproc(graph,x_axis,num_trials,alpha);

		gp << gp.file1d(res) << "with lines title '"<< dirname <<"',";
	}
	gp << std::endl;


	for (int i=0; i<numComp; i++){
		gp << "set xrange [0:1000]\nset yrange [0:1]\n";
		gp << "set title 'component "+std::to_string(i)+"'\n";
		gp << "set terminal x11 "<< NUMACTIONS+1+i <<" \n";
		gp << "plot";
		for (auto dirname: dirnames){
			name = basedir+dirname+"component_acc_";
			std::vector<float> graph;
			std::vector<int> x_axis;

			ifs.open(name+std::to_string(i)+".ser");
			boost::archive::text_iarchive graph_archive(ifs);
			graph_archive & graph;
			ifs.close();
			ifs.clear();

			ifs.open(basedir+dirname+"x_axis.ser");
			boost::archive::text_iarchive axis_archive(ifs);
			axis_archive & x_axis;
			ifs.close();
			ifs.clear();

			std::vector<float> num_trials;
			ifs.open(basedir+dirname+"num_trials.ser");
			boost::archive::text_iarchive num_trials_archive(ifs);
			num_trials_archive & num_trials;
			ifs.close();
			ifs.clear();

			auto res = postproc(graph,x_axis,num_trials,alpha);

			gp << gp.file1d(res) << "with lines title '"<< dirname <<"',";
		}
		gp << std::endl;
	}


}




