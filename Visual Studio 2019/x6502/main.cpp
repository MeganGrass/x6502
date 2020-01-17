// x6502.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "main.h"

// System Includes
#include SYSTEM
System_File ISystem_File, * File;

// Ricoh 6502
#include "..\..\6502\6502.h"
NES6502 INES6502, * x6502;

// System Initialization
VOID InitFramework(VOID) {
    File = &ISystem_File;

    // Ricoh 6502
    x6502 = &INES6502;
    x6502->File = &ISystem_File;
}

// Windows
VOID CommandLine(CONST CHAR* Command) {

    // Parse
    UINT nArg = File->GetArgCount((CHAR*)Command);
    for (UINT n = 0; n < nArg; n++)
    {
        // Command
        CHAR* _Str1 = File->ToUpper(File->GetArg(n, (CHAR*)Command));

        // ROM
        if (!strcmp((CONST CHAR*)_Str1, "ROM")) {
            x6502->Open(File->GetArg(n + 1, (CHAR*)Command));
        }

        // OUT
        if (!strcmp((CONST CHAR*)_Str1, "OUT")) {
            x6502->_File = File->Open(CREATE_FILE, File->GetArg(n + 1, (CHAR*)Command));
            if (!x6502->_File) { x6502->Ready = NO; }
            else { x6502->Ready = YES; }
        }

        // APPEND
        if (!strcmp((CONST CHAR*)_Str1, "APPEND")) {
            x6502->_File = File->Open(APPEND_FILE, File->GetArg(n + 1, (CHAR*)Command));
            if (!x6502->_File) {
                x6502->_File = File->Open(CREATE_FILE, File->GetArg(n + 1, (CHAR*)Command));
            }
            if (!x6502->_File) { x6502->Ready = NO; }
            else { x6502->Ready = YES; }
        }

        // R6502
        if (!strcmp((CONST CHAR*)_Str1, "R6502")) {
            if (x6502->r3000a) {
                x6502->r6502 = YES;
                x6502->r3000a = NO;
            }
        }

        // R3000A
        if (!strcmp((CONST CHAR*)_Str1, "R3000A")) {
            if (x6502->r6502) {
                x6502->r6502 = NO;
                x6502->r3000a = YES;
            }
        }

        // DATA
        if (!strcmp((CONST CHAR*)_Str1, "DATA")) {
            x6502->DataMode = YES;
        }

        // LABEL
        if (!strcmp((CONST CHAR*)_Str1, "LABEL")) {
            x6502->UseLabels = YES;
        }

        // COMMENT
        if (!strcmp((CONST CHAR*)_Str1, "COMMENT")) {
            x6502->AddComments = YES;
        }

        // ADDRESS
        if (!strcmp((CONST CHAR*)_Str1, "ADDRESS")) {
            x6502->AddComments = YES;
            x6502->PrintAddress = YES;
        }

        // ORG
        if (!strcmp((CONST CHAR*)_Str1, "ORG")) {
            CHAR* _OffsetStr = File->GetArg(n + 1, (CHAR*)Command);
            sscanf_s(_OffsetStr, "%X", &x6502->Counter);
        }

        // START
        if (!strcmp((CONST CHAR*)_Str1, "START")) {
            CHAR* _OffsetStr = File->GetArg(n + 1, (CHAR*)Command);
            sscanf_s(_OffsetStr, "%X", &x6502->Start);
            x6502->_Offset = x6502->Start;
        }

        // FINISH
        if (!strcmp((CONST CHAR*)_Str1, "FINISH")) {
            CHAR* _Src = File->GetArg(n + 1, (CHAR*)Command);
            sscanf_s(_Src, "%X", &x6502->Finish);
        }

        // ABOUT
        if (!strcmp((CONST CHAR*)_Str1, "ABOUT")) {
            UCHAR * TXT = File->Buffer((CHAR*)"usage.txt");
            File->Message((CHAR*)TXT);
            delete TXT;
        }
    }

}
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

    // Commandline
    if (strlen(lpCmdLine)) {
        InitFramework();
        CommandLine(lpCmdLine);
        if (!x6502->Ready) {}
        else if (!x6502->Disassemble()) {
            File->Message((CHAR*)"Something went wrong!\r\nPlease check the argument parameters and try again.");
        }
    }

    // Terminate
    return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
