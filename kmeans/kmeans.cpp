#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
using namespace std;
/*
	Kmeans
	@chenbjin 2016-09-14
*/
#define k 3
//样本
struct Sample {
	double f1;
	double f2;
	//vector<double> feature;
};

void loadData(vector<Sample> &data, string filename) {
	ifstream fin(filename, ios::in);
	if (!fin) { return; }
	char buffer[100];
	Sample cur;
	//使用 while(!fin.eof()) 最后一行会多读一次。
	//http://www.cnblogs.com/zhengxiaoping/p/5614317.html
	while (fin.peek() != EOF) {
		fin.getline(buffer, 90);
		sscanf(buffer, "%lf\t%lf", &cur.f1, &cur.f2);
		data.push_back(cur);
	}
	fin.close();
}

//欧式距离
double distance(Sample x, Sample y) {
	return sqrt((x.f1-y.f1)*(x.f1-y.f1) + (x.f2-y.f2)*(x.f2-y.f2));
}

//获取簇的中心
Sample getMeans(vector<Sample> &cluster) {
	Sample mean;
	double f1 = 0.0, f2 = 0.0;
	int n = cluster.size();
	for (int i = 0; i < n; ++i) {
		f1 += cluster[i].f1;
		f2 += cluster[i].f2;
	}
	mean.f1 = f1 / n;
	mean.f2 = f2 / n;
	return mean;
}

//根据簇心，判断样本点属于哪个簇
int clusterOfSample(vector<Sample> &means, Sample x) {
	double dis = distance(means[0], x);
	double tmp;
	int label = 0;
	for (int i = 1; i < k; ++i) {
		tmp = distance(means[i], x);
		if (tmp < dis) {
			dis = tmp;
			label = i;
		}
	}
	return label;
}

//簇集的平方误差,以判断是否达到收敛
double getVar(vector<vector<Sample> > &clusters, vector<Sample> &means) {
	double var = 0.0;
	for (int i = 0; i < k; ++i) {
		vector<Sample> cur = clusters[i];
		for (int j = 0; j < cur.size(); ++j) {
			var += distance(cur[j], means[i]);
		}
	}
	return var;
}

//Kmeans聚类
void Kmeans(vector<Sample> &data) {
	vector<vector<Sample> > clusters(k);
	vector<Sample> means;
	//默认一开始将前K个元组的值作为k个簇的质心（均值)
	for (int i = 0; i < k; ++i) means.push_back(data[i]);
	cout << data.size() << endl;
	//根据簇心对每个样本进行划分
	int label = 0;
	for (int i = 0; i < data.size(); ++i) {
		label = clusterOfSample(means, data[i]);
		clusters[label].push_back(data[i]);
	}
	double oldVar = -1, newVar = getVar(clusters, means);
	while (abs(newVar - oldVar) >= 1) {
		//重新选择簇心
		for (int i = 0; i < k; ++i) {
			means[i] = getMeans(clusters[i]);
		}
		//重新计算平方误差
		oldVar = newVar;
		newVar = getVar(clusters, means);
		for (int i = 0; i < k; ++i) clusters[i].clear();
		//重新进行划分
		for (int i = 0; i < data.size(); ++i) {
			label = clusterOfSample(means, data[i]);
			clusters[label].push_back(data[i]);
		}
	}
	//输出聚类结果
	for (int i = 0; i < k; i++) {
		cout << "第" << i+1 << "个簇：" << endl;
		vector<Sample> t = clusters[i];
		for (int j = 0; j < t.size(); j++)
			cout << "(" << t[j].f1 << "," << t[j].f2 << ")" << "   ";
		cout << endl;
	}
}

int main(int argc, char const *argv[]) {
	vector<Sample> data;
	loadData(data, "data.txt");
	cout << "load..." << endl;
	Kmeans(data);
	return 0;
}
