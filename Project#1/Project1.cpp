/*
	CS 4348 Project#1
	Code Written By Joshua McDonald
	Written in C++

	DESCRIPTION:
		This code is a simulated stack/CPU/core-memory that can execute specific
		registery commands provided in text format. This includes jumps and
		simulated buffering between instructions.

	NOTES:
		Many of the functions could be 'obfiscated' into sub classes and sub functions
		to reduce the size of the main routine and have the code be more 'object oriented'.
		Due to time constraints and the fact this is compiled in unix I decided to just
		nest everything together. If I was to continue working on this piece of code I
		would fix this.
*/

/*UNIX RELATED LIBRARIES*/
#include <unistd.h>
/************************/
#include <cstring>
#include <cctype>
#include <cstdbool>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <sstream>
#include <vector>

using namespace std;

// DEBUG //
bool debug = false;
int loopCounter1 = 0;
int loopCounter2 = 0;
int loopCounter3 = 0;
// DEBUG //

int main(int argc, char* argv[]) {
	srand(time(0));
	int timeOverRide = 999;

	if (argc != 3) { // If arguments are valid
		cout << "ERROR:\n\tInvalid arguments inputed" << endl;
		void exit(int returning_value);
	}

	const int arb = 2; const int arb1 = 1; int CPUPipeMem[arb]; int memPipeCPU[arb]; // Data pipeline variables

	if (pipe(CPUPipeMem) < 0) { // if CPU properly piped to memory
		cout << "ERROR:\n\tCPU is not properly piped into memory" << endl;
		void exit(int returning_value);
	}
	else if (pipe(memPipeCPU) < 0) { // If memory probably piped to CPU
		cout << "ERROR:\n\Memory is not properly piped into CPU" << endl;
		void exit(int returning_value);
	}

	// OPEN FILE //
	char* filename = argv[1];
	FILE* file = fopen(filename, "r");
	//FILE* file = _fsopen(filename, "r, _SH_DENYNO);
	// OPEN FILE //

	if (file == NULL) { // File is empty check
		cout << "ERROR:\n\tSelected file is empty" << endl;
		void exit(int returning_value);
	}

	// DEBUG //
	if (debug) {
		cout << "FIRST FILE PORTION SUCCESS" << endl;
	}
	// DEBUG //

	int chain = fork(); // Start fork between CPU and memory
	if (chain < 0) { // Verify chain did not go out of bounds
		cout << "ERROR:\n\tChaining had an error or went out of bounds" << endl;
		void exit(int returning_value);
	}

	else if (chain == 0) { // If child process returned to parent
		// DEBUG //
		if (debug) {
			cout << "Child process returned to parent" << endl;
		}
		// DEBUG //

		int intEntries[2000]; int userEntries = 0; int systemEntries = 1000; // Setup and declare user entries

		char* currentLine; ssize_t iterateForward; size_t currentLength = 0; int localCurrentLine = userEntries; // Goto next element and setup variables to be scanned 

		while (getline(&currentLine, &currentLength, file) > 0) {
			// DEBUG //
			if (debug) {
				loopCounter1++;
				cout << "File loop: " << loopCounter1 << endl;
			}
			// DEBUG //

			if (currentLine[0] == '\n') { continue; } // If open skip empty lines

			char instruction; int value; // Declare parse values

			if (currentLine[0] == '.') { // Halt
				instruction = '.';
				value = atoi(&currentLine[1]);
			}
			else if (isdigit(currentLine[0])) { // Push forward
				instruction = 'n';
				value = atoi(currentLine);
			} else { continue; } // Default out invalid line format

			switch (instruction) { // Choose current line command
			case '.':
				localCurrentLine = value;
				break;
			case 'n':
				if (localCurrentLine < 2000) {
					intEntries[localCurrentLine++] = value;
				} else { // Default error out entry cap
					cerr << "ERROR:\n\tExceeded maximum number of entries" << endl;
					break;
				}
			}
		}

		while (true) {
			// DEBUG //
			if (debug) {
				loopCounter2++;
				cout << "File loop: " << loopCounter2 << endl;
			}
			// DEBUG //

			// INITIALIZE PROCESS FOR READ & WRITE //
			char localLine[6] = { '\0', '\0', '\0', '\0', '\0', '\0' };
			read(CPUPipeMem[0], &localLine, 5);
			char command = localLine[0];
			for (int i = 0; i < 5; i++) { localLine[i] = localLine[i + 1]; }
			// INITIALIZE PROCESS FOR READ & WRITE //

			if (command == 'r') { // If read detected
				// LOCAL INITIALIZATION //
				char mutate[5] = { '\0', '\0', '\0', '\0', '\0' };
				int num = intEntries[atoi(localLine)];
				int digit = 0, i = 0;
				// LOCAL INITIALIZATION //

				if (num < 0) { // If in bound convert integer to string
					mutate[i++] = '-';
					num = -num;
				}

				while (num > 0 || i == 0) { // Run within bound loop
					digit = num % 10;
					mutate[i++] = '0' + digit;
					num /= 10;
				}

				for (int j = 0, k = i - 1; j < k; j++, k--) { // Reverse current string
					char local = mutate[j];
					mutate[j] = mutate[k];
					mutate[k] = local;
				}

				write(memPipeCPU[1], &mutate, 4);
			}

			if (command == 'w') { // If write detected
				char digit[5];
				read(CPUPipeMem[0], &digit, 4);
				intEntries[atoi(localLine)] = atoi(digit);
			}

			if (command == 'e') { exit(0); } // If Exit detected
		}
	}
	else {
		int timer = atoi(argv[2]); // Global timer argument

		int pc; int sp; int ir; int ac; int x; int y; // Declare registrys in prep for digits to be added to them

		int userTopElement = 999; int systemTopElement = 1999; // Setup bounds of user stack and system stack
		pc = 0; sp = userTopElement; // Pad initial registry for program counter and stack pointer(area)

		int instructionCounter = 0; bool user = true; bool interrupt = false; int interruptCounter = 0; // Declare logic necesary for pushes to next command and interrupts

		while (true) { // ir loop condition
			// DEBUG //
			if (debug) {
				loopCounter3++;
				cout << "File loop: " << loopCounter3 << endl;
			}
			// DEBUG //

			// INITIATE PROCESS BUFFER ZONE //
			char currentInstructionSpace[5] = { '\0', '\0', '\0', '\0', '\0' };
			char buffer[10] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
			bool jumpStatus = false;
			// INITIATE PROCESS BUFFER ZONE //

			if (interrupt && instructionCounter > 0 && (instructionCounter % timer) == 0) { interruptCounter++; } // If interrupts overlapping

			if ((!interrupt && instructionCounter > 0 && (instructionCounter % timer) == 0) || (!interrupt && interruptCounter)) { // Interrupt if global timer set condition
				user = false; interrupt = true; interruptCounter--; // Initialize variables for interrupt mode

				int userSP = sp; int userPC = pc; // Set user input(local digit to parent reg) to PC & SP on CPU

				sp = systemTopElement; pc = 1000; jumpStatus = true; // Set stack bounds

				/* Push user sp to memory*/
				stringstream ss;
				ss << "w" << sp;
				ss >> buffer;
				write(CPUPipeMem[1], &buffer, 5);
				stringstream ss2;
				ss2 << userSP;
				ss2 >> buffer;
				write(CPUPipeMem[1], &buffer, 4);
				sp--;

				/*Push user pc to memory*/
				stringstream ss3;
				ss3 << "w" << sp;
				ss3 >> buffer;
				write(CPUPipeMem[1], &buffer, 5);
				stringstream ss4;
				ss4 << userPC;
				ss4 >> buffer;
				write(CPUPipeMem[1], &buffer, 4);
				sp--;

				continue;
			}

			/*Goto next instruction*/
			stringstream ss5;
			ss5 << "r" << pc;
			ss5 >> buffer;
			write(CPUPipeMem[1], &buffer, 5);
			read(memPipeCPU[0], &currentInstructionSpace, 4);
			instructionCounter++;

			if (currentInstructionSpace[0] != '\0') { // If buffer valid
				ir = atoi(currentInstructionSpace); // Load base instruction set

				if (ir == 1) { // Load digit into accumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);

					read(memPipeCPU[0], &currentMemoryDigit, 4);
					ac = atoi(currentMemoryDigit); // Set element
				}
				else if (ir == 2) { // Load digit at pointer into accumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (atoi(currentMemoryDigit) > 999 && user) { // If digit in range of system but in user mode
						cout << "ERROR:\n\tCurrent range within system but still in user mode." << endl;
						void exit(int returning_value);
					}
					else { // Default load digit into accumulator
						stringstream ss2;
						ss2 << "r" << atoi(currentMemoryDigit);
						ss2 >> buffer;
						write(CPUPipeMem[1], &buffer, 5);
						read(memPipeCPU[0], &currentMemoryDigit, 4);
						ac = atoi(currentMemoryDigit); // Set element
					}
				}
				else if (ir == 3) { // Load digit from pointer targeted at accumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (atoi(currentMemoryDigit) > 999 && user) { // If digit in range of system but in user mode
						cout << "ERROR:\n\tCurrent range within system but still in user mode." << endl;
						void exit(int returning_value);
					}
					else { // Default load digit into accumulator
						stringstream ss2;
						ss2 << "r" << atoi(currentMemoryDigit);
						ss2 >> buffer;
						write(CPUPipeMem[1], &buffer, 5);
						read(memPipeCPU[0], &currentMemoryDigit, 4);
						ac = atoi(currentMemoryDigit); // Set element

						if (atoi(currentMemoryDigit) > 999 && user) { // If digit in range of system but in user mode
							cout << "ERROR:\n\tCurrent range within system but still in user mode." << endl;
							void exit(int returning_value);
						}
						else { // Default if still in range
							stringstream ss3;
							ss3 << "r" << ac;
							ss3 >> buffer;
							write(CPUPipeMem[1], &buffer, 5);
							read(memPipeCPU[0], &currentMemoryDigit, 4);
							ac = atoi(currentMemoryDigit); // Set element
						}
					}
				}
				else if (ir == 4) { // Load digit at x pointer into accumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (x + atoi(currentMemoryDigit) > 999 && user) { // If digit in range of system but in user mode
						cout << "ERROR:\n\tCurrent range within system but still in user mode." << endl;
						void exit(int returning_value);
					}
					else { // Default load digit into accumulator
						stringstream ss2;
						ss2 << "r" << x + atoi(currentMemoryDigit);
						ss2 >> buffer;
						write(CPUPipeMem[1], &buffer, 5);
						read(memPipeCPU[0], &currentMemoryDigit, 4);
						ac = atoi(currentMemoryDigit); // Set element
					}
				}
				else if (ir == 5) { // Load digit at y pointer into accumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (y + atoi(currentMemoryDigit) > 999 && user) { // If digit in range of system but in user mode
						cout << "ERROR:\n\tCurrent range within system but still in user mode." << endl;
						void exit(int returning_value);
					}
					else { // Default load digit into accumulator
						stringstream ss2;
						ss2 << "r" << y + atoi(currentMemoryDigit);
						ss2 >> buffer;
						write(CPUPipeMem[1], &buffer, 5);
						read(memPipeCPU[0], &currentMemoryDigit, 4);
						ac = atoi(currentMemoryDigit); // Set element
					}
				}
				else if (ir == 6) { // Load shifted stack pointer into accumulator location
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					// MEMORY START POINT //

					if (x + sp > 999 && !interrupt) { // If shift occured but bounding error detected
						cout << "ERROR:\n\tOut of range. Shift occured but position out of bounds" << endl;
						void exit(int returning_value);
					}
					else { // Default load digit into accumulator
						stringstream ss;
						ss << "r" << sp + 1 + x;
						ss >> buffer;
						write(CPUPipeMem[1], &buffer, 5);
						read(memPipeCPU[0], &currentMemoryDigit, 4);
						ac = atoi(currentMemoryDigit); // Set element
					}
				}
				else if (ir == 7) { // Store accumulator digit on top of stack pointer
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					/*Push accumulator digit into stack via the adder*/
					stringstream ss2;
					ss2 << "w" << atoi(currentMemoryDigit);
					ss2 >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					stringstream ss3;
					ss3 << ac;
					ss3 >> buffer;
					write(CPUPipeMem[1], &buffer, 4);
				}
				else if (ir == 8) { ac = (rand() % 100) + 1; } // Store random int from (1-100) into accumulator
				else if (ir == 9) { // Write accumulator value as int or char to screen
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (atoi(currentMemoryDigit) == 1) { cout << ac; } // Output int port value
					if (atoi(currentMemoryDigit) == 2) { cout << static_cast<char>(ac); } // Output char port value
				}
				else if (ir == 10) { ac += x; } // Add x digit to accumulator
				else if (ir == 11) { ac += y; } // Add y digit to accumulator
				else if (ir == 12) { ac -= x; } // Subtract x digit from accumulator
				else if (ir == 13) { ac -= y; } // Subtract y digit from accumulator
				else if (ir == 14) { x = ac; } // Copy accumulator digit to x
				else if (ir == 15) { ac = x; } // Copy x digit to accumulator
				else if (ir == 16) { y = ac; } // Copy accumulator digit to y
				else if (ir == 17) { ac = y; } // Copy y digit to accumulator
				else if (ir == 18) { sp = ac; } // Copy values in accumulator to stack pointer
				else if (ir == 19) { ac = sp; } // Copy values in stack pointer to acumulator
				else if (ir == 20) { // Jump to address (default jump routine within adder)
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					pc = atoi(currentMemoryDigit); // Set element
					jumpStatus = true;
				}
				else if (ir == 21) { // Jump to input address only if accumulator detected previous storage
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (ac == 0) { // Shift only if accumulator detected change in digit
						pc = atoi(currentMemoryDigit); // Set element
						jumpStatus = true;
					}
				}
				else if (ir == 22) { // Jump to input address only if accumulator did not detect previous storage
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << pc;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					if (ac != 0) { // Shift only if accumulator detected no change in digit
						pc = atoi(currentMemoryDigit); // Set element
						jumpStatus = true;
					}
				}
				else if (ir == 23) { // Push return address onto stack during jump
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					pc++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss1, ss2;
					ss1 << "r" << pc;
					ss1 >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					/*Write pointer onto stack at specific location*/
					ss2 << "w" << sp;
					ss2 >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					ss2.clear();
					ss2 << pc + 1;
					ss2 >> buffer;
					write(CPUPipeMem[1], &buffer, 4);
					sp--;

					pc = atoi(currentMemoryDigit); // Set element
					jumpStatus = true;
				}

				else if (ir == 24) { // pop return address from stack during jump
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					sp++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream ss;
					ss << "r" << sp;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					pc = atoi(currentMemoryDigit); // Set element
					jumpStatus = true;
				}
				else if (ir == 25) { x++; } // Increment x digit(reg)
				else if (ir == 26) { x--; } // Decrement x digit(reg)
				else if (ir == 27) { // Push accumulator onto stack
					/*Write accumulator digit into stack at specific location*/
					stringstream ss;
					ss << "w" << sp;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					ss.clear();
					ss << ac;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 4);
					sp--;
				}
				else if (ir == 28) { // Pop from stack into acumulator
					// MEMORY START POINT //
					char currentMemoryDigit[5];
					sp++;
					// MEMORY START POINT //

					/*Goto top element*/
					stringstream ss;
					ss << "r" << sp;
					ss >> buffer;
					write(CPUPipeMem[1], &buffer, 5);
					ss.clear();

					read(memPipeCPU[0], &currentMemoryDigit, 4);
					ac = atoi(currentMemoryDigit); // Set element
				}
				else if (ir == 29 && !interrupt) { // System call assuming no interrupt
					user = false; interrupt = true; // Initiate user variables

					int userSP = sp; int userPC = pc + 1; // Set user values

					sp = systemTopElement; pc = 1500; jumpStatus = true; // Setup stack prepping for original digits

					/*Write user digit into memory at stack register*/
					ostringstream oss;
					oss << "w" << sp;
					write(CPUPipeMem[1], oss.str().c_str(), 5);
					oss.str("");
					oss << userSP;
					write(CPUPipeMem[1], oss.str().c_str(), 4);
					sp--;

					/*Write user program counter into stack register*/
					oss.str("");
					oss << "w" << sp;
					write(CPUPipeMem[1], oss.str().c_str(), 5);
					oss.str("");
					oss << userPC;
					write(CPUPipeMem[1], oss.str().c_str(), 4);
					sp--;
				}
				else if (ir == 30) { // Interrupt and return from system call
					user = true; interrupt = false; // Initiate user variables

					// MEMORY START POINT //
					char currentMemoryDigit[5];
					sp++;
					// MEMORY START POINT //

					/*Goto next element*/
					stringstream buffer;
					buffer << "r" << sp;
					write(CPUPipeMem[1], buffer.str().c_str(), 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);

					pc = atoi(currentMemoryDigit); jumpStatus = true; // Iterate user variables

					sp++;
					buffer.str("");
					buffer << "r" << sp;
					write(CPUPipeMem[1], buffer.str().c_str(), 5);
					read(memPipeCPU[0], &currentMemoryDigit, 4);
					sp = atoi(currentMemoryDigit); // Set element
				}
				else if (ir == 50) { // Default out and end execution
					write(CPUPipeMem[1], "e", 5);

					close(CPUPipeMem[0]); close(CPUPipeMem[1]); close(memPipeCPU[0]); close(memPipeCPU[1]); // End pipe links to prevent leaks 

					return 0;
				}

				if (jumpStatus == false) { pc++; } // Move to next instruction if current jump status is false
			}
		}
	}
}