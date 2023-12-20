
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include "prompt.h"
#include "job_manager.h"
#include"redirection.h"
#include "signal_handler.h"

// Normalement dans signal handler 
void ignore_signals_P() {
    struct sigaction act = {0};
    
    // Ignorer le signal Ctrl+C (^C)
    act.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Ignorer le signal Ctrl+Z (^Z)
    if (sigaction(SIGTSTP, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

int main() {
    traiteCommande();
}
