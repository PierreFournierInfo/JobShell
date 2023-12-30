
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include "prompt.h"
#include "job_manager.h"
#include"redirection.h"
#include "signal_handler.h"


void ignore_signals() {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGINT)");
    }

    if (signal(SIGTERM, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGTERM)");
    }

    if (signal(SIGTTIN, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGTTIN)");
    }

    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGQUIT)");
    }

    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGTTOU)");
    }

    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGTSTP)");
    }
}

int main() {
    ignore_signals();
    traiteCommande();
}
