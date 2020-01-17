
class NES6502 {
private:
public:

	// framework
	System_File * File;

	// variable
	_iobuf * _File;		// Disassembly File
	UCHAR * Memory;		// NES File
	ULONG Size;			// NES File Size
	ULONG MemSize;		// NES Memory Size
	CHAR * Dir;			// NES File Directory
	CHAR * Namebase;	// NES File Namebase
	ULONG Start;		// NES File Offset (Beginning Address)
	ULONG Finish;		// NES File Offset (Ending Address)
	ULONG _Offset;		// NES File Offset (Current Read Address)
	ULONG Counter;		// NES Program Counter
	ULONG OrigCounter;	// NES Program Counter (copy of original)
	UCHAR Opcode;		// Current Read Opcode
	UCHAR PrevOpcode;	// Previous Read Opcode
	UCHAR NextOpcode;	// Upcoming Opcode
	BOOL Ready;			// Is Memory Ready to be Disassembled?
	BOOL r6502;			// Output Ricoh 6502 Disassembly?
	BOOL r3000a;		// Output MIPS R3000A Disassembly?
	BOOL DataMode;		// Output #byte disassembly?
	BOOL UseLabels;		// Addresses will be converted to labels (sub_C000, for example)
	BOOL AddComments;	// Two semi-colons will be added to each line for future commenting
	BOOL PrintAddress;	// Counter and File Addresses will be printed for each line
	ULONG nBranch;		// Branch Count, calculated only when UseLabels=TRUE
	ULONG nJump;		// Jump Count, calculated only when UseLabels=TRUE
	ULONG nJsr;			// Jump to SubRoutine count, calculated only when UseLabels=TRUE
	ULONG * Branch;		// Branch Address Array
	ULONG * Jump;		// Jump Address Array
	ULONG * Jsr;		// SubRoutine Address Array

	// boot
	NES6502(VOID);
	~NES6502(VOID);

	// function
	BOOL Open(CONST CHAR * _Filename);
	VOID Close(VOID);
	BOOL Disassemble(VOID);
	BOOL DisassembleData(VOID);
	BOOL DisassembleR2A03(VOID);
	BOOL DisassembleR3000A(VOID);
	VOID GetNextOpcode(VOID);

	// print
	BOOL ScanForLabels(VOID);
	BOOL CreateLabelArrays(VOID);

};