#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable : 4996)// acess deprecated
#pragma warning(disable : 4244) // float to long/int
#pragma warning(disable : 28251) // nekonzistentni poznamka u WinMain
#define MAX 250

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>

#include <dwrite.h>
#include <wincodec.h>
#include <thread>
#include  <io.h>
#include  <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <locale>
#include <codecvt>

#include <shellapi.h>
#include <atlstr.h>
#include <CommCtrl.h>
#include <commdlg.h>
#include <Shlwapi.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/error.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "windowscodecs.lib")

#include "Resource.h"

using namespace std;
using namespace rapidjson;

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif


#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

bool adventure_game = false;
//#define POSOUVANI_KAMERY

//sebr_"prvni pismeno barvy""[k]krystal [o]orb"
int sebr_mk = 0, sebr_ck = 0, sebr_ok = 0, sebr_zk = 0, sebr_zlk = 0;
int sebr_mo = 0, sebr_co = 0, sebr_oo = 0, sebr_zo = 0, sebr_zlo = 0;
int sebr_pen = 0;

HWND shared/*main window*/, manager/*upload-download manager window*/;
int x = 2, y = 2;
int mom_postava = 1;

bool menu = true;
bool menu2 = false;
bool darkmode = false;
bool loaded = false;//pokud se nacetla jina mapa uz drive

int mode = 1;
string adventure_game_path = "";

bool use_it = false;
POINT _cursor = { 0,0 };

bool startpos = false;

enum typ_zdi {
    kamen_ = 1,
    kamen_o = 2,
    bedna = 4,
    start_pos = 5,
    nic__ = 3
};
enum typ_podlahy {
    trava = 1,
    kamen = 2,
    nic = 3
};
enum typ_predmetu {
    krystal_m = 1,/*modry*/
    krystal_c = 2,/*cerveny*/
    krystal_z = 4,/*zeleny*/
    krystal_o = 5, /*oranzovy*/
    krystal_zl = 6,
    //orby
    orb_m = 7,/*modry*/
    orb_c = 8,/*cerveny*/
    orb_z = 9,/*zeleny*/
    orb_o = 10, /*oranzovy*/
    orb_zl = 11,/*zluty*/
    peniz = 12,
    nic_ = 3
};


//y,x
typ_zdi zed[10][17] = {{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__},
{nic__, nic__, nic__, nic__, nic__, nic__, nic__,nic__, nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__,nic__}, };

//y, x
typ_podlahy podlaha[10][17] = { {nic,nic,nic,nic,trava,trava,kamen,kamen,kamen,kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, trava,trava,nic, trava, trava, kamen, kamen, kamen,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, trava, kamen, nic, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, kamen, trava, trava, trava, kamen, kamen,trava,trava,trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, trava, kamen, trava, trava, trava, kamen, kamen, trava, trava, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, trava, kamen, nic, trava, kamen, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, trava, kamen, nic, trava, trava, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{trava, kamen, kamen, nic, kamen, kamen, trava, trava, trava, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{kamen, kamen, kamen, kamen, kamen, kamen, kamen, trava, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen},
{kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen, kamen,kamen,kamen,kamen,kamen,kamen,kamen} };
typ_predmetu predmety[10][17] = { {nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_},
{nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_,nic_, nic_,nic_,nic_,nic_,nic_,nic_,nic_} };

bool Collision(D2D1_RECT_F sprite1, D2D1_RECT_F sprite2, int width, int height)
{
    RECT rect1;
    rect1.left = sprite1.left + 1;
    rect1.top = sprite1.top + 1;
    rect1.right = sprite1.left + width - 1;
    rect1.bottom = sprite1.top + height - 1;

    RECT rect2;
    rect2.left = sprite2.left + 1;
    rect2.top = sprite2.top + 1;
    rect2.right = sprite2.left + width - 1;
    rect2.bottom = sprite2.top + height - 1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);
}

//Creates simple DirectX 2D rect
//left = x, top = y, right = x+width, down = y+height
D2D1_RECT_F SRect(float left, float top, float right, float down) {
    return D2D1::RectF(left, top, right, down);
}

D2D1_RECT_F SRect2(float left, float top, float width, float height) {
    return D2D1::RectF(left, top, left+width, top+height);
}

void FltInfo(float i) {
    string t = to_string(i);
    MessageBoxA(NULL, t.c_str(), t.c_str(), MB_OK);
}

static D2D1_COLOR_F FromRGB(DWORD r, float A = 1.0f)
{
    D2D1_COLOR_F C = {};
    C.r = GetRValue(r) / 255.0f;
    C.g = GetGValue(r) / 255.0f;
    C.b = GetBValue(r) / 255.0f;
    C.a = A;
    return C;
}
D2D1_COLOR_F RGBA(int r, int g, int b, int a = 255) {
    D2D1_COLOR_F C = {};
    C.r = r / 255.0f;
    C.g = g / 255.0f;
    C.b = b / 255.0f;
    C.a = a / 255.0f;
    return C;
}

void eraseSubStr(std::string& mainStr, const std::string& toErase)
{
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);
    if (pos != std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

inline bool exist_read_file(const std::string& name) {
    return (access(name.c_str(), 4) != -1);
}

void KontrolaKrystalu() {
    if (predmety[y][x] == typ_predmetu::krystal_m) {
        sebr_mk++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::krystal_c) {
        sebr_ck++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::krystal_o) {
        sebr_ok++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::krystal_z) {
        sebr_zk++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::krystal_zl) {
        sebr_zlk++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::orb_m) {
        sebr_mo++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::orb_c) {
        sebr_co++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::orb_o) {
        sebr_oo++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::orb_z) {
        sebr_zo++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::orb_zl) {
        sebr_zlo++;
        predmety[y][x] = typ_predmetu::nic_;
    }
    else if (predmety[y][x] == typ_predmetu::peniz) {
        sebr_pen++;
        predmety[y][x] = typ_predmetu::nic_;
    }
}

std::string base_name(std::string const& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

/*

-------------------------------------------------------------------------------------------------------------
SEKCE PRIPOJENI K SERVERU
-------------------------------------------------------------------------------------------------------------

*/


void mParseUrl(char* mUrl, string& serverName, string& filepath, string& filename)
{
    string::size_type n;
    string url = mUrl;

    if (url.substr(0, 7) == "http://")
        url.erase(0, 7);

    if (url.substr(0, 8) == "https://")
        url.erase(0, 8);

    n = url.find('/');
    if (n != string::npos)
    {
        serverName = url.substr(0, n);
        filepath = url.substr(n);
        n = filepath.rfind('/');
        filename = filepath.substr(n + 1);
    }

    else
    {
        serverName = url;
        filepath = "/";
        filename = "";
    }
}

SOCKET connectToServer(char* szServerName, WORD portNum)
{
    struct hostent* hp;
    unsigned int addr;
    struct sockaddr_in server;
    SOCKET conn;

    conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn == INVALID_SOCKET)
        return NULL;

    if (inet_addr(szServerName) == INADDR_NONE)
    {
        hp = gethostbyname(szServerName);
    }
    else
    {
        addr = inet_addr(szServerName);
        hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if (hp == NULL)
    {
        closesocket(conn);
        return NULL;
    }

    server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);
    if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
    {
        closesocket(conn);
        return NULL;
    }
    return conn;
}

int getHeaderLength(char* content)
{
    const char* srchStr1 = "\r\n\r\n", * srchStr2 = "\n\r\n\r";
    char* findPos;
    int ofset = -1;

    findPos = strstr(content, srchStr1);
    if (findPos != NULL)
    {
        ofset = findPos - content;
        ofset += strlen(srchStr1);
    }

    else
    {
        findPos = strstr(content, srchStr2);
        if (findPos != NULL)
        {
            ofset = findPos - content;
            ofset += strlen(srchStr2);
        }
    }
    return ofset;
}
void mBox(string t, string t2, UINT i) {
    MessageBoxA(NULL, t.c_str(), t2.c_str(), i);
}

void IntInfo(int i) {
    string t = to_string(i);
    MessageBoxA(NULL, t.c_str(), t.c_str(), MB_OK);
}
char* readUrl2(char* szUrl, long& bytesReturnedOut, char** headerOut)
{
    const int bufSize = 6000;
    char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
    char* tmpResult = NULL, * result;
    SOCKET conn;
    string server="", filepath="", filename="";
    long totalBytesRead, thisReadSize, headerLen;

    mParseUrl(szUrl, server, filepath, filename);

    //connect 
    conn = connectToServer((char*)"paternremembergame.tode.cz", 80);
    if (conn == NULL) {
        IntInfo(5555);
    }
    //send GET request
    sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
    strcpy(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");//paternremembergame.tode.cz
    strcat(tmpBuffer, "\r\nHost: paternremembergame.tode.cz");
    strcat(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");
    send(conn, sendBuffer, strlen(sendBuffer), 0);
    printf("Buffer being sent:\n%s", sendBuffer);

    // Receive until the peer closes the connection
    totalBytesRead = 0;
    while (1)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv(conn, readBuffer, bufSize, 0);

        if (thisReadSize <= 0)
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);
        memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }
    headerLen = getHeaderLength(tmpResult);
    long contenLen = totalBytesRead - headerLen;
    result = new char[contenLen + 1];
    memcpy(result, tmpResult + headerLen, contenLen);
    result[contenLen] = 0x0;
    char* myTmp;

    myTmp = new char[headerLen + 1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contenLen;
    closesocket(conn);
    return(result);
}

string ReplaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
string StringToHttpRequest(string text) {
    //viz https://www.w3schools.com/tags/ref_urlencode.asp
    string tmp = text;
    tmp = ReplaceAll(tmp, " ", "%20");
    tmp = ReplaceAll(tmp, "\n", "%0D%0A");//CRLF
    tmp = ReplaceAll(tmp, "\t", "%09");
    return tmp;
}


int Upload(string mapname, string content) {
    //errory: https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
    SOCKET con = NULL;
    WSADATA wsaData;
    if (WSAStartup(0x101, &wsaData) != 0) {
        string message = "Cant startup winsocket, do you have wifi turned on?\nError code: " + to_string(WSAGetLastError());
        MessageBoxA(NULL, message.c_str(), "error", MB_OK | MB_ICONERROR);
        return 1;
    }
    con = connectToServer((char*)"paternremembergame.tode.cz", 80);
    if (con == INVALID_SOCKET) {
        string message = "Cant connect to server, do you have wifi turned on?\nError code: " + to_string(WSAGetLastError());
        MessageBoxA(NULL, message.c_str(), "error", MB_OK | MB_ICONERROR);
        return 2;
    }
    string sendBuf;
    stringstream ss;
    ss << "GET /test.php?createfile=maps/" << mapname << "&content=" << content << "&use_log=1 HTTP/1.1";
    ss << "\r\nHost: paternremembergame.tode.cz";
    ss << "\r\n";
    ss << "\r\n";
    sendBuf = ss.str();
    //mBox(sendBuf,"sendBuf", MB_OK);
    int response = send(con, (char*)sendBuf.c_str(), sendBuf.length(), 0);
    if (response == SOCKET_ERROR) {
        if (con == INVALID_SOCKET) {
            string message = "Cant send map to server, are in file special or local keys?\nError code: " + to_string(WSAGetLastError());
            MessageBoxA(NULL, message.c_str(), "error", MB_OK | MB_ICONERROR);
        }
        closesocket(con);
        WSACleanup();
        return 3;
    }
    closesocket(con);
    return 0;
    WSACleanup();
}

string LoadFileToString(string filename) {
    ifstream t(filename.c_str());
    stringstream buffer;
    buffer << t.rdbuf();
    t.close();
    return buffer.str();
}



BOOL GetMapFromServer(string mapname) {
    mapname = StringToHttpRequest(mapname);
    WSADATA wsaData;
    const int bufLen = 1024;
    string url = "https://paternremembergame.tode.cz/test.php?file=maps/" + mapname;
    char* szUrl = &url[0];
    long fileSize=0;
    char* memBuffer, * headerBuffer;
    FILE* fp;

    memBuffer = headerBuffer = NULL;
    if (WSAStartup(0x101, &wsaData) != 0) {
        return 0;
    }
    memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
    //readed data
    if (fileSize != 0)
    {
        fp = fopen(mapname.c_str(), "wb");
        fwrite(memBuffer, 1, fileSize, fp);
        fclose(fp);
        delete(memBuffer);
        delete(headerBuffer);
    }
    return 1;
    WSACleanup();
}
BOOL UploadMapToServer(string mapname) {
    //oddelani mezer, carrige returnu, novy radku -> na http request
    if (!exist_read_file(mapname)) {
        return false;
    }
    string content = LoadFileToString(mapname);
    mapname = base_name(mapname.c_str());
    content = StringToHttpRequest(content);
    mapname = StringToHttpRequest(mapname);
    if (Upload(mapname, content) > 0) {
        return false;
    }
    return true;
    WSACleanup();
}

/*

-------------------------------------------------------------------------------------------------------------
KONEC SEKCE PRIPOJENI K SERVERU
-------------------------------------------------------------------------------------------------------------

*/

HWND CreateChoose(HWND hwndParent) {
    // 46, 28, 81, 12
    int xpos = 46;
    int ypos = 28;
    int nwidth = 120;
    int nheight = 800;

    HWND hWndComboBox = CreateWindow(WC_COMBOBOX, TEXT(""),
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        xpos, ypos, nwidth, nheight, hwndParent, NULL, HINST_THISCOMPONENT,
        NULL);
    TCHAR Planets[19][20] =
    {
        TEXT("trava"), TEXT("kamen-podlaha"), TEXT("kamen-zed"), TEXT("bedna"), TEXT("nic-podlaha"), TEXT("nic-predmet"), TEXT("krystal cerveny"), TEXT("krystal modry"), TEXT("krystal oranzovy"), TEXT("krystal zeleny"), TEXT("krystal zluty"), TEXT("kamen ornament"), TEXT("orb cerveny"), TEXT("orb modry"), TEXT("orb zeleny"), TEXT("orb oranzovy"), TEXT("orb zluty"), TEXT("peniz"), TEXT("startpos")
    };

    TCHAR A[18];
    int  k = 0;

    memset(&A, 0, sizeof(A));
    for (k = 0; k <= 17; k += 1)
    {
        wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Planets[k]);

        // Add string to combobox.
        SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
    }
    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
    return hWndComboBox;
}

INT_PTR CALLBACK EditChoose(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    HWND combo = NULL, check = NULL;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        combo = CreateChoose(hDlg);
        check = GetDlgItem(hDlg, IDC_CHECK1);
        return (INT_PTR)TRUE;
    }break;
    case WM_DESTROY:
    {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            if (SendDlgItemMessage(hDlg, IDC_CHECK1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                //clear everything
                for (int _y = 0; _y < 10; _y++) {
                    for (int _x = 0; _x < 17; _x++) {
                        predmety[_y][_x] = typ_predmetu::nic_;
                        zed[_y][_x] = typ_zdi::nic__;
                        podlaha[_y][_x] = typ_podlahy::nic;
                    }
                }
                SendMessage(shared, WM_PAINT, 0, 0);
            }
            else {
                //not checked
            }
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_CANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                (WPARAM)0, (LPARAM)0);
            TCHAR  ListItem[256];
            (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                (WPARAM)ItemIndex, (LPARAM)ListItem);
            if (ItemIndex == 0) {
                mode = 1;
            }
            else if (ItemIndex == 1) {
                mode = 2;
            }
            else if (ItemIndex == 2) {
                mode = 4;
            }
            else if (ItemIndex == 3) {
                mode = 18;
            }
            else if (ItemIndex == 4) {
                mode = 3;
            }
            else if (ItemIndex == 5) {
                mode = 7;
            }
            else if (ItemIndex == 6) {
                mode = 5;
            }
            else if (ItemIndex == 7) {
                mode = 6;
            }
            else if (ItemIndex == 8) {
                mode = 9;
            }
            else if (ItemIndex == 9) {
                mode = 8;
            }
            else if (ItemIndex == 10) {
                mode = 10;
            }
            else if (ItemIndex == 11) {
                mode = 11;
            }
            else if (ItemIndex == 12) {
                mode = 13;
            }
            else if (ItemIndex == 13) {
                mode = 12;
            }
            else if (ItemIndex == 14) {
                mode = 15;
            }
            else if (ItemIndex == 15) {
                mode = 14;
            }
            else if (ItemIndex == 16) {
                mode = 16;
            }
            else if (ItemIndex == 17) {
                mode = 17;
            }
            else if (ItemIndex == 18) {
                mode = 19;
            }
        }
        break;
    }
    return (INT_PTR)FALSE;
}

