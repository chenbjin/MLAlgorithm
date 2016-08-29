#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <time.h>
using namespace std;
/*
	Logistic
	@chenbjin 2016-08-28
*/

const int MAXIter = 500; //迭代数
const int FeatureL = 3;  //特征数 + bias
const double Alpha = 0.001; //学习率

/* 样本 */
struct Sample {
	vector<double> feature;
	int label;
};

/* sigmoid */
double sigmoid(double x) {
	return 1.0 / (1 + exp(x*(-1)));
}

/* 决策函数 */
int sgn(double prob) {
	if (prob > 0.5) return 1;
	else return 0;
}

/* 读入训练文件 */
void loadData(vector<Sample> &train, string filename) {
	ifstream fin(filename, ios::in);
	if (!fin) { return; }
	char buffer[200];
	Sample cur;
	cur.feature.resize(FeatureL);

	//使用 while(!fin.eof()) 最后一行会多读一次。
	//http://www.cnblogs.com/zhengxiaoping/p/5614317.html
	while (fin.peek() != EOF) {
		fin.getline(buffer, 150);
		sscanf(buffer, "%lf\t%lf\t%d", &cur.feature[0], &cur.feature[1], &cur.label);
		cur.feature[2] = 1.0; // bias
		train.push_back(cur);
	}
	fin.close();
}

/* 训练：梯度下降 */
void trainBGD(vector<Sample> &train, vector<double> &weights, int numIter=MAXIter) {
	weights.resize(FeatureL);
	vector<double> tmp; //中间结果
	tmp.resize(train.size());
	for (int i = 0; i < FeatureL; ++i) weights[i] = 1.0;
	for (int i = 0; i < MAXIter; ++i) {
		int accurary = 0;
		for (int j = 0; j < train.size(); ++j) {
			tmp[j] = 0;
			for (int k = 0; k < FeatureL; ++ k) tmp[j] += train[j].feature[k] * weights[k];
			tmp[j] = sigmoid(tmp[j]);
			if (sgn(tmp[j]) == train[j].label) accurary += 1;
			// error = sigmoid(x_i) - y_i;
			tmp[j] -= train[j].label;

		}
		cout << "Accurary of Iter " << i << ": " << accurary*1.0 / train.size() << endl;
		// weights_k = weights_k - alpha * \sum (x_{ik} * error);
		double sum;
		for (int k = 0; k < FeatureL; ++k) {
			sum = 0.0;
			for (int j = 0; j < train.size(); ++j) sum += train[j].feature[k] * tmp[j];
			weights[k] -= Alpha * sum;
		}
	}
}

/* 训练：随机梯度下降 */
void trainSGD(vector<Sample> &train, vector<double> &weights, int numIter=MAXIter) {
	weights.resize(FeatureL);

	double alpha = 0.001, tmp;
	int randSample, accurary;
	for (int i = 0; i < numIter; ++i) {
		accurary = 0;
		for (int j = 0; j < train.size(); ++j) {
			tmp = 0.0;
			alpha = 4/(1.0+i+j)+0.01;
			randSample = rand() % train.size();
			for (int k = 0; k < FeatureL; ++k) tmp += train[randSample].feature[k] * weights[k];
			tmp = sigmoid(tmp);
			if (sgn(tmp) == train[randSample].label) accurary += 1;
			//error
			tmp -= train[randSample].label;
			//update weights
			for (int k = 0; k < FeatureL; ++k) weights[k] -= alpha * tmp * train[randSample].feature[k];
		}
		cout << "Accurary of Iter " << i << ": " << accurary*1.0 / train.size() << endl;
	}
}

int main(int argc, char const *argv[]) {
	srand((unsigned)time(NULL));
	vector<Sample> train;
	vector<double> weights;

	loadData(train, "testSet.txt");
	cout << "total: " << train.size() << endl;
	trainBGD(train, weights);
	//trainSGD(train, weights, 250);
	for (int i = 0; i < weights.size(); ++i) {
		cout << weights[i] << endl;
	}
	return 0;
}
