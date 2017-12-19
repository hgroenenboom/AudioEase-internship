#include <iostream>
#include <math.h>
using namespace std;

int main() {
	cout << "22000 / 256: " << 22000 / 256.0f << endl;
	float array[90];
	
	for(int i = 0; i < 90; i ++){
		if (i <= 50) {
			array[i] = i;
		} else {
			array[i] = (int) (pow(((float) i - 50) / 40, 5) * 130 + i);
		}
		
		cout << i << ": " << array[i] << " = " << array[i] * 22000 / 256.0f << endl;
	}
	
	//cout << 256 % 256;
}