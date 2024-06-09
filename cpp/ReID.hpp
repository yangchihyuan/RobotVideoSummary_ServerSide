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
    ReID(unsigned int sample_number_limitation);
    void AddSample(array<float, 1536> feature, int id);
    unsigned int GetSampleNumber();
    vector<int> SortByFeatureSimilarity(vector<array<float, 1536>> features);
    bool HaveSufficientSamples();
    void DumpSamples(string file_path);
    void LoadSampleFeature(vector<array<float,1536>>);

    private:
    vector<feature_id_pair> pairs;
    unsigned int sample_number_limitation;
};