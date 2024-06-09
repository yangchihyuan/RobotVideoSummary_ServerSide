#include "ReID.hpp"
#include "utility_compute.hpp"
#include <algorithm>  //for min_element
#include <limits>
#include <fstream>      // std::ofstream

feature_id_pair::feature_id_pair(array<float,1536> input_feature, int input_id)
: feature(input_feature),
id(input_id)
{}

ReID::ReID()
:sample_number_limitation(numeric_limits<unsigned int>::max())
{   
}

ReID::ReID(unsigned int sample_number_limitation)
:sample_number_limitation(sample_number_limitation)
{
    
}

void ReID::AddSample(array<float, 1536> feature, int id)
{
    if( pairs.size() >= sample_number_limitation)
        pairs.erase(pairs.begin());
    pairs.push_back(feature_id_pair(feature, id));
}

unsigned int ReID::GetSampleNumber()
{
    return pairs.size();
}

bool ReID::HaveSufficientSamples()
{
    return pairs.size() == sample_number_limitation;
}

vector<int> ReID::SortByFeatureSimilarity(vector<array<float, 1536>> features)
{
    unsigned int features_size = features.size();
    vector<float> minimal_norm_vector(features_size, numeric_limits<float>::max());
    for(unsigned int f_index = 0 ; f_index < features_size; f_index++)
    {
        array<float, 1536> feature = features[f_index];
        for( unsigned int i=0; i<pairs.size(); i++)
        {
            float norm_value = ComputeL2Norm(feature, pairs[i].feature);
//            int id = pairs[i].id;
            if( norm_value < minimal_norm_vector[f_index])
                minimal_norm_vector[f_index] = norm_value;
        }
    }
    //sort minimal_norm_vector ascendantly
//    int index = min_element(minimal_norm_vector.begin(), minimal_norm_vector.end()) - minimal_norm_vector.begin();
//    return index;
    return SortIndex(minimal_norm_vector, false);
}

void ReID::DumpSamples(string file_path)
{
    ofstream outfile(file_path,ofstream::out);
    for(unsigned int i = 0 ; i < pairs.size(); i++)
    {  
        outfile << i+1 << ",";
        outfile << pairs[i].id;
        array<float, 1536> feature = pairs[i].feature;
        for( unsigned int j=0; j<1536; j++)
        {
            outfile << "," << feature[j];
        }
        outfile << endl;
    }
    outfile.close();
}


void ReID::LoadSampleFeature(vector<array<float,1536>> input_vector)
{
    for(unsigned int i=0; i<input_vector.size(); i++)
    {
        feature_id_pair pair(input_vector[i], 0);
        pairs.push_back(pair);
    }
}
