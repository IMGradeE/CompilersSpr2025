//
// Created by wilke on 4/8/2025.
//

#include <string>
#include <fstream>
#include "../Util/GlobalEnums.h"
#include "SymbolTable.h"
#include <vector>
// global _start declares that the entry point for this program is the _start label so that the linker can provide the entry point to the executable.
/*Instructions:*/
// lea :: load effective address
// mov ::

/*Registers:*/
// rdi :: arg1 (x64 linux, Win64 preserved)
// rsi :: arg2 (x64 linux, Win64 preserved)


/*Sections:*/
// .text :: asm instructions
// .data :: statically allocated variables (compile time literals)
// .bss  :: undeclared data section, for static allocations that will be assigned values with a size known at compile time.

/*Macros
%macro print 2
    mov edi, %1 ; fmt
    mov esi, %2 ; msg
    mov eax, 0 ; I don't know if this is necessary
    call printf
%endmacro
*/

/* NASM Format vars
msg: db "Hello, this is my string", 0 ; allocate a char into register al and null terminate it.
fmtstr: db "%s", 10, 0 ; allocate a char* (with a size of 10?) into register al and null terminate it.
fmtuint: db "%d", 10, 0
fmtfloat: db "%f", 10, 0
fmtuintin: db "%d", 0
fmtfloatin: db "%f", 0
 * */
using namespace std;
class NASMGenerator{
    const string scratch_int_registers[6] = { "ecx", "edx", "r8d", "r9d","r10d","r11d"};
    std::map<const string, string> register_map = map<const string, string>();

    const string func_return_register = "eax";
    const string param_registers[2] = {"esi", "edi"};

    ofstream f;
    string preamble = "\tsection .data\n"
                      "\n"
                      "msg: db \"Hello, this is my string\", 0\n"
                      "fmtstr: db \"%s\", 10, 0\n"
                      "fmtint: db \"%d\", 10, 0\n"
                      "fmtfloat: db \"%f\", 10, 0\n"
                      "fmtuintin: db \"%d\", 0\n"
                      "fmtfloatin: db \"%f\", 0\n"
                      "float1: dd 0.0\n"
                      "\tsection .text\n"
                      "\n"
                      "\textern printf\n"
                      "\textern scanf\n"
                      "\tglobal main\n"
                      "main:\n"
                      "\tpush rbp\n"
                      "\tmov rbp, rsp\n"
                      "\tsub rsp, 4096\n";

public:

    NASMGenerator(string fname = "a.asm"){
        f.open(fname, ios::trunc);
        writeHeader();
    }

    ~NASMGenerator(){
        f.close();
    }

    void writeHeader(){
        f << preamble;
    }

    void write(string asm_){
        f << asm_ << '\n';
    }
    void end(){
        f << "pop rbp ; restore stack base pointer\n"
             "mov rax, 60\n"
             "xor rdi, rdi\n"
             "syscall ; exit cleanly";
    }
};
