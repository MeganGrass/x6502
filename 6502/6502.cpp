
#include SYSTEM
#include "opcode.h"
#include "6502.h"

// boot
NES6502::NES6502(VOID) {
}
NES6502::~NES6502(VOID) {
}
// function
BOOL NES6502::Open(CONST CHAR * _Filename) {

	// Flag
	Ready = NO;

	// Read
	Memory = File->Buffer((CHAR*)_Filename);
	if(!Memory) {return FAIL;}
	Size = File->Size(_Filename);
	Dir = File->GetDirectory((CHAR*)_Filename);
	Namebase = File->GetNamebase((CHAR *)_Filename);
	MemSize = (Finish-Start);

	// Flag
	Ready = YES;

	// Terminate
	return SUCCESS;
}
VOID NES6502::Close(VOID) {

	// Flag
	Ready = NO;
	r6502 = NO;
	r3000a = NO;
	UseLabels = NO;
	AddComments = NO;
	PrintAddress = NO;
	DataMode = NO;

	// Cleanup
	if(_File) {fclose(_File);}
	if(Memory) {delete [] Memory;}
	Size = 0;
	MemSize = 0;
	Start = 0;
	Finish = 0;
	_Offset = 0;
	Counter = 0;
	Opcode = 0;
	PrevOpcode = 0;
	nBranch = 0;
	nJump = 0;
	nJsr = 0;
	Branch = NULL;
	Jump = NULL;

}
BOOL NES6502::Disassemble(VOID) {

	// Flag
	if(!Ready) {return FAIL;}

	// Output
	if(!_File) {
		_File = File->Open(CREATE_FILE, File->Get("%s\\%s.%s",Dir,Namebase,"s"));
		if(!_File) {
			Close();
			return FAIL;
		}
	}

	// Work
	if(DataMode) {
		DisassembleData();
	} else if(r6502) {
		DisassembleR2A03();
	} else if(r3000a) {
		DisassembleR3000A();
	} else {
		DisassembleR2A03();
	}

	// Complete
	Close();
	fclose(_File);

	// Terminate
	return SUCCESS;
}
BOOL NES6502::DisassembleData(VOID) {

	// Flag
	if(!Ready) {return FAIL;}

	_Offset = Start;
	File->Print(_File, "\r\n#org $%04X, $%08X, $%02X",Counter,_Offset,(Finish-Start));
	File->Print(_File, "\t\t;; \r\n");
	do {
		File->Print(_File, "\t\t\t#byte $%02X",Memory[_Offset]);
		File->Print(_File, "\t\t\t;; %04X %08X\t;; \r\n",Counter,_Offset);
		_Offset++;
		Start++;
		Counter++;
	} while(Start != Finish);
//	File->Print(_File, "\r\n");

	// Terminate
	return SUCCESS;
}
BOOL NES6502::DisassembleR2A03(VOID) {

	// Flag
	if(!Ready) {return FAIL;}

	ULONG Value = 0;
	#define relative(a) { \
		if (((a)=Memory[_Offset+1])&0x80) (a) = Counter-(((a)-1)^0xFF); \
		else (a)+=Counter; \
	}
	#define absolute(a) { \
		(a) = Memory[_Offset+1] | Memory[_Offset+2]<<8; \
	}

	if(UseLabels) {
		ScanForLabels();
		CreateLabelArrays();
	}
	_Offset = Start;
	File->Print(_File, "\r\n\r\n#org $%04X, $%08X", Counter, _Offset);
	if(AddComments) {
		File->Print(_File, "\t\t\t;; ");
	}
