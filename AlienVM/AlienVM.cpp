#include <iostream>

using namespace std;

struct VM // size should be 0xa8 == 42x int8
{
public:
	int16_t PC;			// at 0 in struct // type ?
	int16_t AX;			// at 1 in struct // type ? // AX?
	uint32_t* MEM;		// at 2 in struct // type (byte *) // 128 B

	int8_t EXIT;		// at 4 in struct // type char // FLAG?

	uint32_t RESULT;	// at 0x21 in struct // RESULT?
	int16_t ZF;			// at 0x22 in struct
	uint8_t* COMMANDS;	// at 0x24 in struct // 64 KB
	uint32_t* STACK;	// at 0x26 in struct // 2 KB
	int16_t SP;			// at 0x28 in struct // type ? // SP
};

class AlienVirtualMachine
{
private:
	VM vm;
	const char* keycode = "c0d3_r3d_5hutd0wn";
	bool debuging = false;

public:

	AlienVirtualMachine(void* _commands, size_t _commands_size)
	{
		vm.PC = 0;
		vm.AX = 0;
		vm.EXIT = 0;
		vm.MEM = (uint32_t*)calloc(0x80, 1);
		vm.ZF = 0;
		vm.SP = 0;
		vm.COMMANDS = (uint8_t*)calloc(0x10000, 1); // 0x10000 * 1 = 65536 = 64 KB

		if (vm.COMMANDS == NULL)
		{
			cerr << "Couldn't allocate memory for uint8_t* COMMANDS" << endl;
			exit(-1);
		}

		// skiping first 3 - 'UwU' bytes of the file
		memcpy(vm.COMMANDS, (uint8_t*)_commands + 3, _commands_size - 3);


		vm.STACK = (uint32_t*)calloc(0x200, 4); // 0x200 * 4 = 2048 = 2 KB


		if (vm.STACK == NULL)
		{
			cerr << "Couldn't allocate memory for uint32_t* STACK" << endl;
			exit(-1);
		}
	}

	void run()
	{
		while (vm.EXIT == 0)
		{
			step();
		}

		cout << "VM has finished" << endl;
	}

private:

	void step()
	{
		if (vm.PC == 0x38a)
		{
			debuging = true;
		}

		if (debuging)
		{
			status();
			debug();
		}


		if (vm.COMMANDS[vm.PC] > 0x19)
		{
			cout << "dead" << endl;
			exit(0);
		}

		switch (vm.COMMANDS[vm.PC])
		{
		case 0x00:
			add();
			break;

		case 0x01:
			addi();
			break;

			//case 0x02:
			//	sub();
			//	break;

			//case 0x03:
			//	subi();
			//	break;

		case 0x04:
			mul();
			break;

		case 0x05:
			muli();
			break;

			//case 0x06:
			//	div();
			//	break;

		case 0x07:
			cmp();
			break;

			//case 0x08:
			//	jmp();
			//	break;

		case 0x09:
			inv();
			break;

		case 0x0a:
			push();
			break;

		case 0x0b:
			pop();
			break;

		case 0x0c:
			mov();
			break;

		case 0x0d:
			nop();
			break;

		case 0x0e:
			exitt();
			break;

			//case 0x0f:
			//	print();
			//	break;

		case 0x10:
			putc();
			break;

		case 0x11:
			je();
			break;

		case 0x12:
			jne();
			break;

		case 0x13:
			jle();
			break;

		case 0x14:
			jge();
			break;

		case 0x15:
			xorr();
			break;

		case 0x16:
			store();
			break;

		case 0x17:
			load();
			break;

		case 0x18:
			input();
			break;

		default:
			debuging = true;
		}

		if (vm.COMMANDS[vm.PC - 6] != 0x07)
		{ // last command not cmp
			vm.ZF = 0;
	}
	}

	void debug()
	{
		cout << "debug >";
		int x = getchar();
	}

