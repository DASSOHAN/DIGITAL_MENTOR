#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
/* ---------- CONSTANTS ---------- */
#define MAX_STU   50
#define MAX_SUB   5
#define PASS      40
#define ATT_LIMIT 75
#define STK_SIZE  100
#define QUE_SIZE  50
 
char subj[MAX_SUB][20] = {"Maths","Physics","Chemistry","English","CompSci"};
 
/* ---------- STUDENT ARRAY ---------- */
typedef struct {
    int  roll, marks[MAX_SUB], attended, total;
    char name[40];
} Student;
 
Student stu[MAX_STU];
int stuCount = 0;
 
/* ---------- STACK (Alert History) ----------
   LIFO: last alert pushed is shown first       */
char stack[STK_SIZE][150];
int  top = -1;
 
void push(const char *msg) {
    if (top < STK_SIZE - 1) strcpy(stack[++top], msg);
}
void showStack() {
    int i;
    if (top < 0) { printf("  No alerts.\n"); return; }
    for (i = top; i >= 0; i--)
        printf("  %d. %s\n", top - i + 1, stack[i]);
}
 
/* ---------- QUEUE (Help Requests) ----------
   FIFO: first request answered first           */
typedef struct { int roll; char name[40], topic[60]; } HelpReq;
HelpReq que[QUE_SIZE];
int qFront = 0, qRear = 0;
 
void enqueue(int roll, const char *name, const char *topic) {
    if (qRear >= QUE_SIZE) { printf("  Queue full!\n"); return; }
    que[qRear].roll = roll;
    strcpy(que[qRear].name,  name);
    strcpy(que[qRear].topic, topic);
    qRear++;
    printf("  Help request added!\n");
}
void dequeue() {
    HelpReq *r;
    if (qFront == qRear) { printf("  No pending requests.\n"); return; }
    r = &que[qFront++];
    printf("  Helping: %s (Roll %d) - Topic: %s\n", r->name, r->roll, r->topic);
    printf("  TIP: Revise notes, solve examples, ask teacher in doubt session.\n");
}
void showQueue() {
    int i;
    if (qFront == qRear) { printf("  Queue empty.\n"); return; }
    for (i = qFront; i < qRear; i++)
        printf("  %d. Roll %d | %s | %s\n", i-qFront+1, que[i].roll, que[i].name, que[i].topic);
}
 
/* ---------- HELPERS ---------- */
int findRoll(int roll) {
    int i;
    for (i = 0; i < stuCount; i++)
        if (stu[i].roll == roll) return i;
    return -1;
}
 
void checkAlerts(int i) {
    int j;
    char msg[150];
    float att = stu[i].total > 0 ? 100.0f * stu[i].attended / stu[i].total : 0;
    if (att < ATT_LIMIT) {
        snprintf(msg, sizeof(msg), "ATTENDANCE: %s (Roll %d) = %.1f%% < 75%%",
                 stu[i].name, stu[i].roll, att);
        push(msg);
        printf("  !! ALERT: Low attendance (%.1f%%)\n", att);
    }
    for (j = 0; j < MAX_SUB; j++) {
        if (stu[i].marks[j] < PASS) {
            snprintf(msg, sizeof(msg), "MARKS: %s (Roll %d) - %s = %d < 40",
                     stu[i].name, stu[i].roll, subj[j], stu[i].marks[j]);
            push(msg);
            printf("  !! ALERT: Low marks in %s (%d)\n", subj[j], stu[i].marks[j]);
        }
    }
}
 
