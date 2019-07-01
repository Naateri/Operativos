#include <iostream>
#include <thread>

int main(){
	int sum = 0;
	for(int i = 0; i < 1000000; i++){
		sum += i;
		std::this_thread::yield();
	}
	std::cout << "Result: " << sum << std::endl;
	
	return 0;
}
