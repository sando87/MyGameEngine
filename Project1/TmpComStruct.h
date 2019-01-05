#pragma once

#include <vector>
using namespace std;

struct BoneWeight
{
	int boneIndex;
	float weight;
};

struct BoneWeights
{
	vector<BoneWeight> weights;
};