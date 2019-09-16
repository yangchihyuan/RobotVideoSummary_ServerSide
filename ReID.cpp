#include "ReID.hpp"
#include "utility_compute.hpp"
#include <algorithm>  //for min_element

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

int ReID::Retrieve_id(array<float, 1536> feature)
{
    vector<float> norm;
    for( unsigned int i=0; i<pairs.size(); i++)
    {
        float norm_value = ComputeL2Norm(feature, pairs[i].feature);
        norm.push_back(norm_value);
    }
    //find the minimum
    auto iterator = min_element(norm.begin(), norm.end());

    //how to get the index?
}