COORD coord(int x, int y) {
    COORD i = { x,y };
    return i;
}

COORD find_startpos() {
    for (int x = 0; x < 17; x++) {
        for (int y = 0; y < 10; y++) {
            if (zed[y][x] == typ_zdi::start_pos) {
                return coord(x, y);
            }
        }
    }
    return coord(-1,-1);
}


#define RAPIDJSON_ASSERT 0

//FUNKCE: LoadMap()
// filename - nazev .json souboru s mapou
// return true -> v pohode
int LoadMap(string filename, bool print_errors = false) {
    if ((filename == "") || (filename == " ")) {
        return 1;//empty string
    }
    if (!exist_read_file(filename.c_str())) {
        return 2;//not exist, not have rights/cant read
    }
    Document doc;
    ParseResult re = doc.Parse(LoadFileToString(filename.c_str()).c_str());
    if (re.IsError()) {
        if (print_errors) {
            cout << "parse error - result" << endl;
            cout << GetParseErrorFunc(re.Code()) << " ofset: " << re.Offset() << endl;
        }
        return 3;
    }
    if (doc.HasParseError()) {
        cout << "parse error" << endl;
        return 4;
    }

    if ((!doc["info"]["start pos"]["y"].IsInt()) || (!doc["info"]["start pos"]["x"].IsInt())) {
        cout << "not int" << endl;
        return 5;
    }
    y = doc["info"]["start pos"]["y"].GetInt();
    x = doc["info"]["start pos"]["x"].GetInt();

    Value& map = doc["info"]["map"];
    for (const Value& value : map.GetArray()) {
        if ((!value["pos"]["x"].IsInt()) || (!value["pos"]["y"].IsInt()) || (!value["iid"].IsInt()) || (!value["fid"].IsInt()) || (!value["wid"].IsInt())) {
            cout << "not pos y, iid..." << endl;
            return 6;
        }
        int _x = value["pos"]["x"].GetInt();
        int _y = value["pos"]["y"].GetInt();
        int iid, wid, fid;
        iid = value["iid"].GetInt();
        fid = value["fid"].GetInt();
        wid = value["wid"].GetInt();
        zed[_y][_x] = (typ_zdi)wid;
        predmety[_y][_x] = (typ_predmetu)iid;
        podlaha[_y][_x] = (typ_podlahy)fid;
    }
    return 0;
}

void SaveMap(string filename, int startposx, int startposy) {
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();

    //string auth(getenv("USERNAME"));
    Value info(kObjectType), startpos(kObjectType), mapsize(kObjectType);
    Value array(kArrayType);

    mapsize.AddMember("height", 10, allocator);
    mapsize.AddMember("width", 17, allocator);

    startpos.AddMember("x", startposx, allocator);
    startpos.AddMember("y", startposy, allocator);

    for (int _x = 0; _x < 17; _x++) {
        for (int _y = 0; _y < 10; _y++) {
            Value ar(kObjectType);
            Value pos(kObjectType);
            pos.AddMember("x", _x, allocator);
            pos.AddMember("y", _y, allocator);
            ar.AddMember("pos", pos, allocator);
            ar.AddMember("iid", (int)predmety[_y][_x], allocator);
            ar.AddMember("wid", (int)zed[_y][_x], allocator);
            ar.AddMember("fid", (int)podlaha[_y][_x], allocator);
            array.PushBack(ar, allocator);
        }
    }

    info.AddMember("author", "Username", allocator);
    info.AddMember("mapsize", mapsize, allocator);
    info.AddMember("start pos", startpos, allocator);
    info.AddMember("map", array, allocator);

    doc.AddMember("info", info, allocator);

    //safe to file
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    ofstream out(filename.c_str());
    out << buffer.GetString();
    out.close();
}

