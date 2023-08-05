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
#include <stdint.h>
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
#define ALLIMINEROFFSET (0x84 / 4)
#define SOVMINEROFFSET (0x4 / 4) // or 0x4 / 4

#define BUILDINGOFFSET 0x5554
#define ALLIWARFACTORYOFFSET (0x1c / 4)
#define SOVWARFACTORYOFFSET (0x38 / 4)

#define ALLIENGINEEROFFSET	0x03
#define ROCKETEEROFFSET		0x04
#define SPYOFFSET			0x10
#define GGIOFFSET			0x2e
#define SEALOFFSET			0x05
#define TANYAOFFSET			0x18
#define CHRONOOFFSET		0x0f	// chrono legionnaire
#define PSICOMMANDOOFFSET	0x12
#define SNIPEROFFSET		0x15
#define CHRONCOMMOFFSET		0x11
#define SOVENGINEEROFFSET	0x1b
#define CONSCRIPTOFFSET		0x01
#define TESLATROOPEROFFSET	0x02
#define FLAKTROOPEROFFSET	0x19
#define CRAZYIVANOFFSET		0x07
#define YURIOFFSET			0x06
#define YURIPRIMEOFFSET		0x14
#define DESOLATOROFFSET		0x08
#define TERRORISTOFFSET		0x1a
#define CHRONIVANOFFSET		0x13
#define IFVOFFSET			0x26
#define PRISMTANKOFFSET		0x22
#define MIRAGETANKOFFSET	0x25
#define ALLIMCVOFFSET		0x00
#define	NIGHTHAWKOFFSET		0x17
#define	BATTLEFORTOFFSET	0x3d
#define DESTROYEROFFSET		0x12
#define DOPHINOFFSET		0x19
#define CVOFFSET			0x0d	// aircraft carrier
#define AEGISOFFSET			0x14
#define ALLITRANSOFFSET		0x15	// allied transport ship
#define TANKDESTOFFSET		0x1b	// tank destroyer
#define ROBOTTANKOFFSET		0x48
#define FLAKTRACKOFFSET		0x11
#define VTHREEOFFSET		0x0e
#define SPIDEROFFSET		0x10
#define APOCALYPSEOFFSET	0x08
#define SOVMCVOFFSET		0x1a
#define KIROVOFFSET			0x0f
#define SOVTRANSOFFSET		0x04	// soviet transport ship
#define SUBMARINEOFFSET		0x13
#define SEASCORPIONOFFSET	0x24
#define DREADNOUGHTOFFSET	0x16
#define SQUIDOFFSET			0x18
#define DEMOTRUCKOFFSET		0x29	// demolition truck
#define TESLATANKOFFSET		0x1d
#define ALLIPOWEROFFSET		0x00
#define ALLIOREOFFSET		0x01
#define ALLIBARRACKOFFSET	0x03
#define ALLIFACTORYOFFSET	0x07
#define ALLIRADAROFFSET		0x69
#define ALLISHIPYARDOFFSET	0x17
#define ALLIREPAIROFFSET	0x05
#define ALLILABOFFSET		0x06
#define OREPURIFIEROFFSET	0x49
#define PILLBOXOFFSET		0x42
#define PATRIOTOFFSET		0x15
#define PRISMTOWEROFFSET	0x37
#define GAPGENERATOROFFSET	0x3f
#define	SATELLITEOFFSET		0x3e
#define	GRANDCANNONOFFSET	0x40
#define	USARADAROFFSET		0x87
#define SOVPOWEROFFSET		0x09
#define SOVOREOFFSET		0x0f
#define SOVBARRACKOFFSET	0x0b
#define SOVFACTORYOFFSET	0x0e
#define SOVRADAROFFSET		0x0d
#define SOVREAPIROFFSET		0x1a
#define SOVLABOFFSET		0x0a
#define NUCLEARPOWEROFFSET	0x41
#define SOVCLONOFFSET		0x48
#define SENTRYGUNOFFSET		0x14
#define FLAKCANNONOFFSET	0x43
#define TESLACOILOFFSET		0x35
#define	PSYSENSOROFFSET		0x12

#define AIRCRAFTOFFSET		0x5590

#define BLACKEAGLEOFFSET	0x07
#define HARRIEROFFSET		0x01

