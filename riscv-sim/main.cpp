#include "httplib.h"
#include "simple-system.h"
#include "elfio/elfio.hpp"
#include "rv32-hart.h"

#include <vector>
#include <utility>

using namespace std;
using namespace ELFIO;
using namespace riscv_sim;

static auto s_memory = Simple_memory_subsystem();
static auto s_hart = Rv32_hart(s_memory);

void print_registers()
{
	const auto reg = [](Rv_register_id reg_id) {
		return s_hart.get_register(reg_id);
	};

	const string right_pad = "   ";

	cout << hex
		<< "x0 (zero) " << setfill('0') << setw(8) << reg(Rv_register_id::x0) << right_pad
		<< "x11 (a1)  " << setfill('0') << setw(8) << reg(Rv_register_id::x11) << right_pad
		<< "x22 (s6)  " << setfill('0') << setw(8) << reg(Rv_register_id::x22) << right_pad << endl
		<< "x1  (ra)  " << setfill('0') << setw(8) << reg(Rv_register_id::x1) << right_pad
		<< "x12 (a2)  " << setfill('0') << setw(8) << reg(Rv_register_id::x12) << right_pad
		<< "x23 (s7)  " << setfill('0') << setw(8) << reg(Rv_register_id::x23) << right_pad << endl
		<< "x2  (sp)  " << setfill('0') << setw(8) << reg(Rv_register_id::x2) << right_pad
		<< "x13 (a3)  " << setfill('0') << setw(8) << reg(Rv_register_id::x13) << right_pad
		<< "x24 (s8)  " << setfill('0') << setw(8) << reg(Rv_register_id::x24) << right_pad << endl
		<< "x3  (gp)  " << setfill('0') << setw(8) << reg(Rv_register_id::x3) << right_pad
		<< "x14 (a4)  " << setfill('0') << setw(8) << reg(Rv_register_id::x14) << right_pad
		<< "x25 (s9)  " << setfill('0') << setw(8) << reg(Rv_register_id::x25) << right_pad << endl
		<< "x4  (tp)  " << setfill('0') << setw(8) << reg(Rv_register_id::x4) << right_pad
		<< "x15 (a5)  " << setfill('0') << setw(8) << reg(Rv_register_id::x15) << right_pad
		<< "x26 (s10) " << setfill('0') << setw(8) << reg(Rv_register_id::x26) << right_pad << endl
		<< "x5  (t0)  " << setfill('0') << setw(8) << reg(Rv_register_id::x5) << right_pad
		<< "x16 (a6)  " << setfill('0') << setw(8) << reg(Rv_register_id::x16) << right_pad
		<< "x27 (s11) " << setfill('0') << setw(8) << reg(Rv_register_id::x27) << right_pad << endl
		<< "x6  (t1)  " << setfill('0') << setw(8) << reg(Rv_register_id::x6) << right_pad
		<< "x17 (a7)  " << setfill('0') << setw(8) << reg(Rv_register_id::x17) << right_pad
		<< "x28 (t3)  " << setfill('0') << setw(8) << reg(Rv_register_id::x28) << right_pad << endl
		<< "x7  (t2)  " << setfill('0') << setw(8) << reg(Rv_register_id::x7) << right_pad
		<< "x18 (s2)  " << setfill('0') << setw(8) << reg(Rv_register_id::x18) << right_pad
		<< "x29 (t4)  " << setfill('0') << setw(8) << reg(Rv_register_id::x29) << right_pad << endl
		<< "x8  (s0)  " << setfill('0') << setw(8) << reg(Rv_register_id::x8) << right_pad
		<< "x19 (s3)  " << setfill('0') << setw(8) << reg(Rv_register_id::x19) << right_pad
		<< "x30 (t5)  " << setfill('0') << setw(8) << reg(Rv_register_id::x30) << right_pad << endl
		<< "x9  (s1)  " << setfill('0') << setw(8) << reg(Rv_register_id::x9) << right_pad
		<< "x20 (s4)  " << setfill('0') << setw(8) << reg(Rv_register_id::x20) << right_pad
		<< "x31 (t6)  " << setfill('0') << setw(8) << reg(Rv_register_id::x31) << right_pad << endl
		<< "x10 (a0)  " << setfill('0') << setw(8) << reg(Rv_register_id::x10) << right_pad
		<< "x21 (s5)  " << setfill('0') << setw(8) << reg(Rv_register_id::x21) << right_pad
		<< "pc        " << setfill('0') << setw(8) << reg(Rv_register_id::pc) << right_pad << endl
		<< endl;
}

void load_elf(const string& file_path)
{
	elfio reader;

	// Load ELF data
	if (!reader.load(file_path)) {
		cout << "Error: Can't find or process ELF file " << file_path << endl << endl;
		return;
	}

	// Verify 32 bit ELF
	if (reader.get_class() != ELFCLASS32)
		cout << "Error: Only ELF32 is supported." << endl << endl;

	// Verify little endian
	if (reader.get_encoding() != ELFDATA2LSB)
		cout << "Error: Only little endian is supported." << endl << endl;

	// Reset system state
	s_memory.reset();
	s_hart.reset();

	// Print ELF file sections info
	Elf_Half sec_num = reader.sections.size();
	cout << "Number of sections: " << sec_num << endl;
	for (int i = 0; i < sec_num; ++i) {
		const section* psec = reader.sections[i];
		std::cout << " [" << i << "] "
			<< psec->get_name()
			<< "\t"
			<< psec->get_size()
			<< std::endl;

		// Access section's data
		const char* p = reader.sections[i]->get_data();

		// Load text section into memory
		if (psec->get_name() == ".text"
			|| psec->get_name() == ".data"
			|| psec->get_name() == ".rodata"
			|| psec->get_name() == ".bss"
			) {
			for (int j = 0; j < psec->get_size(); ++j) {
				s_memory.write_8(psec->get_address() + j, *(psec->get_data() + j));
			}
		}
	}

	// Set program counter to program entry point
	s_hart.set_register(riscv_sim::Rv_register_id::pc, reader.get_entry());

	// Reset stack pointer to top of memory space
	s_hart.set_register(riscv_sim::Rv_register_id::sp, 0xFFFFFFFF);

	cout << "Loaded " << file_path << endl << endl;
}

void execute(bool single_step)
{
	try {
		if (single_step)
		{
			s_hart.execute_next();
			return;
		}

		while (1) {
			s_hart.execute_next();
		}
	}
	catch (const Rv_ebreak_exception& ex) {
		cout << "EBREAK" << endl << endl;
		print_registers();
	}
}

bool prompt()
{
	string command;

	cout << "> ";
	cin >> command;
	cout << endl;

	if (command == "step") {
		execute(true);
		print_registers();
		return true;
	}
	else if (command == "exit") {
		return false;
	}
	else if (command == "load") {
		string file_path;
		cin >> file_path;
		load_elf(file_path);
		return true;
	}
	else if (command == "run") {
		execute(false);
		return true;
	}
	else {
		cout << "Unknown command: " << command << endl << endl;
		return true;
	}
}

int main()
{
	cout << "RISC-V Simulator" << endl << endl;

	while (prompt())
	{
	}

	return 0;
}
