
#include "command_parser.h"
#include "prompt.h"
#include "command_executor.h"
#include "prompt.h"
#include "job_manager.h"
#include"redirection.h"
#include "signal_handler.h"

int main() {
    ignore_signals();
    traiteCommande();
}
