#pragma once

#include <vector>
#include <array>

using namespace std;

struct feature_id_pair
{
    feature_id_pair(array<float,1536> feature, int id);
    array<float,1536> feature;
    int id;
};

class ReID
{
    public:
    ReID();
    void AddSample(array<float, 1536> feature, int id);
    unsigned int GetSampleNumber();
    int Retrieve_id(array<float, 1536> feature);

    private:
    vector<feature_id_pair> pairs;
    unsigned int sample_number_limitation;
};