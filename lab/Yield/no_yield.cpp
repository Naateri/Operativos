#include <iostream>
#include <thread>

void sum1m(int& sum){
	for(int i = 1; i <= 1000000; i++){
		sum += i;
	}
}

int main(){
	std::thread threads[10];
	int sum[10] = {0};
	std::cout << "Summing to 1 million on 10 threads\n";
	for (int i = 0; i < 10; i++) threads[i] = std::thread(sum1m, std::ref(sum[i]));
	for (auto& th:threads) th.join();
	std::cout << "Result: " << sum[0] << std::endl;
	
	return 0;
}