//	if(UseLabels) {
//		File->Print(_File, "\r\nSUB%04X:",Counter);
//	}
//	if(AddComments) {
//		File->Print(_File, "\t\t\t\t\t\t;; ");
//	}
//	if(PrintAddress) {
//		File->Print(_File, "%04X %08X\t;; ",Counter,_Offset);
//	}
	BOOL PrintReturn = TRUE;
	do {
		if(_Offset >= Finish) {break;}
		if(UseLabels && nJsr) {
			for(ULONG ID = 0; ID < nJsr; ID++)
			{
				if(Counter != Jsr[ID]) {
				} else {
					if(PrevOpcode != 0x60) {
						File->Print(_File, "\r\n\r\n#org $%04X, $%08X", Counter, _Offset);
					} else {
						File->Print(_File, "\r\n#org $%04X, $%08X", Counter, _Offset);
					}
					if(AddComments) {
						File->Print(_File, "\t\t\t;; ");
					}
					File->Print(_File, "\r\nSUB%04X:\t",Jsr[ID]);
					PrintReturn = FALSE;
					break;
				}
			}
		}
		if(UseLabels && nBranch) {
			for(ULONG ID = 0; ID < nBranch; ID++)
			{
				if(Counter != Branch[ID]) {
				} else {
					File->Print(_File, "\r\nBRANCH%04X:\t",Branch[ID]);
					PrintReturn = FALSE;
					break;
				}
			}
		}
		if(UseLabels && nJump) {
			for(ULONG ID = 0; ID < nJump; ID++)
			{
				if(Counter != Jump[ID]) {
				} else {
					File->Print(_File, "\r\nJUMP%04X:\t",Jump[ID]);
					PrintReturn = FALSE;
					break;
				}
			}
		}

		if(PrintReturn) {
			File->Print(_File, "\r\n\t\t\t");
		} else {
			PrintReturn = TRUE;
		}
		Opcode = Memory[_Offset];
		switch(Opcode)
		{

		// RTS
		case 0x60:
		case 0x40:
			Counter++;
			Start++;
			_Offset++;
			File->Print(_File, "%s", OpcodeStr[Opcode]);
			if(AddComments) {
				File->Print(_File, "\t\t\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-1),(_Offset-1));
			}
			File->Print(_File, "\r\n");
//			if(Start != Finish) {
//				File->Print(_File, "\r\n\r\n#org $%04X, $%08X", Counter, _Offset);
//				if(AddComments) {
//					File->Print(_File, "\t\t\t;; ");
//				}
//				if(UseLabels) {
//					File->Print(_File, "\r\nSUB%04X:",Counter);
//				}
//				if(AddComments) {
//					File->Print(_File, "\t\t\t\t\t\t;; ");
//				}
//				if(PrintAddress) {
//					File->Print(_File, "%04X %08X\t;; ",Counter,_Offset);
//				}
//			}
		break;

		// odd, 1-byte opcodes
		case 0x00:
		case 0x08:
		case 0x0A:
		case 0x18:
		case 0x28:
		case 0x2A:
		case 0x38:
		case 0x48:
		case 0x4A:
		case 0x58:
		case 0x68:
		case 0x6A:
		case 0x78:
		case 0x88:
		case 0x8A:
		case 0x98:
		case 0x9A:
		case 0xA8:
		case 0xAA:
		case 0xB8:
		case 0xBA:
		case 0xC8:
		case 0xCA:
		case 0xD8:
		case 0xE8:
		case 0xEA:
		case 0xF8:
			Counter++;
			File->Print(_File, "%s", OpcodeStr[Opcode]);
			if(AddComments) {
				File->Print(_File, "\t\t\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-1),_Offset);
			}
			Start++;
			_Offset++;
		break;

		// (Indirect,X)
		case 0x01:
		case 0x21:
		case 0x41:
		case 0x61:
		case 0x81:
		case 0xA1:
		case 0xC1:
		case 0xE1:
			Counter += 0x02;
			File->Print(_File, "%s ($%02X,X)", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page
		case 0x05:
		case 0x06:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x45:
		case 0x46:
		case 0x65:
		case 0x66:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xC4:
		case 0xC5:
		case 0xC6:
		case 0xE4:
		case 0xE5:
		case 0xE6:
			Counter += 0x02;
			File->Print(_File, "%s $%04X", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Immediate
		case 0x09:
		case 0x29:
		case 0x49:
		case 0x69:
		case 0xA0:
		case 0xA2:
		case 0xA9:
		case 0xC0:
		case 0xC9:
		case 0xE0:
		case 0xE9:
			Counter += 0x02;
			File->Print(_File, "%s #$%02X", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Absolute
		case 0x0D:
		case 0x0E:
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x4D:
		case 0x4E:
		case 0x6D:
		case 0x6E:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0xAC:
		case 0xAD:
		case 0xAE:
		case 0xCC:
		case 0xCD:
		case 0xCE:
		case 0xEC:
		case 0xED:
		case 0xEE:
			Counter += 0x03;
			absolute(Value);
			File->Print(_File, "%s $%04X", OpcodeStr[Opcode],Value);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-3),_Offset);
			}
			Start += 0x03;
			_Offset += 0x03;
		break;

		// Branches
		case 0x10:
		case 0x30:
		case 0x50:
		case 0x70:
		case 0x90:
		case 0xB0:
		case 0xD0:
		case 0xF0:
			Counter += 0x02;
			relative(Value);
			if(UseLabels) {
				File->Print(_File, "%s BRANCH%04X", OpcodeStr[Opcode],Value);
			} else {
				File->Print(_File, "%s $%04X", OpcodeStr[Opcode],Value);
			}
			if(AddComments) {
				File->Print(_File, "\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// (Indirect),Y
		case 0x11:
		case 0x31:
		case 0x51:
		case 0x71:
		case 0x91:
		case 0xB1:
		case 0xD1:
		case 0xF1:
			Counter += 0x02;
			File->Print(_File, "%s ($%02X),Y", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page,X
		case 0x15:
		case 0x16:
		case 0x35:
		case 0x36:
		case 0x55:
		case 0x56:
		case 0x75:
		case 0x76:
		case 0x94:
		case 0x95:
		case 0xB4:
		case 0xB5:
		case 0xD5:
		case 0xD6:
		case 0xF5:
		case 0xF6:
			Counter += 0x02;
			File->Print(_File, "%s $%02X,X", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Absolute,Y
		case 0x19:
		case 0x39:
		case 0x59:
		case 0x79:
		case 0x99:
		case 0xB9:
		case 0xBE:
		case 0xD9:
		case 0xF9:
			Counter += 0x03;
			absolute(Value);
			File->Print(_File, "%s $%04X,Y", OpcodeStr[Opcode],Value);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-3),_Offset);
			}
			Start += 0x03;
			_Offset += 0x03;
		break;

		// Absolute,X
		case 0x1D:
		case 0x1E:
		case 0x3D:
		case 0x3E:
		case 0x5D:
		case 0x5E:
		case 0x7D:
		case 0x7E:
		case 0x9D:
		case 0xBC:
		case 0xBD:
		case 0xDD:
		case 0xDE:
		case 0xFD:
		case 0xFE:
			Counter += 0x03;
			absolute(Value);
			File->Print(_File, "%s $%04X,X", OpcodeStr[Opcode],Value);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-3),_Offset);
			}
			Start += 0x03;
			_Offset += 0x03;
		break;

		// JSR
		case 0x20:
			Counter += 0x03;
			absolute(Value);
			if(UseLabels) {
				File->Print(_File, "%s SUB%04X", OpcodeStr[Opcode],Value);
			} else {
				File->Print(_File, "%s $%04X", OpcodeStr[Opcode],Value);
			}
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-3),_Offset);
			}
			Start += 0x03;
			_Offset += 0x03;
		break;

		// Jump
		case 0x4C:
		case 0x6C:
			Counter += 0x03;
			absolute(Value);
			if(UseLabels) {
				File->Print(_File, "%s JUMP%04X", OpcodeStr[Opcode],Value);
			} else {
				File->Print(_File, "%s $%04X", OpcodeStr[Opcode],Value);
			}
			if(AddComments) {
				File->Print(_File, "\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-3),_Offset);
			}
			Start += 0x03;
			_Offset += 0x03;
		break;

		// Zero Page,Y
		case 0x96:
		case 0xB6:
			Counter += 0x02;
			File->Print(_File, "%s $%04X,Y", OpcodeStr[Opcode],Memory[_Offset+1]);
			if(AddComments) {
				File->Print(_File, "\t\t\t;; ");
			}
			if(PrintAddress) {
				File->Print(_File, "%04X %08X\t;; ",(Counter-2),_Offset);
			}
			Start += 0x02;
			_Offset += 0x02;
		break;

		// Illegal Opcode
		default:
			Counter++;
			File->Print(_File, "#byte $%02X", Opcode);
			if(!PrintAddress) {
				File->Print(_File, "\t\t\t;; Illegal Opcode");
			}
			if(PrintAddress) {
				File->Print(_File, "\t\t\t;; %04X %08X\t;; Illegal Opcode",(Counter-1),_Offset);
			}
			Start++;
			_Offset++;
		break;
		}

		PrevOpcode = Opcode;
	} while(Start != Finish);