	void status()
	{
		//system("cls");
		printf("\n\n--------------------------------------- Stack ---------------------------------------\n");
		printf("\t 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f \n");
		for (int i = 0; i < (0x200 / 4) - 0x60; i++)
		{
			if (i % 16 == 0)
				printf("0x%02x: ", i);
			printf("%04x%s", vm.STACK[i], ((i + 1) % 16 == 0 ? "\n" : " "));
		}

		printf("\n--------------------------------------- Memory --------------------------------------\n");
		printf("\t 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f \n");
		for (int i = 0; i < 0x80 / 4; i++)
		{
			if (i % 16 == 0)
				printf("0x%02x: ", i);
			printf("%04x%s", vm.MEM[i], ((i + 1) % 16 == 0 ? "\n" : " "));
		}

		printf("\n-------------------------------------- Registers ------------------------------------\n");
		printf("PC: 0x%02x\n", vm.PC);
		printf("SP: 0x%02x\n", vm.SP);
		printf("ZF: 0x%02x\n", vm.ZF);

		printf("\n---------------------------------------- Code ---------------------------------------\n");
		for (int i = 0; i < 6; i++)
		{
			printf("%02x%s", vm.COMMANDS[vm.PC + i], (i + 1 < 6 ? " " : ": "));
		}
		printf("%s\n", decode());

		printf("\n");

		FILE* memfile = _fsopen("memfile.bin", "w+b", _SH_DENYNO);
		fwrite(vm.COMMANDS, 1, 0x10000, memfile);
		fclose(memfile);
	}