void showReport(int i) {
    int j, total, best, weak;
    float att, avg;
    att = stu[i].total > 0 ? 100.0f * stu[i].attended / stu[i].total : 0;
    total = 0; best = 0; weak = 0;
    for (j = 0; j < MAX_SUB; j++) {
        total += stu[i].marks[j];
        if (stu[i].marks[j] > stu[i].marks[best]) best = j;
        if (stu[i].marks[j] < stu[i].marks[weak]) weak = j;
    }
    avg = (float)total / MAX_SUB;
    printf("\n  ---- REPORT: %s (Roll %d) ----\n", stu[i].name, stu[i].roll);
    printf("  Attendance: %d/%d = %.1f%%%s\n", stu[i].attended, stu[i].total, att,
           att < ATT_LIMIT ? "  << LOW ALERT >>" : "  OK");
    printf("  %-14s %6s  %s\n", "Subject", "Marks", "Status");
    printf("  ------------------------------------\n");
    for (j = 0; j < MAX_SUB; j++)
        printf("  %-14s %6d  %s%s%s\n", subj[j], stu[i].marks[j],
               stu[i].marks[j] < PASS ? "FAIL <<ALERT>>" : "PASS",
               j == best ? " <STRONGEST" : "",
               j == weak && best != weak ? " <WEAKEST" : "");
    printf("  Average: %.1f | Grade: %s\n", avg,
           avg>=90?"A+":avg>=80?"A":avg>=70?"B":avg>=60?"C":avg>=50?"D":"F");
    printf("  Strongest: %s | Weakest: %s\n", subj[best], subj[weak]);
}
 
/* ---------- FILE I/O (Minimal) ---------- */
void saveFile() {
    FILE *f = fopen("mentor.dat","wb");
    if (!f) { printf("  Save failed.\n"); return; }
    fwrite(&stuCount, sizeof(int), 1, f);
    fwrite(stu, sizeof(Student), stuCount, f);
    fclose(f);
    printf("  Saved %d student(s).\n", stuCount);
}
void loadFile() {
    FILE *f = fopen("mentor.dat","rb");
    if (!f) { printf("  No saved data.\n"); return; }
    fread(&stuCount, sizeof(int), 1, f);
    fread(stu, sizeof(Student), stuCount, f);
    fclose(f);
    printf("  Loaded %d student(s).\n", stuCount);
}
 
/* ---------- TEACHER MENU ---------- */
void teacherMenu() {
    int ch, r, i, j, t, lowAtt, topper, topTotal;
    float att;
    int weak[MAX_SUB];
    float subAvg[MAX_SUB];
    do {
        printf("\n  == TEACHER MENU ==\n"
               "  1.Add Student  2.View Student  3.All Students\n"
               "  4.Update Marks 5.Update Attend 6.Delete Student\n"
               "  7.Dashboard    8.Alerts        9.Help Queue\n"
               "  10.Answer Help 11.Save  12.Back\n"
               "  Choice: ");
        scanf("%d",&ch);
 
        if (ch == 1) {
            Student *s;
            if (stuCount >= MAX_STU) { printf("  Full!\n"); continue; }
            s = &stu[stuCount];
            printf("  Roll: "); scanf("%d",&s->roll);
            if (findRoll(s->roll) != -1) { printf("  Roll exists!\n"); continue; }
            printf("  Name: "); scanf(" %[^\n]", s->name);
            for (j=0; j<MAX_SUB; j++) {
                printf("  %s marks: ", subj[j]); scanf("%d",&s->marks[j]);
                if(s->marks[j]<0) s->marks[j]=0;
                if(s->marks[j]>100) s->marks[j]=100;
            }
            printf("  Attended/Total classes: "); scanf("%d %d",&s->attended,&s->total);
            if(s->attended>s->total) s->attended=s->total;
            stuCount++;
            printf("  Added: %s\n", s->name);
            checkAlerts(stuCount-1);
 
        } else if (ch == 2) {
            printf("  Roll: "); scanf("%d",&r);
            i = findRoll(r);
            if (i<0) printf("  Not found.\n"); else showReport(i);
 
        } else if (ch == 3) {
            printf("  %-6s %-20s %-9s %-7s\n","Roll","Name","Avg","Attend%");
            printf("  ----------------------------------------\n");
            for (i=0; i<stuCount; i++) {
                t=0;
                for(j=0;j<MAX_SUB;j++) t+=stu[i].marks[j];
                att = stu[i].total>0 ? 100.0f*stu[i].attended/stu[i].total : 0;
                printf("  %-6d %-20s %-9.1f %-7.1f\n",
                       stu[i].roll, stu[i].name, (float)t/MAX_SUB, att);
            }
 
        } else if (ch == 4) {
            printf("  Roll: "); scanf("%d",&r);
            i = findRoll(r);
            if (i<0) { printf("  Not found.\n"); continue; }
            for(j=0;j<MAX_SUB;j++) {
                printf("  %s marks: ",subj[j]); scanf("%d",&stu[i].marks[j]);
            }
            checkAlerts(i);
 
        } else if (ch == 5) {
            printf("  Roll: "); scanf("%d",&r);
            i = findRoll(r);
            if (i<0) { printf("  Not found.\n"); continue; }
            printf("  Attended/Total: "); scanf("%d %d",&stu[i].attended,&stu[i].total);
            if(stu[i].attended>stu[i].total) stu[i].attended=stu[i].total;
            checkAlerts(i);
 
        } else if (ch == 6) {
            printf("  Roll: "); scanf("%d",&r);
            i = findRoll(r);
            if (i<0) { printf("  Not found.\n"); continue; }
            stu[i] = stu[--stuCount];
            printf("  Deleted.\n");
 
        } else if (ch == 7) {
            for(j=0;j<MAX_SUB;j++) { weak[j]=0; subAvg[j]=0; }
            lowAtt=0; topper=-1; topTotal=0;
            for(i=0; i<stuCount; i++) {
                att = stu[i].total>0 ? 100.0f*stu[i].attended/stu[i].total : 0;
                if(att<ATT_LIMIT) lowAtt++;
                t=0;
                for(j=0;j<MAX_SUB;j++) {
                    subAvg[j] += stu[i].marks[j];
                    if(stu[i].marks[j]<PASS) weak[j]++;
                    t += stu[i].marks[j];
                }
                if(t>topTotal){ topTotal=t; topper=i; }
            }
            printf("\n  == TEACHER DASHBOARD ==\n");
            printf("  Total Students  : %d\n", stuCount);
            printf("  Low Attendance  : %d\n", lowAtt);
            printf("  %-14s %-9s %-12s\n","Subject","Avg","Weak Students");
            for(j=0;j<MAX_SUB;j++)
                printf("  %-14s %-9.1f %-12d\n", subj[j],
                       stuCount ? subAvg[j]/stuCount : 0, weak[j]);
            if(topper>=0)
                printf("  Topper: %s (Roll %d)\n", stu[topper].name, stu[topper].roll);
 
        } else if (ch == 8)  { showStack();  }
          else if (ch == 9)  { showQueue();  }
          else if (ch == 10) { dequeue();    }
          else if (ch == 11) { saveFile();   }
 
    } while (ch != 12);
}
 
