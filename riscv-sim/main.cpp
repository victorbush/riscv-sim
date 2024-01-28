#include <iomanip>
#include <set>
#include <utility>

#include "simple-system.h"
#include "elfio/elfio.hpp"
#include "rv32-hart.h"
#include "rv-disassembler.h"

using namespace std;
using namespace ELFIO;
using namespace riscv_sim;

static auto s_memory = Simple_memory_subsystem();
static auto s_hart = Rv32_hart(s_memory);

static auto s_program_name_to_path = map<string, string>() = {
	{ "c-printf-newlib", "../../../../examples/c-printf-newlib/program.elf" }
};


static auto s_breakpoints = set<uint32_t>();

static uint64_t s_heap_base = 0;
static uint64_t s_heap_top = 0;

void print_next_instruction(Rv32_hart& hart)
{
	uint32_t pc = hart.get_register(Rv_register_id::pc);
	uint32_t instruction = s_memory.read_32(pc);
	auto result = Rv_disassembler::disassemble(instruction);

	const string& mnemonic = Rv_disassembler::get_mnemonic(result.type);
	cout << "Next instruction: " << hex << "(" << pc << ")" << "     " << mnemonic << " ";

	bool need_comma = false;

	if (result.rd != Rv_register_id::_unused)
	{
		cout << Rv_disassembler::get_register_abi_name(result.rd);
		need_comma = true;
	}

	if (result.rs1 != Rv_register_id::_unused)
	{
		cout << (need_comma ? ", " : "") << Rv_disassembler::get_register_abi_name(result.rs1);
		need_comma = true;
	}

	if (result.rs2 != Rv_register_id::_unused)
	{
		cout << (need_comma ? ", " : "") << Rv_disassembler::get_register_abi_name(result.rs2);
		need_comma = true;
	}

	if (result.format != Rv32_instruction_format::rtype)
		cout << (need_comma ? ", " : "") << hex << result.imm;

	cout << endl;
}

void print_registers()
{
	const auto reg = [](Rv_register_id reg_id) {
		return s_hart.get_register(reg_id);
	};

	const string right_pad = "  |  ";

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

	// Reset heap pointer (will be initialized to right after the data segments)
	s_heap_base = 0;

	// Print ELF file sections info
	Elf_Half sec_num = reader.sections.size();
	cout << "Number of sections: " << sec_num << endl;
	for (int i = 0; i < sec_num; ++i) {
		const section* psec = reader.sections[i];
		std::cout << " [" << i << "] "
			<< psec->get_name()
			<< "\t"
			<< psec->get_size()
			<< "\t"
			<< hex << psec->get_address()
			<< "\t"
			<< hex << psec->get_flags()
			<< std::endl;

		// Update heap pointer if needed
		auto end_addr = psec->get_address() + psec->get_size();
		if (end_addr > s_heap_base)
			s_heap_base = end_addr;

		// Load any sections into memory with the ALLOC flag
		if (psec->get_flags() & SHF_ALLOC) {

			// Access section's data
			const char* section_data = reader.sections[i]->get_data();
			if (!section_data)
				continue;

			for (int j = 0; j < psec->get_size(); ++j) {
				s_memory.write_8(psec->get_address() + j, *(section_data + j));
			}
		}
	}

	s_heap_top = s_heap_base;

	// Set program counter to program entry point
	s_hart.set_register(riscv_sim::Rv_register_id::pc, reader.get_entry());

	// Reset stack pointer to top of memory space
	s_hart.set_register(riscv_sim::Rv_register_id::sp, 0xFFFFFFFF);

	cout << "Loaded " << file_path << endl << endl;

	print_next_instruction(s_hart);
}

