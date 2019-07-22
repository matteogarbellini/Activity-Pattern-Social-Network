#include <iostream>
#include <fstream>
#include <vector>


int main(int argc, char** argv)
{
	std::ifstream input;
	std::ofstream output;

	double a, b;
	int n;

	std::vector<double> x, y;
	std::vector<double> binned_average_x, binned_average_y;
	input.open(argv[1]);
	while(input.good())
	{
		input >> a >> b;
		x.push_back(a);
		y.push_back(b);
		n++;
	}
	input.close();

	std::vector<int> bins = { 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 12000 };


	double sum =0;
	int items = 0;
	for(int i=1; i<bins.size(); i++)
	{
		for(int j=0; j<x.size(); j++)
		{
			if(x[j] >= bins[i-1] && x[j] < bins[i])
			{
				sum += y[j];
				items++; 
			}
		}
		if(items != 0)
		{
			binned_average_x.push_back((bins[i]+ bins[i-1])*0.5);
			binned_average_y.push_back(sum/items);
		}
		items=0;
		sum=0;

	}
	

	for(int i=0 ; i<binned_average_x.size(); i++)
	{
		std::cout<<binned_average_x[i]<< " " << binned_average_y[i] <<std::endl;
	}
	return 0;
}