/* ---------- STUDENT MENU ---------- */
void studentMenu() {
    int r, i, ch;
    float att;
    char topic[60];
    printf("  Roll Number: "); scanf("%d",&r);
    i = findRoll(r);
    if (i<0) { printf("  Not found.\n"); return; }
    printf("  Welcome, %s!\n", stu[i].name);
    do {
        printf("\n  == STUDENT MENU ==\n"
               "  1.My Report  2.Attendance  3.Request Help  4.Back\n"
               "  Choice: ");
        scanf("%d",&ch);
        if (ch==1) {
            showReport(i);
        } else if (ch==2) {
            att = stu[i].total>0 ? 100.0f*stu[i].attended/stu[i].total : 0;
            printf("  Attendance: %d/%d = %.1f%% %s\n",
                   stu[i].attended, stu[i].total, att,
                   att<ATT_LIMIT ? "<< WARNING: Below 75% >>" : "Good!");
        } else if (ch==3) {
            printf("  Topic/Subject you need help with: ");
            scanf(" %[^\n]", topic);
            enqueue(stu[i].roll, stu[i].name, topic);
        }
    } while (ch!=4);
}
 
/* ---------- MAIN ---------- */
int main() {
    int ch;
    char pw[20];
    system ("color 0B");
    
    printf("\n  *** DIGITAL MENTOR ***\n");
    loadFile();
    do {
        printf("\n  1.Teacher Login  2.Student Login  3.Exit\n  Choice: ");
        scanf("%d",&ch);
        if (ch==1) {
            printf("  Password: "); scanf("%s",pw);
            if (strcmp(pw,"teacher123")==0) teacherMenu();
            else printf("  Wrong password!\n");
        } else if (ch==2) {
            if (stuCount==0) printf("  No students yet.\n");
            else studentMenu();
        }
    } while (ch!=3);
    saveFile();
    printf("  Goodbye!\n");
    return 0;
}