#define SYS_getcwd 17
#define SYS_dup 23
#define SYS_fcntl 25
#define SYS_faccessat 48
#define SYS_chdir 49
#define SYS_openat 56
#define SYS_close 57
#define SYS_getdents 61
#define SYS_lseek 62
#define SYS_read 63
#define SYS_write 64
#define SYS_writev 66
#define SYS_pread 67
#define SYS_pwrite 68
#define SYS_fstatat 79
#define SYS_fstat 80
#define SYS_exit 93
#define SYS_exit_group 94
#define SYS_kill 129
#define SYS_rt_sigaction 134
#define SYS_times 153
#define SYS_uname 160
#define SYS_gettimeofday 169
#define SYS_getpid 172
#define SYS_getuid 174
#define SYS_geteuid 175
#define SYS_getgid 176
#define SYS_getegid 177
#define SYS_brk 214
#define SYS_munmap 215
#define SYS_mremap 216
#define SYS_mmap 222
#define SYS_open 1024
#define SYS_link 1025
#define SYS_unlink 1026
#define SYS_mkdir 1030
#define SYS_access 1033
#define SYS_stat 1038
#define SYS_lstat 1039
#define SYS_time 1062
#define SYS_getmainvars 2011

void ecall_handler(Rv32_hart& hart)
{
	// Using newlib as the C library.

	// Parameters passed by registers
	// a7 is the type of syscall
	// a0-a5 are parameters
	// return value is passed back in a0
	// https://git.kernel.org/pub/scm/docs/man-pages/man-pages.git/tree/man2/syscall.2?h=man-pages-5.04#n200
	// https://stackoverflow.com/questions/59800430/risc-v-ecall-syscall-calling-convention-on-pk-linux

	// List of syscall IDs:
	// https://github.com/riscvarchive/riscv-newlib/blob/7a526cdc28a3c4acce98e8a99b06562452c90d07/libgloss/riscv/machine/syscall.h#L43

	auto a0 = hart.get_register(Rv_register_id::a0);
	auto a1 = hart.get_register(Rv_register_id::a1);
	auto a2 = hart.get_register(Rv_register_id::a2);
	auto a3 = hart.get_register(Rv_register_id::a3);
	auto a4 = hart.get_register(Rv_register_id::a4);
	auto a5 = hart.get_register(Rv_register_id::a5);
	auto a6 = hart.get_register(Rv_register_id::a6);
	auto a7 = hart.get_register(Rv_register_id::a7);

	uint32_t ret_val = a0; // TODO : Default to 0?

	switch (a7)
	{
	case SYS_brk:
		s_heap_top += a0;
		ret_val = s_heap_top;
		break;

	case SYS_write:
		cout << "SYS_write:" << endl;

		/*
		_write(int file, const void *ptr, size_t len)
		*/

		uint32_t buf_addr = a1;
		uint32_t count = a2;

		for (int i = 0; i < count; ++i)
		{
			cout << s_memory.read_8(buf_addr + i);
		}

		cout << endl << endl;

		ret_val = count;
		break;
	}

	// Return value
	hart.set_register(Rv_register_id::a0, ret_val);

	// Increment PC
	hart.set_register(Rv_register_id::pc, hart.get_register(Rv_register_id::pc) + 4);
}

void execute(bool single_step)
{
	while (1) {
		try {
			s_hart.execute_next();
		}
		catch (const Rv_ebreak_exception& ex) {
			cout << "EBREAK" << endl << endl;
			print_registers();
			return;
		}
		catch (const Rv_ecall_exception& ex) {
			cout << "ECALL" << endl;
			ecall_handler(s_hart);
		}

		uint32_t pc = s_hart.get_register(Rv_register_id::pc);
		if (s_breakpoints.contains(pc))
		{
			print_registers();
			print_next_instruction(s_hart);
			cout << "BREAKPOINT: " << hex << pc << endl;
			return;
		}

		if (single_step)
		{
			print_registers();
			print_next_instruction(s_hart);
			return;
		}
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
	}
	else if (command == "exit") {
		return false;
	}
	else if (command == "load") {
		string file_path;
		cin >> file_path;

		if (s_program_name_to_path.contains(file_path))
			load_elf(s_program_name_to_path.at(file_path));
		else
			load_elf(file_path);
	}
	else if (command == "run") {
		execute(false);
	}
	else if (command == "break") {
		uint32_t addr;
		cin >> hex >> addr;

		if (s_breakpoints.contains(addr))
			s_breakpoints.erase(addr);
		else
			s_breakpoints.insert(addr);
	}
	else {
		cout << "Unknown command: " << command << endl << endl;
	}

	return true;
}

int main()
{
	cout << "RISC-V Simulator" << endl << endl;

	while (prompt())
	{
	}

	return 0;
}
