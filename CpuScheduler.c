#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// UI Control IDs
#define ID_BTN_RUN 101
#define ID_BTN_RESET 102
#define ID_BTN_CLEAR 103
#define ID_BTN_EXIT 104
#define ID_COMBO 105

// Global UI Handles
HWND hEditN, hEditAT, hEditBT, hCombo, hTable, hGantt, hStats;

// Process Structure
typedef struct {
    char name[10];
    int burstTime;
    int arrivalTime;
    int waitingTime;
    int turnAroundTime;
    int remainingTime;
    int cT;
} Process;

// Algos

void FCFS(Process* p, int n, char* ganttOut) {
    int time = 0;
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrivalTime) {
            time = p[i].arrivalTime; 
        }
        
        
        for(int j = 0; j < p[i].burstTime; j++) {
            char temp[20];
            sprintf(temp, "%s | ", p[i].name);
            strcat(ganttOut, temp);
        }
        
        time += p[i].burstTime;
        p[i].cT = time;
        p[i].turnAroundTime = p[i].cT - p[i].arrivalTime;
        p[i].waitingTime = p[i].turnAroundTime - p[i].burstTime;
        if(p[i].waitingTime < 0) p[i].waitingTime = 0;
    }
}

void SJF(Process* p, int n, char* ganttOut) {
    int completed = 0, time = 0;
    bool* isDone = (bool*)calloc(n, sizeof(bool));

    while (completed != n) {
        int shortest = -1;
        int minm = 9999;

        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= time && !isDone[i] && p[i].burstTime < minm) {
                minm = p[i].burstTime;
                shortest = i;
            }
        }

        
        if (shortest == -1) {
            time++;
            continue;
        }

        for(int j = 0; j < p[shortest].burstTime; j++) {
            char temp[20];
            sprintf(temp, "%s | ", p[shortest].name);
            strcat(ganttOut, temp);
        }

        time += p[shortest].burstTime;
        p[shortest].cT = time;
        p[shortest].turnAroundTime = p[shortest].cT - p[shortest].arrivalTime;
        p[shortest].waitingTime = p[shortest].turnAroundTime - p[shortest].burstTime;
        
        isDone[shortest] = true;
        completed++;
    }
    free(isDone);
}

void SRTF(Process* p, int n, char* ganttOut) {
    int completed = 0, time = 0, minm = 9999, shortest = -1;
    bool check = false;

    while (completed != n) {
        shortest = -1;
        minm = 9999;
        check = false;

        for (int i = 0; i < n; i++) {
            if (p[i].arrivalTime <= time && p[i].remainingTime > 0 && p[i].remainingTime < minm) {
                minm = p[i].remainingTime;
                shortest = i;
                check = true;
            }
        }

       
        if (!check) {
            time++;
            continue;
        }

        char temp[20];
        sprintf(temp, "%s | ", p[shortest].name);
        strcat(ganttOut, temp);

        p[shortest].remainingTime--;

        if (p[shortest].remainingTime == 0) {
            completed++;
            p[shortest].cT = time + 1;
            p[shortest].turnAroundTime = p[shortest].cT - p[shortest].arrivalTime;
            p[shortest].waitingTime = p[shortest].turnAroundTime - p[shortest].burstTime;
            if (p[shortest].waitingTime < 0) p[shortest].waitingTime = 0;
        }
        time++;
    }
}

