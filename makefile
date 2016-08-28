all: logistic.out bayes.out knn.out

logistic.out: logistic.cpp
	g++ -o logistic.out logistic.cpp

bayes.out: bayes.cpp
	g++ -o bayes.out bayes.cpp

knn.out: knn.cpp
	g++ -o knn.out knn.cpp

clean:
	rm *.out