//true v poradku jinak false
bool LoadFile(string open_file_name) {
    int startposx, startposy, width, height;
    if (exist_read_file(open_file_name.c_str())) {
        ifstream in(open_file_name.c_str());
        in >> width;
        in >> height;
        in >> startposx;
        in >> startposy;
        x = startposx;
        y = startposy;
        for (int _y = 0; _y < 10; _y++) {
            for (int _x = 0; _x < 17; _x++) {
                int _t;
                in >> _t;
                if (_t == 1) {
                    zed[_y][_x] = typ_zdi::kamen_;
                }
                else if (_t == 2) {
                    zed[_y][_x] = typ_zdi::kamen_o;
                }
                else if (_t == 4) {
                    zed[_y][_x] = typ_zdi::bedna;
                }
                else {
                    zed[_y][_x] = typ_zdi::nic__;
                }
            }
        }
        for (int _y = 0; _y < 10; _y++) {
            for (int _x = 0; _x < 17; _x++) {
                int t;
                in >> t;
                if (t == 1) {
                    podlaha[_y][_x] = typ_podlahy::trava;
                }
                else if (t == 2) {
                    podlaha[_y][_x] = typ_podlahy::kamen;
                }
                else {
                    podlaha[_y][_x] = typ_podlahy::nic;
                }
            }
        }
        for (int _y = 0; _y < 10; _y++) {
            for (int _x = 0; _x < 17; _x++) {
                int t;
                in >> t;
                if (t == 0) {
                    predmety[_y][_x] = typ_predmetu::nic_;
                }
                else if (t == 1) {
                    predmety[_y][_x] = typ_predmetu::krystal_m;
                }
                else if (t == 2) {
                    predmety[_y][_x] = typ_predmetu::krystal_c;
                }
                else if (t == 3) {
                    predmety[_y][_x] = typ_predmetu::nic_;
                }
                else if (t == 4) {
                    predmety[_y][_x] = typ_predmetu::krystal_z;
                }
                else if (t == 5) {
                    predmety[_y][_x] = typ_predmetu::krystal_o;
                }
                else if (t == 6) {
                    predmety[_y][_x] = typ_predmetu::krystal_zl;
                }
                else if (t == 7) {
                    predmety[_y][_x] = typ_predmetu::orb_m;
                }
                else if (t == 8) {
                    predmety[_y][_x] = typ_predmetu::orb_c;
                }
                else if (t == 9) {
                    predmety[_y][_x] = typ_predmetu::orb_z;
                }
                else if (t == 10) {
                    predmety[_y][_x] = typ_predmetu::orb_o;
                }
                else if (t == 11) {
                    predmety[_y][_x] = typ_predmetu::orb_zl;
                }
                else if (t == 12) {
                    predmety[_y][_x] = typ_predmetu::peniz;
                }
                else {
                    predmety[_y][_x] = typ_predmetu::nic_;
                }
            }
        }
        in.close();
    }
    else {
        return false;
    }
    return true;
}

void LoadLevel(int level) {
    if (level == 1) {
        if (LoadMap("lets_go.json") > 0) {
            mBox("Cant open level", "error", MB_OK | MB_ICONERROR);
        }
    }
    else if (level == 2) {
        if (LoadMap("lets_go.json") > 0) {
            mBox("Cant open level", "error", MB_OK | MB_ICONERROR);
        }
    }
}

RECT ToRect(int x, int y, int width, int height) {
    RECT pos;
    pos.left = x;
    pos.right = y;
    pos.top = y+height;
    pos.bottom = x+width;
    return pos;
}

RECT GetWindowPos() {
    RECT pos;
    pos.left = NULL;
    pos.right = NULL;
    pos.top = NULL;
    pos.bottom = NULL;
    GetWindowRect(shared, &pos);
    return pos;
}

void CreateToolTipForRect(HWND hwndParent, RECT rc)
{
    // Create a tooltip.
    HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, NULL, HINST_THISCOMPONENT, NULL);

    SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    // Set up "tool" information. In this case, the "tool" is the entire parent window.

    TOOLINFOA ti = { 0 };
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hwndParent;
    ti.hinst = HINST_THISCOMPONENT;
   // ti.rect = rc;
    ti.lpszText = const_cast<char*>("hi");;

    GetClientRect(hwndParent, &ti.rect);

    // Associate the tooltip with the "tool" window.
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

class Button {
public:
    D2D1_RECT_F pos;
    ID2D1HwndRenderTarget* target;
    IDWriteTextFormat* format;
    ID2D1SolidColorBrush* color, *colortext;
    wstring text;

    Button() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        color = NULL;
        colortext = NULL;
        text = L"";
    }
    ~Button() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        color = NULL;
        colortext = NULL;
        text = L"";
    }
    Button(Button& src) {
        pos = src.pos;
        target = src.target;
        format = src.format;
        color = src.color;
        text = src.text;
        colortext = src.colortext;
    }
    Button(D2D1_RECT_F _pos, ID2D1HwndRenderTarget* _target, IDWriteTextFormat* _format, ID2D1SolidColorBrush* _color, ID2D1SolidColorBrush* _colortext, wstring _text) {
        pos = _pos;
        target = _target;
        format = _format;
        color = _color;
        text = _text;
        colortext = _colortext;
    }
    float GetWidth() {
        return (pos.right - pos.left);
    }
    float GetHeight() {
        return (pos.bottom - pos.top);
    }

    HRESULT CreateFormat(IDWriteFactory * writeFactory, LPCWSTR fontname, float fontsize, bool align_center) {
        HRESULT hr = writeFactory->CreateTextFormat(
            fontname,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontsize,
            L"", //locale
            &format
        );
        if (align_center) {
            format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }

        return hr;
    }
    HRESULT CreateColor(D2D1_COLOR_F col) {
        HRESULT hr = target->CreateSolidColorBrush(
            col,
            &color
        );
        return hr;
    }
    HRESULT CreateColorText(D2D1_COLOR_F col) {
        HRESULT hr = target->CreateSolidColorBrush(
            col,
            &colortext
        );
        return hr;
    }
    void CreatePos(float x, float y, float width, float height) {
        pos.left = x;
        pos.top = y;
        pos.bottom = y + height;
        pos.right = x + width;
    }
    void SetTarget(ID2D1HwndRenderTarget* _target) {
        target = _target;
    }
    void SetFormat(IDWriteTextFormat* _format) {
        format = _format;
    }
    void SetText(wstring _text) {
        text = _text;
    }
    void SetPos(D2D1_RECT_F _pos) {
        pos = _pos;
    }
    void SetColor(ID2D1SolidColorBrush* _color) {
        color = _color;
    }
    void SetColor2(D2D1_COLOR_F col) {
        color->SetColor(col);
    }
    void SetColorText(ID2D1SolidColorBrush* _colortext) {
        colortext = _colortext;
    }
    void SetColorText2(D2D1_COLOR_F col) {
        colortext->SetColor(col);
    }
    bool HitTest(POINT cursorpos, RECT winpos) {
        if ((cursorpos.y > (winpos.top + pos.top + 29)) && (cursorpos.x > (winpos.left + pos.left)) && (cursorpos.y < (winpos.top + 29 + pos.bottom)) && (cursorpos.x < (winpos.left + pos.right))) {
            return true;
        }
        return false;
    }
    void Draw() {
        target->FillRectangle(pos, color);
        target->DrawText(
            text.c_str(),
            text.length()+1,
            format,
            pos,
            colortext
        );
    }
    bool Erorr() {
        if (target == NULL || format == NULL || color == NULL || colortext == NULL || text == L"" || (pos.left == 0 && pos.right == 0 && pos.bottom == 0 && pos.top == 0)) {
            return true;
        }
        return false;
    }
    void Release() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        color = NULL;
        colortext = NULL;
        text = L"";
    }
};
class BitmapButton {
public:
    D2D1_RECT_F pos;
    ID2D1HwndRenderTarget* target;
    ID2D1Bitmap* bitmap;

    BitmapButton() {
        pos = { 0,0,0,0 };
        target = NULL;
        bitmap = NULL;
    }
    ~BitmapButton() {
        pos = { 0,0,0,0 };
        target = NULL;
        bitmap = NULL;
    }
    BitmapButton(BitmapButton& src) {
        pos = src.pos;
        target = src.target;
        bitmap = src.bitmap;
    }
    BitmapButton(D2D1_RECT_F _pos, ID2D1HwndRenderTarget* _target, ID2D1Bitmap* _bitmap) {
        pos = _pos;
        target = _target;
        bitmap = _bitmap;
    }
    float GetWidth() {
        return (pos.right - pos.left);
    }
    float GetHeight() {
        return (pos.bottom - pos.top);
    }
    void CreatePos(float x, float y, float width, float height) {
        pos.left = x;
        pos.top = y;
        pos.bottom = y + height;
        pos.right = x + width;
    }
    void SetTarget(ID2D1HwndRenderTarget* _target) {
        target = _target;
    }
    void SetBitmap(ID2D1Bitmap* _bitmap) {
        bitmap = _bitmap;
    }
    void SetPos(D2D1_RECT_F _pos) {
        pos = _pos;
    }
    bool HitTest(POINT cursorpos, RECT winpos) {
        if ((cursorpos.y > (winpos.top + pos.top + 29)) && (cursorpos.x > (winpos.left + pos.left)) && (cursorpos.y < (winpos.top + 29 + pos.bottom)) && (cursorpos.x < (winpos.left + pos.right))) {
            return true;
        }
        return false;
    }
    void Draw() {
        target->DrawBitmap(bitmap, pos, 1.0);
    }
    bool Erorr() {
        if (target == NULL  || bitmap == NULL || (pos.left == 0 && pos.right == 0 && pos.bottom == 0 && pos.top == 0)) {
            return true;
        }
        return false;
    }
    void Release() {
        pos = { 0,0,0,0 };
        target = NULL;
        bitmap = NULL;
    }
};
class Text {
public:
    D2D1_RECT_F pos;
    ID2D1HwndRenderTarget* target;
    IDWriteTextFormat* format;
    ID2D1SolidColorBrush *colortext;
    wstring text;
    Text() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        colortext = NULL;
        text = L"";
    }
    ~Text() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        colortext = NULL;
        text = L"";
    }
    Text(Text& src) {
        target = src.target;
        format = src.format;
        text = src.text;
        pos = src.pos;
        colortext = src.colortext;
    }
    Text(D2D1_RECT_F _pos, ID2D1HwndRenderTarget* _target, IDWriteTextFormat* _format, ID2D1SolidColorBrush* _colortext, wstring _text) {
        pos = _pos;
        target = _target;
        format = _format;
        text = _text;
        colortext = _colortext;
    }
    float GetWidth() {
        return (pos.right - pos.left);
    }
    float GetHeight() {
        return (pos.bottom - pos.top);
    }

    HRESULT CreateFormat(IDWriteFactory * writeFactory, LPCWSTR fontname, float fontsize) {
        HRESULT hr = writeFactory->CreateTextFormat(
            fontname,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontsize,
            L"", //locale
            &format
        );
        return hr;
    }
    HRESULT CreateColorText(D2D1_COLOR_F col) {
        HRESULT hr = target->CreateSolidColorBrush(
            col,
            &colortext
        );
        return hr;
    }
    void CreatePos(float x, float y, float width, float height) {
        pos.left = x;
        pos.top = y;
        pos.bottom = y + height;
        pos.right = x + width;
    }
    void SetTarget(ID2D1HwndRenderTarget* _target) {
        target = _target;
    }

    void SetFormat(IDWriteTextFormat* _format) {
        format = _format;
    }
    void SetText(wstring _text) {
        text = _text;
    }
    void SetPos(D2D1_RECT_F _pos) {
        pos = _pos;
    }
    void SetColorText(ID2D1SolidColorBrush* _colortext) {
        colortext = _colortext;
    }
    bool HitTest(POINT cursorpos, RECT winpos) {
        if ((cursorpos.y > (winpos.top + pos.top + 29)) && (cursorpos.x > (winpos.left + pos.left)) && (cursorpos.y < (winpos.top + 29 + pos.bottom)) && (cursorpos.x < (winpos.left + pos.right))) {
            return true;
        }
        return false;
    }
    void Draw() {
        target->DrawText(
            text.c_str(),
            text.length()+1,
            format,
            pos,
            colortext
        );
    }
    bool Erorr() {
        if (target == NULL || format == NULL || colortext == NULL || text == L"" || (pos.left == 0 && pos.right == 0 && pos.bottom == 0 && pos.top == 0)) {
            return true;
        }
        return false;
    }
    void Release() {
        pos = { 0,0,0,0 };
        target = NULL;
        format = NULL;
        colortext = NULL;
        text = L"";
    }
};