//	File->Print(_File, "\r\n");

	// Terminate
	return SUCCESS;
}
BOOL NES6502::DisassembleR3000A(VOID) {

	// Flag
	if(!Ready) {return FAIL;}

	// Terminate
	return SUCCESS;
}
VOID NES6502::GetNextOpcode(VOID) {

	NextOpcode = Memory[_Offset];

}
// print
BOOL NES6502::ScanForLabels(VOID) {

	// Buffer
	ULONG Org = Counter;
	ULONG Pointer = _Offset = Start;
	ULONG Stop = Finish;

	// Work
	do {
		if(Pointer >= Stop) {break;}
		Opcode = Memory[_Offset];
		switch(Opcode)
		{

		// RTS
		case 0x60:
			Org++;
			Pointer++;
			_Offset++;
		break;

		// odd, 1-byte opcodes
		case 0x00:
		case 0x08:
		case 0x0A:
		case 0x18:
		case 0x28:
		case 0x2A:
		case 0x38:
		case 0x40:
		case 0x48:
		case 0x4A:
		case 0x58:
		case 0x68:
		case 0x6A:
		case 0x78:
		case 0x88:
		case 0x8A:
		case 0x98:
		case 0x9A:
		case 0xA8:
		case 0xAA:
		case 0xB8:
		case 0xBA:
		case 0xC8:
		case 0xCA:
		case 0xD8:
		case 0xE8:
		case 0xEA:
		case 0xF8:
			Org++;
			Pointer++;
			_Offset++;
		break;

		// (Indirect,X)
		case 0x01:
		case 0x21:
		case 0x41:
		case 0x61:
		case 0x81:
		case 0xA1:
		case 0xC1:
		case 0xE1:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page
		case 0x05:
		case 0x06:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x45:
		case 0x46:
		case 0x65:
		case 0x66:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xC4:
		case 0xC5:
		case 0xC6:
		case 0xE4:
		case 0xE5:
		case 0xE6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Immediate
		case 0x09:
		case 0x29:
		case 0x49:
		case 0x69:
		case 0xA0:
		case 0xA2:
		case 0xA9:
		case 0xC0:
		case 0xC9:
		case 0xE0:
		case 0xE9:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Absolute
		case 0x0D:
		case 0x0E:
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x4D:
		case 0x4E:
		case 0x6D:
		case 0x6E:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0xAC:
		case 0xAD:
		case 0xAE:
		case 0xCC:
		case 0xCD:
		case 0xCE:
		case 0xEC:
		case 0xED:
		case 0xEE:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Branches
		case 0x10:
		case 0x30:
		case 0x50:
		case 0x70:
		case 0x90:
		case 0xB0:
		case 0xD0:
		case 0xF0:
			nBranch++;
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// (Indirect),Y
		case 0x11:
		case 0x31:
		case 0x51:
		case 0x71:
		case 0x91:
		case 0xB1:
		case 0xD1:
		case 0xF1:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page,X
		case 0x15:
		case 0x16:
		case 0x35:
		case 0x36:
		case 0x55:
		case 0x56:
		case 0x75:
		case 0x76:
		case 0x94:
		case 0x95:
		case 0xB4:
		case 0xB5:
		case 0xD5:
		case 0xD6:
		case 0xF5:
		case 0xF6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Absolute,Y
		case 0x19:
		case 0x39:
		case 0x59:
		case 0x79:
		case 0x99:
		case 0xB9:
		case 0xBE:
		case 0xD9:
		case 0xF9:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Absolute,X
		case 0x1D:
		case 0x1E:
		case 0x3D:
		case 0x3E:
		case 0x5D:
		case 0x5E:
		case 0x7D:
		case 0x7E:
		case 0x9D:
		case 0xBC:
		case 0xBD:
		case 0xDD:
		case 0xDE:
		case 0xFD:
		case 0xFE:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Jump
		case 0x20:
			nJsr++;
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;
		case 0x4C:
		case 0x6C:
			nJump++;
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Zero Page,Y
		case 0x96:
		case 0xB6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Illegal Opcode
		default:
			Org++;
			Pointer++;
			_Offset++;
		break;
		}
	} while(Pointer != Stop);

	// Terminate
	return SUCCESS;
}
BOOL NES6502::CreateLabelArrays(VOID) {

	// Buffer
	ULONG Org = Counter;
	ULONG Pointer = _Offset = Start;
	ULONG Stop = Finish;
	ULONG Value = 0;

	#define LabelRelative(a) { \
		if (((a)=Memory[_Offset+1])&0x80) (a) = Org-(((a)-1)^0xFF); \
		else (a)+=Org; \
	}
	#define absolute(a) { \
		(a) = Memory[_Offset+1] | Memory[_Offset+2]<<8; \
	}

	ULONG bCnt = 0;
	ULONG jCnt = 0;
	ULONG jSrCnt = 0;
	if(nBranch) {
		Branch = new ULONG [nBranch];
	}
	if(nJump) {
		Jump = new ULONG [nJump];
	}
	if(nJsr) {
		Jsr = new ULONG [nJsr];
	}

	// Work
	do {
		if(Pointer >= Stop) {break;}
		Opcode = Memory[_Offset];
		switch(Opcode)
		{

		// Branches
		case 0x10:
		case 0x30:
		case 0x50:
		case 0x70:
		case 0x90:
		case 0xB0:
		case 0xD0:
		case 0xF0:
			Org += 0x02;
			LabelRelative(Value);
			Branch[bCnt] = Value;
			bCnt++;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Jump
		case 0x4C:
		case 0x6C:
			Org += 0x03;
			absolute(Value);
			Jump[jCnt] = Value;
			jCnt++;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// JSR
		case 0x20:
			Org += 0x03;
			absolute(Value);
			Jsr[jSrCnt] = Value;
			jSrCnt++;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// RTS
		case 0x60:
			Org++;
			Pointer++;
			_Offset++;
		break;

		// odd, 1-byte opcodes
		case 0x00:
		case 0x08:
		case 0x0A:
		case 0x18:
		case 0x28:
		case 0x2A:
		case 0x38:
		case 0x40:
		case 0x48:
		case 0x4A:
		case 0x58:
		case 0x68:
		case 0x6A:
		case 0x78:
		case 0x88:
		case 0x8A:
		case 0x98:
		case 0x9A:
		case 0xA8:
		case 0xAA:
		case 0xB8:
		case 0xBA:
		case 0xC8:
		case 0xCA:
		case 0xD8:
		case 0xE8:
		case 0xEA:
		case 0xF8:
			Org++;
			Pointer++;
			_Offset++;
		break;

		// (Indirect,X)
		case 0x01:
		case 0x21:
		case 0x41:
		case 0x61:
		case 0x81:
		case 0xA1:
		case 0xC1:
		case 0xE1:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page
		case 0x05:
		case 0x06:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x45:
		case 0x46:
		case 0x65:
		case 0x66:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xC4:
		case 0xC5:
		case 0xC6:
		case 0xE4:
		case 0xE5:
		case 0xE6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Immediate
		case 0x09:
		case 0x29:
		case 0x49:
		case 0x69:
		case 0xA0:
		case 0xA2:
		case 0xA9:
		case 0xC0:
		case 0xC9:
		case 0xE0:
		case 0xE9:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Absolute
		case 0x0D:
		case 0x0E:
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x4D:
		case 0x4E:
		case 0x6D:
		case 0x6E:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0xAC:
		case 0xAD:
		case 0xAE:
		case 0xCC:
		case 0xCD:
		case 0xCE:
		case 0xEC:
		case 0xED:
		case 0xEE:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// (Indirect),Y
		case 0x11:
		case 0x31:
		case 0x51:
		case 0x71:
		case 0x91:
		case 0xB1:
		case 0xD1:
		case 0xF1:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Zero Page,X
		case 0x15:
		case 0x16:
		case 0x35:
		case 0x36:
		case 0x55:
		case 0x56:
		case 0x75:
		case 0x76:
		case 0x94:
		case 0x95:
		case 0xB4:
		case 0xB5:
		case 0xD5:
		case 0xD6:
		case 0xF5:
		case 0xF6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Absolute,Y
		case 0x19:
		case 0x39:
		case 0x59:
		case 0x79:
		case 0x99:
		case 0xB9:
		case 0xBE:
		case 0xD9:
		case 0xF9:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Absolute,X
		case 0x1D:
		case 0x1E:
		case 0x3D:
		case 0x3E:
		case 0x5D:
		case 0x5E:
		case 0x7D:
		case 0x7E:
		case 0x9D:
		case 0xBC:
		case 0xBD:
		case 0xDD:
		case 0xDE:
		case 0xFD:
		case 0xFE:
			Org += 0x03;
			Pointer += 0x03;
			_Offset += 0x03;
		break;

		// Zero Page,Y
		case 0x96:
		case 0xB6:
			Org += 0x02;
			Pointer += 0x02;
			_Offset += 0x02;
		break;

		// Illegal Opcode
		default:
			Org++;
			Pointer++;
			_Offset++;
		break;
		}
	} while(Pointer != Stop);

	// Terminate
	return SUCCESS;
}