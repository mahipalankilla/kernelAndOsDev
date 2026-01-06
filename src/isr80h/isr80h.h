#ifndef ISR80H_H
#define ISR80H_H

void isr80h_register_commands();

enum SystemCommands
{
    SYSTEM_COMMAND0_SUM,
    SYSTEM_COMMAND1_PRINT
};

#endif