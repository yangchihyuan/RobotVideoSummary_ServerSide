#include "ReID.hpp"
#include "utility_compute.hpp"
#include <algorithm>  //for min_element
#include <limits>

feature_id_pair::feature_id_pair(array<float,1536> input_feature, int input_id)
: feature(input_feature),
id(input_id)
{}


ReID::ReID()
:sample_number_limitation(100)
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



