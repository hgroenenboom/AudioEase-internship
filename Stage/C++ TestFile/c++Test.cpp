#include <iostream>
using namespace std;

int main() {
	float array[100];
	
	for(int i = 0; i < 50; i ++){
		array[i] = (float)i * 0.02 * i * 0.02 * 2048;
		//cout << "i: " << i << " = " << array[i] << endl;
	}
	
	cout << 256 % 256;
}