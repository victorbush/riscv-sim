// riscv-sim.cpp : Defines the entry point for the application.
//

#include "riscv-sim.h"
#include "httplib.h"
#include "simple-system.h"

#include <vector>
#include <utility>

using namespace std;

int main()
{
	//cout << "Hello CMake." << endl;

	//httplib::Server svr;

	//svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
	//	res.set_content("Hello World!", "text/plain");
	//	});

	//svr.listen("0.0.0.0", 8080);

	auto instructions = std::vector<uint32_t>{
		0x3e800093, // addi x1, x0, 1000
		0x1f418113, // addi x2, x3, 500
	};

	auto system = riscv_sim::Simple_system();
	system.load_program(instructions);

	//system.execute_next();
	//system.execute_next();
	//system.execute_next();

	return 0;
}
