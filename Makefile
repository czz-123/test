all:
	g++ get_process.cpp cpu_and_mem.cpp -o out -std=c++17 -lpthread
	g++ -o dec_enc_test cpu_and_mem.cpp dec_enc_test.cpp -lpthread