#define CREDITSPENT_OFFSET 0x2dc
#define BALANCEOFFSET 0x30c
#define USERNAMEOFFSET 0x1602a
#define ISWINNEROFFSET 0x1f7
#define ISLOSEROFFSET 0x1f8

#define POWEROUTPUTOFFSET 0x53a4
#define POWERDRAINOFFSET 0x53a8

#define PRODUCINGBUILDINGINDEXOFFSET 0x564c
#define PRODUCINGUNITINDEXOFFSET 0x5650

#define HOUSETYPECLASSBASEOFFSET 0x34
#define COUNTRYSTRINGOFFSET 0x24

#define COLOROFFSET 0x56f9

struct ColorStruct {
	uint8_t rgb[3]; // R G B 0 1 2
};

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

unsigned producingBuildingIndexPtr;
unsigned producingUnitIndexPtr;
unsigned producingBuildingIndex;
unsigned producingUnitIndex;

unsigned houseTypeClassBasePtr;
unsigned houseTypeClassBase;

unsigned countryNamePtr;
char countryName[0x40];

unsigned GIcount, ALLIDOGcount, SOVDOGcount;
unsigned ALLITANKcount, SOVTANKcount;

unsigned ALLIMinerCount, SOVMinerCount;

unsigned ALLIWarFactoryCount, SOVWarFactoryCount;

unsigned playerColorPtr;
struct ColorStruct playerColor;

const char *allieCountries[] = {
  "Americans",
  "Alliance" // Korea
  "French",
  "Germans",
  "British"
};
const char *sovietCountries[] = {
  "Africans", // Libya
  "Arabs", // Iraq
  "Confederation", // Cuba
  "Russians"
};

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

	  /*
	  // currently unusable
	  // Producing building
	  producingBuildingIndexPtr = realClassBase + PRODUCINGBUILDINGINDEXOFFSET;
	  ReadProcessMemory(handle,
	  	(const void *)producingBuildingIndexPtr,
		&producingBuildingIndex,
		4,
		NULL);
	  printf("Player %d producingBuildingIndex %u\n", i, producingBuildingIndex);
	  */
	 // HouseTypeClassBase
	 houseTypeClassBasePtr = realClassBase + HOUSETYPECLASSBASEOFFSET;
	 ReadProcessMemory(handle,
	   (const void *)houseTypeClassBasePtr,
	   &houseTypeClassBase,
	   4,
	   NULL);
	 printf("Player %d houseTypeClassBase %x\n", i, houseTypeClassBase);

	 // Country name
	 countryNamePtr = houseTypeClassBase + COUNTRYSTRINGOFFSET;
	 ReadProcessMemory(handle,
	   (const void *)countryNamePtr,
	   &countryName,
	   25,
	   NULL);
	 printf("Player %d countryName %s\n", i, countryName);

	 playerColorPtr = houseTypeClassBase + COLOROFFSET;
	 ReadProcessMemory(handle,
	   (const void *)playerColorPtr,
	   &playerColor,
	   3,
	   NULL);
	printf("Player %d color r %u g %u b %u\n", i, playerColor.rgb[0], playerColor.rgb[1], playerColor.rgb[2]);

	  // building part
	  itemPtr = realClassBase + BUILDINGOFFSET;
	  ReadProcessMemory(handle,
		(const void *)itemPtr,
		&itemArrayBase,
		4,
		NULL);
	  printf("Building itemPtr %x\n", itemPtr);
	  printf("Building itemArrayBase %x\n", itemArrayBase);

	  // War Factory
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * ALLIWARFACTORYOFFSET),
		&ALLIWarFactoryCount,
		4,
		NULL);
	  printf("Player %d ALLIWARFACTORYcount %u\n", i, ALLIWarFactoryCount);

	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * SOVWARFACTORYOFFSET),
		&SOVWarFactoryCount,
		4,
		NULL);
	  printf("Player %d SOVWARFACTORYcount %u\n", i, SOVWarFactoryCount);




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

	  // ALLIMiner
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * ALLIMINEROFFSET),
	    &ALLIMinerCount,
	    4,
	    NULL);
	  printf("Player %d Alli Miner count %u\n", i, ALLIMinerCount);
	  // SOVMiner
	  ReadProcessMemory(handle,
	    (const void *)(itemArrayBase + 4 * SOVMINEROFFSET),
	    &SOVMinerCount,
	    4,
	    NULL);
	  printf("Player %d Sov Miner count %u\n", i, SOVMinerCount);
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