	char* decode()
	{
		char* decoded = (char*)calloc(80, 1);

		if (decoded == NULL)
		{
			cerr << "Couldn't allocate memory for char* decoded in decode()" << endl;
			exit(-1);
		}

		switch (vm.COMMANDS[vm.PC])
		{
		case 0x00:
			sprintf(decoded, "ADD\t\tMEM[%02x]  MEM[%02x]  ->  MEM[%02x]\n\t\t   ADD\t\t%02x\t %02x\t  ->  MEM[%02x]",
				vm.COMMANDS[vm.PC + 2], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1],
				vm.MEM[vm.COMMANDS[vm.PC + 2]], vm.MEM[vm.COMMANDS[vm.PC + 3]], vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x01:
			sprintf(decoded, "ADDI\t\tMEM[%02x]  %02x  ->  MEM[%02x]\n\t\t   ADDI\t\t%02x\t %02x  ->  MEM[%02x]",
				vm.COMMANDS[vm.PC + 2], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1],
				vm.MEM[vm.COMMANDS[vm.PC + 2]], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x04:
			sprintf(decoded, "MUL\t\tMEM[%02x]  MEM[%02x]  ->  MEM[%02x]\n\t\t   MUL\t\t%02x\t %02x  ->  MEM[%02x]",
				vm.COMMANDS[vm.PC + 2], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1],
				vm.MEM[vm.COMMANDS[vm.PC + 2]], vm.MEM[vm.COMMANDS[vm.PC + 3]], vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x05:
			sprintf(decoded, "MULI\t\tMEM[%02x]  %02x  ->  MEM[%02x]\n\t\t   MULI\t\t%02x\t %02x  ->  MEM[%02x]",
				vm.COMMANDS[vm.PC + 2], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1],
				vm.MEM[vm.COMMANDS[vm.PC + 2]], vm.COMMANDS[vm.PC + 3], vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x07:
			sprintf(decoded, "CMP\t\t%02x  %02x", vm.COMMANDS[vm.PC + 1], vm.COMMANDS[vm.PC + 2]);
			break;

		case 0x09:
			sprintf(decoded, "INV\t\tsyscall(0x%02x, STACK[SP-1], STACK[SP-2], STACK[SP-3])\n\t\t   INV\t\tsyscall(%s, %02x, %02x, %02x)",
				vm.COMMANDS[vm.PC + 1],
				(vm.COMMANDS[vm.PC + 1] == 0x65 ? "setgid" : "???"),
				(vm.COMMANDS[vm.PC + 2] > 0 ? vm.STACK[vm.SP - 1] : 0),
				(vm.COMMANDS[vm.PC + 2] > 1 ? vm.STACK[vm.SP - 2] : 0),
				(vm.COMMANDS[vm.PC + 2] > 2 ? vm.STACK[vm.SP - 3] : 0));
			break;

		case 0x0a:
			sprintf(decoded, "PUSH\t\tMEM[%02x]\n\t\t   PUSH\t\t%02x", vm.COMMANDS[vm.PC + 1], vm.MEM[vm.COMMANDS[vm.PC + 1]]);
			break;

		case 0x0b:
			sprintf(decoded, "POP\t\tMEM[%02x]", vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x0c:
			sprintf(decoded, "MOV\t\tMEM[%02x]  %02x",
				vm.COMMANDS[vm.PC + 1], *(uint32_t*)(vm.COMMANDS + vm.PC + 2));
			break;

		case 0x0d:
			sprintf(decoded, "NOP");
			break;

		case 0x0e:
			sprintf(decoded, "EXIT");
			break;

		case 0x10:
			sprintf(decoded, "PUTC\t\t'%c'", (vm.COMMANDS[vm.PC + 1] != '\n' ? (char)vm.COMMANDS[vm.PC + 1] : '\\n'));
			break;

		case 0x11:
			sprintf(decoded, "JE\t\tMEM[%02x]  MEM[%02x]  ->  CODE[%02x]\n\t\t   JE\t\t%02x\t %02x\t  ->  CODE[%02x]",
				vm.COMMANDS[vm.PC + 1], (uint32_t)vm.COMMANDS[vm.PC + 2], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6,
				vm.MEM[vm.COMMANDS[vm.PC + 1]], vm.MEM[vm.COMMANDS[vm.PC + 2]], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6);
			break;

		case 0x12:
			sprintf(decoded, "JNE\t\tMEM[%02x]  MEM[%02x]  ->  CODE[%02x]\n\t\t   JNE\t\t%02x\t %02x\t  ->  CODE[%02x]",
				vm.COMMANDS[vm.PC + 1], (uint32_t)vm.COMMANDS[vm.PC + 2], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6,
				vm.MEM[vm.COMMANDS[vm.PC + 1]], vm.MEM[vm.COMMANDS[vm.PC + 2]], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6);
			break;

		case 0x13:
			sprintf(decoded, "JLE\t\tMEM[%02x]  MEM[%02x]  ->  CODE[%02x]\n\t\t   JLE\t\t%02x\t %02x\t  ->  CODE[%02x]",
				vm.COMMANDS[vm.PC + 1], (uint32_t)vm.COMMANDS[vm.PC + 2], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6,
				vm.MEM[vm.COMMANDS[vm.PC + 1]], vm.MEM[vm.COMMANDS[vm.PC + 2]], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6);
			break;

		case 0x14:
			sprintf(decoded, "JGE\t\tMEM[%02x]  MEM[%02x]  ->  CODE[%02x]\n\t\t   JGE\t\t%02x\t %02x\t  ->  CODE[%02x]",
				vm.COMMANDS[vm.PC + 1], (uint32_t)vm.COMMANDS[vm.PC + 2], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6,
				vm.MEM[vm.COMMANDS[vm.PC + 1]], vm.MEM[vm.COMMANDS[vm.PC + 2]], ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6);
			break;

		case 0x15:
			sprintf(decoded, "XOR\t\tMEM[%02x]  MEM[%02x]  ->   MEM[%02x]\n\t\t   XOR\t\t%02x\t %02x\t  ->   MEM[%02x]",
				vm.COMMANDS[vm.PC + 2], vm.COMMANDS[vm.PC + 3],
				vm.COMMANDS[vm.PC + 1], vm.MEM[vm.COMMANDS[vm.PC + 2]], vm.MEM[vm.COMMANDS[vm.PC + 3]], vm.COMMANDS[vm.PC + 1]);
			break;

		case 0x16:
			sprintf(decoded, "STORE\tCODE[MEM[%02x]]\tMEM[%02x]\n\t\t   STORE\tCODE[%02x]\t%02x",
				vm.COMMANDS[vm.PC + 1], vm.COMMANDS[vm.PC + 2], *(int32_t*)(vm.MEM + vm.COMMANDS[vm.PC + 1]),
				vm.MEM[vm.COMMANDS[vm.PC + 2]]);
			break;

		case 0x17:
			sprintf(decoded, "LOAD\t\tMEM[%02x]  CODE[MEM[%02x]]\n\t\t   LOAD\t\tMEM[%02x]\t CODE[%02x]\n\t\t   LOAD\t\tMEM[%02x]  %02x",
				vm.COMMANDS[vm.PC + 1], vm.COMMANDS[vm.PC + 2],
				vm.COMMANDS[vm.PC + 1], vm.MEM[vm.COMMANDS[vm.PC + 2]],
				vm.COMMANDS[vm.PC + 1], vm.COMMANDS[vm.MEM[vm.COMMANDS[vm.PC + 2]]]);
			break;

		case 0x18:
			sprintf(decoded, "INPUT\tMEM[%02x]  getchar()",
				vm.COMMANDS[vm.PC + 1]);
			break;

		default:
			sprintf(decoded, "??????");
		}

		return decoded;
	}


	void add() // 0x00
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.MEM[vm.COMMANDS[vm.PC + 2]] + vm.MEM[vm.COMMANDS[vm.PC + 3]];

		vm.PC += 6;
	}

