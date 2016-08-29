#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;
/*
	KNN
	@chenbjin 2016-08-28
*/
typedef vector<double> FVector;
const int LabelTypes = 4;

/* 已知的训练样本 */
struct Sample {
	FVector feature;
	char label;
};

/* 邻居 */
struct Neibor {
	double dis;
	int idx;
};

/* 类别 */
struct LabelCnt {
	int cnt;
	int idl;
};

/* 比较函数 */
bool cmp(Neibor a, Neibor b) {
	return a.dis < b.dis;
}
/*
bool operator< (const Neibor &a, const Neibor &b) ｛
	return a.dis < b.dis;
｝
*/
bool operator < (const LabelCnt &a, const LabelCnt &b) {
	if (a.cnt == b.cnt) return a.idl < b.idl;
	return a.cnt > b.cnt;
}

/* 欧式距离 */
double distance(FVector &f1, FVector &f2) {
	double ans = 0.0;
	for (int i = 0; i < f1.size(); ++i) {
		ans += (f1[i]-f2[i]) * (f1[i]-f2[i]);
	}
	return ans;
}

int main(int argc, char const *argv[]) {
	int k, l , m, n;
	vector<Sample> train;
	
	while(cin >> k >> l >> m >> n) {
		int i = 0, j = 0;
		train.resize(m);
		for (i = 0; i < m; ++i) {
			train[i].feature.resize(l);
			for (j = 0; j < l; ++j) {
				cin >> train[i].feature[j];
			}
			cin >> train[i].label;
		}
		Sample test;
		for (i = 0; i < n; ++i) {
			test.feature.resize(l);
			for (j = 0; j < l; ++j) {
				cin >> test.feature[j];
			}
			vector<Neibor> NNdis;
			NNdis.resize(m);
			int s = 0;
			/* 计算所有邻居距离 */
			for (s = 0; s < m; ++s) {
				NNdis[s].idx = s;
				NNdis[s].dis = distance(test.feature, train[s].feature);
			}
			sort(NNdis.begin(), NNdis.end(), cmp);
			//sort(NNdis.begin(), NNdis.end());
			vector<LabelCnt> labels;
			labels.resize(LabelTypes);
			for (s = 0; s < LabelTypes; ++s) {
				labels[s].cnt = 0;
				labels[s].idl = s;
			}
			/* 找 k 近邻 */
			for (s = 0; s < k; ++s) {
				int idx = NNdis[s].idx;
				int idl = train[idx].label - 'A';
				labels[idl].cnt ++;
			}
			sort(labels.begin(), labels.end());

			/* 统计结果 */
			vector<int> ans;
			ans.push_back(labels[0].idl);
			for (s = 1; s < LabelTypes; ++s) {
				if (labels[s].cnt == labels[s-1].cnt) {
					ans.push_back(labels[s].idl);
				}else break;
			}
			for (s = 0; s < ans.size(); ++s)
				cout << (char)(ans[s]+'A');
			cout << endl;
		}
	}
	return 0;
}
