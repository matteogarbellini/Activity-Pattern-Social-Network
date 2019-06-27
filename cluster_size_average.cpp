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
	std::vector<double> average_x, average_y;
	input.open(argv[1]);
	while(input.good())
	{
		input >> a >> b;
		x.push_back(a);
		y.push_back(b);
		n++;
	}
	input.close();

	double sum=0;
	double previous;
	int items=0;

	for(int i=0; i<n; i++)
	{
		if(i==0)
		{
			sum+= y[i];
			previous = x[i];
			items++;
		}
		else
		{
			if(x[i]==previous)
			{
				sum+= y[i];
				previous = x[i];
				items++;
			}
			else
			{
				average_x.push_back(previous);
				average_y.push_back((double)sum/items);
				sum = y[i];
				previous = x[i];
				items=1;
			}
		}
	}
	
	output.open(argv[2]);
	for(int i=0; i<average_x.size(); i++)
	{
		output << average_x[i] << " " << average_y[i] << std::endl;
	}
	output.close();




	return 0;
}