Button play, mapedit, play2, back, _1, _2, _exitbut;
BitmapButton _upload, _download;
Text sometext, choose;

ID2D1Bitmap* downl, * downd, * uplol, * uplod;

class AdentureGame
{
public:
    AdentureGame();
    ~AdentureGame();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

private:
    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(
        UINT width,
        UINT height
    );

    HRESULT LoadResourceBitmap(
        ID2D1RenderTarget* pRenderTarget,
        IWICImagingFactory* pIWICFactory,
        PCWSTR resourceName,
        PCWSTR resourceType,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap** ppBitmap
    );

    HRESULT LoadBitmapFromFile(
        ID2D1RenderTarget* pRenderTarget,
        IWICImagingFactory* pIWICFactory,
        PCWSTR uri,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap** ppBitmap
    );


    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
private:
    HWND m_hwnd;
    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pMyBrush, * m_pMyBrush2, *m_pMyBrush3, * m_pMyBrush4,* m_pMyBrush5, *m_pMyBrush6,* m_pMyBrush7, * m_pMyBrush8, * m_pMyBrush9;
    ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
    IWICImagingFactory* m_pWICFactory;
    ID2D1SolidColorBrush* m_pBrush1, * white;

    IDWriteFactory* m_pDWriteFactory;
    IDWriteTextFormat* m_pTextFormat, *mini_text;

    ID2D1Bitmap* postava, * podlaha_kamen, * zed_nah/*nahore*/;
    ID2D1Bitmap* pos1, * pos2, * pos3, * pos4, * kamen, * kamen2;//postavy
    ID2D1Bitmap* trava, * krystal_m, * krystal_c, * krystal_z, * krystal_o, * krystal_zl;
    ID2D1Bitmap* orb_m, * orb_c, * orb_z, * orb_o, * orb_zl;//orby
    ID2D1Bitmap* peniz, * bedna, * startpos, * cihlazed;


};

AdentureGame::AdentureGame() :
    m_hwnd(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL),
    m_pMyBrush(NULL),
    m_pCornflowerBlueBrush(NULL),
    m_pBrush1(NULL),
    postava(NULL),
    m_pWICFactory(NULL),
    podlaha_kamen(NULL),
    zed_nah(NULL),
    pos1(NULL),
    pos2(NULL),
    pos3(NULL),
    pos4(NULL),
    kamen(NULL),
    trava(NULL),
    krystal_m(NULL),
    krystal_c(NULL),
    krystal_o(NULL),
    krystal_z(NULL),
    krystal_zl(NULL),
    kamen2(NULL),
    orb_m(NULL),
    orb_c(NULL),
    orb_o(NULL),
    orb_z(NULL),
    orb_zl(NULL),
    m_pDWriteFactory(NULL),
    m_pTextFormat(NULL),
    mini_text(NULL),
    peniz(NULL),
    bedna(NULL),
    m_pMyBrush2(NULL),
    m_pMyBrush3(NULL),
    m_pMyBrush4(NULL),
    m_pMyBrush5(NULL),
    m_pMyBrush6(NULL),
    m_pMyBrush7(NULL),
    m_pMyBrush8(NULL),
    m_pMyBrush9(NULL),
    startpos(NULL),
    white(NULL),

    cihlazed(NULL)
{
}


AdentureGame::~AdentureGame()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pMyBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
    SafeRelease(&m_pBrush1);
    SafeRelease(&postava);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&podlaha_kamen);
    SafeRelease(&zed_nah);
    SafeRelease(&pos1);
    SafeRelease(&pos2);
    SafeRelease(&pos3);
    SafeRelease(&pos4);
    SafeRelease(&kamen);
    SafeRelease(&trava);
    SafeRelease(&krystal_m);
    SafeRelease(&krystal_c);
    SafeRelease(&krystal_o);
    SafeRelease(&krystal_z);
    SafeRelease(&krystal_zl);
    SafeRelease(&kamen2);
    SafeRelease(&orb_m);
    SafeRelease(&orb_c);
    SafeRelease(&orb_z);
    SafeRelease(&orb_o);
    SafeRelease(&orb_zl);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&mini_text);
    SafeRelease(&peniz);
    SafeRelease(&bedna);
    SafeRelease(&m_pMyBrush2);
    SafeRelease(&m_pMyBrush3);
    SafeRelease(&m_pMyBrush4);
    SafeRelease(&m_pMyBrush5);
    SafeRelease(&m_pMyBrush6);
    SafeRelease(&m_pMyBrush7);
    SafeRelease(&m_pMyBrush8);
    SafeRelease(&m_pMyBrush9);
    SafeRelease(&startpos);
    SafeRelease(&white);
    SafeRelease(&cihlazed);
    SafeRelease(&downl);
    SafeRelease(&downd);
    SafeRelease(&uplod);
    SafeRelease(&uplol);
}


HRESULT AdentureGame::LoadResourceBitmap(
    ID2D1RenderTarget* pRenderTarget,
    IWICImagingFactory* pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap** ppBitmap
)
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void* pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                        pSource,
                        destinationWidth,
                        destinationHeight,
                        WICBitmapInterpolationModeCubic
                    );
                    if (SUCCEEDED(hr))
                    {
                        hr = pConverter->Initialize(
                            pScaler,
                            GUID_WICPixelFormat32bppPBGRA,
                            WICBitmapDitherTypeNone,
                            NULL,
                            0.f,
                            WICBitmapPaletteTypeMedianCut
                        );
                    }
                }
            }
        }
        else
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }
    }
    if (SUCCEEDED(hr))
    {
        //create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
        );
    }



    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT AdentureGame::LoadBitmapFromFile(
    ID2D1RenderTarget* pRenderTarget,
    IWICImagingFactory* pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap** ppBitmap
)
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );
    if (SUCCEEDED(hr))
    {

        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (destinationWidth != 0 || destinationHeight != 0)
        {
            UINT originalWidth, originalHeight;
            hr = pSource->GetSize(&originalWidth, &originalHeight);
            if (SUCCEEDED(hr))
            {
                if (destinationWidth == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
                    destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
                }
                else if (destinationHeight == 0)
                {
                    FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
                    destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
                }

                hr = pIWICFactory->CreateBitmapScaler(&pScaler);
                if (SUCCEEDED(hr))
                {
                    hr = pScaler->Initialize(
                        pSource,
                        destinationWidth,
                        destinationHeight,
                        WICBitmapInterpolationModeCubic
                    );
                }
                if (SUCCEEDED(hr))
                {
                    hr = pConverter->Initialize(
                        pScaler,
                        GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone,
                        NULL,
                        0.f,
                        WICBitmapPaletteTypeMedianCut
                    );
                }
            }
        }
        else // Don't scale the image.
        {
            hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
        );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);

    return hr;
}

