# Graduation Project of 2012

[中文版本](https://github.com/LeiHao0/Fake-Everything/blob/master/README_CN.md)

This was my graduation project in 2012. At that time, I was greatly shocked by the search speed of **Everything**, so I decided to implement one myself.

If you don't know what **Everything** is, please refer to [this](<http://en.wikipedia.org/wiki/Everything_(software)>).

> Everything is a proprietary and free Windows desktop search engine that can **quickly** find files and folders on NTFS volumes by their names.

I apologize for my almost zero experience with `MFC` and `C++` at that time.

If you want to know how to get all the files on an NTFS volume and hash them, please read `QSearch/Volume.h`.

---

Oh, the language in the image is still Chinese \^\_\^

![QSearch](http://farm4.staticflickr.com/3762/10834695513_052494fb0d.jpg)

## Usage

1. Save the image above as -> \*.jpg
2. Rename .jpg to .7z
3. Extract \*.7z -> (QSearch.exe config.ini)

I hope to refactor it someday, but now I no longer use Windows.

---

## Research Content

- Read NTFS MFT.
- Building of hash table.
- System path of files.
- GUI and worker threads.
- Various search methods.
- MFC interface.

## Research Plan

- Early April - Mid-April: Determine research direction.
- Late April - End of April: Preparation of each knowledge point and simple tests.
- Early May: Design of program and debugging and operation.
- Mid-May - End of May: Writing of graduation thesis.

## Features and Innovation

- CPU and memory usage are within a reasonable range.
- The software is stable, and provides different search options.
- Compared to the Windows search function, it can locate files at a super-fast speed.

---

# Quickly Locating NTFS Disk Files

**Abstract**

> This article introduces how to enumerate the names of all files and folders on the hard disk under the windows NTFS disk format, and using the C++ STL to build a hash table, as well as MFC's GUI and worker threads, to achieve simple searching like searching for keywords in Google. Finally, it instantly returns all matching files/folders and recursively obtains the system path of the file/folder based on the user's input keyword.

**Keywords**: **NTFS, fast, keyword, search, file path**

## 1 Introduction

In Windows directory structure, files are arranged in B+ trees on NTFS volumes. When searching for files in directories, B+ tree search method is used to search the root node (starting from the root directory) first. Then, the file name to be searched is compared with the file name corresponding to the sub-node in the root node to determine which sub-node corresponding storage area to search, and then the sub-node is searched as the current root node until the file is found. [1]

Although the search provided by the Microsoft system can search text content, the speed is not ideal.

In most cases, we only want to know which folder the file is stored in the computer, and this program solves this problem very well.

## 2 Brief Introduction of NTFS

### 2.1 NTFS

NTFS (New Technology File System) is the standard file system for Windows NT and later Windows.

NTFS replaces the File Allocation Table (FAT) file system and provides a file system for Microsoft's Windows series of operating systems.

NTFS makes several improvements to the FAT and HPFS (High Performance File System), such as supporting metadata and using advanced data structures to improve performance, reliability, and disk space utilization. [2]

### 2.2 Status Quo

With the popularity of Windows 2000/XP based on NT kernel, many personal users began to use NTFS.

NTFS also stores data files in clusters, but the size of clusters in NTFS does not depend on the size of disks or partitions.

Reducing the size of clusters not only reduces waste of disk space, but also reduces the possibility of generating disk fragments.

NTFS supports file encryption management function, which can provide users with higher-level security guarantees. [3]

## 3 Preparation

### 3.1 Design Idea

Read all file names on the disk as data, and match them in the data based on the user's input keywords.
Return the correct path together with the data to open the file conveniently.

The specific design idea is shown in Figure 3-1.

![Figure 3-1 Design Idea](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_00.jpg)

The biggest advantage of hash table is to greatly reduce the time and cost of storing and searching data. It can be seen as constant time, and the cost is only more memory consumption.

However, in the case of increasing available memory, the approach of using space to exchange time is worthwhile.

In addition, easy coding is one of its characteristics.

### 3.2 What is USN

The most important thing in the program is to read USN.

USN is the abbreviation of Update Service Number Journal or Change Journal, which records the modified information in the NTFS volume and can monitor the number of modified files and directories in the partition, and record the modification time and content of the monitored object.

When USN logging is enabled, for each NTFS volume, when there is information about adding, deleting and modifying files, NTFS records it using USN logging and stores it in the format of USN_RECORD.

### 3.3 Why is it Fast

The USN log is equivalent to the directory of WORD, providing an index. When the content of the article changes, the USN log will record when the modification was made, but it does not record what was modified specifically, so the index file is very small.

Similarly, when you want to find a specific paragraph in the article, you don't need to scroll the mouse madly, just look at the directory, and positioning only needs to hold down ctrl+click the mouse.

Similarly, when you want to find a specific file, you can directly check the USN log (that is, the index established) to know whether this file exists.

PS: Although Windows is not "everything is a file" (one of the basic philosophies of Unix/Linux), folders also exist in the form of files, so you can also use USN to find the location.

### 3.4 Environment

Since the NTFS format is originally a Microsoft patent, it provides a series of API functions for us to conveniently access.

As an old but classic programming tool, Visual C++ 6.0 was chosen. However, NTFS format was not available at that time, so VC2005 was chosen as the integrated development environment, which can only run on windows systems from 2000 onwards.

## 4 Implementation of the Program

### 4.1 Reading USN

The following are API functions provided by Microsoft^[4]^, included in the header file `<Winioctl.h>`.

#### 4.1.1 Checking Disk Format

To find out which disk is in NTFS format, call the following function:

```cpp
GetVolumeInformation(
	lpRootPathName: PChar;               // String of the disk drive code
	lpVolumeNameBuffer: PChar;           // Volume label name of the disk drive
	nVolumeNameSize: DWORD;              // Length of the volume label name of the disk drive
	lpVolumeSerialNumber: PDWORD;        // Volume label serial number of the disk drive
	var lpMaximumComponentLength: DWORD; // Maximum file name length allowed by the system
    var lpFileSystemFlags: DWORD;        // File system identifier
	lpFileSystemNameBuffer: PChar;       // Format type
	nFileSystemNameSize: DWORD           // Length of the file operating system name
);
```

The lpFileSystemNameBuffer returned is what we need and will return strings such as "FAT32" and "NTFS".

Then, a loop is used to count the NTFS-formatted drive letters A-Z, and then initialize.

#### 4.1.2 Getting Drive Handle

```cpp
HANDLE hVol = CreateFile(
	"drive letter string",  // must be in the form of \.\C: (A-Z)
	GENERIC_READ | GENERIC_WRITE, // can be
	FILE_SHARE_READ | FILE_SHARE_WRITE, // must include FILE_SHARE_WRITE
```

```
## 4.1 Get the USN Journal Records

### 4.1.1 USN Journal Data Structure

The `USN_RECORD_V2` structure is used to store the record of each change that occurs. Here is the structure:

```cpp
typedef struct {
    DWORD    RecordLength;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORDLONG FileReferenceNumber;
    DWORDLONG ParentFileReferenceNumber;
    USN      Usn;
    LARGE_INTEGER TimeStamp;
    DWORD    Reason;
    DWORD    SourceInfo;
    DWORD    SecurityId;
    FileAttributes FileAttributes;
    WORD     FileNameLength;
    WORD     FileNameOffset;
    WCHAR    FileName[1];
} USN_RECORD_V2, *PUSN_RECORD_V2;
```

### 4.1.2 Get the Volume Handle

To get the volume handle, use the `CreateFile` function with the `\\.\volume path` format.

```cpp
CreateFileW(L"\\\\.\\C:",
	NULL,
	FILE_SHARE_READ | FILE_SHARE_WRITE,
	NULL,
	OPEN_EXISTING,
	FILE_ATTRIBUTE_READONLY,
	NULL);
```

A few things to note:

1. `CreateFile` returns a handle that will be used later in the code;
2. The drive letter must be in the `\\.\volume` format. In C++ language, the backslash `\` is used as an escape character, so they need to be doubled (`\\`);
3. Administrator rights are required (UAC may pop up in Vista, Win7).

If `hVol != INVALID_HANDLE_VALUE`, the handle is obtained successfully and the next step can be taken.

### 4.1.3 Initialize the USN Journal

Use `FSCTL_CREATE_USN_JOURNAL` as the control code for `DeviceIoControl`.

`Cujd` is a pointer to an input buffer that points to the `CREATE_USN_JOURNAL_DATA` structure.

### 4.1.4 Retrieve Information from the USN Journal

Use `FSCTL_QUERY_USN_JOURNAL` as the control code for `DeviceIoControl`.

`lpOutBuffer` will return a `USN_JOURNAL_DATA` structure:

```cpp
typedef struct {
    DWORDLONG UsnJournalID;
    USN       FirstUsn;
    USN       NextUsn;
    USN       LowestValidUsn;
    USN       MaxUsn;
    DWORDLONG MaximumSize;
    DWORDLONG AllocationDelta;
} USN_JOURNAL_DATA, *PUSN_JOURNAL_DATA;
```

`UsnJournalID`, `FirstUsn`, and `NextUsn` will be used in the next step.

#### 4.1.5 Obtaining information about USN Journal files

Since `USN` is stored in the form of `USN_RECORD`, its structure is as follows:

```cpp
typedef struct {
	DWORD RecordLength; // length of record
	WORD MajorVersion; // major version
	WORD MinorVersion; // minor version
	DWORDLONG FileReferenceNumber; // file reference number
	DWORDLONG ParentFileReferenceNumber; // parent directory reference number
	USN Usn; // USN
	LARGE_INTEGER TimeStamp; // timestamp
	DWORD Reason; // reason
	DWORD SourceInfo; // source information
	DWORD SecurityId; // security
	ID DWORD FileAttributes; // file attributes
	WORD FileNameLength; // length of file name
	WORD FileNameOffset; // file name offset
	DWORD ExtraInfo1;
	DWORD ExtraInfo2; DWORD ExtraInfo3; // Hypothetically added in version 2.3
	WCHAR FileName[1]; // pointer to the first character of the file name
} USN_RECORD, * PUSN_RECORD;
```

Note the variables `FileReferenceNumber`, `ParentFileReferenceNumber`, `FileNameLength`, `FileName`. 

These variables are crucial.

`DeviceIoControl()` and `FSCTL_ENUM_USN_DATA` work together:

```C++
while (0! = DeviceIoControl(hVol,
	FSCTL_ENUM_USN_DATA,
	&med,
	sizeof(med),
	Buffer,
```

```
BUF_LEN,
&usnDataSize,
NULL)) {

DWORD dwRetBytes = usnDataSize - sizeof(USN);
// Find the first USN record
UsnRecord = (PUSN_RECORD)(((PCHAR)Buffer) + sizeof(USN));

while (dwRetBytes > 0) {

    // Information obtained
    CString CfileName(UsnRecord->FileName, UsnRecord->FileNameLength / 2);
    pfrnName.filename = nameCur.filename = CfileName;
    pfrnName.pfrn = nameCur.pfrn = UsnRecord->ParentFileReferenceNumber;

    // Vector
    VecNameCur.push_back(nameCur);

    // Build hash...
    frnPfrnNameMap[UsnRecord->FileReferenceNumber] = pfrnName;
    // Get the next record
    DWORD recordLen = UsnRecord->RecordLength;
    dwRetBytes -= recordLen;
    UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) + recordLen);

}
// Get the next page of data
med.StartFileReferenceNumber = *((USN *)&Buffer);
```

Here, `Med` is:

```C++
MFT_ENUM_DATA med;

med.StartFileReferenceNumber = 0;

med.LowUsn = 0;//UsnInfo.FirstUsn;
```


Here it was found by testing that sometimes using `FirstUsn` is incorrect, resulting in incomplete data being retrieved, so it is better to directly write 0.

`med.HighUsn = UsnInfo.NextUsn;`

In this loop, the file names obtained each time are inserted into the vector and hash table separately (as described below).

#### 4.1.6 Delete USN log file

```C++
DeviceIoControl(hVol,
	FSCTL_DELETE_USN_JOURNAL,
	&dujd,
	sizeof (dujd),
	NULL,
	0,
	&br,
	NULL)
```

#### 4.1.7 Enumeration of all files

Save the data first as a text file named Allfile.txt for analysis, as shown in Figure 4-1.

![Figure 4-1 Allfile.txt](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_01.jpg)

![Figure 4-1 Allfile.txt](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_02.jpg)

Using the D drive on my own computer as a test, all files were enumerated.

As shown in the figure, there are about 400,000 files, a total of over 1 million records, and the file size is about 20MB.

For the amount of memory occupied by the program later, there is a rough estimate of about 50MB, even with only 1GB of memory.

With the raw data, the analysis is as follows:

For example, the file Linux programming practice.isz has the system path: `D:\Ghost\Linux\Linux programming practice.isz`. Looking at the results in the file:

```
D:\
Frn: 5000000000005
Pfrn: NULL
……
……
Ghost
Frn: 20000000000fd
Pfrn: 5000000000005
……
……
Linux
Frn: e0000000010d2
Pfrn: 20000000000fd
……
```


......
Linux Programming Practice.isz
Frn: 190000000003b2
Pfrn: e0000000010d2
We can get:
Linux Programming Practice.isz->Pfrn == Linux->Frn
Linux->Pfrn == Ghost->Frn
Ghost->Pfrn == D:\ -> Frn
D:\ -> Pfrn Stop
```

So a recursive function (a function that calls itself, either directly or indirectly) can be used to obtain the complete path.

### 4.2 Building Lookup Data

#### 4.2.1 Building Vector

```cpp
typedef struct _name_cur {
    CString filename;
    DWORDLONG pfrn;
} Name_Cur;
```

Vector:

An order container.

Elements in the vector are accessed by their positional index.

Elements can be added to the vector by calling the push_back or insert function.

Adding elements to the vector may require reallocating memory for the container, and may also invalidate all iterators.

Adding (or deleting) elements in the middle of the vector will invalidate all iterators to elements after the insertion (or deletion) point.

Using a Vector to store <filename, current directory> is not suitable for quick lookup methods such as binary search due to the requirements of fuzzy lookup. Instead, the linear traversal method of Vector from begin to end is more suitable for this requirement.

#### 4.2.2 Building Hashtable

```C++
typedef struct pfrn_name {
    DWORDLONG pfrn;
    CString filename;
} Pfrn_Name;

typedef map<DWORDLONG, Pfrn_Name> Frn_Pfrn_Name_Map;
```

Here we use STL's map library function^[5]^, map is a collection of key-value pairs.

The map type is often understood as an associative array: a value can be obtained using a key as an index, just like a built-in array type.

The essence of association is that the value of an element is associated with a specific key, not obtained by the position of the element in the array.

1. Considering that if one were to implement their own hash function, there might be several bugs that would affect the progress, it is better to use the stable map provided by STL instead to implement.
2. "Don't Reinvent the Wheel", don't reinvent what has already been invented^[6]^
   1. For example, Google's Android system uses the well-established Linux kernel because Google may not have the ability to develop a system kernel on its own. I believe that as a world-class giant company like Google, it is not impossible for them to possess this capability, but rather this is a demonstration of the principle to not reinvent a wheel that has already been invented.

#### 4.2.3 Inserting Data:

This is where we use the loop in section 4.1.5 to obtain information from the USN Journal file, and insert the `filename`, `pfrn`, and `frn` obtained from the `USN_RECORD` information into both the vector and hash table.

```
pfrnName.filename = nameCur.filename = CfileName;
pfrnName.pfrn = nameCur.pfrn = UsnRecord->ParentFileReferenceNumber;
VecNameCur.push_back(nameCur);
frnPfrnNameMap[UsnRecord->FileReferenceNumber] = pfrnName;
```

### 4.3 User Interface

When starting a project, VC2005 provides command-line (console) projects and GUI projects.

Command-line programming is very convenient, fast, practical, and relatively simple. 
When the console can meet the requirements, there is no need for a GUI. 

Or sometimes the target platform cannot display a graphical user interface. 

However, GUI has more obvious advantages, such as menu system and better interactivity. 

In addition, the mouse is very useful during menu selection, field data movement and input process, and GUI is the only way to go.

Therefore, when writing a program, try out intermediate data on the console first. When there are no problems, switch to the GUI interface.

MFC: Microsoft Foundation Classes, also known as MFC for short, is Microsoft's biggest contribution to "reducing the tediousness, boredom, and difficulty of Windows program design".

MFC makes the creation of dialog boxes extremely simple.

It also implements a message dispatching system, which handles common errors relating to WPARAM and LPARAM.

MFC is even the driving force behind some people learning C++.

This program is implemented with MFC provided by Microsoft as shown in Figure 4-2:

![Figure 4-2 MFC Interface](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_04.jpg)

#### 4.3.1 KISS Principle

The KISS principle stands for "Keep It Simple and Stupid", emphasizing the importance of simplicity and ease of operation in design.

The KISS principle is possibly the most esteemed principle in design, from home decor and interface design to operation design. Complex things are increasingly being frowned upon, while simple things are being appreciated more and more. Examples of negative UI design include Chinese websites and complex interface designs.

"IKEA" emphasizes simple and efficient home design and manufacturing concepts; "Microsoft" emphasizes a "what you see is what you get" philosophy; "Google" has a simple and straightforward business style that adheres to the KISS principle without exception. ^[7]^

#### 4.3.2 Functionality

Under the KISS principle, simplicity should be emphasized as much as possible.

Adding Editbox, button, listbox, and menu controls, the functionality is as follows:

- Editbox
  - Obtain the user input string to be searched for
- Button
  1. Obtain the string from the EditBox
  2. Display a prompt if the string is too short
  3. Add a magnifying glass image for obvious functionality
- Listbox
  1. Display the matching filenames and their paths, and double-clicking opens the file
  2. Due to the length of the path, add a horizontal scrollbar
  3. Double-clicking on a result file path in the Listbox opens the selected file
- Menu
  - A menu that allows case-sensitive/insensitive searching and searching in either a forward or backward order.

### 4.4 Adding Threads

#### 4.4.1 Why Add Threads?

The benefits of threads: threads are inexpensive.

Threads start and exit quickly and have a minimal impact on system resources.

Taking these factors into consideration:

1. Due to the limitations of IO, multiple threads reading MFT from the same hard drive simultaneously may not be very meaningful. 
2. Accessing MFT from different partitions on the same hard drive may actually affect speed, but it can greatly improve efficiency when accessing multiple hard drives. However, computer users with multiple hard drives may have formed a RAID array. The best solution is to read the disks in sequence from A-Z. 
3. Initially, when creating MFC, executing in order resulted in the interface not being displayed until all data was calculated, which could easily make users lose patience. 
4. The ultimate solution is to have one UI thread and one worker thread to calculate data in the background. 

#### 4.4.2 Starting a Worker Thread in MFC

MFC has supported multithreading for a long time. 

In a typical MFC program, multithreading support is hidden behind a very impressive amount of work. 

MFC even tries to reinforce certain Win32 concepts related to multithreading. 

Both the GUI and worker threads are started using `AfxBeginThread()`. However, MFC uses the overloading feature of C++ functions to provide two different declarations for this function. The compiler automatically selects the correct one based on the parameters you provide.^[9]^ 

Use `AfxBeginThread` to start a thread. 

`pParam` is an arbitrary 4-byte value that is passed to the new thread. 

It can be an integer, a pointer, or simply 0. 

Only the first two parameters are used here, the thread function in the object and a pointer to the object. 

Two threads can be seen in the task manager, as shown in Figure 4-3. 

![Figure 4-3 Displaying the Number of Threads in the Task Manager](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_03.jpg)

When the worker thread completes its task, it will automatically terminate and return to a single thread. 

#### 4.4.3 Minimizing to the Notification Area

Nowadays, more and more programs click on the close button in the upper right corner to minimize to the tray icon. To truly exit the program, you need to right-click on the tray icon and select "exit". 

Minimizing to the system tray while keeping the program running can make it easier to find files. 

- Minimize to the taskbar. 
- Clicking close will exit to the system tray and will not be displayed on the taskbar. Double-click to restore the window display.

- Add menu to tray icon.

### 4.5 Search

#### 4.5.1 Wildcards

In actual use, file names cannot always be remembered clearly, for example, test.2012-5-14.txt, often users input "test.txt", so fuzzy search is needed.

As creating a hash table with <filename, pfrn> under console, if fuzzy search is needed, the hash table is unnecessary, and vector container can be directly utilized.

Considering the actual situation, strict wildcards are unnecessary, '* ?' can solve most of the search problems, and users generally may not know how to use wildcards, let alone regular expressions.

Therefore, replace '* ?' with "space" to implement the search method of separating keywords with spaces like that of Baidu and Google, which is a good solution.

#### 4.5.2 Case sensitivity and order

There are generally three situations:

1. Users may not remember the case of file names or paths.
2. Sometimes there are too many files, and strict case sensitivity is needed to filter the correct files.
3. Users may not remember the order of keywords.

Therefore, the following two options are provided:

1. Strict case sensitivity.
2. No order.

According to the general situation, the default search method is case-insensitive and has order.

#### 4.5.3 User privacy and system paths

1. Considering that some folders are used to store users' private files.
2. System folders, such as 'c:\windows\*', are generally not necessary for users and searching for them will only increase unnecessary files, adding to the burden on the system.

Therefore, it is necessary to add an option to exclude folders, as shown in Figure 4-4.

![Figure 4-4 Exclude folders interface](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_06.jpg)

The corresponding function is:

```cpp
bool isIgnore( vector<string>* pignorelist ) {
	string tmp = CW2A(path);
	for ( vector<string>::iterator it = pignorelist->begin();
		it != pignorelist->end(); ++it ) {
			size_t i = it->length();

```cpp
bool cmpStrStr::cmpStrFilename(CString str, CString filename) {
	CString strtmp;
	CString filenametmp;

	if ( !infilename(strtmp, filenametmp) ) {
		return false;
	}

	if ( uplow ) {
		strtmp.MakeLower();
		filenametmp.MakeLower();
	}

	if ( isOrder ) {
		if ( strtmp.Find(filenametmp) != -1 ) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if ( filenametmp.Find(strtmp) != -1 ) {
			return true;
		}
		else {
			return false;
		}
	}
}
```

`infilename` 函数：

```cpp
bool cmpStrStr::infilename(CString& strtmp, CString& filenametmp) {
	if ( str.IsEmpty() ) {
		strtmp = "";
		filenametmp = "";
		return false;
	}
	else {
		// 从尾部查找 如果找到'\' 说明str只是一部分路径
		int pos = str.ReverseFind('\\');

		if ( pos == -1 ) {
			strtmp = str;
			filenametmp = "";
			return false;
		}
		else {
			strtmp = str.Right(str.GetLength() - (pos + 1));
			filenametmp = str.Mid(pos + 1);
			return true;
		}
	}
}
```

```cpp
int pos = 0;

int end = str.GetLength();

while ( pos < end ) {
	// For str, get the keywords separated by each space
	pos = str.Find( _T(' ') );

	CString strtmp;
	if ( pos == -1 ) {
		// No spaces
		strtmp = str;
		pos = end;
	} else {
		strtmp = str.Mid(0, pos-0);
	}

	if ( !infilename(strtmp, filename) ) {
		return false;
	}

	str.Delete(0, pos);
	str.TrimLeft(' ');
}
```

It's convenient to modify the string matching algorithm in the infilename function to achieve certain extensions.

```cpp
CString filenametmp(filename);
int pos;

if ( !uplow ) {
	// Case-sensitive
	filenametmp.MakeLower();
	pos = filenametmp.Find(strtmp.MakeLower());
} else {
	pos = filenametmp.Find(strtmp);
}

if ( -1 == pos ) {
	return false;
}

if ( !isOrder ) {
	// No order
	filename.Delete(0, pos+1);
}
```

#### 4.5.5 Path Output

Once we have the matched file name, the next step is to obtain the system path of the file.

Pass the full name of the matched file name into the frnPfrnNameMap hash table constructed by **4.2.2** to recursively obtain the path.

```C++
CString Volume::getPath(DWORDLONG frn, CString &path) {

	// Look up 2
	Frn_Pfrn_Name_Map::iterator it = frnPfrnNameMap.find(frn);
```

```cpp
if (it != frnPfrnNameMap.end()) {

		  if (0 != it->second.pfrn) {
			  getPath(it->second.pfrn, path);
		  }

		  path += it->second.filename;
		  path += (_T("\\"));
}

return path;
```

## 5 Complexity Analysis

### 5.1 Time Complexity

In theory, the time complexity is:

![Figure 5-1 Complexity Analysis](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_05.jpg)

On an AMD Athlon (tm) II X2 245 2.9GHz processor, the result is almost instantaneously completed.

### 5.2 Space Complexity

The number of files on the computer is huge - usually on the order of 100,000 to 1 million - so the program's memory usage can be negligible.

234,708 files are about 43M.

For a system with at least 2G of memory, it is not a burden.

## 6 Problems Encountered

### 6.1 Unable to initialize USN files

The USN log file on the O drive on the computer could not be initialized, and the following was found through debugging:

`DeviceIoControl` returns 0, `GetLastError 0x70`

Consulting MSDN, the reason is `ERROR_SHARING_PAUSED`

It turned out that I forgot to initialize and add the following code, and the program can access the O drive normally:

```cpp
CREATE_USN_JOURNAL_DATA cujd;
cujd.MaximumSize = 0; // 0 means default value
cujd.AllocationDelta = 0; // 0 means default value
```

### 6.2 Issue with wchar and char in unicode 

Initially, I used the string function of the standard library as the processing method for strings, but for double-byte wchar in unicode, I ran into quite a few troubles in string conversion. Finally, all default strings were set to CString and initialized with `_T("")` to solve the problem.

### 6.3 Minor issue with #ifdef placement

One cannot set breakpoints inside `#ifdef`. Later realized that `#define` was written before `include`.

### 6.4 Parameter Passing

Initially designed with a procedural philosophy and got stuck in trouble passing the same handle by passing arguments. Later switched to the use of class methods, which improved a lot.

## 7 Minor optimizations

### 7.1 Reading Speed

When constructing the hash function, thought of three methods and conducted experiments. Using the `time` function to measure the time used, the results are as follows:

1. Modify the code to read `usn` once and write to a temporary file for later reading and constructing the hash table.
   - Time averages around 27s per execution.
2. Use a vector to store data in memory and release afterwards.
   - Results are excellent with a time nearly 10 times faster!!! As shown in Figure 7-1:

![7-1 Traversing All Disks](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_06.jpg)

Completely within an acceptable range.

The first run clearly took longer, but subsequent runs pre-read files from the disk cache, causing the speed to skyrocket.

Analysis indicates that the hotspots are disk seek time and file reading.

### 7.2 File Opening

In section **4.3.2**, when double-clicking on the file path in the ListBox, opening the response file with the `system()` system function would pop up a black console window, and the main window would be locked until the file was closed before continuing execution. This was very unfavorable for user experience.

Switched to calling `ShellExecute` function to open a new process, so the main window could continue executing.

### 7.3 Progress Bar

After Version 1.0 was released, some students reported that there was no prompt when the program was counting the files in the background, so they didn't know when it was done.

Added a progress bar to the bottom of the main interface to enhance interactivity, as shown in Figure 7-2:

![Figure 7-2 Progress Bar](https://raw.githubusercontent.com/LeiHao0/BlogAssets/assets/QSearch_07.jpg)

Of course, the thread function has also been modified as necessary:

# 8 Conclusion

At this point, the basic functions of the program are complete, and it has been tested on multiple computers and different systems (XP and later versions of Windows), with almost no cases of program hanging or crashing.

Due to time constraints, many features have not been implemented, such as the interface cannot stretch, and dynamic statistical file data is not available.

# References

- [1] Hardware White Paper. NTFS File System Specification[OL]. Baidu Document, 2011: 30.
- [2] Beiyu. Technical Research on NTFS File System[OL]. 2007: 7.
- [3] Infant. Interactive Encyclopedia[OL]. http://www.hudong.com/wiki/ntfs
- [4] Microsoft. MSDN Library[OL]. 2012 :
- [5] Stanley B.Lippman / Josée LaJoie / Barbara E.Moo. C\+\+ Primer[M]. - Addison-Wesley Professional , 2006 : 10.3
- [6] Eric S. Raymond. Translator: Jiang Hong, He Yuan, Cai Xiaojun. UNIX Art of Programming[m]. Publishing House of Electronics Industry - Social, 2006:
- [7] Chen Hao. Some Principles of Software Design[OL]. http://coolshell.cn/articles/4535.html
- [8] Sun Xin. In-depth Explanation of VC++[m]. Publishing House of Electronics Industry, 2006
- [9] Jim Beveridge / Robert Wiener. Multithreading Applications in Win32: The Complete Guide to Threads
- [10] Addison-Wesley Professional, 1996: 223-243