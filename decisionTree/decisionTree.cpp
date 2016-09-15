#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <set>
using namespace std;
/*
	DecisionTree
	@chenbjin 2016-08-29
*/
typedef map<int,int> Map;
typedef map<int,int>::iterator MIter;
typedef set<int> Set;
typedef set<int>::iterator SIter;
const int FeatureL = 2;
/* 样本 */
struct Sample {
	vector<int> feature; //规范化的离散属性特征
	int label;		     //规范化的分类标签
};

/* 数据导入 */
void loadData(vector<Sample> &data, string filename) {
	ifstream fin(filename, ios::in);
	if (!fin) { return; }
	char buffer[200];
	Sample cur;
	cur.feature.resize(FeatureL);
	while (fin.peek() != EOF) {
		fin.getline(buffer, 150);
		sscanf(buffer, "%d\t%d\t%d", &cur.feature[0], &cur.feature[1], &cur.label);
		data.push_back(cur);
	}
	fin.close();
}

/* 计算数据集的香农熵 H = -\sum_{i=1}^n p(x_i)log(p(x_i)) */
double calcEntropy(vector<Sample> &data) {
	double entropy = 0.0, prob = 0.0;
	int dataSize = data.size();
	Map labelCounts;
	// 统计各类别
	for (int i = 0; i < dataSize; ++i) {
		if (labelCounts.find(data[i].label) == labelCounts.end())
			labelCounts[data[i].label] = 0;
		labelCounts[data[i].label] ++;
	}
	// 计算熵
	for (MIter it = labelCounts.begin(); it != labelCounts.end(); ++it) {
		prob = 1.0*(it->second) / dataSize;
		entropy -= prob * log2(prob);
	}
	return entropy;
}

/* 划分数据集：根据第axis个特征的值value进行筛选 */
vector<Sample> splitDataSet(vector<Sample> &data, int axis, int value) {
	vector<Sample> subDataSet;
	for (int i = 0; i < data.size(); ++i) {
		if (data[i].feature[axis] == value) {
			Sample cur;
			cur.label = data[i].label;
			for(int j = 0; j < data[i].feature.size(); ++j)
				if (j != axis) cur.feature.push_back(data[i].feature[j]);
			subDataSet.push_back(cur);
		}
	}
	return subDataSet;
}

/* 选择最好的划分特征：信息增益最大 */
int chooseBestFeature(vector<Sample> &data) {
	double baseEntropy = calcEntropy(data);  //原始数据的信息熵
	double bestInfoGain = 0.0, prob = 0.0;
	int bestFeature = -1;
	vector<Sample> subDataSet;
	// 遍历每个特征
	for (int i = 0; i < data[0].feature.size(); ++i) {
		Set uniqVals; // 第i个特征的所有值
		for (int j = 0; j < data.size(); ++j) {
			if (uniqVals.find(data[j].feature[i]) == uniqVals.end())
				uniqVals.insert(data[j].feature[i]);
		}
		//根据当前特征值计算信息增益
		double newEntropy = 0.0, infoGain = 0.0;
		for (SIter it = uniqVals.begin(); it != uniqVals.end(); ++it) {
			subDataSet = splitDataSet(data, i, *it);
			prob = 1.0 * subDataSet.size() / data.size();
			newEntropy += prob * calcEntropy(subDataSet);
			subDataSet.clear();
		}
		infoGain = baseEntropy - newEntropy;
		if (infoGain > bestInfoGain) {
			bestInfoGain = infoGain;
			bestFeature = i;
		}
	}
	return bestFeature;
}

/* 多数表决：特征已处理完，叶子节点采用多数表决 */
int majorityLabel(vector<int> &labels) {
	Map labelCnt;
	int maxCnt = 0, majLabel = -1;
	for (int i = 0; i < labels.size(); ++i) {
		if (labelCnt.find(labels[i]) == labelCnt.end())
			labelCnt[labels[i]] = 0;
		labelCnt[labels[i]]++;
		if (labelCnt[labels[i]] > maxCnt) {
			maxCnt = labelCnt[labels[i]];
			majLabel = labels[i];
		}
	}
	return majLabel;
}

/* 决策树结点 */
struct DCTreeNode {
	string axis; //第axis个属性特征
	int cnt;
	int fvalue; //当前属性值
	int label;
	DCTreeNode* children[FeatureL];
	DCTreeNode(){}
};

/* 决策树构建 */
DCTreeNode* createTree(vector<Sample> &data, vector<string> feature_axis) {
	int bestFeature = chooseBestFeature(data);
	if (bestFeature == -1) return NULL;
	DCTreeNode* root;
	root->axis = feature_axis[bestFeature];
	Set featureVals; // bestFeature特征的所有值
	for (int j = 0; j < data.size(); ++j) {
		if (featureVals.find(data[j].feature[bestFeature]) == featureVals.end())
			featureVals.insert(data[j].feature[bestFeature]);
	}
	//root->children.resize(featureVals.size());
	int k = 0;
	for (SIter it = featureVals.begin(); it != featureVals.end(); ++it) {
		root->fvalue = *it;
		vector<Sample> subDataSet = splitDataSet(data, bestFeature, *it);
		vector<int> labelList;
		for (int i = 0; i < subDataSet.size(); ++i) labelList.push_back(subDataSet[i].label);
		if (count(labelList.begin(), labelList.end(), labelList[0]) == labelList.size()) {
			root->label = labelList[0];
			root->children[k++] = NULL;
		}else if (subDataSet[0].feature.size() < 1) {
			root->label = majorityLabel(labelList);
			root->children[k++] = NULL;
		}else {
			feature_axis.erase(feature_axis.begin()+bestFeature);
			//cout << feature_axis[0] << endl;
			//feature_axis.erase(find(feature_axis.begin(),feature_axis.end(),feature_axis[bestFeature]));
			root->children[k++] = createTree(subDataSet,feature_axis);
		}
	}
	return root;
}

void printTree(DCTreeNode* root) {
	if (root == NULL) return ;
	cout << root->axis << endl;
	for (int i = 0; i < FeatureL; ++i) {
		cout << root->fvalue << " " << root->label << endl;
		printTree(root->children[i]);
	}
}

int main(int argc, char const *argv[]) {
	/* code */
	vector<Sample> data;
	loadData(data, "dataSet.txt");
	cout << calcEntropy(data) << endl;
	cout << chooseBestFeature(data) << endl;
	vector<string> feature_axis;
	feature_axis.push_back("surfacing");
	feature_axis.push_back("flippers");
	DCTreeNode* root = createTree(data, feature_axis);
	cout << "done" << endl;
	printTree(root);
	return 0;
}