	void addi() // 0x01
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.MEM[vm.COMMANDS[vm.PC + 2]] + vm.COMMANDS[vm.PC + 3];

		vm.PC += 6;
	}

	void sub() // 0x02
	{

	}

	void subi() // 0x03
	{

	}

	void mul() // 0x04
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.MEM[vm.COMMANDS[vm.PC + 2]] * vm.MEM[vm.COMMANDS[vm.PC + 3]];

		vm.PC += 6;
	}

	void muli() // 0x05
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.MEM[vm.COMMANDS[vm.PC + 2]] * vm.COMMANDS[vm.PC + 3];

		vm.PC += 6;
	}

	void div() // 0x06
	{

	}

	void cmp() // 0x07
	{
		uint8_t a = vm.COMMANDS[vm.PC + 2];
		uint8_t b = vm.COMMANDS[vm.PC + 3];

		if (a == b)
		{
			vm.ZF = 1;
		}

		vm.PC += 6;
	}

	void jmp() // 0x08
	{
		vm.PC = vm.MEM[vm.COMMANDS[vm.PC + 2]];
		vm.PC += 6;
	}

	void inv() // 0x09
	{
		// https://thevivekpandey.github.io/posts/2017-09-25-linux-system-calls.html
		// there in the bin file is only one syscall with number 0x65 - setgid(0)
		// That call will change Group ID of the proccess to 0, which is gid of root's group
		// I assume it is for debugging protection purposes
		int8_t syscall_no = vm.COMMANDS[vm.PC + 1];
		int8_t argc = vm.COMMANDS[vm.PC + 2];

		uint32_t arg1 = 0, arg2 = 0, arg3 = 0, result = 0;

		if (argc > 0)
			arg1 = vm.STACK[--vm.SP];

		if (argc > 1)
			arg2 = vm.STACK[--vm.SP];

		if (argc > 2)
			arg3 = vm.STACK[--vm.SP];

		//result = syscall((int64_t)syscall_no, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3);

		vm.RESULT = result;
		vm.PC += 6;
	}

	void push() // 0x0a
	{
		vm.STACK[vm.SP++] = vm.MEM[vm.COMMANDS[vm.PC + 1]];

		vm.PC += 6;
	}

	void pop() // 0x0b
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.STACK[--vm.SP];

		vm.PC += 6;
	}

	void mov() // 0x0c
	{
		//vm.MEM[vm.COMMANDS[vm.PC + 1]] = (uint32_t)vm.COMMANDS[vm.PC + 2];

		vm.MEM[vm.COMMANDS[vm.PC + 1]] = *(uint32_t*)(vm.COMMANDS + vm.PC + 2);

		vm.PC += 6;
	}

	void nop() // 0x0d
	{
		vm.PC += 6;
	}

	void exitt() // 0x0e
	{
		vm.EXIT = 1;

		vm.PC += 6;
	}

	void print() // 0x0f
	{

	}

	void putc() // 0x10
	{
		putchar(vm.COMMANDS[vm.PC + 1]);
		vm.PC += 6;
	}

	void je() // 0x11
	{
		if (vm.MEM[vm.COMMANDS[vm.PC + 1]] == vm.MEM[vm.COMMANDS[vm.PC + 2]])
		{
			vm.PC = ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6;
		}
		else
			vm.PC += 6;
	}

	void jne() // 0x12
	{
		if (vm.MEM[vm.COMMANDS[vm.PC + 1]] != vm.MEM[vm.COMMANDS[vm.PC + 2]])
		{
			vm.PC = ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6;
		}
		else
			vm.PC += 6;
	}

	void jle() // 0x13
	{
		if (vm.MEM[vm.COMMANDS[vm.PC + 1]] <= vm.MEM[vm.COMMANDS[vm.PC + 2]])
		{
			vm.PC = ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6;
		}
		else
			vm.PC += 6;
	}

	void jge() // 0x14
	{
		if (vm.MEM[vm.COMMANDS[vm.PC + 1]] >= vm.MEM[vm.COMMANDS[vm.PC + 2]])
		{
			vm.PC = ((int16_t)vm.COMMANDS[vm.PC + 3]) * 6;
		}
		else
			vm.PC += 6;
	}

	void xorr() // 0x15
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.MEM[vm.COMMANDS[vm.PC + 2]] ^ vm.MEM[vm.COMMANDS[vm.PC + 3]];

		vm.PC += 6;
	}

	void store() // 0x16
	{
		vm.COMMANDS[*(int32_t*)(vm.MEM + vm.COMMANDS[vm.PC + 1])] = vm.MEM[vm.COMMANDS[vm.PC + 2]];

		vm.PC += 6;
	}

	void load() // 0x17
	{
		vm.MEM[vm.COMMANDS[vm.PC + 1]] = vm.COMMANDS[*(int32_t*)(vm.MEM + vm.COMMANDS[vm.PC + 2])];

		vm.PC += 6;
	}

	void input() // 0x18
	{
		cout << "> ";
		int x = getchar();
		//unsigned char x = 0x63;

		vm.MEM[vm.COMMANDS[vm.PC + 1]] = x;

		vm.PC += 6;
	}
};


int main()
{
	cout << "Starting VM..." << endl;

	cout << "Opening and reading the command file..." << endl;
	FILE* bin_file = fopen("bin", "rb");

	if (bin_file == NULL)
	{
		cerr << "Couldn't open the command file!" << endl;
		exit(-1);
	}

	fseek(bin_file, 0, 2);
	size_t file_size = ftell(bin_file);
	cout << "Command file size: " << file_size << endl; // 16387 B

	rewind(bin_file);

	void* commands = malloc(file_size);

	if (commands == NULL)
	{
		cerr << "Couldn't allocate memory for reading from the command file" << endl;
		exit(-1);
	}

	size_t read_length = fread(commands, 1, file_size, bin_file);
	cout << "Read " << read_length << " bytes from the file" << endl;
	fclose(bin_file);


	cout << "Creating a Virtual Machine..." << endl;
	AlienVirtualMachine vm = AlienVirtualMachine(commands, file_size);

	cout << "Running the Virtual Machine..." << endl << endl;
	vm.run();


	return 0;
}