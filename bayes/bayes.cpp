#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;
/*
	Bayes
	@chenbjin 2016-08-28
*/
const int FeatureL = 2; //特征数
const int MaxValue = 11; //特征取值范围 1－10

/* 样本 */
struct Sample {
	vector<int> feature;
	int label;
};

/* 数据集导入 */
void loadData(vector<Sample> &data, string filename) {
	ifstream fin(filename, ios::in);
	if (!fin) { return; }
	char buffer[200];
	Sample cur;
	cur.feature.resize(FeatureL);
	//使用 while(!fin.eof()) 最后一行会多读一次。
	//http://www.cnblogs.com/zhengxiaoping/p/5614317.html
	while (fin.peek() != EOF) {
		fin.getline(buffer, 150);
		sscanf(buffer, "%d\t%d\t%d", &cur.feature[0], &cur.feature[1], &cur.label);
		data.push_back(cur);
	}
	fin.close();
}

/* 计算先验概率 */
double getPriorProb(vector<Sample> &data, int label) {
	int cnt = 0;
	for (int i = 0; i < data.size(); ++i) {
		if (data[i].label == label) {
			cnt ++;
		}
	}
	if (cnt == 0) return 0.0;
	return 1.0*cnt / data.size();
}

/* 计算条件概率
即在类别label条件下第idx个特征为k的概率： p(idx=k｜label)
*/
double getConProb(vector<Sample> &data, int idx, int k, int label) {
	int cnt_label = 0, cnt_k = 0;
	for (int i = 0; i < data.size(); i++) {
		if (data[i].label == label) {
			cnt_label ++;
			if (data[i].feature[idx] == k)
				cnt_k ++;
		}
	}
	if (cnt_k == 0) return 0.0;
	return 1.0*cnt_k / cnt_label;
}

/* 按分隔符sep 分割字符串 */
void split(string &s, char sep, vector<int> &values) {
	int num = 0;
	for(int i = 0; i < s.size(); ++i) {
		if (s[i] == sep) {
			if (num > 0) values.push_back(num);
			num = 0;
		}else {
			num += num*10 + s[i]-'0';
		}
	}
	if (num > 0 ) values.push_back(num);
}

int main(int argc, char const *argv[]) {
	string s, ss;
	getline(cin, s);
	vector<int> problems;
	split(s, '\t', problems);
	getline(cin, ss);

	vector<Sample> data;
	loadData(data, "dataSet.txt");

	cout << "sample num:" << data.size() << endl;
	/*
	Sample sample;
	sample.feature.resize(FeatureL);
	int tt = 1000;
	while (tt --) {
		cin >> sample.feature[0] >> sample.feature[1] >> sample.label;
		data.push_back(sample);
	}
	*/
	double prior1 = getPriorProb(data, 1);
	double prior2 = getPriorProb(data, 2);

	//计算各个特征的条件概率
	vector<double> prob_1_0(MaxValue, 0.0);
	vector<double> prob_1_1(MaxValue, 0.0);
	vector<double> prob_2_0(MaxValue, 0.0);
	vector<double> prob_2_1(MaxValue, 0.0);

	for(int i = 1; i <= 10; ++i) {
		prob_1_0[i] = getConProb(data, 0, i, 1);
		prob_1_1[i] = getConProb(data, 1, i, 1);
		prob_2_0[i] = getConProb(data, 0, i, 2);
		prob_2_1[i] = getConProb(data, 1, i, 2);
	}

	// 输出结果
	for (int i = 0; i < problems.size(); ++i) {
		if (problems[i] == 1) {
			cout << fixed << setprecision(3) << prior2 << endl;
		}else if (problems[i] == 2) {
			for (int i = 1; i <= 9; ++i) {
				cout << fixed << setprecision(3) << prob_2_0[i] << ",";
			}
			cout << fixed << setprecision(3) << prob_2_0[10] << endl;
		}else if (problems[i] == 3) {
			// 后验概率
			double prob = 0.0;
			prob = prior2 * prob_2_0[9] * prob_2_1[9];
			prob /= (prior1 * prob_1_0[9] * prob_1_1[9] + prob);
			cout << fixed << setprecision(3) << prob << endl;
		}
	}

	return 0;
}
