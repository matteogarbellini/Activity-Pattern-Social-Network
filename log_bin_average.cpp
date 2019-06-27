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

	double sum=0;
	int index=1;
	int ord=10;
	int items=0;
	int bins=0;


	//find max
	double max=0;
	for(int i=0; i<n; i++)
	{
		if(x[i]>max)
		{
			max = x[i];
		}
	}

// // // // // // // //
	/*
	for(int i=0; i<n; i++)
	{
		if(x[i]<index*ord)
		{
			sum+= y[i];
			items++;
		}
		if(x[i]>=index*ord || i==n-1)		
		{
			bins++;
			binned_average_x.push_back(index*ord - ord*0.5);
			binned_average_y.push_back(sum/items);
			index++;
			sum = y[i];
			items = 1;
			if(index == 9)
			{
				index=1;
				ord = ord*10;
			}
		}
	}
	*/
// // // // // // // //


	for(int i=1; i<=9; i++)
	{
		for(int j=0; j<n; j++)
		{
			if(x[j]>=i*ord && x[j]<(i+1)*ord)
			{
				sum+= y[j];
				items++;
			}
		}
		binned_average_x.push_back(i*ord - ord*0.5);
		binned_average_y.push_back(sum/items);
		bins++;
		sum=0;
		items=0;
		if(i==9)
		{
			ord = ord*10;
			i=1; 
		}
		if(i*ord > max)
		{
			break;
		}
	}


	output.open(argv[2]);
	for(int i=0; i<bins; i++)
	{
		output << binned_average_x[i] << " " << binned_average_y[i] <<std::endl;  
	}


	output.close();













	return 0;
}