//MAIN WINDOW 

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Setup Input Labels & Boxes
            CreateWindow("STATIC", "Processes:", WS_VISIBLE | WS_CHILD, 20, 20, 80, 25, hwnd, NULL, NULL, NULL);
            hEditN = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD | ES_NUMBER, 120, 20, 150, 25, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Arrival:", WS_VISIBLE | WS_CHILD, 20, 60, 80, 25, hwnd, NULL, NULL, NULL);
            hEditAT = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD, 120, 60, 300, 25, hwnd, NULL, NULL, NULL);

            CreateWindow("STATIC", "Burst:", WS_VISIBLE | WS_CHILD, 20, 100, 80, 25, hwnd, NULL, NULL, NULL);
            hEditBT = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_VISIBLE | WS_CHILD, 120, 100, 300, 25, hwnd, NULL, NULL, NULL);

            // Algorithm Dropdown
            CreateWindow("STATIC", "Algorithm:", WS_VISIBLE | WS_CHILD, 20, 145, 80, 25, hwnd, NULL, NULL, NULL);
            hCombo = CreateWindow("COMBOBOX", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 100, 140, 120, 100, hwnd, (HMENU)ID_COMBO, NULL, NULL);
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"FCFS");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"SJF");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"SRTF");
            SendMessage(hCombo, CB_SETCURSEL, 2, 0);

            // The 4 Action Buttons 
            CreateWindow("BUTTON", "RUN", WS_VISIBLE | WS_CHILD, 240, 138, 60, 30, hwnd, (HMENU)ID_BTN_RUN, NULL, NULL);
            CreateWindow("BUTTON", "RESET", WS_VISIBLE | WS_CHILD, 310, 138, 60, 30, hwnd, (HMENU)ID_BTN_RESET, NULL, NULL);
            CreateWindow("BUTTON", "CLEAR", WS_VISIBLE | WS_CHILD, 380, 138, 60, 30, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
            CreateWindow("BUTTON", "EXIT", WS_VISIBLE | WS_CHILD, 450, 138, 60, 30, hwnd, (HMENU)ID_BTN_EXIT, NULL, NULL);

            // Output Areas 
            hTable = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "TABLE OUTPUT", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY | WS_VSCROLL, 20, 180, 490, 150, hwnd, NULL, NULL, NULL);
            hGantt = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "GANTT CHART", WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE | WS_VSCROLL, 20, 340, 490, 80, hwnd, NULL, NULL, NULL);
            hStats = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "STATS", WS_VISIBLE | WS_CHILD | ES_READONLY, 20, 430, 490, 25, hwnd, NULL, NULL, NULL);
            // font setting
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
            SendMessage(hTable, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hGantt, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hStats, WM_SETFONT, (WPARAM)hFont, TRUE);
            break;
        }

        case WM_COMMAND: {
            // Handle Assignment Buttons
            if (LOWORD(wParam) == ID_BTN_RESET) {
                SetWindowText(hEditN, "");
                SetWindowText(hEditAT, "");
                SetWindowText(hEditBT, "");
            }

            if (LOWORD(wParam) == ID_BTN_CLEAR) {
                SetWindowText(hTable, "");
                SetWindowText(hGantt, "");
                SetWindowText(hStats, "");
            }

            if (LOWORD(wParam) == ID_BTN_EXIT) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }

            if (LOWORD(wParam) == ID_BTN_RUN) {
                char n_str[10], at_str[512], bt_str[512];
                GetWindowText(hEditN, n_str, 10);
                GetWindowText(hEditAT, at_str, 512);
                GetWindowText(hEditBT, bt_str, 512);

                int n = atoi(n_str);
                if (n <= 0) { MessageBox(hwnd, "Please enter a valid number of processes.", "Error", MB_OK); return 0; }

                int AT[100], BT[100];
                int countAT = 0, countBT = 0;

                // Parse comma separated inputs
                char* token = strtok(at_str, ", ");
                while (token) { AT[countAT++] = atoi(token); token = strtok(NULL, ", "); }
                
                token = strtok(bt_str, ", ");
                while (token) { BT[countBT++] = atoi(token); token = strtok(NULL, ", "); }

                if (countAT != n || countBT != n) {
                    MessageBox(hwnd, "Number of Arrival/Burst inputs must match Processes (N).", "Input Mismatch", MB_OK);
                    return 0;
                }

                Process* p = (Process*)malloc(n * sizeof(Process));
                for (int i = 0; i < n; i++) {
                    sprintf(p[i].name, "P%d", i + 1);
                    p[i].arrivalTime = AT[i];
                    p[i].burstTime = BT[i];
                    p[i].remainingTime = BT[i];
                }

                char ganttOut[2048] = "|";
                int sel = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                
                // Execute Selected Algorithm
                if (sel == 0) FCFS(p, n, ganttOut);
                else if (sel == 1) SJF(p, n, ganttOut);
                else if (sel == 2) SRTF(p, n, ganttOut);

                // Format Table String
                char finalTable[4096] = "P\tAT\tBT\tCT\tTAT\tWT\r\n--------------------------------------------\r\n";
                char line[256];
                float totalWT = 0, totalTAT = 0;

                for (int i = 0; i < n; i++) {
                    sprintf(line, "%s\t%d\t%d\t%d\t%d\t%d\r\n", 
                            p[i].name, p[i].arrivalTime, p[i].burstTime, 
                            p[i].cT, p[i].turnAroundTime, p[i].waitingTime);
                    strcat(finalTable, line);
                    totalWT += p[i].waitingTime;
                    totalTAT += p[i].turnAroundTime;
                }

                // Format Stats String
                char statsOut[256];
                sprintf(statsOut, "Avg WT = %.2f | Avg TAT = %.2f", totalWT / n, totalTAT / n);

                
                SetWindowText(hTable, finalTable);
                SetWindowText(hGantt, ganttOut);
                SetWindowText(hStats, statsOut);

                free(p);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SchedulerClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "CPU Scheduler", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 550, 520, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}