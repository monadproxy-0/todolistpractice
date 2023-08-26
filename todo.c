#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

#define MAX_TASKS 100
#define COLUMN_PADDING 2

struct task {
    char title[50];
    bool done;
    int id;
    char location[50];
} task;

int printtask(struct task t) {
    printf("%s\t", t.title);
    printf("%s\n", t.location);
    return 0;
}

int printtable(struct task *t, int numTasks) {
    // todo: make headings into variables
    int idwid = strlen("ID\n");
    int taskwid = strlen("Task\n");
    int locwid = strlen("Location\n");
    int compwid = strlen("Complete?\n");

    // iterate through to find column widths
    for (int i = 0; i < numTasks; i++) {
        int tasklen = strlen(t[i].title);
        int loclen = strlen(t[i].location);
        int idlen = snprintf(NULL, 0, "%d", t[i].id);
        
        if (tasklen > taskwid) {
            taskwid = tasklen;
        }
        if (loclen > locwid) {
            locwid = loclen;
        }
        if (idlen > idwid) {
            idwid = idlen;
        }
    }

    printf("%-*s%-*s%-*s%-*s\n", idwid + COLUMN_PADDING, "ID", taskwid + COLUMN_PADDING, "Task", locwid + COLUMN_PADDING, "Location", compwid + COLUMN_PADDING, "Complete?");

    for (int i = 0; i < numTasks; i++) {
        char marker = t[i].done ? 'x' : '-';
        printf("%-*d%-*s%-*s%-*c\n", idwid + COLUMN_PADDING, t[i].id, taskwid  + COLUMN_PADDING, t[i].title, locwid + COLUMN_PADDING, t[i].location, compwid + COLUMN_PADDING, marker);
    }

    return 0;
}

int addtask(struct task *t, int numTasks, char title[], char location[]) {

    // add task
    struct task newTask;
    strcpy(newTask.title, title);
    strcpy(newTask.location, location);
    newTask.done = false;
    newTask.id = numTasks;
    t[numTasks] = newTask;
    numTasks++;
    printf("Added task #%d: %s\n", numTasks-1, t[numTasks-1].title);
    return numTasks;
}

int deletetask(struct task *t, int numTasks, int id) {
    int idold = id;
    while (id < numTasks) {
        t[id] = t[id+1];
        t[id].id--;
        id++;
    }
    printf("Deleted task #%d\n", idold);
    return 0;
}

int completetask(struct task *t, int id) {
    t[id].done = true;
    return 0;
}

int savetasks(struct task *t, int numTasks) {
    FILE* data;
    if ( (data = fopen("tasklist.bin", "wb")) == NULL )
    {
        printf("Error opening file\n");
        return 1;   
    }

    int items_written = fwrite(t, sizeof(struct task), numTasks, data);
    if (items_written != (size_t)numTasks) {
        printf("Error writing data to file\n");
        fclose(data);
        return 1;
    }
    fclose(data);
    return 0;
}

int loadtasks(struct task *t) {
    struct task temptasks;
    int numTasks = 0;
    FILE* data;
    if ( (data = fopen("tasklist.bin", "rb")) == NULL )
    {
        return -1;
    }
    while(fread(&temptasks, sizeof(struct task), 1, data)) {
        t[numTasks] = temptasks;
        numTasks++;
    }
    fclose(data);
    return numTasks;
}

int main(int argc, char *argv[]) {
    // setup task storage
    struct task *tasks;
    int numTasks = 0;
    tasks = malloc(sizeof(struct task) * MAX_TASKS); // Janky af, add realloc later

     numTasks = loadtasks(tasks);
     if (numTasks == -1) {
         printf("No tasklist found, creating new one\n");
     }

    if (argc == 1) {
        //numTasks = loadtasks(tasks);
        printtable(tasks, numTasks);
    } else if (argc = 4) {
        if (strcmp(argv[1], "add") == 0) {
            numTasks = addtask(tasks, numTasks, argv[2], argv[3]);
            savetasks(tasks, numTasks);
        } else if (strcmp(argv[1], "delete") == 0) {
            int id = atoi(argv[2]);
            deletetask(tasks, numTasks, id);
            numTasks--;
            savetasks(tasks, numTasks);
        } else if (strcmp(argv[1], "complete") == 0) {
            int id = atoi(argv[2]);
            completetask(tasks, id);
            savetasks(tasks, numTasks);
        } else {
            printf("Usage: todo [add task location]\n");
            return 1;
        }
    } else {
        printf("Usage: todo [add task location]\n");
        return 1;
    }

    return 0;
}