void AdentureGame::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT AdentureGame::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = AdentureGame::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
        wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
        wcex.lpszClassName = L"D2DAdentureGame";//IDI_SMALL

        RegisterClassEx(&wcex);

        FLOAT dpiX, dpiY;

        dpiX = (FLOAT)GetDpiForWindow(GetDesktopWindow());
        dpiY = dpiX;

        // Create the window.
        m_hwnd = CreateWindow(
            L"D2DAdentureGame",
            L"Adventure Game",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(800.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(650.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
        );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            shared = m_hwnd;
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

//Manager window 

LRESULT CALLBACK ManagerProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

//creates and shows manager window
HRESULT CreateManagerWindow() {
    HRESULT hr;
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ManagerProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = HINST_THISCOMPONENT;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
    wcex.lpszClassName = L"D2DAdentureGameManager";//IDI_SMALL
    RegisterClassEx(&wcex);
    manager = CreateWindow(
        L"D2DAdentureGameManager",
        L"Adventure Game Upload-Download Map Manager",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        600,
        600,
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        nullptr
    );
    hr = manager ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        ShowWindow(manager, SW_NORMAL);
        UpdateWindow(manager);
    }
    return hr;
}
string TCHAR260ToStr(TCHAR str[20]) {
    wstring tmp(str);
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    std::string converted_str = converter.to_bytes(tmp);
    return converted_str;
}


INT_PTR CALLBACK DownloadProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
    {
        return (INT_PTR)TRUE;
    }
    case WM_DESTROY:
    {
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    }break;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_CANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == ID_DOWN) {
            TCHAR mapname[20];
            WORD ccMapname;
            ccMapname = (WORD)SendDlgItemMessage(hDlg,
                IDE_EDIT1,
                EM_LINELENGTH,
                (WPARAM)0,
                (LPARAM)0);

            if (ccMapname >= 261)
            {
                MessageBox(hDlg,
                    L"Too many characters.",
                    L"Error",
                    MB_OK | MB_ICONERROR);

                EndDialog(hDlg, TRUE);
                return FALSE;
            }
            else if (ccMapname == 0)
            {
                MessageBox(hDlg,
                    L"No characters entered.",
                    L"Error",
                    MB_OK | MB_ICONERROR);

                EndDialog(hDlg, TRUE);
                return FALSE;
            }

            // Put the number of characters into first word of buffer. 
            *((LPWORD)mapname) = ccMapname;

            // Get the characters. 
            SendDlgItemMessage(hDlg,
                IDE_EDIT1,
                EM_GETLINE,
                (WPARAM)0,       // line 0 
                (LPARAM)mapname);

            // Null-terminate the string. 
            mapname[ccMapname] = 0;

            // Call a local password-parsing function. 
            string fullname = TCHAR260ToStr(mapname);
            if (exist_read_file(fullname)) {
                mBox("Some map has same name as map name, which you want to download", "Warning", MB_OK | MB_ICONASTERISK);
            }
            if (GetMapFromServer(fullname)) {
                mBox(fullname + " is downloaded from server", "Info", MB_OK);
            }
            else {
                mBox(fullname + " do not downloaded from server", "Info", MB_OK);
            }
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK ManagerProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
    switch (message) {
    case WM_CREATE: 
    {
        //DeleteMenu(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND);
        DeleteMenu(GetSystemMenu(hWnd, FALSE), SC_MAXIMIZE, MF_BYCOMMAND);
        DrawMenuBar(hWnd);
        DragAcceptFiles(hWnd, true);
    }break;
    case WM_PAINT:
    {
        HDC dc = GetDC(hWnd);
        RECT rc = {120,200,600,600};
        HFONT hFont;
        hFont = CreateFont(30, 10, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
        SelectObject(dc, hFont);
        DrawText(dc, L"Drag & Drop here Maps to upload", -1, &rc, DT_SINGLELINE | DT_NOCLIP);
        RECT  rect;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DeleteDC(dc);
        DeleteObject(hFont);
    }break;
    case WM_DROPFILES:
    {
        HDROP hdrop =(HDROP)wParam;
        UINT  uNumFiles;
        char szNextFile[MAX_PATH];

        uNumFiles = DragQueryFileA(hdrop, -1, NULL, 0);

        if (uNumFiles > 0) {
            if (uNumFiles < 2) {
                if (DragQueryFileA(hdrop, 0, szNextFile, MAX_PATH) > 0)
                {
                    if (MessageBoxA(NULL, szNextFile, "Do you want to upload: ", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        string mapname = const_cast<const char*>(szNextFile);
                        if (UploadMapToServer(mapname)) {
                            MessageBoxA(NULL, "Map succefuly uploaded to server", "Info", MB_OK);
                        }
                        else {
                            MessageBoxA(NULL, "Map not uploaded to server", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                    else {
                        //dismised upload
                        MessageBoxA(NULL, "Uploading map dismised", "Info", MB_OK);
                    }
                }
                else {
                    //1 file, but cant query it
                    MessageBoxA(NULL, "Cant query dragged file", "Error", MB_OK | MB_ICONERROR);
                }
            }
            else {
                MessageBoxA(NULL, "You can upload only 1 file!", "Warning", MB_OK | MB_ICONASTERISK);
                // 2+ files 
            }
        }
        DragFinish(hdrop);
    }break;
    case WM_DESTROY:
        //PostQuitMessage(0);
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
)
{
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            AdentureGame app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}
HRESULT AdentureGame::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);


    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pTextFormat
        );
    }
    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            20,
            L"", //locale
            &mini_text
        );
    }
    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        mini_text->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        mini_text->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return hr;
}

HRESULT AdentureGame::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );


        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
               // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120,255),
                &m_pMyBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush2
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush3
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush4
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush5
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush6
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush7
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush8
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                // D2D1::ColorF(D2D1::ColorF::ColorF(0.26f, 0.49f, 0.94f, 1.0f)),
                RGBA(0, 78, 120, 255),
                &m_pMyBrush9
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                &white
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBrush1
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create WIC factory.
            hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_IWICImagingFactory,
                reinterpret_cast<void**>(&m_pWICFactory)
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"podlahakamen",
                L"Image",
                500,
                500,
                &podlaha_kamen
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po1",
                L"Image",
                500,
                500,
                &pos1
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po2",
                L"Image",
                500,
                500,
                &pos2
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po3",
                L"Image",
                500,
                500,
                &pos3
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"po4",
                L"Image",
                500,
                500,
                &pos4
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a bitmap by loading it from a file.
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"kamen",
                L"Image",
                500,
                500,
                &kamen
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"trava",
                L"Image",
                500,
                500,
                &trava
            );
        }

        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"mkrystal",
                L"Image",
                500,
                500,
                &krystal_m
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"ckrystal",
                L"Image",
                500,
                500,
                &krystal_c
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"okrystal",
                L"Image",
                500,
                500,
                &krystal_o
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zkrystal",
                L"Image",
                500,
                500,
                &krystal_z
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zlkrystal",
                L"Image",
                500,
                500,
                &krystal_zl
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zed2",
                L"Image",
                500,
                500,
                &kamen2
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"morb",
                L"Image",
                500,
                500,
                &orb_m
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"corb",
                L"Image",
                500,
                500,
                &orb_c
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"oorb",
                L"Image",
                500,
                500,
                &orb_o
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zorb",
                L"Image",
                500,
                500,
                &orb_z
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"zlorb",
                L"Image",
                500,
                500,
                &orb_zl
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"peniz",
                L"Image",
                500,
                500,
                &peniz
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"bedna",
                L"Image",
                500,
                500,
                &bedna
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"startpos",
                L"Image",
                500,
                500,
                &startpos
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"cihlazed",
                L"Image",
                500,
                500,
                &cihlazed
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"uplol",
                L"Image",
                500,
                500,
                &uplol
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"uplod",
                L"Image",
                500,
                500,
                &uplod
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"downl",
                L"Image",
                500,
                500,
                &downl
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"downd",
                L"Image",
                500,
                500,
                &downd
            );
        }

        sometext.SetTarget(m_pRenderTarget);
        sometext.SetColorText(m_pCornflowerBlueBrush);
        sometext.SetText(L"Adventure Game");
        sometext.SetPos(SRect2(800, 600, 200, 200));
        //sometext.CreateFormat(m_pDWriteFactory, L"Century", 30);
        sometext.SetFormat(m_pTextFormat);

        play.SetColor(m_pMyBrush);
        //play.CreateColor(D2D1::ColorF(D2D1::ColorF::GreenYellow));
        play.SetTarget(m_pRenderTarget);
        play.SetColorText(m_pBrush1);
        play.SetText(L"Fun Play");
        play.SetPos(SRect2(600, 400, 200, 200));
        //play.SetFormat(m_pTextFormat);
        play.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        mapedit.SetColor(m_pMyBrush2);
        //mapedit.CreateColor(D2D1::ColorF(D2D1::ColorF::GreenYellow));
        mapedit.SetTarget(m_pRenderTarget);
        mapedit.SetColorText(m_pBrush1);
        mapedit.SetText(L"Map Editor");
        mapedit.SetPos(SRect2(600, 700, 200, 200));
        //play.SetFormat(m_pTextFormat);
        mapedit.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        play2.SetColor(m_pMyBrush3);
        //play.CreateColor(D2D1::ColorF(D2D1::ColorF::GreenYellow));
        play2.SetTarget(m_pRenderTarget);
        play2.SetColorText(m_pBrush1);
        play2.SetText(L"Play");
        play2.SetPos(SRect2(600, 400, 200, 200));
        //play.SetFormat(m_pTextFormat);
        play2.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        //menu2
        choose.SetTarget(m_pRenderTarget);
        choose.SetColorText(m_pCornflowerBlueBrush);
        choose.SetText(L"Choose a level");
        choose.SetPos(SRect2(800, 600, 200, 200));
        choose.SetFormat(m_pTextFormat);

        back.SetColor(m_pMyBrush4);
        back.SetTarget(m_pRenderTarget);
        back.SetColorText(m_pBrush1);
        back.SetText(L"Back");
        back.SetPos(SRect2(600, 400, 200, 200));
        back.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        _1.SetColor(m_pMyBrush5);
        _1.SetTarget(m_pRenderTarget);
        _1.SetColorText(m_pBrush1);
        _1.SetText(L"Level 1");
        _1.SetPos(SRect2(600, 400, 200, 200));
        _1.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        _2.SetColor(m_pMyBrush6);
        _2.SetTarget(m_pRenderTarget);
        _2.SetColorText(m_pBrush1);
        _2.SetText(L"Level 2");
        _2.SetPos(SRect2(600, 400, 200, 200));
        _2.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        _exitbut.SetColor(m_pMyBrush7);
        _exitbut.SetTarget(m_pRenderTarget);
        _exitbut.SetColorText(m_pBrush1);
        _exitbut.SetText(L"Exit");
        _exitbut.SetPos(SRect2(600, 400, 200, 200));
        _exitbut.CreateFormat(m_pDWriteFactory, L"Century", 30, true);

        _download.SetTarget(m_pRenderTarget);
        _download.SetPos(SRect2(600, 400, 200, 100));
        _download.SetBitmap(downd);

        _upload.SetTarget(m_pRenderTarget);
        _upload.SetPos(SRect2(600, 500, 200, 100));
        _upload.SetBitmap(uplod);

    }

    return hr;
}

void AdentureGame::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pMyBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}

