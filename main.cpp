/*
    Copyright (C) 2023 Guokai Chen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, version 3 of the
    License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h> //for PROCESSENTRY32, needs to be included after windows.h

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

#define MAXPLAYERS 8
#define INVALIDCLASS 0xffffffffu
#define INFOFFSET 0x557c
#define GIOFFSET 0x0
#define ALLIDOGOFFSET 0x1c
#define SOVDOGOFFSET 0x9

#define TANKOFFSET 0x5568
#define ALLITANKOFFSET 0x9
#define SOVTANKOFFSET 0x3 

#define CREDITSPENT_OFFSET 0x2dc
#define BALANCEOFFSET 0x30c
#define USERNAMEOFFSET 0x1602a
#define ISWINNEROFFSET 0x1f7
#define ISLOSEROFFSET 0x1f8

#define POWEROUTPUTOFFSET 0x53a4
#define POWERDRAINOFFSET 0x53a8

unsigned counts[0x2000];

bool inited = false;
int validHouses = 0;
unsigned *HouseBase[MAXPLAYERS];

unsigned fixedPoint = (0xa8b230);
unsigned fixedPointValue;
unsigned classBasePtr;

unsigned classBaseArrayPtr = 0xa8022c; 
unsigned classBaseArray;
unsigned realClassBasePtr;
unsigned realClassBase;
unsigned itemPtr;
unsigned itemArrayBase;

unsigned balancePtr;
unsigned spentCreditPtr;
unsigned balance;
unsigned spentCredit;

unsigned userNamePtr;
char UserName[0x20];

unsigned isWinnerPtr;
unsigned isLoserPtr;
bool isWinner;
bool isLoser;

unsigned powerOutputPtr;
unsigned powerDrainPtr;
unsigned powerOutput;
unsigned powerDrain;

unsigned GIcount, ALLIDOGcount, SOVDOGcount;
unsigned ALLITANKcount, SOVTANKcount; 

DWORD findPidByName(const char* name)
{
  HANDLE h;
  PROCESSENTRY32 singleProcess;
  h = CreateToolhelp32Snapshot( //takes a snapshot of specified processes
    TH32CS_SNAPPROCESS, //get all processes
    0); //ignored for SNAPPROCESS

  singleProcess.dwSize = sizeof(PROCESSENTRY32);

  do {

    if (strcmp(singleProcess.szExeFile, name) == 0)
    {
      DWORD pid = singleProcess.th32ProcessID;
      printf("PID Found: %lu\n", pid);
      CloseHandle(h);
      return pid;
    }

  } while (Process32Next(h, &singleProcess));

  CloseHandle(h);

  return 0;
}

void ReadClassBase() {
  HANDLE handle = OpenProcess(
    PROCESS_QUERY_INFORMATION | //Needed to get a process' token
    PROCESS_CREATE_THREAD |    //for obvious reasons
    PROCESS_VM_OPERATION |    //required to perform operations on address space of process (like WriteProcessMemory)
    PROCESS_VM_READ,  //required for read data
    FALSE,      //don't inherit handle
    findPidByName("gamemd-spawn.exe"));

  if (handle == NULL)
  {
    fprintf(stderr, "Could not open process\n");
    return;
  }
  
  ReadProcessMemory(handle,
    (const void *)fixedPoint,
    &fixedPointValue,
    4,
    NULL);
    
  ReadProcessMemory(handle,
    (const void *)classBaseArrayPtr,
    &classBaseArray,
	4,
	NULL);
	//printf("AAA classBaseArray %x\n", classBaseArray);
	
  unsigned classbasearray = (fixedPointValue + 1120 * 4);
  for (int i = 0; i < MAXPLAYERS; i++) {
  	ReadProcessMemory(handle,
  	  (const void *)classbasearray,
  	  &classBasePtr,
  	  4,
  	  NULL);
  	classbasearray += 4;
  	if (classBasePtr != INVALIDCLASS) {
  	  //printf("debug classBasePtr %u\n", classBasePtr);
  	  realClassBasePtr = classBasePtr * 4 + classBaseArray;
  	  //printf("real classbasearray %x\n", realClassBasePtr);
  	  
  	  ReadProcessMemory(handle,
		(const void *)realClassBasePtr,
		&realClassBase,
		4,
		NULL);
  	  
	  // Balance
      balancePtr = realClassBase + BALANCEOFFSET;
  	  ReadProcessMemory(handle,	
		(const void *)balancePtr,
		&balance,
		4,
		NULL);
	  printf("Player %d balance %u\n", i, balance);

	  // Spent money
	  spentCreditPtr = realClassBase + CREDITSPENT_OFFSET;
	  ReadProcessMemory(handle,
	  	(const void *)spentCreditPtr,
		&spentCredit,
		4,
		NULL);
	  printf("Player %d spent %u\n", i, spentCredit);

	  // User name
	  userNamePtr = realClassBase + USERNAMEOFFSET;
	  ReadProcessMemory(handle,
	  	(const void *)userNamePtr,
		&UserName,
		0x20,
		NULL);
	  printf("Player %d name %ls\n", i, UserName);

	  // IsWinner
	  isWinnerPtr = realClassBase + ISWINNEROFFSET;
	  ReadProcessMemory(handle,
		(const void *)isWinnerPtr,
		&isWinner,
		1,
		NULL);
	  printf("Player %d isWinner %u\n", i, isWinner);

	  // IsLoser
	  isLoserPtr = realClassBase + ISLOSEROFFSET;
	  ReadProcessMemory(handle,
		(const void *)isLoserPtr,
		&isLoser,
		1,
		NULL);
	  printf("Player %d isLoser %u\n", i, isLoser);

	  // Power output
	  powerOutputPtr = realClassBase + POWEROUTPUTOFFSET;
	  ReadProcessMemory(handle,
	  	(const void *)powerOutputPtr,
		&powerOutput,
		4,
		NULL);
	  printf("Player %d powerOutput %u\n", i, powerOutput);

	  // Power drain
	  powerDrainPtr = realClassBase + POWERDRAINOFFSET;
	  ReadProcessMemory(handle,
	  	(const void *)powerDrainPtr,
		&powerDrain,
		4,
		NULL);
	  printf("Player %d powerDrain %u\n", i, powerDrain);

  	  // infantry part
  	  itemPtr = realClassBase + INFOFFSET;
  	  ReadProcessMemory(handle,
		(const void *)itemPtr,
		&itemArrayBase,
		4,
		NULL);
	  printf("Infantry itemPtr %x\n", itemPtr);
	  printf("Infantry itemArrayBase %x\n", itemArrayBase);
	  // GI
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * GIOFFSET),
	    &GIcount,
	    4,
	    NULL);
	  // ALLIDOG
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * ALLIDOGOFFSET),
	    &ALLIDOGcount,
	    4,
	    NULL);
	  // SOVDOG
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * SOVDOGOFFSET),
	    &SOVDOGcount,
	    4,
	    NULL);
	  
	  printf("Player %d SOVDOG count %u ALLIDOG count %u\n", i, SOVDOGcount, ALLIDOGcount);
	  // (unit)tank part
	  itemPtr = realClassBase + TANKOFFSET;
	  ReadProcessMemory(handle,
		(const void *)itemPtr,
		&itemArrayBase,
		4,
		NULL);
	  printf("Tank itemPtr %x\n", itemPtr);
	  printf("Tank itemArrayBase %x\n", itemArrayBase);
	  // ALLITank
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * ALLITANKOFFSET),
	    &ALLITANKcount,
	    4,
	    NULL);
	  // SOVTank
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * SOVTANKOFFSET),
	    &SOVTANKcount,
	    4,
	    NULL);
	  printf("Player %d Sov Tank count %u AlliTank count %u\n", i, SOVTANKcount, ALLITANKcount);
	  /*
	  for (int i = 0; i < 0x200; i++) {
	  	if (counts[i] == 9) {
	  		printf("find offset %u\n", i);
		  }
	  }*/
	  
	}
  }
  
  
}



int main(int argc, char** argv) {
	while (!findPidByName("gamemd-spawn.exe")) {
		printf("Waiting game process\n");
		Sleep(1000);
	}
	while (1) {
		ReadClassBase();
		Sleep(1000); // sleep 1s
	} 
	return 0;
}
