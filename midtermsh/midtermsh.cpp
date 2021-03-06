#include<cstring>
#include<string>
#include <unistd.h>
#include <vector>
#include <a.out.h>
#include <sys/wait.h>
#include <iostream>
#include <error.h>

using namespace std;
const int MAX_N = 255;

vector<vector<string> > comands;

int *pipefd = new int[2];

string old_buf = "";
int l_comands = 0;
int r_comands = 0;
int cpid_size = 0;
bool stop = false;
int cpid[MAX_N];

void hdl(int sig) {
    for(int i = 0; i < cpid_size; i++) {
        kill(cpid[i], sig);
    }
    stop = true;
}

void exec_all(int l_comands, int r_comands) {
    cpid_size = r_comands - l_comands;
    if (pipe(pipefd) == -1) {
        perror("global_pipe_failed");
        exit(EXIT_FAILURE);
    }

    for (int i = l_comands; i < r_comands; i++) {
        char *args[comands[i].size() + 1];
        for (int j = 0; j < comands[i].size(); j++) {
            args[j] = const_cast<char *>(comands[i][j].c_str());
        }
        args[comands[i].size()] = nullptr;
        int *new_pipefd = new int[2];
        if (pipe(new_pipefd) == -1) {
            perror("local_pipe_failed");
            exit(EXIT_FAILURE);
        }

        int *old_pipe = pipefd;
        pipefd = new_pipefd;

        cpid[i - l_comands] = fork();
        if (cpid[i - l_comands] == 0) {
            if (i != l_comands) {
                dup2(old_pipe[0], STDIN_FILENO);
            }
            if (i + 1 != r_comands) {
                dup2(new_pipefd[1], STDOUT_FILENO);
            }
            close(old_pipe[0]);
            close(new_pipefd[1]);
            execvp(args[0], args);
        } else {
            close(old_pipe[0]);
            close(new_pipefd[1]);
        }
    }
    for (int i = l_comands; i < r_comands; i++) {
        waitpid(cpid[i - l_comands], NULL, 0);
    }
}

size_t parse(char buf[]) {

    size_t size = strlen(buf);
    string tmp = old_buf;
    bool flag = true;

    for (int i = 0; i < size; i++) {
        if ((buf[i] == ' ' || buf[i] == '\n') && tmp != "") {
            if (flag) {
                comands.push_back({});
                flag = false;
            }
            comands.back().push_back(tmp);
            tmp = "";
        } else if (buf[i] == '|') {
            tmp = "";
            r_comands++;
            flag = true;
        } else {
            if (buf[i] != ' ') {
                tmp += buf[i];
            }
        }
        if (buf[i] == '\n') {
            tmp = "";
            flag = true;
            r_comands++;
            exec_all(l_comands, r_comands);
            l_comands = r_comands;
        }
    }
    old_buf = tmp;
    return tmp.size();
}


int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = hdl;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;
    sigaction(SIGINT, &act, 0);

    if (stop) {
        perror("SIGINT HERE");
        exit(EXIT_FAILURE);
    }
    int read_cnt = 0;
    do {
        char *buf = new char[MAX_N];
        write(STDOUT_FILENO, "$\n", 2);
        read_cnt = read(STDIN_FILENO, buf, MAX_N);
        size_t par = parse(buf);
        if (par < read_cnt) {
            write(STDIN_FILENO, const_cast<char *>(old_buf.c_str()), old_buf.size() );
        }
        if (read_cnt == -1) {
            return 0;
        }
    } while (read_cnt != 0);
    return 0;
}