LRESULT CALLBACK AdentureGame::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        AdentureGame* pAdentureGame = (AdentureGame*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pAdentureGame)
        );
        //hide title bar
       // SetWindowLong(hwnd, GWL_STYLE, 0); 

        //load if command line argument: Game1 path *map*
        //*map* replace with map full path
        string open_file_name = "";
        LPWSTR* szArglist;
        int nArgs;

        szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        if (NULL == szArglist)
        {
            MessageBox(NULL, L"CommandLineToArgvW failed\n", L"info", MB_OK);
        }
        else {
            if (szArglist[1] != L"path") {
                open_file_name = "";
            }
            else {
                wstring ws(szArglist[2]);
                string tmp_name = string(ws.begin(), ws.end());
                open_file_name = tmp_name;
            }
        }
        LocalFree(szArglist);

        if (open_file_name != "") {
            if (!LoadMap(open_file_name)) {
                string mes = open_file_name + " neexistuje nebo nelze otevrit";
                MessageBoxA(NULL, mes.c_str(), "Error", MB_OK | MB_ICONERROR);
            }
            else {
                adventure_game = false;
                menu = false;
                menu2 = false;
                SendMessage(hwnd, ID_PLAY, 0, 0);
            }
        }

        result = 1;
        SendMessage(hwnd, WM_PAINT, 0, 0);
    }
    else
    {
        AdentureGame* pAdentureGame = reinterpret_cast<AdentureGame*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pAdentureGame)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pAdentureGame->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_RBUTTONDOWN:
            {
                RECT pos;
                POINT cursor;
                pos.left = 0;
                pos.bottom = 0;
                pos.right = 0;
                pos.top = 0;
                cursor.x = 0;
                cursor.y = 0;

                GetCursorPos(&cursor);
                GetWindowRect(shared, &pos);
                if (menu) {
                    HMENU hPopupMenu = CreatePopupMenu();
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_PLAY, L"Run adventure game");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_PLAY2, L"Run adventure game fun");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_MAPEDITOR, L"Run map editor");
                    TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
                }
                if (!adventure_game) {
                    _cursor.y = cursor.y;
                    _cursor.x = cursor.x;
                    HMENU hPopupMenu = CreatePopupMenu();
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETTRAVA, L"Nastavit na travu");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETKAMEN, L"Nastavit na kamen-podlaha");
                    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_MENUBREAK, 0, L"");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZED, L"Nastavit na kamena-zed");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETBEDNA, L"Nastavit na bednu");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZEDO, L"Nastavit na ornament kamen-zed");
                    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_MENUBREAK, 0, L"");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SMAZANI, L"Smazani podlahy a zdi");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SMAZANI_P, L"Smazani predmetu");
                    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_MENUBREAK, 0, L"");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETCKRYSTAL, L"Nastavit na cerveny krystal");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETMKRYSTAL, L"Nastavit na modry krystal");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZKRYSTAL, L"Nastavit na zeleny krystal");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETOKRYSTAL, L"Nastavit na oranzovy krystal");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZLKRYSTAL, L"Nastavit na zluty krystal");
                    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_MENUBREAK, 0, L"");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETCORB, L"Nastavit na cerveny orb");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETMORB, L"Nastavit na modry orb");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZORB, L"Nastavit na zeleny orb");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETOORB, L"Nastavit na oranzovy orb");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETZLORB, L"Nastavit na zluty orb");
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETPENIZ, L"Nastavit na peniz");

                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SETSTART, L"Nastavit na startpos");

                    TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor.x, cursor.y, 0, hwnd, NULL);
                }
            }break;
            case WM_MOUSEMOVE:
            {
                POINT cursor;
                cursor.x = 0;
                cursor.y = 0;
                //                RGBA(0, 78, 120, 255),
                GetCursorPos(&cursor);
                if (menu) {
                    if (play.HitTest(cursor, GetWindowPos())) {
                        play.SetColor2(RGBA(131, 203, 242, 255));
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (mapedit.HitTest(cursor, GetWindowPos())) {
                        mapedit.SetColor2(RGBA(131, 203, 242, 255));
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (play2.HitTest(cursor, GetWindowPos())) {
                        play2.SetColor2(RGBA(131, 203, 242, 255));
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (_exitbut.HitTest(cursor, GetWindowPos())) {
                        _exitbut.SetColor2(RGBA(131, 203, 242, 255));
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (_download.HitTest(cursor, GetWindowPos())) {
                        _download.SetBitmap(downl);
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (_upload.HitTest(cursor, GetWindowPos())) {
                        _upload.SetBitmap(uplol);
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else {
                        play.SetColor2(RGBA(0, 78, 120, 255));
                        play2.SetColor2(RGBA(0, 78, 120, 255));
                        mapedit.SetColor2(RGBA(0, 78, 120, 255));
                        _exitbut.SetColor2(RGBA(0, 78, 120, 255));
                        _upload.SetBitmap(uplod);
                        _download.SetBitmap(downd);
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
                if (menu2) {
                    if (back.HitTest(cursor, GetWindowPos())) {
                        back.SetColor2(RGBA(131, 203, 242, 255));
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (_1.HitTest(cursor, GetWindowPos())) {
                        _1.SetColor2(RGBA(131, 203, 242, 255));
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if (_2.HitTest(cursor, GetWindowPos())) {
                        _2.SetColor2(RGBA(131, 203, 242, 255));
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else{
                        _1.SetColor2(RGBA(0, 78, 120, 255));
                        _2.SetColor2(RGBA(0, 78, 120, 255));
                        back.SetColor2(RGBA(0, 78, 120, 255));
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
            }break;
            case WM_LBUTTONDOWN:
            {

                if (menu) { 
                    POINT cursor;
                    cursor.x = 0;
                    cursor.y = 0;

                    GetCursorPos(&cursor);
                    if (play.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_COMMAND, ID_PLAY, 0);
                    }
                    else if (mapedit.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_COMMAND, ID_MAPEDITOR, 0);
                    }
                    else if (play2.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_COMMAND, ID_MENU2, 0);
                    }
                    else if (_exitbut.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_DESTROY, 0, 0);
                    }
                    else if (_upload.HitTest(cursor, GetWindowPos())) {
                        CreateManagerWindow();
                    }
                    else if (_download.HitTest(cursor, GetWindowPos())) {
                        DialogBox(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_DOWNMAP), NULL, DownloadProc);
                    }
                }
                else if (menu2) {
                    POINT cursor;
                    cursor.x = 0;
                    cursor.y = 0;

                    GetCursorPos(&cursor);
                    if (_1.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_COMMAND, ID_PLAY2, 1);
                    }
                    else if (_2.HitTest(cursor, GetWindowPos())) {
                        SendMessage(hwnd, WM_COMMAND, ID_PLAY2, 2);
                    }
                    else if (back.HitTest(cursor, GetWindowPos())) {
                        menu2 = false;
                        menu = true;
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
                if (!adventure_game) {
                    //po kliknti - pridani
                    RECT pos;
                    POINT cursor;
                    pos.left = 0;
                    pos.bottom = 0;
                    pos.right = 0;
                    pos.top = 0;
                    cursor.x = 0;
                    cursor.y = 0;

                    GetCursorPos(&cursor);
                    GetWindowRect(shared, &pos);

                    if (use_it) {
                        use_it = false;
                        cursor.x = _cursor.x;
                        cursor.y = _cursor.y;
                    }
                    if (cursor.y > pos.top && cursor.x > pos.left && cursor.y < pos.bottom + 40 && cursor.x < pos.right) {
                        //je v okne
                        cursor.y = cursor.y - pos.top - 40;
                        cursor.x = cursor.x - pos.left;
                        float _y = cursor.y / 80;
                        float _x = cursor.x / 80;
                        int _xx = round(_x);
                        int _yy = round(_y);
                        if ((mode != 1) && (mode != 2) && (zed[_yy][_xx] == typ_zdi::start_pos)) {
                            ::startpos = false;
                        }
                        if (mode == 1) {
                            //zed[_yy][_xx] = typ_zdi::nic__;
                            podlaha[_yy][_xx] = typ_podlahy::trava;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 2) {
                            //zed[_yy][_xx] = typ_zdi::nic__;
                            podlaha[_yy][_xx] = typ_podlahy::kamen;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 3) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            podlaha[_yy][_xx] = typ_podlahy::nic;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 4) {
                            zed[_yy][_xx] = typ_zdi::kamen_;
                            //podlaha[_yy][_xx] = typ_podlahy::nic;
                            predmety[_yy][_xx] = typ_predmetu::nic_;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 5) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::krystal_m;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 6) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::krystal_c;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 7) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::nic_;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 8) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::krystal_z;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 9) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::krystal_o;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 10) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::krystal_zl;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 11) {
                            zed[_yy][_xx] = typ_zdi::kamen_o;
                            predmety[_yy][_xx] = typ_predmetu::nic_;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 12) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::orb_m;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 13) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::orb_c;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 14) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::orb_o;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 15) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::orb_z;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 16) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::orb_zl;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 17) {
                            zed[_yy][_xx] = typ_zdi::nic__;
                            predmety[_yy][_xx] = typ_predmetu::peniz;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 18) {
                            zed[_yy][_xx] = typ_zdi::bedna;
                            predmety[_yy][_xx] = typ_predmetu::nic_;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if (mode == 19) {
                            if (!::startpos) {
                                ::startpos = true;
                                zed[_yy][_xx] = typ_zdi::start_pos;
                                predmety[_yy][_xx] = typ_predmetu::nic_;
                                SendMessage(hwnd, WM_PAINT, 0, 0);
                            }
                            else {
                                COORD i = find_startpos();
                                zed[i.Y][i.X] = typ_zdi::nic__;
                                ::startpos = true;
                                zed[_yy][_xx] = typ_zdi::start_pos;
                                SendMessage(hwnd, WM_PAINT, 0, 0);
                            }
                        }
                    }
                }
            }break;
            case WM_KEYDOWN:
            {
                if (menu) {
                    if (GetAsyncKeyState(0x74) & 0x8000) {
                        //f5 pressed
                        DialogBox(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_DOWNMAP), NULL, DownloadProc);
                    }
                    else if (GetAsyncKeyState(0x76) & 0x8000) {
                        //f7
                        if (darkmode) {
                            darkmode = false;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else {
                            darkmode = true;
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                    }
                    else if (GetAsyncKeyState(0x77) & 0x8000) {
                        CreateManagerWindow();
                    }
                    else if (GetAsyncKeyState(0x75) & 0x8000) {
                        //f6
                        OPENFILENAMEA ofn;
                        char fileName[MAX_PATH] = "";
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(OPENFILENAME);
                        ofn.hwndOwner = shared;
                        ofn.lpstrFilter = "Map files(*.json)\0*.json";
                        ofn.lpstrFile = fileName;
                        ofn.nMaxFile = MAX_PATH;
                        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                        ofn.lpstrDefExt = "";

                        string fileNameStr;
                        if (GetOpenFileNameA(&ofn)) {
                            fileNameStr = fileName;
                            fileNameStr = ofn.lpstrFile;

                        }
                        if (fileNameStr != "") {
                            string mapname = fileNameStr;
                            mapname = base_name(mapname);
                            if (UploadMapToServer(mapname)) {
                                MessageBox(NULL, L"map succefully uploaded to server", L"Info", MB_OK);
                            }
                            else {
                                MessageBox(NULL, L"map do not uploaded to server - some error occured", L"Error", MB_OK);
                            }
                        }
                    }
                }
                else if (menu2) {
                    if (GetAsyncKeyState(0x1B) & 0x8000) {
                        //esc end
                        menu2 = false;
                        menu = true;
                        ::startpos = false;
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
                if (adventure_game) {
                    if (GetAsyncKeyState(0x1B) & 0x8000) {
                        //esc end
                        menu2 = false;
                        menu = true;
                        adventure_game = false;
                        ::startpos = false;
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    if (GetAsyncKeyState(0x70) & 0x8000) {
                        //key f1
                        std::wstring _1 = L"x: " + std::to_wstring(x) + L" y: " + std::to_wstring(y);
                        MessageBox(NULL, _1.c_str(), _1.c_str(), MB_OK | MB_ICONINFORMATION);
                    }
                    else if ((GetAsyncKeyState(0x27) & 0x8000) || (GetAsyncKeyState(0x44) & 0x8000)) {
                        //key-right pressed | key d 
                        if ((zed[y][x + 1] == typ_zdi::bedna) && (zed[y][x + 2] != typ_zdi::kamen_) && (zed[y][x + 2] != typ_zdi::kamen_o)) {
                            zed[y][x + 1] = typ_zdi::nic__;
                            podlaha[y][x + 1] = typ_podlahy::trava;
                            zed[y][x + 2] = typ_zdi::bedna;
                            x++;
                            mom_postava = 3;
                            KontrolaKrystalu();
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if ((zed[y][x + 1] != typ_zdi::kamen_) && (zed[y][x + 1] != typ_zdi::kamen_o) && (zed[y][x+1] != typ_zdi::bedna)) {
                            x++;
                            mom_postava = 3;
                        }
                        KontrolaKrystalu();
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if ((GetAsyncKeyState(0x25) & 0x8000) || (GetAsyncKeyState(0x41) & 0x8000)) {
                        //key-left pressed | key a
                        if ((zed[y][x - 1] == typ_zdi::bedna) && (zed[y][x - 2] != typ_zdi::kamen_) && (zed[y][x - 2] != typ_zdi::kamen_o)) {
                            zed[y][x - 1] = typ_zdi::nic__;
                            podlaha[y][x - 1] = typ_podlahy::trava;
                            zed[y][x - 2] = typ_zdi::bedna;
                            x--;
                            mom_postava = 4;
                            KontrolaKrystalu();
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if ((zed[y][x - 1] != typ_zdi::kamen_) && (zed[y][x - 1] != typ_zdi::kamen_o) && (zed[y][x-1] != typ_zdi::bedna)) {
                            x--;
                            mom_postava = 4;
                        }
                        KontrolaKrystalu();
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if ((GetAsyncKeyState(0x28) & 0x8000) || (GetAsyncKeyState(0x53) & 0x8000)) {
                        //key-down pressed | key s 
                        if ((zed[y+1][x] == typ_zdi::bedna) && (zed[y+2][x] != typ_zdi::kamen_) && (zed[y+2][x] != typ_zdi::kamen_o)) {
                            zed[y+1][x] = typ_zdi::nic__;
                            podlaha[y+1][x] = typ_podlahy::trava;
                            zed[y+2][x] = typ_zdi::bedna;
                            y++;
                            mom_postava = 1;
                            KontrolaKrystalu();
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if ((zed[y + 1][x] != typ_zdi::kamen_) && (zed[y + 1][x] != typ_zdi::kamen_o) && (zed[y + 1][x] != typ_zdi::bedna)) {
                            y++;
                            mom_postava = 1;
                        }
                        KontrolaKrystalu();
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    else if ((GetAsyncKeyState(0x26) & 0x8000) || (GetAsyncKeyState(0x57) & 0x8000)) {
                        //key-up pressed | key w 
                        if ((zed[y - 1][x] == typ_zdi::bedna) && (zed[y - 2][x] != typ_zdi::kamen_) && (zed[y - 2][x] != typ_zdi::kamen_o)) {
                            zed[y - 1][x] = typ_zdi::nic__;
                            podlaha[y - 1][x] = typ_podlahy::trava;
                            zed[y - 2][x] = typ_zdi::bedna;
                            y--;
                            mom_postava = 2;
                            KontrolaKrystalu();
                            SendMessage(hwnd, WM_PAINT, 0, 0);
                        }
                        else if ((zed[y - 1][x] != typ_zdi::kamen_) && (zed[y - 1][x] != typ_zdi::kamen_o) && (zed[y - 1][x] != typ_zdi::bedna)) {
                            y--;
                            mom_postava = 2;
                        }
                        KontrolaKrystalu();
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }

                    else if (GetAsyncKeyState(VK_F2) & 0x8000) {
                        //key f2
                        string str = "Pocet sebranych krystalu - modre: " + to_string(sebr_mk) + ", cervene: " + to_string(sebr_ck) + ", zelene: " + to_string(sebr_zk) + ", oranzove: " + to_string(sebr_ok) + ", zlute: " + to_string(sebr_zlk) + "\n" + "Pocet sebranych orbu - modre: " + to_string(sebr_mo) + ", cervene: " + to_string(sebr_co) + ", zelene: " + to_string(sebr_zo) + ", oranzove: " + to_string(sebr_oo) + ", zlute: " + to_string(sebr_zlo);
                        MessageBoxA(NULL, str.c_str(), "Info", MB_OK | MB_ICONINFORMATION);
                    }
                }
                else {
                    if (GetAsyncKeyState(0x1B) & 0x8000) {
                        //esc end
                        menu2 = false;
                        menu = true;
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                    if (GetAsyncKeyState(0x54) & 0x8000) {
                        //t pressed - pridani travy
                        mode = 1;
                    }
                    else  if (GetAsyncKeyState(0x4B) & 0x8000) {
                        //k pressed - pridani kamene
                        mode = 2;
                    }
                    else  if (GetAsyncKeyState(0x47) & 0x8000) {
                        //g pressed - umazani
                        mode = 3;
                    }
                    else  if (GetAsyncKeyState(0x5A) & 0x8000) {
                        //z pressed - pridani zdi
                        mode = 4;
                    }
                    else  if (GetAsyncKeyState(0x4D) & 0x8000) {
                        //m pressed - pridani modreho krystalu
                        mode = 6;
                    }
                    else  if (GetAsyncKeyState(0x43) & 0x8000) {
                        //c pressed - pridani cerveneho krystalu
                        mode = 5;
                    }
                    else  if (GetAsyncKeyState(0x51) & 0x8000) {
                        //q pressed - smazani krystalu
                        mode = 7;
                    }
                    else  if (GetAsyncKeyState(0x4C) & 0x8000) {
                        //l pressed - pridani zeleneho krystalu
                        mode = 8;
                    }
                    else  if (GetAsyncKeyState(0x52) & 0x8000) {
                        //r pressed - pridani oranzoveho krystalu
                        mode = 9;
                    }
                    else  if (GetAsyncKeyState(0x44) & 0x8000) {
                        //d pressed - pridani zluteho krystalu
                        mode = 10;
                    }
                    else if (GetAsyncKeyState(0x41) & 0x8000) {
                        //a pressed - pridani kamenu s ornamentem
                        mode = 11;
                    }
                    else if (GetAsyncKeyState(0x30) & 0x8000) {
                        //0 pressed - pridani moderho orbu
                        mode = 12;
                    }
                    else if (GetAsyncKeyState(0x31) & 0x8000) {
                        //1 pressed - pridani cerveneho orbu
                        mode = 13;
                    }
                    else if (GetAsyncKeyState(0x32) & 0x8000) {
                        //2 pressed - pridani zeleneho orbu
                        mode = 15;
                    }
                    else if (GetAsyncKeyState(0x33) & 0x8000) {
                        //3 pressed - pridani oranzoveho orbu
                        mode = 14;
                    }
                    else if (GetAsyncKeyState(0x34) & 0x8000) {
                        //4 pressed - pridani zluteho orbu
                        mode = 16;
                    }
                    else if (GetAsyncKeyState(0x35) & 0x8000) {
                        //5 pressed - pridani penizu
                        mode = 17;
                    }
                    else if (GetAsyncKeyState(0x36) & 0x8000) {
                        //6 pressed - pridani bednu
                        mode = 18;
                    }
                    else if (GetAsyncKeyState(0x72) & 0x8000) {
                        //f3
                        DialogBox(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_EDITCHOOSE), hwnd, EditChoose);
                    }
                    else if (GetAsyncKeyState(0x70) & 0x8000) {
                        //key f1
                        std::wstring _1 = to_wstring(mode);
                        MessageBox(NULL, _1.c_str(), _1.c_str(), MB_OK | MB_ICONINFORMATION);
                    }
                    else  if (GetAsyncKeyState(0x53) & 0x8000) {
                        //s pressed - ulozeni
                        OPENFILENAMEA ofn2;
                        char fileName[MAX_PATH] = "";
                        ZeroMemory(&ofn2, sizeof(ofn2));
                        ofn2.lStructSize = sizeof(OPENFILENAME);
                        ofn2.hwndOwner = shared;
                        ofn2.lpstrFilter = "Map files(*.json)\0*.json";
                        ofn2.lpstrFile = fileName;
                        ofn2.nMaxFile = MAX_PATH;
                        ofn2.Flags = OFN_EXPLORER;
                        ofn2.lpstrDefExt = "";
                        

                        string fileNameStr;
                        if (GetSaveFileNameA(&ofn2))
                            fileNameStr = fileName;
                        if (fileNameStr != "") {
                            COORD i = find_startpos();//startpos,
                            SaveMap(fileNameStr, i.X, i.Y);
                        }
                    }
                }
                if (GetAsyncKeyState(0x4F) & 0x8000) {
                    //o pressed - open mapa
                    OPENFILENAMEA ofn;
                    char fileName[MAX_PATH] = "";
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = shared;
                    ofn.lpstrFilter = "Map files(*.json)\0*.json";
                    ofn.lpstrFile = fileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = "";

                    string fileNameStr;
                    if (GetOpenFileNameA(&ofn)) {
                        fileNameStr = fileName;
                    }
                    if (fileNameStr != "") {
                        if (LoadMap(fileNameStr) > 0) {
                            string mes = fileNameStr + " nelze otevrit";
                            MessageBoxA(NULL, mes.c_str(), "Error", MB_OK | MB_ICONERROR);
                        }
                        SendMessage(hwnd, WM_PAINT, 0, 0);
                    }
                }
            }break;
            case WM_COMMAND:
            {
                POINT cursor;
                cursor.x = 0;
                cursor.y = 0;
                GetCursorPos(&cursor);
                int i = LOWORD(wParam);
                switch (i) {
                case ID_PLAY:
                {
                    ShowWindow(shared, SW_MAXIMIZE);
                    adventure_game = true;
                    menu = false;
                    //LoadMap("lets_go.txt");
                    if (!loaded) {
                        LoadMap("lets_go.json");
                    }
                    SetWindowTextA(hwnd, "Adventure Game - Fun Play");
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }break;

                case ID_PLAY2:
                {
                    ShowWindow(shared, SW_MAXIMIZE);
                    adventure_game = true;
                    menu = false;
                    menu2 = false;
                    LoadLevel(LOWORD(lParam));
                    SetWindowTextA(hwnd, "Adventure Game - Play");
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }break;
                case ID_MENU2:
                {
                    //ShowWindow(shared, SW_MAXIMIZE);
                    adventure_game = true;
                    menu = false;
                    menu2 = true;
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }break;
                case ID_MAPEDITOR:
                {
                    ShowWindow(shared, SW_MAXIMIZE);
                    HICON hIcon = LoadIcon(GetModuleHandle(NULL), (MAKEINTRESOURCE(IDI_MAPMAKER)));
                    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    adventure_game = false;
                    menu = false;
                    LoadMap("lets_go.json");
                    SetWindowTextA(hwnd, "Adventure Game - Map Editor");
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                }break;
                case ID_SETTRAVA:
                {
                    mode = 1;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETKAMEN:
                {
                    mode = 2;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZED:
                {
                    mode = 4;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZEDO:
                {
                    mode = 11;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETBEDNA:
                {
                    mode = 18;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SMAZANI:
                {
                    mode = 3;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SMAZANI_P:
                {
                    mode = 7;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETPENIZ:
                {
                    mode = 17;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETCKRYSTAL:
                {
                    mode = 5;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETMKRYSTAL:
                {
                    mode = 6;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETOKRYSTAL:
                {
                    mode = 9;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZLKRYSTAL:
                {
                    mode = 10;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZKRYSTAL:
                {
                    mode = 8;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETCORB:
                {
                    mode = 13;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETMORB:
                {
                    mode = 12;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETOORB:
                {
                    mode = 14;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZLORB:
                {
                    mode = 15;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETZORB:
                {
                    mode = 16;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                case ID_SETSTART:
                {
                    mode = 19;
                    use_it = true;
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, 0);
                }break;
                };
            }break;
            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
               
                pAdentureGame->OnRender();
                ValidateRect(hwnd, NULL);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}


HRESULT AdentureGame::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        if (darkmode) {
            m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        }
        else {
            m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        }

        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
        // Draw a grid background.
        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);
        

        if(menu){

            play2.SetPos(SRect2(((width - 150) / 2), ((height - 100) / 2)-50, 150, 100));
            play.SetPos(SRect2(((width - 150) / 2), ((height - 100) / 2)+75, 150, 100));
            mapedit.SetPos(SRect2(((width - 150) / 2), ((height - 100) / 2)+200, 150, 100));
            sometext.SetPos(SRect2(((width - 600) / 2), ((height - 200) / 2) - 200, 600, 200));
            _exitbut.SetPos(SRect2(((width - 150) / 2) + 250, ((height - 100) / 2) + 200, 150, 100));
            _download.SetPos(SRect2(((width - 150) / 2) - 300, ((height - 100) / 2) + 230, 150, 100));
            _upload.SetPos(SRect2(((width - 150) / 2) - 300, ((height - 100) / 2) + 105, 150, 100));

            mapedit.Draw();
            play.Draw();
            sometext.Draw();
            play2.Draw();
            _exitbut.Draw();
            _download.Draw();
            _upload.Draw();
        }
        else if (menu2) {
            choose.SetPos(SRect2(((width - 600) / 2), ((height - 200) / 2) - 200, 600, 200));
            _1.SetPos(SRect2(((width - 150) / 2), ((height - 100) / 2) - 50, 150, 100));
            _2.SetPos(SRect2(((width - 150) / 2), ((height - 100) / 2) + 75, 150, 100));

            back.SetPos(SRect2(((width - 150) / 2)-200, ((height - 100) / 2) + 150, 150, 100));

            choose.Draw();
            _1.Draw();
            _2.Draw();
            back.Draw();
        }
        else {
            // Dark mode :
            if (!adventure_game) {
                if (darkmode) {
                    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
                    for (int x = 0; x < width; x += 80)
                    {
                        m_pRenderTarget->DrawLine(
                            D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                            D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                            white,
                            0.5f
                        );
                    }

                    for (int y = 0; y < height; y += 80)
                    {
                        m_pRenderTarget->DrawLine(
                            D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                            D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                            white,
                            0.5f
                        );
                    }
                }
            }
#ifndef POSOUVANI_KAMERY

            for (int _x = 0; _x < 17; _x++) {
                for (int _y = 0; _y < 10; _y++) {

                    if (podlaha[_y][_x] == typ_podlahy::trava) {
                        m_pRenderTarget->DrawBitmap(trava, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (podlaha[_y][_x] == typ_podlahy::kamen) {
                        m_pRenderTarget->DrawBitmap(podlaha_kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }

                    //musi byt zvlast jinak by tam bylo na sobe - nema pozadi obrazek
                    if (zed[_y][_x] == typ_zdi::kamen_o) {
                        m_pRenderTarget->DrawBitmap(kamen2, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (zed[_y][_x] == typ_zdi::kamen_) {
                        m_pRenderTarget->DrawBitmap(kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (zed[_y][_x] == typ_zdi::bedna) {
                        m_pRenderTarget->DrawBitmap(bedna, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    if (adventure_game == false) {
                        if (zed[_y][_x] == typ_zdi::start_pos) {
                            m_pRenderTarget->DrawBitmap(startpos, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }
                    }

                    if (predmety[_y][_x] == typ_predmetu::krystal_m) {
                        m_pRenderTarget->DrawBitmap(krystal_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::krystal_c) {
                        m_pRenderTarget->DrawBitmap(krystal_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::krystal_z) {
                        m_pRenderTarget->DrawBitmap(krystal_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::krystal_o) {
                        m_pRenderTarget->DrawBitmap(krystal_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::orb_m) {
                        m_pRenderTarget->DrawBitmap(orb_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::orb_c) {
                        m_pRenderTarget->DrawBitmap(orb_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::orb_zl) {
                        m_pRenderTarget->DrawBitmap(orb_zl, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::orb_z) {
                        m_pRenderTarget->DrawBitmap(orb_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::orb_o) {
                        m_pRenderTarget->DrawBitmap(orb_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y][_x] == typ_predmetu::peniz) {
                        m_pRenderTarget->DrawBitmap(peniz, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else {

                    }
                }
            }

            D2D1_RECT_F postava_pos = D2D1::RectF(x * 80, y * 80, (x * 80) + 80, (y * 80) + 80);

            if (adventure_game) {
                if (mom_postava == 1) {
                    m_pRenderTarget->DrawBitmap(pos1, postava_pos, 1.0);
                }
                else if (mom_postava == 2) {
                    m_pRenderTarget->DrawBitmap(pos2, postava_pos, 1.0);
                }
                else if (mom_postava == 3) {
                    m_pRenderTarget->DrawBitmap(pos3, postava_pos, 1.0);
                }
                else if (mom_postava == 4) {
                    m_pRenderTarget->DrawBitmap(pos4, postava_pos, 1.0);
                }
            }
#else
            //pocet zobrazenych ctvercu - pro vetsi rychlost
            int zobraz_sirka = round(width / 80) + 1;
            int zobraz_vyska = round(height / 80) + 1;

            int hrac_x = round(((width - 80) / 2) / 80) * 80;
            int hrac_y = round(((height - 80) / 2) / 80) * 80;

            int hrac_x2 = (hrac_x / 80) - x;
            int hrac_y2 = (hrac_y / 80) - y;

            for (int _x = 0; _x < 17; _x++) {
                for (int _y = 0; _y < 10; _y++) {
                    if (zed[_y + y - 4][_x + x - 7] == true) {
                        m_pRenderTarget->DrawBitmap(kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (podlaha[_y + y - 4][_x + x - 7] == typ_podlahy::trava) {
                        m_pRenderTarget->DrawBitmap(trava, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (podlaha[_y + y - 4][_x + x - 7] == typ_podlahy::kamen) {
                        m_pRenderTarget->DrawBitmap(podlaha_kamen, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }

                    if (predmety[_y + y - 4][_x + x - 7] == typ_predmetu::krystal_m) {
                        m_pRenderTarget->DrawBitmap(krystal_m, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y + y - 4][_x + x - 7] == typ_predmetu::krystal_c) {
                        m_pRenderTarget->DrawBitmap(krystal_c, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y + y - 4][_x + x - 7] == typ_predmetu::krystal_z) {
                        m_pRenderTarget->DrawBitmap(krystal_z, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y - y][_x + x - 7] == typ_predmetu::krystal_o) {
                        m_pRenderTarget->DrawBitmap(krystal_o, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else if (predmety[_y - y][_x + x - 7] == typ_predmetu::krystal_zl) {
                        m_pRenderTarget->DrawBitmap(krystal_zl, SRect(_x * 80, _y * 80, (_x * 80) + 80, (_y * 80) + 80), 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                    else {

                    }
                }
            }
            D2D1_RECT_F postava_pos = SRect(hrac_x, hrac_y, hrac_x + 80, hrac_y + 80);
#endif
        }
        hr = m_pRenderTarget->EndDraw();
    }
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

void AdentureGame::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}